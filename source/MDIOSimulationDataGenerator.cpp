#include "MDIOSimulationDataGenerator.h"

MDIOSimulationDataGenerator::MDIOSimulationDataGenerator()
{
}

MDIOSimulationDataGenerator::~MDIOSimulationDataGenerator()
{
}

void MDIOSimulationDataGenerator::Initialize( U32 simulation_sample_rate, MDIOAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	// 400 Khz is the most used frequency
	mClockGenerator.Init( 400000, mSimulationSampleRateHz ); 
	
    mMdio = mSimulationChannels.Add( mSettings->mMdioChannel, mSimulationSampleRateHz, BIT_HIGH );
    mMdc = mSimulationChannels.Add( mSettings->mMdcChannel, mSimulationSampleRateHz, BIT_HIGH );

    mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 20.0 ) ); //insert 10 bit-periods of idle

    mValue = 0;
}

U32 MDIOSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

    while( mMdc->GetCurrentSampleNumber() < adjusted_largest_sample_requested )
    {
		// TODO: the OpCode, PHY, DevType, and data should be random to cover every combination 
		CreateMdioC45Transaction( C45_READ,    	// OpCode
                                  0x0A,        	// PHY Address
                                  DEV_PCS,     	// DevType
                                  0x2584,      	// Register Address
                                  mValue++     	// Data
                                  );  
		
		mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 10.0 ) ); //insert 10 bit-periods of idle
		
		CreateMdioC22Transaction( C22_READ,    	// OpCode
                                  0x0B,        	// PHY Address
                                  0x05, 		// Register Address
                                  mValue++     	// Data
                                  );  
		
		mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 20.0 ) ); //insert 10 bit-periods of idle
		
    }

    *simulation_channels = mSimulationChannels.GetArray();
    return mSimulationChannels.GetCount();
}

void MDIOSimulationDataGenerator::CreateMdioC22Transaction( MdioOpCode opCode, U8 phyAddress, U8 regAddress, U16 data )
{
    // A Clause 22 transaction consists of ONE frame containing a 5 bit register address, and a 16 bit data
    CreateStart(C22_START);
    CreateOpCode(opCode);
    CreatePhyAddress(phyAddress);
    CreateRegAddress(regAddress);
    CreateTurnAround();
    CreateData(data);
}

void MDIOSimulationDataGenerator::CreateMdioC45Transaction( MdioOpCode opCode, U8 phyAddress, MdioDevType devType, U16 regAddress, U16 data )
{
    // A Clause 45 transaction consists of TWO frames. 
    // The first frame contains a 16 bit register address, the second has the 16 bit data 

    // First frame
    CreateStart(C45_START);
    CreateOpCode(C45_ADDRESS);
    CreatePhyAddress(phyAddress);
    CreateDevType(devType);
    CreateTurnAround();
    CreateAddressOrData(regAddress);

    // Second frame
    CreateStart(C45_START);
    CreateOpCode(opCode);
    CreatePhyAddress(phyAddress);
    CreateDevType(devType);
    CreateTurnAround();
    CreateAddressOrData(data);
}

void MDIOSimulationDataGenerator::CreateStart(MdioStart start)
{
    if( mMdc->GetCurrentBitState() == BIT_HIGH )
    {
        mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 1.0 ) );
        mMdc->Transition();
		mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 0.5 ) );
    }
    
	BitExtractor bit_extractor( start, AnalyzerEnums::MsbFirst, 2 );

    for( U32 i=0; i<2; i++ )
    {
        CreateBit( bit_extractor.GetNextBit() );
    }

}

void MDIOSimulationDataGenerator::CreateOpCode(MdioOpCode opCode)
{
    BitExtractor bit_extractor( opCode, AnalyzerEnums::MsbFirst, 2 );

    for( U32 i=0; i<2; i++ )
    {
        CreateBit( bit_extractor.GetNextBit() );
    }
}

void MDIOSimulationDataGenerator::CreatePhyAddress(U8 address)
{
	BitExtractor bit_extractor( address, AnalyzerEnums::MsbFirst, 5 );

    for( U32 i=0; i<5; i++ )
    {
        CreateBit( bit_extractor.GetNextBit() );
    }
}

void MDIOSimulationDataGenerator::CreateRegAddress(U8 address)
{

	BitExtractor bit_extractor( address, AnalyzerEnums::MsbFirst, 5 );

    for( U32 i=0; i<5; i++ )
    {
        CreateBit( bit_extractor.GetNextBit() );
    }

}

void MDIOSimulationDataGenerator::CreateDevType(MdioDevType devType)
{
    BitExtractor bit_extractor( devType, AnalyzerEnums::MsbFirst, 5 );

    for( U32 i=0; i<5; i++ )
    {
        CreateBit( bit_extractor.GetNextBit() );
    }
}

// TODO TA can take less time (that's why I didn't use CreateBit)
void MDIOSimulationDataGenerator::CreateTurnAround()
{
    
	mMdio->TransitionIfNeeded(BIT_HIGH); // value 1
	
	mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 0.5 ) ); 
	
	mMdc->Transition(); // MDC posedge
    
	mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 1.0 ) ); 	
	
	mMdc->Transition(); // MDC negedge
	
	mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 0.5 ) ); 
	
	mMdio->Transition(); // value 0

	mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 0.5 ) ); 
	
	mMdc->Transition(); // MDC posedge
    
	mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 1.0 ) ); 	
	
	mMdc->Transition(); // MDC negedge
	
	mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 0.5 ) ); 
}

void MDIOSimulationDataGenerator::CreateData(U16 data)
{
    BitExtractor bit_extractor( data, AnalyzerEnums::MsbFirst, 16 );

    for( U32 i=0; i<16; i++ )
    {
        CreateBit( bit_extractor.GetNextBit() );
    }

}

// NOTE: currently it has the same behaviour as CreateData(U16)
void MDIOSimulationDataGenerator::CreateAddressOrData(U16 data)
{
    CreateData(data); 
    CreateBit( BIT_HIGH );  // Release the bus (normally pulled up) 
}

void MDIOSimulationDataGenerator::CreateBit( BitState bit_state )
{
    if( mMdc->GetCurrentBitState() != BIT_LOW )
        AnalyzerHelpers::Assert( "CreateBit() expects to be entered with MDC low" );

    mMdio->TransitionIfNeeded( bit_state );

    mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 0.5 ) );

    mMdc->Transition(); // MDC posedge

    mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 1.0 ) );

    mMdc->Transition(); // MDC negedge
	
	mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 0.5 ) );

}
