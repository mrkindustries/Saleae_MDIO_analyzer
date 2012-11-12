#include "MDIOAnalyzer.h"
#include "MDIOAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

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

void MDIOAnalyzer::WorkerThread()
{
	// std::cout << "Hi" << std::endl;
	/*
	mResults.reset( new MDIOAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mMdioChannel );

	mSampleRateHz = GetSampleRate();

	// mMDio and mMdc have the actual data from the channel
	mMdio = GetAnalyzerChannelData( mSettings->mMdioChannel );
	mMdc = GetAnalyzerChannelData( mSettings->mMdcChannel );

	// go to the high state (before the start frame)
	if( mMdio->GetBitState() == BIT_LOW )
		mMdio->AdvanceToNextEdge();
	
	//AdvanceToStartFrame();
	//StartFrame();

	/*
	U32 samples_per_bit = mSampleRateHz / mSettings->mBitRate;
	U32 samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( mSettings->mBitRate ) );

	for( ; ; )
	{
		U8 data = 0;
		U8 mask = 1 << 7;
		
		mMdio->AdvanceToNextEdge(); //falling edge -- beginning of the start bit

		U64 starting_sample = mMdio->GetSampleNumber();

		mMdio->Advance( samples_to_first_center_of_first_data_bit );

		for( U32 i=0; i<8; i++ )
		{
			//let's put a dot exactly where we sample this bit:
			mResults->AddMarker( mMdio->GetSampleNumber(), AnalyzerResults::Dot, mSettings->mInputChannel );

			if( mMdio->GetBitState() == BIT_HIGH )
				data |= mask;

			mMdio->Advance( samples_per_bit );

			mask = mask >> 1;
		}


		//we have a byte to save. 
		Frame frame;
		frame.mData1 = data;
		frame.mFlags = 0;
		frame.mStartingSampleInclusive = starting_sample;
		frame.mEndingSampleInclusive = mMdio->GetSampleNumber();

		mResults->AddFrame( frame );
		mResults->CommitResults();
		ReportProgress( frame.mEndingSampleInclusive );
	}
	*/
	
}

void MDIOAnalyzer::AdvanceToStartFrame() 
{
	for( ; ; )
	{
		// starts high 
		mMdio->AdvanceToNextEdge();

		if( mMdio->GetBitState() == BIT_LOW )
		{
			mMdc->AdvanceToAbsPosition( mMdio->GetSampleNumber() );
			break;
		}	
	}
	
	// Put a marker in the start position
	mResults->AddMarker( mMdio->GetSampleNumber(), AnalyzerResults::Start, mSettings->mMdioChannel );
}

void MDIOAnalyzer::StartFrame() 
{
	/*
	// is MDIO line is low
	mMdio->AdvanceToNextEdge();
	if( mMdio->GetBitState() == BIT_LOW )
		{
			mMdc->AdvanceToAbsPosition( mMdio->GetSampleNumber() );
			break;
		}	
	}
	
	// Put a marker in the start position
	mResults->AddMarker( mMdio->GetSampleNumber(), AnalyzerResults::Start, mSettings->mMdioChannel );
	*/
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
	return 2000000; // WARNING: check
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