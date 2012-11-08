#ifndef MDIO_SIMULATION_DATA_GENERATOR
#define MDIO_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <AnalyzerSettings.h>
#include <AnalyzerHelpers.h>
#include "MDIOAnalyzerSettings.h"

class MDIOAnalyzerSettings;

class MDIOSimulationDataGenerator
{
public:
	MDIOSimulationDataGenerator();
	~MDIOSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, MDIOAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	MDIOAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;
    U16 mValue;

protected:	/** MDIO specific functions */
    void CreateMdioC22Transaction( MdioOpCode OpCode, U8 phyAddress, U8 regAddress, U16 data );
    void CreateMdioC45Transaction( MdioOpCode opCode, U8 phyAddress, MdioDevType devType, U16 regAddress, U16 data );
    void CreateStart(MdioStart start);          /** 2 bits: 01 for Clause 22, 00 for Clause 45 */
    void CreateOpCode(MdioOpCode opCode);       /** 2 bits */
    void CreatePhyAddress(U8 address);          /** 5 bits */
    void CreateRegAddress(U8 address);          /** 5 bits (used on Clause 22) */
    void CreateDevType(U8 devType);             /** 5 bits (used on Clause 45) */
    void CreateTurnAround();                    /** 2 bits: Turnaround time to change bus ownership from STA to MMD if required */
    void CreateData(U16 data);                  /** 16 bits (used on Clause 22) */
    void CreateAddressOrData(U16 data);         /** 16 bits (used on Clause 45) */
    void CreateByte( U8 data);
    void CreateBit( BitState bit_state );

protected:
    ClockGenerator mClockGenerator;

	std::string mSerialText;
	U32 mStringIndex;

    SimulationChannelDescriptorGroup mSimulationChannels;
    SimulationChannelDescriptor *mMdio;
    SimulationChannelDescriptor *mMdc;

};
#endif //MDIO_SIMULATION_DATA_GENERATOR
