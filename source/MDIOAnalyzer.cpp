#include "MDIOAnalyzer.h"
#include "MDIOAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

// TODO: fix missing rising arrow in packet end boundary

MDIOAnalyzer::MDIOAnalyzer()
:	Analyzer(),  
	mSettings( new MDIOAnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

MDIOAnalyzer::~MDIOAnalyzer()
{
	KillThread();
}

// TODO use CancelPacketAndStartNewPacket() if error
void MDIOAnalyzer::WorkerThread()
{
	mResults.reset( new MDIOAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mMdioChannel );

	mSampleRateHz = GetSampleRate();
	mPacketInTransaction = 0;
	mTransactionID = 0;

	// mMDio and mMdc have the actual data from the channel
	mMdio = GetAnalyzerChannelData( mSettings->mMdioChannel );
	mMdc = GetAnalyzerChannelData( mSettings->mMdcChannel );

	for( ; ; )
	{	
		
		// normally pulled up
		AdvanceToHighMDIO();
		
		// go to the beggining of a start frame (MDIO HIGH->LOW transition)
		AdvanceToStartFrame();
		
		// Put a marker in the start position
		mResults->AddMarker( mMdio->GetSampleNumber(), AnalyzerResults::Start, mSettings->mMdioChannel );
		
		// process the packet
		ProcessStartFrame();
		ProcessOpcodeFrame();
		ProcessPhyAddrFrame();
		ProcessRegAddrDevTypeFrame();
		ProcessTAFrame();
		ProcessAddrDataFrame();

		// Put a marker in the end of the packet
		mResults->AddMarker( mMdio->GetSampleNumber(), AnalyzerResults::Stop, mSettings->mMdioChannel );
		
		// add arrows in clock posedge
		U32 count = mMdcArrowLocations.size();
		for( U32 i=0; i<count; i++ ) 
		{
			mResults->AddMarker( mMdcArrowLocations[i], AnalyzerResults::UpArrow, mSettings->mMdcChannel );
		}
		mMdcArrowLocations.clear();
		
		// commit the generated frames to a packet
		U64 packet_id = mResults->CommitPacketAndStartNewPacket();
		
		// add the current packet to the current transaction
		mResults->AddPacketToTransaction(mTransactionID, packet_id);
		
		// finally commit the results to the MDIOAnalyzerResults class
		mResults->CommitResults();
		
		// Check if it is the end of a C22 or C45 transaction (C45 transaction consists of two frames)
		if ( ( currentPacket == MDIO_C22_PACKET ) or 
			 ( currentPacket == MDIO_C45_PACKET and mPacketInTransaction == 2 ) ) 
		{
			mTransactionID++;
			mPacketInTransaction = 0;
		}

	}
		
	
}

void MDIOAnalyzer::AdvanceToStartFrame() 
{
	if ( mMdio->GetBitState() == BIT_LOW ) 
	{
		AnalyzerHelpers::Assert( "AdvanceToStartFrame() must be called with MDIO line with HIGH state" );
	}
	
	mMdio->AdvanceToNextEdge(); // high->low transition
	mMdc->AdvanceToAbsPosition( mMdio->GetSampleNumber() );
	
}

void MDIOAnalyzer::ProcessStartFrame() 
{
	// starting sample of the start frame
	U64 starting_sample = mMdio->GetSampleNumber();

	// get the value of the two bits of the start frame
	U64 mdc_rising_edge;
	BitState bit0, bit1;
	GetBit(bit0, mdc_rising_edge); // sample first bit (TODO check if first bit is 0)
	GetBit(bit1, mdc_rising_edge); // sample second bit
	
	// create and set the start frame
	Frame frame;
	frame.mType = ( bit1 == BIT_HIGH ) ? MDIO_C22_START : MDIO_C45_START;
	frame.mFlags = 0;
	frame.mStartingSampleInclusive = starting_sample;
	frame.mEndingSampleInclusive = mMdio->GetSampleNumber();
	
	mResults->AddFrame( frame );
	ReportProgress( frame.mEndingSampleInclusive );
	
	currentPacket = ( frame.mType == MDIO_C22_START ) ? MDIO_C22_PACKET : MDIO_C45_PACKET;
	if (currentPacket == MDIO_C45_PACKET) 
	{
		mPacketInTransaction++;
	}
}
	

void MDIOAnalyzer::ProcessOpcodeFrame() 
{
	// starting sample of the start frame
	U64 starting_sample = mMdio->GetSampleNumber()+1;

	// get the value of the two bits of the start frame
	U64 mdc_rising_edge;
	BitState bit0, bit1;
	GetBit(bit0, mdc_rising_edge); // sample first bit (TODO check if first bit is 0)
	// U64 starting_sample = mdc_rising_edge; // or set the mStartingSampleInclusive here... 
	GetBit(bit1, mdc_rising_edge); // sample second bit
	
	// reconstruct the opcode 
	DataBuilder opcode;
	U64 value;
	opcode.Reset( &value, AnalyzerEnums::MsbFirst, 2 );
	opcode.AddBit(bit0);
	opcode.AddBit(bit1);
	
	// create and set the opcode frame
	Frame frame;
	
	switch( value ) 
	{
		case C45_ADDRESS: 					frame.mType = ( currentPacket == MDIO_C22_PACKET ) 
														  ? MDIO_UNKNOWN : MDIO_C45_OP_ADDR; break;	// Clause 22 opcode cannot be C45_ADDRESS (00)
		case C45_WRITE | C22_WRITE: 		frame.mType = MDIO_OP_W; break;
		case C22_READ | C45_READ_AND_ADDR:	frame.mType = ( currentPacket == MDIO_C22_PACKET ) 
														  ? MDIO_OP_R : MDIO_C45_OP_READ_INC_ADDR; break;
		case C45_READ:						frame.mType = ( currentPacket == MDIO_C22_PACKET ) 
														  ? MDIO_UNKNOWN : MDIO_OP_R; break;	// Clause 22 opcode cannot be C45_READ (11)
		default: 							frame.mType = MDIO_UNKNOWN;
	}
	
	if (frame.mType == MDIO_UNKNOWN) 
	{
		frame.mFlags = DISPLAY_AS_WARNING_FLAG;
	}
	else 
	{
		frame.mFlags = 0;
	}
	
	frame.mStartingSampleInclusive = starting_sample;
	frame.mEndingSampleInclusive = mMdio->GetSampleNumber();
	
	mResults->AddFrame( frame );
	ReportProgress( frame.mEndingSampleInclusive );
}

void MDIOAnalyzer::ProcessPhyAddrFrame() 
{
	// starting sample of the start frame
	U64 starting_sample = mMdio->GetSampleNumber()+1;
	
	// get the value of the the 5 bits of the phyaddr frame
	DataBuilder opcode;
	U64 value;
	opcode.Reset( &value, AnalyzerEnums::MsbFirst, 5 );
	U64 mdc_rising_edge;
	for(U32 i=0; i < 5; ++i) 
	{
		BitState bit;
		GetBit(bit, mdc_rising_edge);
		opcode.AddBit(bit);
	}
	
	// create and set the phyaddr frame
	Frame frame;
	frame.mType = MDIO_PHYADDR;	
	frame.mData1 = value;
	frame.mFlags = 0;
	frame.mStartingSampleInclusive = starting_sample;
	frame.mEndingSampleInclusive = mMdio->GetSampleNumber();
	
	mResults->AddFrame( frame );
	ReportProgress( frame.mEndingSampleInclusive );
}

void MDIOAnalyzer::ProcessRegAddrDevTypeFrame() 
{
	// starting sample of the start frame
	U64 starting_sample = mMdio->GetSampleNumber()+1;
	
	// get the value of the 5 bits of register address or devtype frame
	DataBuilder opcode;
	U64 value;
	opcode.Reset( &value, AnalyzerEnums::MsbFirst, 5 );
	U64 mdc_rising_edge;
	for(U32 i=0; i < 5; ++i) 
	{
		BitState bit;
		GetBit(bit, mdc_rising_edge);
		opcode.AddBit(bit);
	}
	
	// create and set the phyaddr frame
	Frame frame;
	if (currentPacket == MDIO_C22_PACKET) 
	{
		frame.mType = MDIO_C22_REGADDR; 
	}
	else 
	{
		frame.mType = GetDevType(value);
	}
	frame.mData1 = value;
	frame.mFlags = 0;
	frame.mStartingSampleInclusive = starting_sample;
	frame.mEndingSampleInclusive = mMdio->GetSampleNumber();
	
	mResults->AddFrame( frame );
	ReportProgress( frame.mEndingSampleInclusive );
}

void MDIOAnalyzer::ProcessTAFrame()
{
	// starting sample of the TA frame
	U64 starting_sample = mMdio->GetSampleNumber()+1;

	// get the value of the two bits of the start frame
	U64 mdc_rising_edge;
	BitState bit0, bit1;
	// TODO: check TA bits?
	GetBit(bit0, mdc_rising_edge); 
	GetBit(bit1, mdc_rising_edge); 
	
	Frame frame;
	frame.mType = MDIO_TA;
	frame.mFlags = 0;
	frame.mStartingSampleInclusive = starting_sample;
	frame.mEndingSampleInclusive = mMdio->GetSampleNumber();
	
	mResults->AddFrame( frame );
	ReportProgress( frame.mEndingSampleInclusive );
	
}

void MDIOAnalyzer::ProcessAddrDataFrame() 
{
	U64 starting_sample = mMdio->GetSampleNumber()+1;
	
	DataBuilder opcode;
	U64 value;
	opcode.Reset( &value, AnalyzerEnums::MsbFirst, 16 );
	U64 mdc_rising_edge;
	for(U32 i=0; i < 16; ++i) 
	{
		BitState bit;
		GetBit(bit, mdc_rising_edge);
		opcode.AddBit(bit);
	}
	
	Frame frame;
	frame.mType = (currentPacket == MDIO_C22_PACKET) ? MDIO_C22_DATA : MDIO_C45_ADDRDATA;	
	frame.mData1 = value;
	frame.mFlags = 0;
	frame.mStartingSampleInclusive = starting_sample;
	frame.mEndingSampleInclusive = mMdio->GetSampleNumber();
	
	mResults->AddFrame( frame );
	ReportProgress( frame.mEndingSampleInclusive );
}

// TODO: better position for the "end" marker
void MDIOAnalyzer::AdvanceToHighMDIO() 
{
	if( mMdio->GetBitState() == BIT_LOW )
	{
		mMdio->AdvanceToNextEdge(); // go to high state
		mMdc->AdvanceToAbsPosition( mMdio->GetSampleNumber() );
	}	

}

MDIOFrameType MDIOAnalyzer::GetDevType(const U64 & value)
{
	switch (value) {
		case DEV_RESERVED: 	return MDIO_C45_DEVTYPE_RESERVED;
		case DEV_PMD_PMA: 	return MDIO_C45_DEVTYPE_PMD_PMA;
		case DEV_WIS: 		return MDIO_C45_DEVTYPE_WIS;
		case DEV_PCS: 		return MDIO_C45_DEVTYPE_PCS;
		case DEV_PHY_XS: 	return MDIO_C45_DEVTYPE_PHY_XS;
		case DEV_DTE_XS: 	return MDIO_C45_DEVTYPE_DTE_XS;
		default: 			return MDIO_C45_DEVTYPE_OTHER;
	}
}

void MDIOAnalyzer::GetBit( BitState& bit_state, U64& mdc_rising_edge )
{
	// mMdc must be low coming into this function
	mMdc->AdvanceToNextEdge(); //posedge
	mdc_rising_edge = mMdc->GetSampleNumber();
	mMdcArrowLocations.push_back( mdc_rising_edge );
	
	mMdio->AdvanceToAbsPosition( mdc_rising_edge );  //data read on SCL posedge

	bit_state = mMdio->GetBitState(); // sample the mMdio channel
	
	mMdc->AdvanceToNextEdge(); // negedge 
	mMdio->AdvanceToAbsPosition( mMdc->GetSampleNumber() ); // advance mMdio 
	
}

bool MDIOAnalyzer::NeedsRerun()
{
	return false;
}

U32 MDIOAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 MDIOAnalyzer::GetMinimumSampleRateHz()
{
	return 2000000; // Minimum sampling rate: 2 Mhz.
}

const char* MDIOAnalyzer::GetAnalyzerName() const
{
	return "MDIO";
}

const char* GetAnalyzerName()
{
	return "MDIO";
}

Analyzer* CreateAnalyzer()
{
	return new MDIOAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}