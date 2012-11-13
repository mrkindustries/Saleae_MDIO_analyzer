#include "MDIOAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "MDIOAnalyzer.h"
#include "MDIOAnalyzerSettings.h"
#include <iostream>
#include <fstream>
#include <sstream>

// TODO: check length of generated strings in the bubbles
// TODO: instead of MDIO_C45_ADDRDATA -> MDIO_C45_DATA and MDIO_C45_ADDR types

MDIOAnalyzerResults::MDIOAnalyzerResults( MDIOAnalyzer* analyzer, MDIOAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

MDIOAnalyzerResults::~MDIOAnalyzerResults()
{
}

void MDIOAnalyzerResults::GenStartString(const Frame & frame, const char* clause, bool tabular) 
{
	if (!tabular) 
	{  
		AddResultString( "S C", clause );
	}
	AddResultString( "START C", clause );
}

void MDIOAnalyzerResults::GenOpString(const Frame & frame, 
									  const char* opcode_str0, const char* opcode_str1, 
									  const char* opcode_str2, bool tabular) 
{
	if (!tabular) 
	{
		AddResultString( "OP[", opcode_str0, "]" );
		AddResultString( "OP [", opcode_str1, "]" );
	}
	AddResultString( "OPCODE [", opcode_str2, "]" );

}

void MDIOAnalyzerResults::GenPhyAddrString(const Frame & frame, DisplayBase display_base, bool tabular) 
{
	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 5, number_str, 128 );

	if (!tabular) 
	{
		AddResultString( "PHY" );
		AddResultString( "PHY[", number_str, "]" );
		AddResultString( "PHYADR[", number_str, "]" );
	}
	AddResultString( "PHY Address [", number_str, "]" );
}

void MDIOAnalyzerResults::GenC22RegAddrString(const Frame & frame, DisplayBase display_base, bool tabular) 
{
	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 5, number_str, 128 );
	
	if (!tabular) 
	{
		AddResultString( "REG" );
		AddResultString( "REG[", number_str, "]" );
		AddResultString( "REGADR[", number_str, "]" );
	}
	AddResultString( "Register Address [", number_str, "]" );
}

void MDIOAnalyzerResults::GenC45DevTypeString(const Frame & frame, DisplayBase display_base,
											  const char* devtype, bool tabular) 
{
	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 5, number_str, 128 );

	if (!tabular) 
	{
		AddResultString( "DEV" );
		AddResultString( "DEVTYPE[", devtype, "]" );
	}
	AddResultString("DEVTYPE [", devtype, "] - ", number_str);
}

void MDIOAnalyzerResults::GenTAString(const Frame & frame, DisplayBase display_base) 
{
	AddResultString( "TA" );
}

void MDIOAnalyzerResults::GenC22DataString(const Frame & frame, DisplayBase display_base, bool tabular) 
{
	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 16, number_str, 128 );
	
	if (!tabular) 
	{
		AddResultString( "D" );
		AddResultString( "D[", number_str, "]" );
	}
	AddResultString( "DATA [", number_str, "]" );
}

void MDIOAnalyzerResults::GenC45AddrDataString(const Frame & frame, DisplayBase display_base, bool tabular) 
{
	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 16, number_str, 128 );
	
	if (!tabular) 
	{
		AddResultString( "A/D" );
		AddResultString( "A/D[", number_str, "]" );
	}
	AddResultString( "Address/Data [", number_str, "]" );
}

void MDIOAnalyzerResults::GenUnknownString(bool) 
{
	AddResultString("Unknown");
}

void MDIOAnalyzerResults::GenTabularText(U64 frame_index, DisplayBase display_base, bool tabular) 
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );
    
    switch (frame.mType) 
	{
		case MDIO_C22_START: 			GenStartString(frame, "22", tabular); break;
		case MDIO_C45_START: 			GenStartString(frame, "45", tabular); break;
		case MDIO_OP_W: 				GenOpString(frame, "W", "Wrt", "Write", tabular); break;
		case MDIO_OP_R: 				GenOpString(frame, "R", "Rd", "Read", tabular); break;
		case MDIO_C45_OP_ADDR: 			GenOpString(frame, "A", "Addr", "Address", tabular); break;
		case MDIO_C45_OP_READ_INC_ADDR:	GenOpString(frame, "R+A", "Rd +Ad", "Read-Increment-Address", tabular); break;
		case MDIO_PHYADDR: 				GenPhyAddrString(frame, display_base, tabular); break;
		case MDIO_C22_REGADDR: 			GenC22RegAddrString(frame, display_base, tabular); break;
		case MDIO_C45_DEVTYPE_RESERVED: GenC45DevTypeString(frame, display_base, "Reserved", tabular); break;
		case MDIO_C45_DEVTYPE_PMD_PMA: 	GenC45DevTypeString(frame, display_base, "PMD/PMA", tabular); break;
		case MDIO_C45_DEVTYPE_WIS: 		GenC45DevTypeString(frame, display_base, "WIS", tabular); break;
		case MDIO_C45_DEVTYPE_PCS: 		GenC45DevTypeString(frame, display_base, "PCS", tabular); break;
		case MDIO_C45_DEVTYPE_PHY_XS: 	GenC45DevTypeString(frame, display_base, "PHY XS", tabular); break;
		case MDIO_C45_DEVTYPE_DTE_XS: 	GenC45DevTypeString(frame, display_base, "DTE XS", tabular); break;
		case MDIO_C45_DEVTYPE_OTHER: 	GenC45DevTypeString(frame, display_base, "Other", tabular); break;
		case MDIO_TA: 					GenTAString(frame, display_base); break;
		case MDIO_C22_DATA: 			GenC22DataString(frame, display_base, tabular); break;
		case MDIO_C45_ADDRDATA: 		GenC45AddrDataString(frame, display_base, tabular); break;
		case MDIO_UNKNOWN:				GenUnknownString(tabular); break;
    }
}

void MDIOAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& /*channel*/, DisplayBase display_base )
{
	GenTabularText(frame_index, display_base, false);
}

// export_type_user_id is needed if we have more than one export type
void MDIOAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 /*export_type_user_id*/ )
{
	
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Packet ID,MDIOClause,OP,PHYADDR,REGADDR/DEVTYPE,ADDR/DATA" << std::endl;
	
	U64 num_frames = GetNumFrames();
	U64 num_packets = GetNumPackets();
	
	for( U32 packet_id=0; packet_id < num_packets; ++packet_id )
	{
		
		// get the frames contained in packet with index packet_id
		U64 first_frame_id;
		U64 last_frame_id;
		GetFramesContainedInPacket( packet_id, &first_frame_id, &last_frame_id );
		
		U64 frame_id = first_frame_id;
		
		// get MDIO_START frame to get the MDIOClause column value
		Frame frame = GetFrame( frame_id );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		// Time [s] and Packet ID column
		file_stream << time_str << "," << packet_id << ",";
		
		if (frame.mType == MDIO_C22_START) 
		{
			file_stream << "22,";
		}
		else if (frame.mType == MDIO_C45_START) 
		{
			file_stream << "45,";
		}
		else 
		{
			file_stream << ",";
		}

		++frame_id;

		if (frame_id > last_frame_id)
			continue;
		
		// OP frame
		frame = GetFrame( frame_id );
		
		switch (frame.mType) 
		{
			case MDIO_OP_W: 				file_stream << "Write,"; break;
			case MDIO_OP_R: 				file_stream << "Read,"; break;
			case MDIO_C45_OP_ADDR: 			file_stream << "Address,"; break;
			case MDIO_C45_OP_READ_INC_ADDR: file_stream << "Read +Addr,"; break;
			default: 						file_stream << ","; break;
		}

		++frame_id;
		
		if (frame_id > last_frame_id)
			continue;
		
		// PHYADDR frame
		frame = GetFrame( frame_id );
		
		if (frame.mType != MDIO_PHYADDR) 
		{
			file_stream << ",";
		}
		
		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 5, number_str, 128 );
		
		file_stream << number_str << ",";
		
		++frame_id;
		
		if (frame_id > last_frame_id)
			continue;
		
		// REGADR or DEVTYPE frame
		frame = GetFrame( frame_id );

		char number_str2[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 5, number_str2, 128 );
		
		switch (frame.mType) 
		{
			case MDIO_C22_REGADDR: 			file_stream << number_str2 << ","; break;
			case MDIO_C45_DEVTYPE_RESERVED:	file_stream << number_str2 << "(Reserved),"; break;
			case MDIO_C45_DEVTYPE_PMD_PMA:	file_stream << number_str2 << "(PMD/PMA),"; break;
			case MDIO_C45_DEVTYPE_WIS: 		file_stream << number_str2 << "(WIS),"; break;
			case MDIO_C45_DEVTYPE_PCS:		file_stream << number_str2 << "(PCS),"; break;
			case MDIO_C45_DEVTYPE_PHY_XS:	file_stream << number_str2 << "(PHY XS),"; break;
			case MDIO_C45_DEVTYPE_DTE_XS:	file_stream << number_str2 << "(DTE XS),"; break;
			case MDIO_C45_DEVTYPE_OTHER:	file_stream << number_str2 << "(Other),"; break;
			default:						file_stream << ","; 
		}
		
		++frame_id;
		
		if (frame_id > last_frame_id)
			continue;
		
		// MDIO_C22_DATA or MDIO_C45_ADDRDATA frame
		frame = GetFrame( frame_id );
		
		if (frame.mType == MDIO_C22_DATA or frame.mType == MDIO_C45_ADDRDATA) 
		{
			char number_str[128];
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 16, number_str, 128 );
			file_stream << number_str;
		}
		else 
		{
			file_stream << ",";
		}
	
		file_stream << std::endl;
	
		// check for cancel and update progress
		if( UpdateExportProgressAndCheckForCancel( packet_id, num_packets ) )
		{
			return;
		}
	
	}

	UpdateExportProgressAndCheckForCancel( num_packets, num_packets );
	// std::ofstream closes when goes out of scope (RAII)
}

void MDIOAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
	GenTabularText(frame_index, display_base, true);
}

void MDIOAnalyzerResults::GeneratePacketTabularText( U64 /*packet_id*/, DisplayBase /*display_base*/ )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}

void MDIOAnalyzerResults::GenerateTransactionTabularText( U64 /*transaction_id*/, DisplayBase /*display_base*/ )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}