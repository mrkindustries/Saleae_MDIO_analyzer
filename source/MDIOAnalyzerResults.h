#ifndef MDIO_ANALYZER_RESULTS
#define MDIO_ANALYZER_RESULTS

#include <AnalyzerResults.h>

// Flags for MDIO_START frame type
#define MDIO_FLAG_C45_START ( 1 << 0 )
#define MDIO_FLAG_C22_START ( 1 << 1 )

// Flags for MDIO_OP frame type
#define MDIO_FLAG_OP_W ( 1 << 0 )
#define MDIO_FLAG_OP_C45_ADDR ( 1 << 1 )
#define MDIO_FLAG_OP_C22_R ( 1 << 2 )
#define MDIO_FLAG_OP_C45_R ( 1 << 3 )
#define MDIO_FLAG_OP_C45_READ_INCADDR ( 1 << 4 )

// Flags for MDIO_C45_DEVTYPE frame type
#define MDIO_FLAG_DEVTYPE_RESERVED ( 1 << 0 )
#define MDIO_FLAG_DEVTYPE_PMD_PMA ( 1 << 1 )
#define MDIO_FLAG_DEVTYPE_WIS ( 1 << 2 )
#define MDIO_FLAG_DEVTYPE_PCS ( 1 << 3 )
#define MDIO_FLAG_DEVTYPE_PHY_XS ( 1 << 4 )
#define MDIO_FLAG_DEVTYPE_DTE_XS ( 1 << 5 )

class MDIOAnalyzer;
class MDIOAnalyzerSettings;

enum MDIOFrameType { MDIO_START = 0, MDIO_OP, MDIO_PHYADDR, MDIO_C22_REGADDR, 
                     MDIO_C45_DEVTYPE, MDIO_TA, MDIO_C22_DATA, MDIO_C45_ADDRDATA };

class MDIOAnalyzerResults : public AnalyzerResults
{
public:
	MDIOAnalyzerResults( MDIOAnalyzer* analyzer, MDIOAnalyzerSettings* settings );
	virtual ~MDIOAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

	void GenTabularText(U64 frame_index, DisplayBase display_base, bool tabular); 

    void GenStartString(const Frame & frame, bool tabular);
	void GenOpString(const Frame & frame, bool tabular); 
    void GenPhyAddrString(const Frame & frame, DisplayBase display_base, bool tabular); 
    void GenC22RegAddrString(const Frame & frame, DisplayBase display_base, bool tabular);
	void GenC45DevTypeString(const Frame & frame, DisplayBase display_base, bool tabular);
	void GenC45DevType(const Frame & frame, DisplayBase display_base, bool tabular); 
    void GenTAString(const Frame & frame, DisplayBase display_base); 
    void GenC22DataString(const Frame & frame, DisplayBase display_base, bool tabular); 
    void GenC45AddrDataString(const Frame & frame, DisplayBase display_base, bool tabular); 
    void GenUnknownString();

protected:  //vars
	MDIOAnalyzerSettings* mSettings;
	MDIOAnalyzer* mAnalyzer;
};

#endif //MDIO_ANALYZER_RESULTS
