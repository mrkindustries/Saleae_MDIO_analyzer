#ifndef MDIO_ANALYZER_H
#define MDIO_ANALYZER_H

#include <Analyzer.h>
#include "MDIOAnalyzerResults.h"
#include "MDIOSimulationDataGenerator.h"

class MDIOAnalyzerSettings;
class ANALYZER_EXPORT MDIOAnalyzer : public Analyzer
{
public:
	MDIOAnalyzer();
	virtual ~MDIOAnalyzer();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();
	
protected: // functions
	void AdvanceToStartFrame();
	void StartFrame();

protected: //vars
	std::auto_ptr< MDIOAnalyzerSettings > mSettings;
	std::auto_ptr< MDIOAnalyzerResults > mResults;
	
	AnalyzerChannelData* mMdio;
	AnalyzerChannelData* mMdc;

	MDIOSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //MDIO_ANALYZER_H
