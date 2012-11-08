#include "MDIOAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


MDIOAnalyzerSettings::MDIOAnalyzerSettings()
:	mDataChannel( UNDEFINED_CHANNEL ),
    mClkChannel( UNDEFINED_CHANNEL ),
    mClauseVersion( 45 )
{
    mDataChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mDataChannelInterface->SetTitleAndTooltip( "MDIO", "MDIO data bus" );
    mDataChannelInterface->SetChannel( mDataChannel );

    mClkChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mClkChannelInterface->SetTitleAndTooltip( "MDC", "MDIO clock line " );
    mClkChannelInterface->SetChannel( mClkChannel );

    mClauseVersionInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mClauseVersionInterface->SetTitleAndTooltip( "Protocol Version",  "Version can be Clause 22 or Clause 45, newer devices will likely be Clause 45" );
    mClauseVersionInterface->AddNumber(22, "Clause 22", "The Clause 22 MDIO interface specifies 5 PHY-address bits which allows for up to 32 PHY devices attached to a single MDIO data line. Thus a single MAC can control 32 PHY's.\nIt also specifies 5 register-address bits which allows up to 32 control registers per PHY");
    mClauseVersionInterface->AddNumber(45, "Clause 45", "The 802.3ae Clause 45 interface allows for 16-bit register addressing (65536 registers) for 32 PHY devices per MDIO. These features were added for 10 Gigabit Ethernet support and use different opcodes and start sequences. Opcodes 00(set address) and 11(read)/01(write)/10(read increment) are used as two serial transactions to read and write registers.");

    AddInterface( mDataChannelInterface.get() );
    AddInterface( mClkChannelInterface.get() );
    AddInterface( mClauseVersionInterface.get() );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
    AddChannel( mDataChannel, "MDIO", false );
    AddChannel( mClkChannel, "MDC", false );
}

MDIOAnalyzerSettings::~MDIOAnalyzerSettings()
{
}

bool MDIOAnalyzerSettings::SetSettingsFromInterfaces()
{
    mDataChannel = mDataChannelInterface->GetChannel();
    mClkChannel = mClkChannelInterface->GetChannel();
    mClauseVersion = mClauseVersionInterface->GetNumber();

	ClearChannels();
    AddChannel( mDataChannel, "MDIO", true );
    AddChannel( mClkChannel, "MDC", true );

	return true;
}

void MDIOAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mDataChannelInterface->SetChannel( mDataChannel );
    mClkChannelInterface->SetChannel( mClkChannel );
    mClauseVersionInterface->SetNumber( mClauseVersion );
}

void MDIOAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

    text_archive >> mDataChannel;
    text_archive >> mClkChannel;
    text_archive >> mClauseVersion;

	ClearChannels();
    AddChannel( mDataChannel, "MDIO", true );
    AddChannel( mClkChannel, "MDC", true );

	UpdateInterfacesFromSettings();
}

const char* MDIOAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

    text_archive << mDataChannel;
    text_archive << mClkChannel;
    text_archive << mClauseVersion;

	return SetReturnString( text_archive.GetString() );
}
