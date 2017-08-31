// Indicator.cpp: implementation of the CIndicator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "indicator.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=46AC337901ED
CIndicator::CIndicator()
{
	m_count = 0;
	m_pData = NULL;
	m_iEndItems = m_iLastsCount = m_iBeginItems = m_iLastsCountMACD = 0;
	m_prevGain = m_prevLoss = 0;
	m_dVal=0;
	m_bJBP=false;
	m_bStock=false;
}

//##ModelId=46AC337901F9
CIndicator::~CIndicator()
{
	free(m_pData);
	m_pData=NULL;
}


float CIndicator::_R(float x)
{
	char sz[10];
	if(!m_bJBP)
		sprintf(sz,"%1.4f",x);
	else
		sprintf(sz,"%1.2f",x);

	return (float)atof(sz);
}

static long GetMinuts(char* lpszTime);


void CIndicator::ShiftBeginDate(int iShift)
{
	m_iBeginItems +=iShift;
	if(m_iBeginItems<0)
		m_iBeginItems = 0;
}

void CIndicator::SetBeginDate(const char* i_date)
{
	ASSERT(m_pData);
	MarketData record={0};
     strcpy(m_BegTime, i_date);
	long l1,l2;
	strcpy(m_date , i_date);
	char szDate[20] = {0};
	char szTime[20];
	int i;
	memcpy(szDate,i_date,8);
	strcpy(szTime,i_date+8);
	for(i=0;i<m_count;i++)
	{
		memcpy(&record,m_pData+i*sizeof(MarketData),sizeof(MarketData));
		l1=atol(record.szDate);
		l2=atol(szDate);
		if(l1>=l2)
		{
			record.szTime[2]=szTime[2]=0;
			l1=atol(record.szTime);
			l2=atol(szTime);
			break;
			if(l1>=l2)
				break;
		}
	}
//	if(i!=m_count)
		m_iBeginItems = i;
}

int CIndicator::GetBar(char* lpszDate,char* lpszTime)
{
	ASSERT(m_pData);
	MarketData record={0};
	long l1,l2,i;
	char szTime[20];
	sprintf(szTime,"%s",lpszTime);
	for(i=0;i<m_count;i++)
	{
		memcpy(&record,m_pData+i*sizeof(MarketData),sizeof(MarketData));
		l1=atol(record.szDate);
		l2=atol(lpszDate);
		if(l1>=l2)
		{
			record.szTime[2]=szTime[2]=0;
			l1=atol(record.szTime);
			l2=atol(szTime);
			break;
			if(l1>=l2)
				break;
		}
	}
	return i;
}


void CIndicator::SetEndDate(const char* i_date)
{
	ASSERT(m_pData);
	MarketData record={0};
	long l1,l2,i;
	strcpy(m_date , i_date);
	char szDate[20] = {0};
	char szTime[20];
	memcpy(szDate, i_date, 8);
	strcpy(szTime, i_date + 8);
	for(i=0;i<m_count;i++)
	{
		memcpy(&record,m_pData+i*sizeof(MarketData),sizeof(MarketData));
		l1=atol(record.szDate);
		l2=atol(szDate);
		if(l1>=l2)
		{
			record.szTime[2]=szTime[2]=0;
			l1=atol(record.szTime);
			l2=atol(szTime);
			break;
			if(m_iTF==1440)
				break;
			if(l1>=l2)
				break;
		}
	}
	m_iEndItems =  i;
    strcpy(m_EndTime , i_date);
}


void CIndicator::GetData(BYTE* pData, int Items)
{
	m_pData = pData;
	m_iEndItems = m_count = Items;

	m_iBeginItems = 0;
//	m_iTF = GetTF();
}


float CIndicator::iOpen(long i)
{
	MarketData record={0};
	int j=i+m_iBeginItems ;
	if(j<0 || j> m_count)
		return 0;
	ASSERT(j<m_count && j >=0);
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
//	TRACE("open Item(%d) %s %s  open=%f\n",i,record.szDate,record.szTime,record.dOpen);
	return record.dOpen;
}

float CIndicator::GetData1(long i)
{
	ASSERT(i<m_count && i >=0);
	MarketData record={0};
	int j=i+m_iBeginItems ;
	ASSERT(j<m_count);
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
//	TRACE("open Item(%d) %s %s  open=%f\n",i,record.szDate,record.szTime,record.dOpen);
	return record.dData1;
}

float CIndicator::GetData2(long i)
{
	ASSERT(i<m_count && i >=0);
	MarketData record={0};
	int j=i+m_iBeginItems ;
	ASSERT(j<m_count);
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
//	TRACE("open Item(%d) %s %s  open=%f\n",i,record.szDate,record.szTime,record.dOpen);
	return record.dData2;
}

float CIndicator::GetMa(long i,int iPeriod,int iPrice)
{
	MarketData record={0};
    if(iPeriod<=0) return 0;
	int j=i+m_iBeginItems ;
	if(j>=m_count || j<0)
		return 0;
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
//	TRACE("open Item(%d) %s %s  open=%f\n",i,record.szDate,record.szTime,record.dOpen);
	switch(iPrice) 
	{
	case PRICE_CLOSE:
		ASSERT( iPeriod < (sizeof(record.dMaC)/sizeof(float)));
		return record.dMaC[iPeriod-1];
		break;
	case PRICE_OPEN:
		ASSERT( iPeriod < (sizeof(record.dMa)/sizeof(float)));
		return record.dMa[iPeriod-1];
		break;
	case PRICE_HIGH:
		ASSERT( iPeriod < (sizeof(record.dMaH)/sizeof(float)));
		return record.dMaH[iPeriod-1];
		break;
	case PRICE_LOW:
		ASSERT( iPeriod < (sizeof(record.dMaL)/sizeof(float)));
		return record.dMaL[iPeriod-1];
		break;
	}
	return 0;
}

