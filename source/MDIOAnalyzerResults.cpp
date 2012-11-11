#include "MDIOAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "MDIOAnalyzer.h"
#include "MDIOAnalyzerSettings.h"
#include <iostream>
#include <fstream>
#include <sstream>

// TODO: check length of generated strings

MDIOAnalyzerResults::MDIOAnalyzerResults( MDIOAnalyzer* analyzer, MDIOAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

MDIOAnalyzerResults::~MDIOAnalyzerResults()
{
}

void MDIOAnalyzerResults::GenStartString(const Frame & frame, bool tabular=false) 
{
	if ( (frame.mFlags & MDIO_FLAG_C22_START) != 0 ) 
	{
		if (tabular) 
		{  
			AddResultString( "START C22 Packet (01)" );
		}
		else {
			AddResultString( "S C22" );
			AddResultString( "Start C22 Packet (01)" );
		}
	} 
	else if ( (frame.mFlags & MDIO_FLAG_C45_START) != 0 ) 
	{
		if (tabular) 
		{
			AddResultString( "START C45 Packet (00)" );
		}
		else 
		{
			AddResultString( "S C45" );
			AddResultString( "START C45 Packet (00)" );
		}
	} 
	else 
	{
		// error
	}	
}

void MDIOAnalyzerResults::GenOpString(const Frame & frame, bool tabular=false) 
{
	if ( (frame.mFlags & MDIO_FLAG_OP_W) != 0 ) 
	{
		if (tabular) 
		{
			AddResultString( "OPCODE [Write] (01)" );
		}
		else 
		{
			AddResultString( "OP[W]" );
			AddResultString( "OP[Write]" );
			AddResultString( "OPCODE [Write] (01)" );
		}
	} 
	else if ( (frame.mFlags & MDIO_FLAG_OP_C45_ADDR) != 0 ) 
	{
		if (tabular) 
		{
			AddResultString( "OPCODE [Address] (00)" );
		}
		else 
		{
			AddResultString( "OP[A]" );
			AddResultString( "OP[Addr]" );
			AddResultString( "OPCODE [Address] (00)" );
		}
	} 
	else if ( (frame.mFlags & MDIO_FLAG_OP_C22_R ) != 0 )
	{
		if (tabular) 
		{
			AddResultString( "OPCODE [Read] (10)" );
		}
		else 
		{
			AddResultString( "OP[R]" );
			AddResultString( "OP[Read]" );
			AddResultString( "OPCODE [Read] (10)" );
		}
	}	
	else if ( (frame.mFlags & MDIO_FLAG_OP_C45_R ) != 0 )
	{
		if (tabular) 
		{
			AddResultString( "OPCODE [Read] (11)" );
		}
		else 
		{
			AddResultString( "OP[R]" );
			AddResultString( "OP[Read]" );
			AddResultString( "OPCODE [Read] (11)" );
		}
	}	

	else if ( (frame.mFlags & MDIO_FLAG_OP_C45_READ_INCADDR) != 0 )
	{
		if (tabular) 
		{
			AddResultString( "OPCODE [Read-Increment-Address] (10)" );
		}
		else 
		{
			AddResultString( "OP[R +A]" );
			AddResultString( "OP[Read +Addr]" );
			AddResultString( "OPCODE [Read-Increment-Address] (10)" );
		}
	}
}

void MDIOAnalyzerResults::GenPhyAddrString(const Frame & frame, DisplayBase display_base, bool tabular=false) 
{
	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 5, number_str, 128 );

	if (tabular) 
	{
		std::stringstream ss;
		ss << "PHY Address [" << number_str << "]";
		AddResultString( ss.str().c_str() );
		return;
	}
	
	AddResultString( "PHY" );

	std::stringstream ss; 
	ss << "PHY[" << number_str << "]";
	AddResultString( ss.str().c_str() );
	
	ss.str("");
	ss << "PHYADR[" << number_str << "]";
	AddResultString( ss.str().c_str() );
}

