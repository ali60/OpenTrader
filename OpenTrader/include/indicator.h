// Indicator.h: interface for the CIndicator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INDICATOR_H__E7E497C8_BCBC_45F9_893D_F6661BE01ABC__INCLUDED_)
#define AFX_INDICATOR_H__E7E497C8_BCBC_45F9_893D_F6661BE01ABC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MODE_SMA			1
#define MODE_EMA			2
#define MODE_LWMA			3
#define MODE_SSMA			4

#define PRICE_CLOSE			1
#define PRICE_OPEN			2
#define PRICE_HIGH			3
#define PRICE_LOW			4
#define PRICE_MIDDLE		5

#define METHOD_MAIN			1
#define METHOD_SIGNAL		2

#define TF_M1				1			
#define TF_M5				5
#define TF_M15				15
#define TF_M30				30
#define TF_H1				60
#define TF_H4				240
#define TF_D1				1440
#define TF_W1				1440*7


#define IND_MA_COUNT		80

struct LastItem
{
	float	ItemO;
	float	ItemC;
	float	ItemH;
	float	ItemL;
	int		period;
};

struct PivotData
{
	float R3;
	float R2;
	float R1;
	float pivod;
	float S3;
	float S2;
	float S1;
};

typedef struct 
{
	char		szDate[9];
	char		szTime[6];
	float		dHigh;
	float		dLow;
	float		dOpen;
	float		dClose;
	int			iVoldume;
	float		dData1;
	float		dData2;
	float		dMa[IND_MA_COUNT+1];
	float		dMaC[IND_MA_COUNT+1];
	float		dMaH[IND_MA_COUNT+1];
	float		dMaL[IND_MA_COUNT+1];
	float		dRSI[70];
	float		dKSto[70];
	float		dHighet[70];
	float		dLowest[70];
}MarketData;


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


class CIndicator  
{
	float m_dVal;

protected:
	char		m_date[16];
	LastItem	m_last[250];
	float		m_lastMACD[100];
	int			m_iLastsCount;
	int			m_iLastsCountMACD;
	float		m_prevGain,m_prevLoss;
	char		m_szDateTime[20];
    char        m_BegTime[16],m_EndTime[16];
	MarketData	m_record;
public:
	std::string	m_strIndexName;
	BYTE*		m_pData;
	int			m_iBeginItems,m_iEndItems;
	bool		m_bJBP;
	bool		m_bStock;

	float preEMA(int iItem,int iPriod,int iType,int iPrice);
	void PrepareIndicatorAlldata(void);
	float GetLast(int iPeriod,int iType);
	int GetBar(char* lpszDate,char* lpszTime);
	void ShiftBeginDate(int iShift);
	void SetLast(float Item , int iPeriod,int iType);
	float GetLastMACD(int iPeriod);
	void SetLastMACD(float Item , int iPeriod);
	int GetTF(void);
	int GetPrevDay(int iItem,float &dClose,float &dHigh,float &dLow);
	int m_count;
	void SetBeginDate(const char* i_date);
	void SetEndDate(const char* i_date);
	void ResetIndDate(void){m_iBeginItems =0;m_iEndItems=m_count;}

	int GetNextDayIndex(int iItem);


    char* GetBeginDate(){return m_BegTime;}
	char* GetEndDate(){return m_EndTime;}
	void GetData(BYTE* pData,int Items);
	int GetCount() {return (m_iEndItems - m_iBeginItems);}
	bool IsValid(long);
	void ResetIndicator(void);
	CIndicator();
	virtual ~CIndicator();
	float iOpen(long i);
	float GetData1(long i);
	float GetData2(long i);
	float GetMa(long i,int iPeriod,int iPrice=PRICE_OPEN);
	float GetRSI(long i,int iPeriod);
	float iClose(long i,bool bLog=false);
	float iHigh(long i);
	float iLow(long i);
	void GetRecord(int item,MarketData* o_record);
	int iValume(long i);
	float iHighestHigh(long i,int iPeriod);
	float iLowestLow(long i,int iPeriod);

	float iHighFractal(long i,int iPeriod,int iType);
	float iLowFractal(long i,int iPeriod,int iType);
	float  iValumePercent(long iItem,int iPeriod);
	void iDateTime(long i,char* lpszDateTime);
	long iTime(long i);
	float GetPrice(int iItem ,int iPriceType);
	long iDate(long i);
	float iMA(int iItem,int iPriod,int iType,int iPrice);
	float iRSI(int iItem,int iPeriod,int iPrice);
	float Stochastic(int iItem,int iKPeriod,int iDPeriod,int iMethod=METHOD_MAIN, int iMaType=MODE_SMA)
	{
		return 0;
	}
	float GetStochastic_K(long i, int iPeriod);

	float LinearRegression(int iItem,int iPeriod,int iPrice);
 	float iMACD(int iItem,int fast_ema_period, int slow_ema_period, int signal_period, int iPrice, int mode);
	float iMAOnArray(float* i_array,int i_len,int iPeriod,int iMaMethod,int iShift);
	float HAS(int iItem,int iMaType1,int iPeriod1,int iMaType2,int iPeriod2);
	float PivotPoint(int iItem,PivotData* pData);
	float BearPercent(long iItem,int iPeriod);
	float BullPercent(long iItem,int iPeriod);
	bool   GetCurRecord(int iItem,MarketData* pRecord=NULL);
	float StochasticK(int iItem,int iKPeriod,int iSlowing=3);
	float StochasticD(int iItem,int iKPeriod,int iDPeriod,int iSlowing=3);
	float StochasticKFast(int iItem,int iKPeriod);


	void	ReloadMACD(int iMa1,int iMa2,int iMa3);
	int	   GetThisDay(int iItem,float &dOpen,float &dClose,float &dHigh,float &dLow);
	int	   GetThisDayUntilTime(int iItem,int iTime,float &dOpen,float &dClose,float &dHigh,float &dLow);
	float WilliamR(int iItem,int iKPeriod);
	int   Sync(CIndicator* pInd,int item,bool bJustDate=true);
	time_t iCTime(long i);
	int GetThisDayUntilTime2(int iItem,int iEndTime,float &dHigh,float &dLow);
	float GetAppriximateClose(long item,int iCount);
	float _R(float x);
	int		m_iTF;
	float	m_spread;
	int	m_kValue;

	
};

time_t MKTimestamp(const char* szDate,const char* szTime);



#endif // !defined(AFX_INDICATOR_H__E7E497C8_BCBC_45F9_893D_F6661BE01ABC__INCLUDED_)

