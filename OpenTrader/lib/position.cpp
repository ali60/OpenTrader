// Position.cpp: implementation of the CPosition class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "position.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using std::ofstream;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPosition::CPosition()
{
	reset();
	m_bLog = false;
	m_bStock = false;
}

CPosition::CPosition(CIndicator * pInd,bool bLog )
{
	reset();
	m_pIndicator = pInd;
	m_bLog = bLog;
	if(bLog)
	{
                m_file.open(POSITION_REPORT_FILE,std::ofstream::out);
		if(!m_file.good())
		{
			ASSERT(0);
		}

	}
}

CPosition::~CPosition()
{
	if(m_bLog)
	m_file.close();

}

long CPosition::OpenPosition(int iPosType,int iItem,float dPrice,float dTP,float dSL)
{
	ASSERT(m_iIndex<20);
	ASSERT(m_iIndex<5);
	if(iPosType == POS_SELL && m_bStock)
		return 0;
	m_pos[m_iIndex].lTicket		= m_iIndex+1;
	m_pos[m_iIndex].iPosType	= iPosType;
	m_pos[m_iIndex].dOpenPrice	= dPrice;
	m_pos[m_iIndex].dTP	= dTP;
	m_pos[m_iIndex].dSL	= dSL;
	m_pos[m_iIndex].dClosePrice	= 0;
	m_pos[m_iIndex].iPosFrame	= iItem;
	if(iPosType == POS_BUY)
	{
		m_report.iBuyTrades++;
	}
	else
		m_report.iSellTrades++;
	if(m_bLog)
	{
		char sz[20];
		m_pIndicator->iDateTime(iItem,sz);
		if(iPosType == POS_BUY)
			Log("[%s] Buy@%f TP=%f SL=%f",sz,m_pos[m_iIndex].dOpenPrice,dTP,dSL);
		else
			Log("[%s] Sell@%f TP=%f SL=%f",sz,m_pos[m_iIndex].dOpenPrice,dTP,dSL);
	}
	m_iIndex++;
	return m_pos[m_iIndex-1].lTicket;	
}


long CPosition::ModifyPosition(int lTicket,float dTP,float dSL)
{
	ASSERT(m_iIndex<20);
	int i;
	for(i=0;i<m_iIndex;i++)
		if(m_pos[i].lTicket	== lTicket)
			break;
	if(i==20 || m_iIndex==0 || i==m_iIndex)
		return 0;
//	m_pos[m_iIndex].lTicket		= m_iIndex+1;
//	m_pos[i].dClosePrice = dTP;
	if(dTP)
 		m_pos[i].dTP	= dTP;
	if(dSL>0)
 		m_pos[i].dSL	= dSL;
	return m_pos[i].lTicket;	
}