void MDIOAnalyzerResults::GenC22RegAddrString(const Frame & frame, DisplayBase display_base, bool tabular=false) 
{
	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 5, number_str, 128 );
	
	if (tabular) 
	{
		std::stringstream ss;
		ss << "Register Address [" << number_str << "]";
		AddResultString( ss.str().c_str() );
		return;
	}
	
	AddResultString( "REG" );
	
	std::stringstream ss; 
	ss << "REG[" << number_str << "]";
	AddResultString( ss.str().c_str() );
	
	ss.str("");
	ss << "REGADR[" << number_str << "]";
	AddResultString( ss.str().c_str() );
}

void MDIOAnalyzerResults::GenC45DevTypeString(const Frame & frame, DisplayBase display_base, bool tabular=false) 
{
	// TODO: add number_str
	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 5, number_str, 128 );
	
	if (!tabular) 
	{
		AddResultString( "DEV" );
	}
	
	if( (frame.mFlags & MDIO_FLAG_DEVTYPE_RESERVED) != 0 ) 
	{
		std::stringstream ss;
		ss << "DEVTYPE [Reserved] - " << number_str;
		if (tabular) 
		{
			AddResultString( ss.str().c_str() );
		}
		else 
		{
			AddResultString( "DEVTYPE[Reserved]" );
			AddResultString( ss.str().c_str() );
		}
	}
	else if ( (frame.mFlags & MDIO_FLAG_DEVTYPE_PMD_PMA) != 0 ) 
	{
		std::stringstream ss;
		ss << "DEVTYPE [PMD/PMA] - " << number_str;
		if (tabular) 
		{
			AddResultString( ss.str().c_str() );
		}
		else 
		{
			AddResultString( "DEVTYPE[PMD/PMA]" );
			AddResultString( ss.str().c_str() );
		}
	}
	else if ( (frame.mFlags & MDIO_FLAG_DEVTYPE_WIS) != 0 ) 
	{
		std::stringstream ss;
		ss << "DEVTYPE [WIS] - " << number_str;
		if (tabular) 
		{
			AddResultString( ss.str().c_str() );
		}
		else 
		{
			AddResultString( "DEVTYPE[WIS]" );
			AddResultString( ss.str().c_str() );
		}
	}
	else if ( (frame.mFlags & MDIO_FLAG_DEVTYPE_PCS) != 0 )
	{
		std::stringstream ss;
		ss << "DEVTYPE [PCS] - " << number_str;
		if (tabular) 
		{
			AddResultString( ss.str().c_str() );
		}
		else 
		{
			AddResultString( "DEVTYPE[PCS]" );
			AddResultString( ss.str().c_str() );
		}
	}
	else if ( (frame.mFlags & MDIO_FLAG_DEVTYPE_PHY_XS) != 0 )
	{
		std::stringstream ss;
		ss << "DEVTYPE [PHY XS] - " << number_str;
		if (tabular) 
		{
			AddResultString( ss.str().c_str() );
		}
		else 
		{
			AddResultString( "DEVTYPE[PHY XS]" );
			AddResultString( ss.str().c_str() );
		}
	} 
	else if ( (frame.mFlags & MDIO_FLAG_DEVTYPE_DTE_XS) != 0 )
	{
		std::stringstream ss;
		ss << "DEVTYPE [DTE XS] - " << number_str;
		if (tabular) 
		{
			AddResultString( ss.str().c_str() );
		}
		else 
		{
			AddResultString( "DEVTYPE[DTE XS]" );
			AddResultString( ss.str().c_str() );
		}
	}
	else
	{
		AddResultString( "DEVTYPE[Unknown!]" );
	}

}

void MDIOAnalyzerResults::GenTAString(const Frame & frame, DisplayBase display_base) 
{
	AddResultString( "TA" );
}