float CIndicator::GetAppriximateClose(long item,int iCount)
{
	float dSum=0;
	for(int i=1;i<=iCount;i++)
	{
		dSum += (iOpen(item-i)-iClose(item-i));
	}
	dSum /= (float)(iCount);
	return iOpen(item) + dSum;

}



long CIndicator::iTime(long i)
{
	ASSERT(i<m_count);
	MarketData record={0};
	int j=i+m_iBeginItems ;
	ASSERT(j<m_count);
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	sprintf(m_szDateTime,"%s%s",record.szDate,record.szTime);
	return GetMinuts(record.szTime);
}

long CIndicator::iDate(long i)
{
	ASSERT(i<m_count);
	MarketData record={0};
	char szDay[3]={0};
	int j=i+m_iBeginItems ;
	ASSERT(j<m_count);
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	strcpy(szDay,record.szDate+6);
	return atol(szDay);
}


void CIndicator::iDateTime(long i,char* lpszDateTime)
{
	ASSERT(i<m_count);
	MarketData record={0};
	int j=i+m_iBeginItems ;
	ASSERT(j<m_count);
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	sprintf(lpszDateTime,"%s %s",record.szDate,record.szTime);
	return;
}

bool CIndicator::IsValid(long i)
{
	int j=i+m_iBeginItems ;
	if(j<m_count && j >0)
		return true;
	return false;

}
#define GET_DT_ITEM(x,i) ((int)(x[i]-'0')*10+ (x[i+1]-'0'))

time_t MKTimestamp(const char* szDate, const char* szTime)
{
	time_t rawtime;
	struct tm * timeinfo;
	char temp[10] = { 0 };

	time(&rawtime);
	timeinfo = gmtime(&rawtime);
	memcpy(temp, szDate, 4); 
	timeinfo->tm_year = atoi(temp);
	timeinfo->tm_mon = GET_DT_ITEM(szDate,4);
	timeinfo->tm_mday = GET_DT_ITEM(szDate,6);
	if(szTime){
		timeinfo->tm_hour = GET_DT_ITEM(szTime,0 );
		timeinfo->tm_min = GET_DT_ITEM(szTime,2);
	}
	else
	{
		timeinfo->tm_hour = 0;
		timeinfo->tm_min = 0;

	}
	timeinfo->tm_sec = 0;
	timeinfo->tm_isdst = 0; // disable daylight saving time
	time_t ret = mktime(timeinfo);
	return ret;
}
time_t CIndicator::iCTime(long i)
{
	MarketData record={0};
	int j=i+m_iBeginItems ;
	ASSERT(j<m_count);
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	char dt[50] = { 0 };
	char tt[50] = { 0 };
	strcpy(dt,record.szDate);
	strcat(tt,record.szTime);
    time_t temp = MKTimestamp(dt,tt);  // t is now your desired time_t
    return temp;
}


//##ModelId=46AC337901FD
float CIndicator::iClose(long i,bool bLog)
{
	MarketData record={0};
	int j=i + m_iBeginItems ;
	if(j<0 || j> m_count)
		return 0;
	ASSERT(j<m_count);
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	if(bLog)
	{
		//TRACE("Item(%d) %s %s  close=%f\n",i,record.szDate,record.szTime,record.dClose);
	}
	return (record.dClose);
}


//##ModelId=46AC337901FD
int CIndicator::iValume(long i)
{
	MarketData record={0};
	int j=i + m_iBeginItems ;
	ASSERT(j<m_count);
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	return (record.iVoldume);
}

//##ModelId=46AC337901FF
float CIndicator::iHigh(long i)
{
	ASSERT(i<m_count);
	MarketData record={0};
//	int j=m_lRecordCound - i -1;
	int j=i + m_iBeginItems ;
	if(j<0 || j> m_count)
		return 0;
	ASSERT(j<m_count);
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	return record.dHigh;
}

//##ModelId=46AC33790201
float CIndicator::iLow(long i)
{
	ASSERT(i<m_count);
	MarketData record={0};
	int j=i + m_iBeginItems ;
	if(j<0 || j> m_count)
		return 0;
	ASSERT(j<m_count);
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	return record.dLow;
}

float CIndicator::GetLast(int iPeriod,int iType)
{
	for(int i=0;i<m_iLastsCount;i++)
		if(m_last[i].period == iPeriod)
		{
			switch(iType)
			{
			case PRICE_CLOSE	:
				return m_last[i].ItemC;
			case PRICE_OPEN	:
				return m_last[i].ItemO;
			case PRICE_HIGH	:
				return m_last[i].ItemH;
			case PRICE_LOW	:
				return m_last[i].ItemL;
			}
		}
	return 0.0;

}



