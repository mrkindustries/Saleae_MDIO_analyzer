#include "MDIOAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


MDIOAnalyzerSettings::MDIOAnalyzerSettings()
:	mMdioChannel( UNDEFINED_CHANNEL ),
    mMdcChannel( UNDEFINED_CHANNEL ),
    mClauseVersion( 45 )
{
    mMdioChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mMdioChannelInterface->SetTitleAndTooltip( "MDIO", "MDIO data bus" );
    mMdioChannelInterface->SetChannel( mMdioChannel );

    mMdcChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mMdcChannelInterface->SetTitleAndTooltip( "MDC", "MDIO clock line " );
    mMdcChannelInterface->SetChannel( mMdcChannel );

    mClauseVersionInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mClauseVersionInterface->SetTitleAndTooltip( "Protocol Version",  "Version can be Clause 22 or Clause 45, newer devices will likely be Clause 45" );
    mClauseVersionInterface->AddNumber(22, "Clause 22", "The Clause 22 MDIO interface specifies 5 PHY-address bits which allows for up to 32 PHY devices attached to a single MDIO data line. Thus a single MAC can control 32 PHY's.\nIt also specifies 5 register-address bits which allows up to 32 control registers per PHY");
    mClauseVersionInterface->AddNumber(45, "Clause 45", "The 802.3ae Clause 45 interface allows for 16-bit register addressing (65536 registers) for 32 PHY devices per MDIO. These features were added for 10 Gigabit Ethernet support and use different opcodes and start sequences. Opcodes 00(set address) and 11(read)/01(write)/10(read increment) are used as two serial transactions to read and write registers.");

    AddInterface( mMdioChannelInterface.get() );
    AddInterface( mMdcChannelInterface.get() );
    AddInterface( mClauseVersionInterface.get() );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
    AddChannel( mMdioChannel, "MDIO", false );
    AddChannel( mMdcChannel, "MDC", false );
}

MDIOAnalyzerSettings::~MDIOAnalyzerSettings()
{
}

bool MDIOAnalyzerSettings::SetSettingsFromInterfaces()
{
    mMdioChannel = mMdioChannelInterface->GetChannel();
    mMdcChannel = mMdcChannelInterface->GetChannel();
    mClauseVersion = mClauseVersionInterface->GetNumber();

	ClearChannels();
    AddChannel( mMdioChannel, "MDIO", true );
    AddChannel( mMdcChannel, "MDC", true );

	return true;
}

void MDIOAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mMdioChannelInterface->SetChannel( mMdioChannel );
    mMdcChannelInterface->SetChannel( mMdcChannel );
    mClauseVersionInterface->SetNumber( mClauseVersion );
}

void MDIOAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

    text_archive >> mMdioChannel;
    text_archive >> mMdcChannel;
    text_archive >> mClauseVersion;

	ClearChannels();
    AddChannel( mMdioChannel, "MDIO", true );
    AddChannel( mMdcChannel, "MDC", true );

	UpdateInterfacesFromSettings();
}

const char* MDIOAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

    text_archive << mMdioChannel;
    text_archive << mMdcChannel;
    text_archive << mClauseVersion;

	return SetReturnString( text_archive.GetString() );
}