void MDIOAnalyzerResults::GenC22DataString(const Frame & frame, DisplayBase display_base, bool tabular=false) 
{
	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 16, number_str, 128 );
	
	if (tabular) 
	{
		std::stringstream ss; 
		ss << "DATA [" << number_str << "]";
		AddResultString( ss.str().c_str() );
		return;
	}
	
	AddResultString( "D" );
	
	std::stringstream ss; 
	ss << "D[" << number_str << "]";
	AddResultString( ss.str().c_str() );
	
	ss.str(""); 
	ss << "DATA [" << number_str << "]";
	AddResultString( ss.str().c_str() );
	
}

void MDIOAnalyzerResults::GenC45AddrDataString(const Frame & frame, DisplayBase display_base, bool tabular=false) 
{
	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 16, number_str, 128 );
	
	if (tabular) 
	{
		std::stringstream ss; 
		ss << "ADDR/DATA [" << number_str << "]";
		AddResultString( ss.str().c_str() );
		return;
	}
	
	AddResultString( "A/D" );
	
	std::stringstream ss; 
	ss << "A/D[" << number_str << "]";
	AddResultString( ss.str().c_str() );
	
	ss.str(""); 
	ss << "ADDR/DATA [" << number_str << "]";
	AddResultString( ss.str().c_str() );	
}

void MDIOAnalyzerResults::GenUnknownString() 
{
	AddResultString("Unknown");
}

void MDIOAnalyzerResults::GenTabularText(U64 frame_index, DisplayBase display_base, bool tabular=false) 
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );
    
    switch (frame.mType) 
	{
      case MDIO_START:          GenStartString(frame, tabular); break;
      case MDIO_OP:             GenOpString(frame, tabular); break;
      case MDIO_PHYADDR:        GenPhyAddrString(frame, display_base, tabular); break;
      case MDIO_C22_REGADDR:    GenC22RegAddrString(frame, display_base, tabular); break;
      case MDIO_C45_DEVTYPE:    GenC45DevType(frame, display_base, tabular); break;
      case MDIO_TA:             GenTAString(frame, display_base); break;
      case MDIO_C22_DATA:       GenC22DataString(frame, display_base, tabular); break;
      case MDIO_C45_ADDRDATA:   GenC45AddrDataString(frame, display_base, tabular); break;
      default: 					GenUnknownString();
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
		
		if (frame.mType != MDIO_START) 
		{
			// error
		}
		
		if ( (frame.mFlags & MDIO_FLAG_C22_START) != 0) 
		{
			file_stream << "22,";
		}
		if ( (frame.mFlags & MDIO_FLAG_C22_START) != 0) 
		{
			file_stream << "45,";
		}

		++frame_id;

		if (frame_id > last_frame_id)
			continue;
		
		// OP frame
		frame = GetFrame( frame_id );
		
		if (frame.mType != MDIO_OP) 
		{
			file_stream << ",";
		}
		
		if ( (frame.mFlags & MDIO_FLAG_OP_W) != 0 ) 
		{
			file_stream << "Write,";
		} 
		else if ( (frame.mFlags & MDIO_FLAG_OP_C45_ADDR) != 0 ) 
		{
			file_stream << "Address,";
		} 
		else if ( (frame.mFlags & MDIO_FLAG_OP_C22_R ) != 0 or (frame.mFlags & MDIO_FLAG_OP_C45_R ) != 0 )
		{
			file_stream << "Read,";
		}	
		else if ( (frame.mFlags & MDIO_FLAG_OP_C45_READ_INCADDR) != 0 )
		{
			file_stream << "Read +Addr,";
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
		
		// TODO add the string instead of the DEVTYPE (i.e. Reserved, PMD/PMA, etc.)
		if (frame.mType == MDIO_C22_REGADDR or frame.mType == MDIO_C45_DEVTYPE) 
		{
			char number_str[128];
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 5, number_str, 128 );
			file_stream << number_str << ",";
		}
		else 
		{
			file_stream << ",";
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
			// 
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