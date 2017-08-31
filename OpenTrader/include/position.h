// Position.h: interface for the CPosition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSITION_H__4D926F4A_551E_4BC3_9935_18D519DF7DEA__INCLUDED_)
#define AFX_POSITION_H__4D926F4A_551E_4BC3_9935_18D519DF7DEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "indicator.h"

#define POS_BUY				1
#define POS_SELL			2
#define POS_CLOSE			3

#ifdef _WIN32
#define  POSITION_REPORT_FILE "c:\\fxData\\PositionsReport.txt"
#else
#define  POSITION_REPORT_FILE "PositionsReport.txt"
#endif


typedef struct 
{
	long	lTicket;
	int		iPosType;
	int		iPosFrame;
	float  dOpenPrice;
	float  dClosePrice;
	float  dTP;
	float  dSL;
}positionData;

#define INIT_DEPOSIT		1*1000


typedef struct  
{
	long lProfits[500];
	int iCount;
}TradeSeq;

typedef struct 
{
	int			iTotalTrades;
	int			iBuyTrades;
	int			iSellTrades;
	int			iSuccess;
	int			iFail;
	int			iSuccessBuy;
	int			iSuccessSell;
	int			iFailBuy;
	int			iFailSell;
	int			iSuccessRaio;
	long			lProfitPips;
	int			iCloseWithTP;
	int			iCloseWithSL;
	int			iNormalClose;
	long			lBalance;				
	long			iLargestWin;				
	long			iLargestLoss;				
	long			lTotalWin;				
	long			lTotalLose;				
	long			lAveWin;				
	long			lAveLose;				
	long			lTimeProfit;				
    long                        lWaightedProfit;
	long			lProfitByRisk;
	TradeSeq		Profites;
}positionsReport;



class CPosition
{
protected:
	positionData		m_pos[20];
	TradeSeq		m_sep;
	positionsReport		m_report;
	CIndicator*		m_pIndicator;
	std::ofstream		m_file;
    int                         m_iTradeNo;
public:
	int			m_iIndex;
	bool			m_bLog;
	bool			m_bStock;
	void			Log(const char* i_lpszData,...) ;
	CPosition();
	CPosition(CIndicator * pInd,bool bLog=false);
	virtual ~CPosition();
	long OpenPosition(int iPosType,int iItem,float dPrice,float dTP,float dSL);
	long ClosePosition(long lTicket,int iItem,int iPriceType);
	void GetIndicator(CIndicator* pInd){m_pIndicator = pInd;};
	int Tick(int iCurItem);
	void reset(void);
	void GetReport(	positionsReport* pReport);
	int OrdersTotal(void);
	int OrderType(int iTicket);
	long ModifyPosition(int lTicket,float dTP,float dSL);
	long GetEquity(long lTicket,int iItem,int iPriceType);

};

#endif // !defined(AFX_POSITION_H__4D926F4A_551E_4BC3_9935_18D519DF7DEA__INCLUDED_)
