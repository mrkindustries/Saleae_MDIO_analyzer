#include "MDIOSimulationDataGenerator.h"

#include <AnalyzerHelpers.h>

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

    mClockGenerator.Init( 400000, simulation_sample_rate );

    mMdio = mSimulationChannels.Add( settings->mMdioChannel, mSimulationSampleRateHz, BIT_HIGH );
    mMdc = mSimulationChannels.Add( settings->mMdcChannel, mSimulationSampleRateHz, BIT_HIGH );

    mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 10.0 ) ); //insert 10 bit-periods of idle

    mValue = 0;
}

U32 MDIOSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

    while( mMdc->GetCurrentSampleNumber() < adjusted_largest_sample_requested )
    {
        /** TODO: check in the settings if we expect C22 or C45 protocol, different packets should be sent accordingly */
        CreateMdioC45Transaction( C45_READ,    // OpCode
                                  0x0A,        // PHY Address
                                  DEV_PCS,     // DevType
                                  0x2584,      // Register Address
                                  mValue++     // Data
                                  );  /** TODO: the OpCode, PHY, DevType, and data should be random to cover every combination */

        mSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 20.0 ) ); //insert 20 bit-periods of idle
    }

    *simulation_channels = mSimulationChannels.GetArray();
    return mSimulationChannels.GetCount();
}

void MDIOSimulationDataGenerator::CreateMdioC22Transaction( MdioOpCode opCode, U8 phyAddress, U8 regAddress, U16 data )
{
    /** A Clause 22 transaction consists of ONE frame containing a 5 bit register address, and a 16 bit data */
    CreateStart(C22_START);
    CreateOpCode(opCode);
    CreatePhyAddress(phyAddress);
    CreateRegAddress(regAddress);
    CreateTurnAround();
    CreateData(data);
}

void MDIOSimulationDataGenerator::CreateMdioC45Transaction( MdioOpCode opCode, U8 phyAddress, MdioDevType devType, U16 regAddress, U16 data )
{
    /** A Clause 45 transaction consists of TWO frames. The first frame contains a 16 bit register address, the second has the 16 bit data */

    /** First frame */
    CreateStart(C45_START);
    CreateOpCode(C45_ADDRESS);
    CreatePhyAddress(phyAddress);
    CreateDevType(devType);
    CreateTurnAround();
    CreateAddressOrData(regAddress);

    /** Second frame */
    CreateStart(C45_START);
    CreateOpCode(opCode);
    CreatePhyAddress(phyAddress);
    CreateDevType(devType);
    CreateTurnAround();
    CreateAddressOrData(data);
}

void MDIOSimulationDataGenerator::CreateStart(MdioStart start)
{

}

void MDIOSimulationDataGenerator::CreateOpCode(MdioOpCode opCode)
{

}

void MDIOSimulationDataGenerator::CreatePhyAddress(U8 address)
{

}

void MDIOSimulationDataGenerator::CreateRegAddress(U8 address)
{

}

void MDIOSimulationDataGenerator::CreateDevType(U8 devType)
{

}

void MDIOSimulationDataGenerator::CreateTurnAround()
{

}

void MDIOSimulationDataGenerator::CreateData(U16 data)
{

}

void MDIOSimulationDataGenerator::CreateAddressOrData(U16 data)
{

}