void CIndicator::SetLast(float Item , int iPeriod,int iType)
{
	int i;
	for( i=0;i<m_iLastsCount;i++)
		if(m_last[i].period == iPeriod)
		{
			switch(iType)
			{
			case PRICE_CLOSE	:
				m_last[i].ItemC = Item;
				break;
			case PRICE_OPEN	:
				m_last[i].ItemO = Item;
				break;
			case PRICE_HIGH	:
				m_last[i].ItemH = Item;
				break;
			case PRICE_LOW	:
				m_last[i].ItemL = Item;
				break;
			}
			return;
		}
	switch(iType)
	{
	case PRICE_CLOSE	:
		m_last[i].ItemC = Item;
		break;
	case PRICE_OPEN	:
		m_last[i].ItemO = Item;
		break;
	case PRICE_HIGH	:
		m_last[i].ItemH = Item;
		break;
	case PRICE_LOW	:
		m_last[i].ItemL = Item;
		break;
	}
	m_last[i].period = iPeriod;
	m_iLastsCount=i+1;
	
}





float CIndicator::GetPrice(int iItem ,int iPriceType)
{
	float price=0;
	switch(iPriceType)
	{
	case PRICE_CLOSE:
		price = iClose(iItem);
		break;
	case PRICE_OPEN:
		price = iOpen(iItem );
		break;
	case PRICE_HIGH:
		price = iHigh(iItem );
		break;
	case PRICE_LOW:
		price = iLow(iItem );
		break;
	case PRICE_MIDDLE:
		price = (iOpen(iItem ) + iClose(iItem ))/2;
		break;
	}
	return price;
}

//##ModelId=46AC3379020A

float CIndicator::preEMA(int iItem,int iPriod,int iType,int iPrice)
{
	int prd=1,i=0;
	float sum=0,price=0,k=0,dRet=0,LastPrice=0;
	switch(iType)
	{
	case MODE_SMA:
		if(iItem<iPriod)
			return 0;
		prd = iPriod ;
		for(i=iItem-prd;i<iItem;i++)
		{
			if(i<0) continue;
			price = GetPrice(i,iPrice);
			sum +=price;
		}
		sum=    sum/(float)iPriod;
		return sum;
		
		break;
	case MODE_EMA:
		prd = (iItem<iPriod) ? iItem : iPriod;
		prd = iPriod;
		k = 2/(1+(float)(prd));
		price = GetPrice(iItem,iPrice);
		LastPrice = GetLast(iPriod,iPrice);
		if(!LastPrice)
		{
			SetLast(price,iPriod,iPrice);
			return price;
		}
		dRet = /*_R*/((price - LastPrice)*k + LastPrice);
		//		k = (int)(floor((floor(m_last)- m_last)*1000000)) % 100;
		SetLast(dRet,iPriod,iPrice);
		break;
	}



	return dRet;

}

float CIndicator::iMA(int iItem,int iPriod,int iType,int iPrice)
{
	int prd=1,i=0,j;
	float sum=0,price=0,k=0,dRet=0,LastPrice=0;
//	if(iItem==0) 
//	{
//		price = GetPrice(iItem,iPrice);
//		SetLast(price,iPriod);
//		return price;
//	}
	switch(iType)
	{
	case MODE_SMA:
		{

			MarketData record={0};
			if(iPriod<=0) return 0;
			int j=i+m_iBeginItems ;
			if(j>=m_count || j<0)
				return 0;
			memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
		//	TRACE("open Item(%d) %s %s  open=%f\n",i,record.szDate,record.szTime,record.dOpen);
			ASSERT( iPriod < (sizeof(record.dMaC)/sizeof(float)));
			return record.dMaC[iPriod-1];
		}

// 		if(iItem<iPriod)
// 			return 0;
// 		prd = iPriod ;
// 		for(i=iItem-prd;i<iItem;i++)
// 		{
// 			if(i<0) continue;
// 			price = GetPrice(i,iPrice);
// 			sum +=price;
// 		}
// 		sum=    sum/(float)iPriod;
// 		return sum;

		break;
	case MODE_EMA:
		return GetMa(iItem,iPriod,iPrice);
// 		prd = (iItem<iPriod) ? iItem : iPriod;
// 		prd = iPriod;
// 		k = 2/(1+(float)(prd));
// 		price = GetPrice(iItem,iPrice);
// 		LastPrice = GetLast(iPriod,iPrice);
// 		if(!LastPrice)
// 		{
// 			SetLast(price,iPriod,iPrice);
// 			return price;
// 		}
// 		dRet = /*_R*/((price - LastPrice)*k + LastPrice);
// //		k = (int)(floor((floor(m_last)- m_last)*1000000)) % 100;
// 		SetLast(dRet,iPriod,iPrice);
		return (dRet);
	case MODE_SSMA:
		if(iItem+1<iPriod) return 0.0;
		price = GetPrice(iItem,iPrice);
		LastPrice = GetLast(iPriod,iPrice);
		if(!LastPrice)
		{
			sum = 0;
			for(i=iItem-iPriod;i<=iItem;i++)
			{
				if(i<0) continue;
				price = GetPrice(i,iPrice);
				sum +=price;
			}
			sum=    sum/(float)(iPriod);
			SetLast(sum,iPriod,iPrice);
			return sum;
		}
		dRet = ((LastPrice)*(float)(iPriod-1) + price)/(float)iPriod;
//		k = (int)(floor((floor(m_last)- m_last)*1000000)) % 100;
		SetLast(dRet,iPriod,iPrice);
		dRet = _R(dRet);
		return (dRet);
	case MODE_LWMA:
		if(iItem<iPriod) return 0.0;
 		for(j=iPriod,i=iItem;i>(iItem-iPriod);i--,j--)
		{
			price = GetPrice(i,iPrice);
			sum += price*j;
			k+=j;
		}
		dRet = sum / k;
		return (dRet);
	default:
		return 0;
	}
	return 0;
}

