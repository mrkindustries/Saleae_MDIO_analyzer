#ifndef MDIO_ANALYZER_SETTINGS
#define MDIO_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class MDIOAnalyzerSettings : public AnalyzerSettings
{
public:
	MDIOAnalyzerSettings();
	virtual ~MDIOAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
    Channel mDataChannel;
    Channel mClkChannel;
    U32 mClauseVersion;

protected:
    std::auto_ptr< AnalyzerSettingInterfaceChannel >	mDataChannelInterface;
    std::auto_ptr< AnalyzerSettingInterfaceChannel >	mClkChannelInterface;
    std::auto_ptr< AnalyzerSettingInterfaceNumberList >	mClauseVersionInterface;
};

#endif //MDIO_ANALYZER_SETTINGS