long CPosition::ClosePosition(long lTicket,int iItem,int iPriceType)
{
	ASSERT(m_iIndex<20);
	int i;
	for(i=0;i<m_iIndex;i++)
		if(m_pos[i].lTicket	== lTicket)
			break;
	if(i==20 || m_iIndex==0 || i==m_iIndex)
		return 0;
	if(!m_pos[i].dClosePrice)
	{
		if(!m_pIndicator->IsValid(iItem))
			return 0;
		m_pos[i].dClosePrice	= m_pIndicator->GetPrice(iItem,iPriceType);
		m_report.iNormalClose++;
	}
	float profit;
	if(m_pos[i].iPosType == POS_BUY)
	{
//		m_pos[i].dOpenPrice += m_pIndicator->m_spread;
		profit = m_pos[i].dClosePrice - m_pos[i].dOpenPrice;
		if(m_bStock)
			profit *= (float)0.95;
	}
	if(m_pos[i].iPosType == POS_SELL)
	{
//		m_pos[i].dClosePrice += m_pIndicator->m_spread;
		profit = m_pos[i].dOpenPrice - m_pos[i].dClosePrice;
	}
//	profit -= m_pIndicator->m_spread;
	int pips = (int)roundf(profit * (float)m_pIndicator->m_kValue);
	pips -= (int)(m_pIndicator->m_spread * ((float)m_pIndicator->m_kValue));
	m_report.lProfitPips += pips;
	m_report.lProfitByRisk += (long)((float)m_report.lProfitByRisk * (float)pips*0.001);
    m_report.lWaightedProfit += (pips*m_iTradeNo);
	if(pips>=0)
	{
		m_report.iSuccess++;
		if(m_pos[i].iPosType  == POS_BUY)
			m_report.iSuccessBuy++;
		else
			m_report.iSuccessSell++;
		m_report.lTotalWin += pips;
		m_report.lAveWin = m_report.lTotalWin / m_report.iSuccess; 
	}
	else
	{
		if(m_pos[i].iPosType  == POS_BUY)
			m_report.iFailBuy++;
		else
			m_report.iFailSell++;
		m_report.iFail ++;
		m_report.lTotalLose += (-pips);
		m_report.lAveLose = m_report.lTotalLose / m_report.iFail; 
	}
	m_report.iTotalTrades++;
	m_report.iSuccessRaio = (int)(((float)m_report.iSuccess / (float)m_report.iTotalTrades)*100.00);
//	m_report.lBalance += ((float)pips * 0.914);
	m_report.lBalance += (pips );
	m_report.lTimeProfit += (pips*iItem );
	if(m_bLog)
	{
		char sz[20];
		m_pIndicator->iDateTime(iItem,sz);
		if(m_pos[i].iPosType  == POS_BUY)
			Log("[%s] CloseBuy@%f Price=%f profit=%d Balance=%d BPByR=%d",sz,m_pos[i].dClosePrice,profit,pips,m_report.lBalance,m_report.lProfitByRisk);
		else
			Log("[%s] CloseSell@%f Price=%f profit=%d Balance=%d BPByR=%d",sz,m_pos[i].dClosePrice,profit, pips,m_report.lBalance,m_report.lProfitByRisk);
	}
	if(m_report.lBalance > m_report.iLargestWin)
		m_report.iLargestWin = m_report.lBalance;
	if(m_report.lBalance < m_report.iLargestLoss )
		m_report.iLargestLoss = m_report.lBalance;
	if( (i==0) &&  (m_iIndex==2))
		memcpy(&m_pos[0],&m_pos[1],sizeof(positionData));

	//sequence of trades
	if(m_report.Profites.iCount<sizeof(m_report.Profites.lProfits)/sizeof(long))
	{
		m_report.Profites.lProfits[m_report.Profites.iCount]=pips;
		m_report.Profites.iCount++;

	}
	m_iIndex--;
    m_iTradeNo++;
	return pips;	
}

int CPosition::Tick(int iCurItem)
{
	int iRet= 0; 
	int count = OrdersTotal();
	for(int i=0;i<count;i++)
	{
		if(m_pos[i].iPosType == POS_BUY)
		{
			
			if(m_pos[i].dTP && !m_pos[i].dClosePrice)
				if(m_pIndicator->iHigh(iCurItem) >= m_pos[i].dTP)
				{
					if(m_pos[i].iPosFrame == iCurItem)
					{
						float dOpen = m_pIndicator->iOpen(iCurItem);
						if(dOpen != m_pos[i].dOpenPrice)
						{
						//if bearish bar
							if(!( (dOpen  <= m_pos[i].dOpenPrice) && (m_pos[i].dOpenPrice <= m_pos[i].dTP)))
								continue;
						}
					}
					m_pos[i].dClosePrice = m_pos[i].dTP;
					m_report.iCloseWithTP++;
//					ClosePosition(m_pos[i].lTicket,iCurItem,0);
					if(m_bLog)
					{
						char sz[20];
						m_pIndicator->iDateTime(iCurItem,sz);
						Log("[%s] TP Hit@%f  ",sz,m_pos[i].dTP);
					}
					iRet= m_pos[i].lTicket;
				}
			if(m_pos[i].dSL && !m_pos[i].dClosePrice)
				if(m_pIndicator->iLow(iCurItem) <= m_pos[i].dSL)
				{
					if(m_pos[i].iPosFrame == iCurItem)
					{
						float dOpen = m_pIndicator->iOpen(iCurItem);
						if( dOpen != m_pos[i].dOpenPrice)
						{
							//if bearish bar
							if(!( (dOpen  >= m_pos[i].dOpenPrice) && (m_pos[i].dOpenPrice >= m_pos[i].dSL)))
								continue;
						}
					}
					m_pos[i].dClosePrice = m_pos[i].dSL;
					m_report.iCloseWithSL++;
//					ClosePosition(m_pos[i].lTicket,iCurItem,0);
					if(m_bLog)
					{
						char sz[20];
						m_pIndicator->iDateTime(iCurItem,sz);
						Log("[%s] SL Hit@%f  ",sz,m_pos[i].dSL);
					}
					iRet= m_pos[i].lTicket;
				}
		}
		else
		if(m_pos[i].iPosType == POS_SELL)
		{
			
			if(m_pos[i].dTP && !m_pos[i].dClosePrice)
				if(m_pIndicator->iLow(iCurItem) <= m_pos[i].dTP)
				{
					if(m_pos[i].iPosFrame == iCurItem)
					{
						float dOpen = m_pIndicator->iOpen(iCurItem);
						if( dOpen != m_pos[i].dOpenPrice)
						{
							//if bearish bar
							if(!( (dOpen  >= m_pos[i].dOpenPrice) && (m_pos[i].dOpenPrice >= m_pos[i].dTP)))
								continue;
						}
					}
					m_pos[i].dClosePrice = m_pos[i].dTP;
					m_report.iCloseWithTP++;
					if(m_bLog)
					{
						char sz[20];
						m_pIndicator->iDateTime(iCurItem,sz);
						Log("[%s] TP Hit@%f",sz,m_pos[i].dTP);
					}
					iRet= m_pos[i].lTicket;
//					ClosePosition(m_pos[i].lTicket,iCurItem,0);

				}
			if(m_pos[i].dSL && !m_pos[i].dClosePrice)
				if(m_pIndicator->iHigh(iCurItem) >= m_pos[i].dSL)
				{
					if(m_pos[i].iPosFrame == iCurItem)
					{
						float dOpen = m_pIndicator->iOpen(iCurItem);
						if( dOpen != m_pos[i].dOpenPrice)
						{
							//if bearish bar
							if(!( (dOpen  <= m_pos[i].dOpenPrice) && (m_pos[i].dOpenPrice <= m_pos[i].dSL)))
								continue;
						}
					}
					m_pos[i].dClosePrice = m_pos[i].dSL;
					m_report.iCloseWithSL++;
					if(m_bLog)
					{
						char sz[20];
						m_pIndicator->iDateTime(iCurItem,sz);
						Log("[%s] SL Hit@%f  ",sz,m_pos[i].dSL);
					}
					iRet= m_pos[i].lTicket;
//					ClosePosition(m_pos[i].lTicket,iCurItem,0);
				}
		}

	}
	return iRet;
}