float  CIndicator::iValumePercent(long iItem,int iPeriod)
{
	int i;
	int ll=10000000 ,hh=-10000,temp;
	for(i=iItem-iPeriod;i<iItem;i++)
	{
		temp =iValume(i);
		if(ll> temp  )
			ll= temp;
		if(hh< temp )
			hh= temp;
	}
	float K = (float)(((iValume(iItem-1) - ll)/(hh-ll))*100.00);
	return (float)K;

}


float CIndicator::BearPercent(long iItem,int iPeriod)
{
	int i,iBears=0;
	float temp=0;
	for(i=iItem-iPeriod;i<iItem;i++)
	{
		if(iOpen(i) > temp)
			iBears++;
		temp = iOpen(i);
	}
	float K = ((float)(iBears)/(float)(iItem))*(float)(100.0);
	return (float)K;

}


float CIndicator::BullPercent(long iItem,int iPeriod)
{
	int i,iBulls=0;
	float temp=0;
	for(i=iItem-iPeriod;i<iItem;i++)
	{
		if(iOpen(i) < temp)
			iBulls++;
		temp = iOpen(i);
	}
	float K = ((float)(iBulls)/(float)(iItem))*(float)(100.00);
	return (float)K;
	
}

bool CIndicator::GetCurRecord(int iItem,MarketData* pRecord)
{
	int j=iItem+m_iBeginItems ;
	if(j>m_count && j<0)
	{
		memset(&m_record,0,sizeof(m_record));
		return false;
	}
	memcpy(&m_record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	if(pRecord)
		memcpy(pRecord,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	return true;

}


float CIndicator::StochasticK(int iItem,int iKPeriod,int iSlowing)
{
	float dRes=0;
	int k;
	MarketData record={0};
	//---- %K line
	float sumlow=0.0;
	float sumhigh=0.0;
	for(k=(iSlowing-1);k>=0;k--)
	{
		GetRecord(iItem-k,&record);
		sumlow+=record.dClose-record.dLowest[iKPeriod-1];
		sumhigh+=record.dHighet[iKPeriod-1]-record.dLowest[iKPeriod-1];
	}
	if(sumhigh==0.0) return 0;
	dRes = (float)(sumlow/sumhigh)*(float)(100.00);
	return dRes;
}	

//************************************
// Method:    StochasticD
// FullName:  CIndicator::StochasticD
// Access:    public 
// Returns:   float
// Qualifier:
// Parameter: int iItem
// Parameter: int iKPeriod
// Parameter: int iDPeriod
// Parameter: int iSlowing
//************************************
float CIndicator::StochasticD(int iItem,int iKPeriod,int iDPeriod,int iSlowing)
{
	float dSum=0;
	for(int i=(iItem-iDPeriod)+1;i<=iItem;i++)
	{
		dSum += StochasticK(i,iKPeriod,iSlowing);
	}
	
	return dSum/(float)iDPeriod;
}	

float CIndicator::StochasticKFast(int iItem,int iKPeriod)
{
	//	if(iItem<iKPeriod) return 0;
	int i;
	float ll=10000 ,hh=-10000,temp;
	for(i=iItem-iKPeriod;i<iItem;i++)
	{
		if(ll> (temp =GetPrice(i,PRICE_LOW)) )
			ll= temp;
		if(hh< (temp=GetPrice(i,PRICE_HIGH)) )
			hh= temp;
	}
	if (hh == ll)
		return 0;
	float K = ((iOpen(iItem) - ll)/(hh-ll))*(float)(100.00);
	return K;
}	



float CIndicator::WilliamR(int iItem,int iKPeriod)
{
//	if(iItem<iKPeriod) return 0;
	int i;
	float ll=10000 ,hh=-10000,temp;
	for(i=iItem-iKPeriod;i<=iItem;i++)
	{
		if(ll> (temp =GetPrice(i,PRICE_LOW)) )
			ll= temp;
		if(hh< (temp=GetPrice(i,PRICE_HIGH)) )
			hh= temp;
	}
	float K = ((hh-iClose(iItem) )/(hh-ll))*(float)(100.00);
	return 100-K;
}	


float CIndicator::GetRSI(long i,int iPeriod)
{
	int j=i+m_iBeginItems ;
	ASSERT(j<m_count && j>=0);
	MarketData record={0};
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	//	TRACE("open Item(%d) %s %s  open=%f\n",i,record.szDate,record.szTime,record.dOpen);
	return record.dRSI[iPeriod-1];

}

float CIndicator::GetStochastic_K(long i, int iPeriod)
{
	int j = i + m_iBeginItems;
	ASSERT(j<m_count && j >= 0);
	MarketData record = { 0 };
	memcpy(&record, m_pData + j * sizeof(MarketData), sizeof(MarketData));
	//	TRACE("open Item(%d) %s %s  open=%f\n",i,record.szDate,record.szTime,record.dOpen);
	return record.dKSto[iPeriod - 1];

}


float CIndicator::iRSI(int iItem,int iPeriod,int iPrice)
{
	if(iItem<iPeriod) return 0;
	int i;
	float rsi=0;
	if(iItem==iPeriod)//calculate First RS
	{
		m_prevGain = m_prevLoss =0;
		float	diff;
		for(i=1;i<=iPeriod;i++)
		{
			diff = GetPrice(i, iPrice) - GetPrice(i-1, iPrice);
			if(diff>0) m_prevGain+=diff;
			else	   m_prevLoss-=diff;
		}
		m_prevGain /= (float)iPeriod;
		m_prevLoss /= (float)iPeriod;
		if(m_prevLoss == 0)
			m_prevLoss = 1;
		float FirstRS = (m_prevGain)/(m_prevLoss);
		rsi = 100 - 100/(1+FirstRS);
		return rsi;
	}
	float	diff = GetPrice(iItem, iPrice) - GetPrice(iItem-1, iPrice),d1=0,d2=0;
	if(diff>0) d1= diff ;
	else d2 = -diff;
	m_prevGain = ((m_prevGain*(float)(iPeriod-1)+d1)/(float)iPeriod);
	m_prevLoss = ((m_prevLoss*(float)(iPeriod-1)+d2)/(float)iPeriod);
	if(m_prevLoss == 0)
		m_prevLoss = 1;
	float sRS = m_prevGain/m_prevLoss;
	rsi = 100 - 100/(1+sRS);
	return rsi;
}	

float CIndicator::iMAOnArray(float* i_array,int i_len,int iPeriod,int iMaMethod,int iShift)
{
	int iBeg=0,i;
	float sum=0,p=0,k,last;
	switch(iMaMethod)
	{
	case MODE_SMA:
		if(i_len<iPeriod)
			return 0;
		iBeg = i_len - iPeriod-iShift;
		for(i=iBeg;i<iPeriod;i++)
		{
			if(i<0) continue;
			sum +=i_array[i];
		}
		sum=    sum/(float)iPeriod;
		return sum;

		break;
	case MODE_EMA:
		k = 2/(1+(float)(iPeriod));
		last = i_array[0];
		for(i=1;i<(i_len-iShift);i++)
		{
			last = (i_array[i] - last)*k + last;
		}
		return (last);
	case MODE_SSMA:
		k = 2/(1+(float)(iPeriod));
		last = i_array[0];
		for(i=1;i<(i_len-iShift);i++)
		{
			last = (i_array[i] - last)*k + last;
		}
		return (last);
	}
	return 0;
}	


#define ARRAY_LEN          10000*20

float ExtMapBuffer5[ARRAY_LEN];
float ExtMapBuffer6[ARRAY_LEN];
float ExtMapBuffer7[ARRAY_LEN];
float ExtMapBuffer8[ARRAY_LEN];
float g_Temp[ARRAY_LEN]={0};
int g_pos =0;
bool g_bFirst =true;

float CIndicator::HAS(int iItem,int iMaType1,int iPeriod1,int iMaType2,int iPeriod2)
{
	float maOpen, maClose, maLow, maHigh;
	float haOpen=0, haHigh=0, haLow=0, haClose=0;
	if(iItem<=10) 
	return(0);
	int pos=g_pos;
//	maOpen=iMA(iItem,iPeriod1,iMaType1,PRICE_OPEN);
//	maClose=iMA(iItem,iPeriod1,iMaType1,PRICE_CLOSE);
//	maLow=iMA(iItem,iPeriod1,iMaType1,PRICE_LOW);
//	maHigh=iMA(iItem,iPeriod1,iMaType1,PRICE_HIGH);
	maOpen=GetMa(iItem,iPeriod1,PRICE_OPEN);
	maClose=GetMa(iItem,iPeriod1,PRICE_CLOSE);
	maLow=GetMa(iItem,iPeriod1,PRICE_LOW);
	maHigh=GetMa(iItem,iPeriod1,PRICE_HIGH);

	if(g_bFirst)
	{
		haOpen=(maOpen+maClose)/2;
		g_bFirst  = false;
	}
	else
		haOpen=(ExtMapBuffer5[pos-1]+ExtMapBuffer6[pos-1])/2;
	haClose=(maOpen+maHigh+maLow+maClose)/4;
	haHigh=max(maHigh, max(haOpen, haClose));
	haLow=min(maLow, min(haOpen, haClose));

	if (haOpen<haClose) 
	{
		ExtMapBuffer7[pos]=haLow;
		ExtMapBuffer8[pos]=haHigh;
	//         Print("Up..");
	} 
	else
	{
		ExtMapBuffer7[pos]=haHigh;
		ExtMapBuffer8[pos]=haLow;
	//         Print("down..");
	} 
	ExtMapBuffer5[pos]=haOpen;
	ExtMapBuffer6[pos]=haClose;
	g_pos++;


    ASSERT(g_pos < ARRAY_LEN);
	float d1=iMAOnArray(ExtMapBuffer7,g_pos,iPeriod2,iMaType2,0);
	float d2=iMAOnArray(ExtMapBuffer8,g_pos,iPeriod2,iMaType2,0);
	float d3=iMAOnArray(ExtMapBuffer5,g_pos,iPeriod2,iMaType2,0);
	float d4=iMAOnArray(ExtMapBuffer6,g_pos,iPeriod2,iMaType2,0);
	if(d2>d3)
	{
		return 1;
	}
	if(d2<d3)
	{
		return -1;
	}
	return 0;
}	

void CIndicator::ResetIndicator(void)
{
	memset(ExtMapBuffer5,0,sizeof(ExtMapBuffer5));
	memset(ExtMapBuffer6,0,sizeof(ExtMapBuffer6));
	memset(ExtMapBuffer7,0,sizeof(ExtMapBuffer7));
	memset(ExtMapBuffer8,0,sizeof(ExtMapBuffer8));
	g_pos =0;
	g_bFirst  = 1;
	m_iLastsCountMACD = m_iLastsCount = 0;
	
}

static long GetMinuts(char* lpszTime)
{
	char s1[3]={0};
	char s2[3]={0};
	memcpy(s1,lpszTime,2);
	memcpy(s2,lpszTime+3,2);
	return atol(s1)*60 + atol(s2);
}
int CIndicator::GetTF(void)
{
	MarketData rc1={0},rc2={0};
	char s1[3]={0},s2[3]={0};
	memcpy(&rc1,m_pData,sizeof(MarketData));
	memcpy(&rc2,m_pData+sizeof(MarketData),sizeof(MarketData));
	int t1 = GetMinuts(rc1.szTime);
	int t2 = GetMinuts(rc2.szTime);
	int d1 = atoi(rc1.szDate);
	int d2 = atoi(rc2.szDate);
	if( t1!= t2 && d1 == d2)
		return abs(t2-t1);
	if(abs(d2-d1) == 1 && t1!= t2)
		return abs(t2-t1);
	if(abs(d2-d1) >= 1)
		return TF_D1;
	if(abs(d2-d1) == 7)
		return TF_W1;
	return 0;

}

int CIndicator::GetPrevDay(int iItem,float &dClose,float &dHigh,float &dLow)
{
	ASSERT(iItem<m_count && iItem >=0);
	dClose = dHigh = dLow =0;
	int j=iItem + m_iBeginItems ;
	switch(m_iTF) 
	{
	case TF_M15:
		if(j< (48*2+1) )
			return 0;
		break;
	case TF_M30:
		if(j< (48+1) )
			return 0;
		break;
	case TF_H1:
		if(j< (24+1) )
			return 0;
		break;
	case TF_H4:
		if(j< (8+1) )
			return 0;
		break;
	case TF_D1:
		if(j< (1+1) )
			return 0;
		break;
	default:
		return 240;
	}
	MarketData record={0};
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	int cur = atol(record.szDate);
	do 
	{
		j--;
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	} while(cur == atol(record.szDate));
	cur = atol(record.szDate);
	dClose = record.dClose;
	float dMax = 0,dMin=10000;
	int iBars=0;
	do 
	{
		if(record.dHigh > dMax)
			dMax = record.dHigh;
		if(record.dLow < dMin)
			dMin = record.dLow;
		j--;
		iBars ++;
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	} while(cur == atol(record.szDate) && j>0);
	dHigh = dMax;
	dLow = dMin;
	cur = atol(record.szDate);
	if(iBars==1)
	{
		dMax = 0,dMin=10000;
		iBars=0;
		do 
		{
			if(record.dHigh > dMax)
				dMax = record.dHigh;
			if(record.dLow < dMin)
				dMin = record.dLow;
			j--;
			iBars ++;
			memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
		} while(cur == atol(record.szDate) && j>0);
		dHigh = dMax;
		dLow = dMin;
	}
	return 1;

}

//for genetic strategy4 I use this
int CIndicator::GetNextDayIndex(int iItem)
{
	int j=iItem + m_iBeginItems ;
	ASSERT(j<m_count && j >=0);
	MarketData record={0};
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	int cur = atol(record.szDate);
	do 
	{
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
		j++;
		if(j>m_count)
			return m_count-1;
	} while(cur == atol(record.szDate));
	return j-m_iBeginItems;
	
}


float CIndicator::PivotPoint(int iItem,PivotData* pData)
{
	float yh ,yl,yc; 
	if(!GetPrevDay(iItem,yc,yh,yl))
		return 0;
	float R = yh - yl;//range
	float p =  pData->pivod = (yh+ yl + yc)/3;// Standard Pivot
	pData->R3 = p + (R * (float)1.000);
	pData->R2 = p + (R * (float)0.618);
	pData->R1 = p + (R * (float)0.382);
	pData->S1 = p - (R * (float)0.382);
	pData->S2 = p - (R * (float)0.618);
	pData->S3 = p - (R * (float)1.000);
	return pData->pivod;	
}


int CIndicator::GetThisDay(int iItem,float &dOpen,float &dClose,float &dHigh,float &dLow)
{
	ASSERT(iItem<m_count && iItem >=0);
	dClose = dHigh = dLow =0;
	MarketData record={0};
	int j=iItem + m_iBeginItems ;
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	int cur = atol(record.szTime);
//	if(cur)
//		return 0;
	while(cur)
	{
		j--;
		if(j<0)
			return 0;
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
		cur = atol(record.szTime);
	}
	dOpen = record.dOpen;
	cur = atol(record.szDate);
//	TRACE("date=%s\n",record.szDate);
	float dMax = 0,dMin=10000;
	do 
	{
		if(record.dHigh > dMax)
			dMax = record.dHigh;
		if(record.dLow < dMin)
			dMin = record.dLow;
		j++;
		dClose = record.dClose;
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	} while(cur == atol(record.szDate) );
	dHigh = dMax;
	dLow = dMin;
	return 1;

}

int CIndicator::GetThisDayUntilTime(int iItem,int iTime,float &dOpen,float &dClose,float &dHigh,float &dLow)
{
	ASSERT(iItem<m_count && iItem >=0);
	dClose = dHigh = dLow =0;
	MarketData record={0};
	int j=iItem + m_iBeginItems ;
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	int cur = atol(record.szTime);
	if(cur != iTime)
		return 0;
	while(cur)
	{
		j--;
		if(j<0)
			return 0;
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
		cur = atol(record.szTime);
	}
	dOpen = record.dOpen;
	cur = atol(record.szDate);
//	TRACE("date=%s\n",record.szDate);
	float dMax = 0,dMin=10000;
	do 
	{
		if(record.dHigh > dMax)
			dMax = record.dHigh;
		if(record.dLow < dMin)
			dMin = record.dLow;
		j++;
		dClose = record.dClose;
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	} while(atol(record.szTime) < iTime);
	dHigh = dMax;
	dLow = dMin;
	return 1;

}

int CIndicator::GetThisDayUntilTime2(int iItem,int iEndTime,float &dHigh,float &dLow)
{
	ASSERT(iItem<m_count && iItem >=0);
	dHigh = dLow =0;
	MarketData record={0};
	int j=iItem + m_iBeginItems ;
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	int cur = atol(record.szTime);
	float dMax = 0,dMin=10000;
	do 
	{
		if(record.dHigh > dMax)
			dMax = record.dHigh;
		if(record.dLow < dMin)
			dMin = record.dLow;
		j++;
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	} while(atol(record.szTime) < iEndTime);
	dHigh = dMax;
	dLow = dMin;
	return 1;

}

//
float CIndicator::iHighestHigh(long i,int iPeriod)
{
	MarketData record={0};
	int j=i + m_iBeginItems,k=0 ;
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	//	TRACE("date=%s\n",record.szDate);
	float dMax = 0,dMin=10000;
	do 
	{
		if(record.dHigh > dMax)
			dMax = record.dHigh;
		if(record.dLow < dMin)
			dMin = record.dLow;
		j--;
		if(j<0) break;
		k++;
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	} while(k<iPeriod);
	return dMax;

}


float CIndicator::iLowestLow(long i,int iPeriod)
{
	MarketData record={0};
	int j=i + m_iBeginItems,k=0 ;
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	//	TRACE("date=%s\n",record.szDate);
	float dMax = 0,dMin=10000;
	do 
	{
		if(record.dHigh > dMax)
			dMax = record.dHigh;
		if(record.dLow < dMin)
			dMin = record.dLow;
		j--;
		if(j<0) break;
		k++;
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	} while(k<iPeriod);
	return dMin;

}

float CIndicator::iHighFractal(long i,int iPeriod,int iType)
{
	MarketData record={0};
	MarketData r1,r2;
	int j=i + m_iBeginItems,k=0 ;
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	//	TRACE("date=%s\n",record.szDate);
	float dMax = 0,dMin=10000;
	do 
	{
		//current record
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
		//previous record
		memcpy(&r1,m_pData+(j-1)*sizeof(MarketData),sizeof(MarketData));
		//next record
		if(j+1 < m_count)
			memcpy(&r2,m_pData+(j+1)*sizeof(MarketData),sizeof(MarketData));
		else
			memset(&r2,0,sizeof(r2));
		if( (record.dHigh > r1.dHigh && record.dHigh > r2.dHigh ) )
		{
			if(record.dHigh > dMax)
				dMax = record.dHigh;
		}
		j--;
		k++;
	} while(k<iPeriod);
	return dMax;

}



float CIndicator::iLowFractal(long i,int iPeriod,int iType)
{
	MarketData record={0};
	MarketData r1,r2;
	int j=i + m_iBeginItems,k=0 ;
	memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
	//	TRACE("date=%s\n",record.szDate);
	float dMax = 0,dMin=10000;
	do 
	{
		//current record
		memcpy(&record,m_pData+j*sizeof(MarketData),sizeof(MarketData));
		//previous record
		memcpy(&r1,m_pData+(j-1)*sizeof(MarketData),sizeof(MarketData));
		//next record
		if(j+1 < m_count)
			memcpy(&r2,m_pData+(j+1)*sizeof(MarketData),sizeof(MarketData));
		else
			memset(&r2,0,sizeof(r2));
		if( (record.dLow < r1.dLow && record.dLow < r2.dLow) )
		{
			if(record.dHigh < dMin)
				dMin = record.dHigh;
		}
		j--;
		k++;
	} while(k<iPeriod);
	return dMin;

}




float CIndicator::iMACD(int iItem,int fast_ema_period, int slow_ema_period, int signal_period, int iPrice, int mode)
{
	float dMa1 = 0;
	float dMa2 = 0;
	if(mode == METHOD_MAIN)
	{
		dMa1 = GetMa(iItem,fast_ema_period);
		dMa2 = GetMa(iItem,slow_ema_period);
		m_dVal = dMa1-dMa2;
		return m_dVal;
	}
	float dVal=m_dVal;
// 	if(m_iLastsCountMACD<signal_period)
// 	{
// 		for(int i=0;i<signal_period;i++)
// 			m_lastMACD[i] = GetMa(iItem-(signal_period + i),fast_ema_period)- GetMa(iItem-(signal_period + i),slow_ema_period);
// 		m_iLastsCountMACD = signal_period;
// //		m_lastMACD[m_iLastsCountMACD++] = dVal;
// //		float dSum=0;
// //		for(int i=0;i<m_iLastsCountMACD;i++)
// //			dSum += m_lastMACD[i] ;
// //		return dSum/(float)i;
// 	}
	float dSum=0;

	for(int i=0;i<signal_period;i++)
	{
		dMa1 = GetMa(iItem-i,fast_ema_period);
		dMa2 = GetMa(iItem-i,slow_ema_period);
		dSum += (dMa1-dMa2) ;
	}
	return (dSum/(float)signal_period);
}

void CIndicator::GetRecord(int item,MarketData* o_record)
{
	int j=item+m_iBeginItems ;
	ASSERT(j<m_count && j>=0);	
	memcpy(o_record,m_pData+j*sizeof(MarketData),sizeof(MarketData));

}





////////////////////////////////////////////////////////////////////
//this function synchronizes the two indicator and they will be at the same time
//if bJustDate = TRUE just date is compared
int   CIndicator::Sync(CIndicator* pInd,int item,bool bJustDate)
{
	MarketData record={0},rec;
	pInd->GetRecord(item,&record);
	long l1,l2,k=0;
	int ibeg =m_iBeginItems,iend = m_iEndItems;
	l1= atol(record.szDate);
	while(true)
	{
		int i=(iend - ibeg)/2+ibeg;
		memcpy(&rec,m_pData+i*sizeof(MarketData),sizeof(MarketData));
		l2 = atol(rec.szDate);
			
		if(l1 == l2)
		{
			return i-m_iBeginItems;
		}


		if(l1 < l2)
		{
			iend = i;
		}

		if(l1>l2)
		{
			ibeg = i;
		}
		k++;
		if(k==100)
		{
			iend = m_count;
			ibeg = 0;
		}
		if(k == 200)
			break;

	}

	exit(0);
	return -1;

}


float CIndicator::LinearRegression(int iItem,int iPeriod,int iPrice)
{
	int n=0;
	float sumx=0, sumy=0, sumxy=0, sumx2=0, sumy2=0;
	float m=0, yint=0, r=0;
	float dPrice = 0;
	if(iItem<iPeriod)
		return 0;
	for(n = 0; n <= iPeriod-1; n++)
	{ 
		dPrice = GetPrice(iItem - n,iPrice);
		sumx = sumx + n;
		sumy = sumy + dPrice;
		sumxy = sumxy + n * dPrice;
		sumx2 = sumx2 + n * n;
		sumy2 = sumy2 + dPrice * dPrice; 
	}                  
    float dPeriod = (float)iPeriod;
	m=(dPeriod*sumxy-sumx*sumy)/(dPeriod*sumx2-sumx*sumx); 
	yint=(sumy+m*sumx)/dPeriod;
	float dTemp = sqrt((dPeriod*sumx2-sumx*sumx)*(dPeriod*sumy2-sumy*sumy));
	if(!dTemp) 
		dTemp =1;
	r=(dPeriod*sumxy-sumx*sumy)/dTemp; 
	sumx=yint-m*dPeriod;
	return sumx;
		//Print (" "+shift+" "+LRLBuffer[shift]);
	
}



void CIndicator::PrepareIndicatorAlldata(void)
{
	MarketData record={0};
	ResetIndicator();
	m_iBeginItems = 0;
	for(register int i=0;i<m_count;i++)
	{
		memcpy(&record,m_pData+i*sizeof(MarketData),sizeof(MarketData));
		for(int j=1;j<=IND_MA_COUNT;j++)
		{
			record.dMa[j-1] = preEMA(i,j,MODE_EMA,PRICE_OPEN);
//			record.dMaC[j-1] = preEMA(i,j,MODE_SMA,PRICE_CLOSE);
//			record.dMaH[j-1] = iMA(i,j,MODE_EMA,PRICE_HIGH);
//			record.dMaL[j-1] = iMA(i,j,MODE_EMA,PRICE_LOW);
		}
		memcpy(m_pData+i*sizeof(MarketData),&record,sizeof(MarketData));
			
	}
	int i,j;
	for(j=1;j<=70;j++)
	for(i=0;i<m_count;i++)
	{
		memcpy(&record,m_pData+i*sizeof(MarketData),sizeof(MarketData));
		record.dRSI[j-1] = iRSI(i,j,PRICE_OPEN);
 		if(j<45)
 			record.dKSto[j-1] = StochasticKFast(i,j);
		record.dHighet[j-1] = iHighestHigh(i,j);
		record.dLowest[j-1] = iLowestLow(i,j);
		memcpy(m_pData+i*sizeof(MarketData),&record,sizeof(MarketData));

	}

	for(i=0;i<m_count;i++)
	{
		memcpy(&record,m_pData+i*sizeof(MarketData),sizeof(MarketData));
//		record.dData1 = iMACD(i,12,26,7,PRICE_OPEN,METHOD_MAIN);
//		record.dData2 = iMACD(i,12,26,7,PRICE_OPEN,METHOD_SIGNAL);
//		record.dData2 = iMACD(i,12,26,5,PRICE_OPEN,METHOD_SIGNAL);
		memcpy(m_pData+i*sizeof(MarketData),&record,sizeof(MarketData));
			
	}
}

void CIndicator::ReloadMACD(int iMa1,int iMa2,int iMa3)
{
	MarketData record={0};
	ResetIndicator();
	m_iBeginItems = 0;
	for(int i=0;i<m_count;i++)
	{
		memcpy(&record,m_pData+i*sizeof(MarketData),sizeof(MarketData));
		record.dData1 = iMACD(i,iMa1,iMa2,iMa3,PRICE_OPEN,METHOD_MAIN);
		record.dData2 = iMACD(i,iMa1,iMa2,iMa3,PRICE_OPEN,METHOD_SIGNAL);
		memcpy(m_pData+i*sizeof(MarketData),&record,sizeof(MarketData));
			
	}
}





