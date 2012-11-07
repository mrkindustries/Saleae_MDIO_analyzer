#ifndef MDIO_SIMULATION_DATA_GENERATOR
#define MDIO_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
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

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //MDIO_SIMULATION_DATA_GENERATOR