void CPosition::reset(void)
{
	memset(&m_report,0,sizeof(m_report));
	m_report.lProfitByRisk =  INIT_DEPOSIT;
	m_report.iLargestLoss =  100000;
	m_report.iLargestWin =  -100000;
	m_iIndex = 0;
    m_iTradeNo = 1;
	memset(m_pos,0,sizeof(m_pos));
}

void CPosition::GetReport(	positionsReport* pReport)
{
	if(m_report.iLargestLoss ==  100000)
		m_report.iLargestLoss=0;
	if(m_report.iLargestWin ==  -100000)
		m_report.iLargestWin = 0;
	memcpy(pReport ,&m_report,sizeof(m_report));
}

void CPosition::Log(const char* i_lpszData,...) 
{
	if(!m_bLog)
		return;
	va_list args;
	va_start(args, i_lpszData);
	char temp_str[500];
	vsprintf(temp_str,i_lpszData, args);	
	va_end(args);
	strcat(temp_str,"\n");
	m_file<<std::string(temp_str);
}

int CPosition::OrdersTotal(void)
{
	return m_iIndex;
}

int CPosition::OrderType(int iTicket)
{
	for(int i=0;i<m_iIndex;i++)
	{
		if(m_pos[i].lTicket == iTicket)
			return m_pos[i].iPosType;
	}
	return 0;
}


long CPosition::GetEquity(long lTicket,int iItem,int iPriceType)
{
	int i;
	ASSERT(m_iIndex<20);
	for( i=0;i<m_iIndex;i++)
		if(m_pos[i].lTicket	== lTicket)
			break;
	if(i==20 || m_iIndex==0 || i==m_iIndex)
		return 0;
	if(!m_pos[i].dClosePrice)
	{
		float dClose = 0;
		float profit=0;
		dClose	= m_pIndicator->GetPrice(iItem,iPriceType);
		if(m_pos[i].iPosType == POS_BUY)
		{
			profit = dClose - m_pos[i].dOpenPrice;
		}
		if(m_pos[i].iPosType == POS_SELL)
		{
			dClose += m_pIndicator->m_spread;
			profit = m_pos[i].dOpenPrice - dClose;
		}
		long pips = (int)(profit * (float)m_pIndicator->m_kValue);
		return pips;
	}
	return 0;
}
