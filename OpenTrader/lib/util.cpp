#include "stdafx.h"
#include <fstream>
#include <iterator>
#include <vector>
#include <map>
#include <sstream>


#include "indicator.h"
#include "util.h"

#include "optimizer.h"

using namespace std;

//we want to use POSIX functions
#pragma warning( push )
#pragma warning( disable : 4996 )


void LogInfo(const char* str, ...)
{
	if (str)
	{
		va_list args;
		va_start(args, str);

		vfprintf(stdout, str, args);

		va_end(args);
	}
}

#ifndef _WIN32
void* _aligned_malloc(
	 size_t _Size,
	size_t _Alignment)
{
	return  aligned_alloc(_Alignment, _Size);
}


#endif



void LogError(const char* str, ...)
{
	if (str)
	{
		va_list args;
		va_start(args, str);

		vfprintf(stderr, str, args);

		va_end(args);
	}
}

void GenerateParamsStr(tradeParameters param, char* out_str)
{
	if (!param.counts)
		return;
	out_str[0] = '\0';
	for (int i = 0; i < param.counts; i++)
		sprintf(out_str + strlen(out_str), "%d,", param.params[i]);
	out_str[strlen(out_str) - 1] = 0;

}

// Upload the OpenCL C source code to output argument source
// The memory resource is implicitly allocated in the function
// and should be deallocated by the caller
int ReadSourceFromFile(const char* fileName, char** source, size_t* sourceSize)
{
	int errorCode = 0;

	FILE* fp = NULL;
#ifdef _WIN32
	fopen_s(&fp, fileName, "rb");
#else
	fp = fopen(fileName, "rb");
#endif
	if (fp == NULL)
	{
		LogError("Error: Couldn't find program source file '%s'.\n", fileName);
		errorCode = -30;
	}
	else {
		fseek(fp, 0, SEEK_END);
		*sourceSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		*source = new char[*sourceSize];
		if (*source == NULL)
		{
			LogError("Error: Couldn't allocate %d bytes for program source from file '%s'.\n", *sourceSize, fileName);
			errorCode = -6;
		}
		else {
			fread(*source, 1, *sourceSize, fp);
		}
	}
	return errorCode;
}
#pragma warning( pop )


long GetFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}


void FindSpread(const char* lpszFilename,appUmf* umf, bool bMain)
{
	std::string str(lpszFilename);
	umf->spread = 4;
	bool bJBP = false;
	int len = str.length(), i = 0;
	char Delimiter = '\\';
#ifndef WIN32
	Delimiter = '/';
#endif
	i = len - 1;
	while (str[i] != Delimiter)
		i--;
	if (i == len)
	{
		LogError("Find spead failed %s\n", lpszFilename);
	}
	umf->name = str.substr(i + 1, len - i - 5);

	i = 0;
	while (str[len - 3 - 2 - i] >= '0' && str[len - 3 - 2 - i] <= '9')
		i++;
	std::string  strCur = str.substr(len - 3 - 1 - i - 6, 6);
	std::string	  strTF = str.substr(len - 7, 3);
	while (strTF[0] <'0' || strTF[0] >'9')
		strTF.erase(0);
	umf->time_frame = stol(strTF);
	umf->kValue = 10000;
	if (strCur == "EURUSD")
		umf->spread = 2;
	else
		if (strCur == "GBPUSD")
			umf->spread = 3;
	else if (strCur == "USDSGD")
		umf->spread = 5;
	else if (strCur == "USDCHF")
		umf->spread = 3;
	else if (strCur == "EURGBP")
		umf->spread = 2;
	else if (strCur == "AUDUSD")
		umf->spread = 3;
	else if (strCur == "AUDCAD")
		umf->spread = 8;
	else if (strCur == "EURCHF")
		umf->spread = 3;
	else if (strCur == "NZDUSD")
		umf->spread = 4;
	else if (strCur == "USDCAD")
		umf->spread = 4;
	else if (strCur == "CADCHF")
		umf->spread = 6;
	else if (strCur == "GBPAUD")
		umf->spread = 9;
	else if (strCur == "EURAUD" || strCur == "AUDCHF" || strCur == "AUDSGD" || strCur == "NZDCAD")
		umf->spread = 1;
	else if (strCur == "GBPCHF")
		umf->spread = 7;
	else if (strCur == "GBPJPY" || strCur == "CADJPY")
	{
		umf->kValue = 100;
		umf->spread = 6;
		bJBP = true;
	}
	else if (strCur == "SGDJPY" || strCur == "CHFJPY")
	{
		umf->kValue = 100;
		umf->spread = 4;
		bJBP = true;
	}
	else if (strCur == "AUDJPY")
	{
		umf->kValue = 100;
		umf->spread = 5;
		bJBP = true;
	}
	else if (strCur == "NZDJPY")
	{
		umf->kValue = 100;
		umf->spread = 8;
		bJBP = true;
	}
	else if (strCur == "SILVER")
	{
		umf->kValue = 100;
		umf->spread = 5;
		bJBP = true;
	}
	else if (strCur == "CL")
	{
		umf->kValue = 100;
		umf->spread = 1;
		bJBP = true;
	}
	else if (umf->name.substr(0, 4) == "GOLD")
	{
		umf->kValue = 10;
		umf->spread = 5;
		bJBP = true;
	}
	else if (umf->name.substr(0, 1) == "Z")
	{
		umf->kValue = 10;
		umf->spread = 5;
		bJBP = true;
	}
	else if (umf->name.substr(0, 2) == "CL")
	{
		umf->kValue = 100;
		umf->spread = 1;
		bJBP = true;
	}
	else
		if (strCur == "USDJPY" || strCur == "EURJPY")
		{
			umf->kValue = 100;
			bJBP = true;
			umf->spread = 3;
		}
		else
			if (strCur == "CHFJPY")
			{
				umf->kValue = 100;
				bJBP = true;
				umf->spread = 4;
			}
	if (bMain)
	{
		umf->spread = umf->spread;
		umf->kValue = umf->kValue;
		umf->jpb = bJBP;
	}
	else
	{
		// 		umf->m_ind2->spread = umf->spread;
		// 		umf->m_ind2->kValue = umf->kValue;
		// 		umf->m_ind2->m_bJBP = bJBP;
	}
}


void FindSpreadStock(std::string lpszFilename, appUmf* umf)
{
	std::string str=lpszFilename;
	umf->spread = 4;
	bool bJBP = false;
	int len = str.length(), i = 0;
	i = len - 1;
	while (str[i] != '\\')
		i--;
	umf->name = str.substr(i + 1, len - i - 5);

	i = 0;
	umf->spread = 3;
	umf->kValue = 1;
	umf->jpb = false;
}


bool LoadCSVFileStock(std::string lpszFilename,appUmf* umf )
{
	//	_beginthread(ProgressThread,0,NULL);
	//	CProgressDlg* pDlg=new CProgressDlg();
	//	pDlg->Create(CProgressDlg::IDD);
	//	pDlg->ShowWindow(SW_SHOW);
	ifstream file;
	char szTemp[1024];
	int i=0,j=0,k=0;
	BYTE* pData = NULL;
	if(umf->pData)
		free(umf->pData);
	if (GetFileSize(lpszFilename) < 1024)
		return false;
	std::string strLine,str;
	file.open(lpszFilename);
	if(!file)
		return false;
	while(file>> strLine)
		i++;
	int len = sizeof(MarketData)*i;
	pData = (BYTE*)malloc(len+1);
	memset(pData,0,len);
	file.seekg(0,file.beg);
	MarketData record;
	int kRev = i-1;
	i=0;
	bool bNYSE=false;
	//2006.11.22,05:30,1.28650,1.28710,1.28640,1.28670,98
	//Date,time,open,high,low,close,volume
	while(file>> strLine)
	{
		j=0;
		strLine = szTemp;
		if(strLine.length()<20)
			continue;
		if(strLine.find("<TICKER>")!=-1)
			continue;
		if(strLine.find("Date")!=-1)
		{
			bNYSE = true;
			continue;
		}
		if(strLine.length()<4)
			continue;
		//date
		j = strLine.find(",",0);
		if(j==-1) continue;
		j++;
		if(!bNYSE)
		{
			k = strLine.find(",",j);
			if(j==-1) continue;
			str = strLine.substr(j,k-j);
			if(str.substr(0,2)!="20")
				continue;

		}
		else
		{
			str = strLine.substr(0,j-1);
			k=j-1;

		}
		memset(&record,0,sizeof(MarketData));
		str.erase( std::remove(str.begin(), str.end(), '.'), str.end() );
		str.erase( std::remove(str.begin(), str.end(), '-'), str.end() );
		strcpy(record.szDate,str.c_str());
		//time
		strcpy(record.szTime,"0000");
		j=k+1;
		//open
		k = strLine.find(",",j);
		if(k==-1) continue;
		str = strLine.substr(j,k-j);
		record.dOpen=stof(str);
		j=k+1;
		//high
		k = strLine.find(",",j);
		if(k==-1) continue;
		str = strLine.substr(j,k-j);
		record.dHigh=stof(str);
		j=k+1;
		//low
		k = strLine.find(",",j);
		if(k==-1) continue;
		str = strLine.substr(j,k-j);
		record.dLow=stof(str);
		j=k+1;
		//close
		k = strLine.find(",",j);
		if(k==-1) continue;
		str = strLine.substr(j,k-j);
		record.dClose=stof(str);
		j=k+1;
		//volume
		k = strLine.find(",",j);
		if(k==-1) continue;
		str = strLine.substr(j,k-j);
		record.iVoldume =stoi(str);
		if(bNYSE)
		{
			if(kRev<10)
				kRev=kRev;
			if(kRev>0)
				memcpy(pData + (kRev-1)*sizeof(MarketData),&record,sizeof(MarketData));
			kRev--;
		}
		else
		{

			memcpy(pData + i*sizeof(MarketData),&record,sizeof(MarketData));
		}
		i++;
		strLine="";
		memset(szTemp,0,sizeof(szTemp));
	}
	umf->rec_count = i-1;
	file.close();
//	pInd->GetData(pData,i);
	char s[10]={0};
	if(bNYSE)
		memcpy(&record,pData+(i-1)*sizeof(MarketData),sizeof(MarketData));

	memcpy(s,record.szDate,4);
	int y = atoi(s);
	int m = (record.szDate[4]-'0')*10 + (record.szDate[5]-'0');
	int d = (record.szDate[6]-'0')*10 + (record.szDate[7]-'0');
//	time_t time(y,m,d,0,0,0);
	memcpy(&record,pData,sizeof(MarketData));
	memcpy(s,record.szDate,4);
	s[4]=0;
	y = atoi(s);
	m = (record.szDate[4]-'0')*10 + (record.szDate[5]-'0');
	d = (record.szDate[6]-'0')*10 + (record.szDate[7]-'0');
//	time_t time2(y,m,d,0,0,0);
	FindSpreadStock(lpszFilename,umf);

	umf->name = umf->name;


//	pInd->PrepareIndicatorAlldata();
	umf->stock = true;




	//	pDlg->DestroyWindow();
	//	delete pDlg;
	return true;
}




int  LoadData(const char* i_lpszFileName, appUmf* umf)
{
	ifstream  file;
	int i=0,j=0,k=0;
	BYTE* pData = NULL;
	if(umf->pData)
		free(umf->pData);
	std::string strLine,str;
	file.open(i_lpszFileName);
	if (!file.good()) {
		LogError("file %s open failed [%s]\n", i_lpszFileName, strerror(errno));
		return false;
	}
	char szTemp[1024];
	while(file>> strLine)
		i++;
	LogInfo("read %d\n",i);
	if (!i){
		LogError("file %s open empty\n", i_lpszFileName);
		return false;
	}
	int len = sizeof(MarketData)*i;
	pData = (BYTE*)malloc(len+1);
	memset(pData,0,len);
	file.close();
	file.open(i_lpszFileName);
	file.seekg(0);
	MarketData record;
	i=0;

	//2006.11.22,05:30,1.28650,1.28710,1.28640,1.28670,98
	//Date,time,open,high,low,close,volume
	while(file>>strLine)
	{
		if(i==0 && strLine.find("<TICKER>")!=-1)
		{
			file.close();
			free(pData);
			return LoadCSVFileStock(i_lpszFileName,umf);
		}
		if(i==0 && strLine.find("Date")!=-1)
		{
			file.close();
			free(pData);
			return LoadCSVFileStock(i_lpszFileName,umf);
		}
		j=0;
		if(strLine.length()<20) 
			continue;
//		LogInfo("%s\n", strLine.c_str());
		memset(&record,0,sizeof(MarketData));
		//default value
		strcpy(record.szTime,"0000");

		//date
		j = strLine.find(",",0);
		if(j==-1) continue;
		str = strLine.substr(0,j);
		if (str.find(',') >= 0)
			str.erase( std::remove(str.begin(), str.end(), ','), str.end() );
		if (str.find('.') >= 0)
			str.erase( std::remove(str.begin(), str.end(), '.'), str.end() );
		if(str.length()==8)
		{
			strcpy(record.szDate,str.c_str());
		}
		else//for metaTrader5
		{
			strcpy(record.szDate,str.substr(8).c_str());
			int ss = str.find(":");
			if(ss)
			{
				str.erase( std::remove(str.begin(), str.end(), ':'), str.end() );
				strcpy(record.szTime,str.substr(str.length()-4).c_str());

			}
		}

		//time
		j++;
		k = strLine.find(",",j);
		if(k==-1) continue;
		str = strLine.substr(j,k-j);
		if(str.find(":")>=0)
		{
			str.erase( std::remove(str.begin(), str.end(), ':'), str.end() );
			strcpy(record.szTime,str.c_str());
			j=k+1;

			//open
			k = strLine.find(",",j);
			if(k==-1) continue;
			str = strLine.substr(j,k-j);
		}
		record.dOpen=stof(str);
		j=k+1;
		//high
		k = strLine.find(",",j);
		if(k==-1) continue;
		str = strLine.substr(j,k-j);
		record.dHigh=stof(str);
		j=k+1;
		//low
		k = strLine.find(",",j);
		if(k==-1) continue;
		str = strLine.substr(j,k-j);
		record.dLow=stof(str);
		j=k+1;
		//close
		k = strLine.find(",",j);
		if(k==-1) continue;
		str = strLine.substr(j,k-j);
		record.dClose=stof(str);
		j=k+1;
		//volume
		if ((strLine.length() - j) > 0) {
			str = strLine.substr(j, strLine.length() - j);
			record.iVoldume = stoi(str);
		}
		else
		{
			LogError("format error\n");
		}
		memcpy(pData + i*sizeof(MarketData),&record,sizeof(MarketData));
		i++;
		strLine="";
		memset(szTemp,0,sizeof(szTemp));
	}
	umf->rec_count = i;
	file.close();
	LogInfo("file loaded\n");
	FindSpread(i_lpszFileName,umf,true);
	umf->pData = pData;
	LogInfo("file %s opened with %d records\n", i_lpszFileName, umf->rec_count);

//	umf->m_pInd1->PrepareIndicatorAlldata();
//	umf->m_pInd1->m_strIndexName = umf->name;

//	pDlg->DestroyWindow();
//	delete pDlg;
	return true;

}
/*
int GetStrategyName(std::string strStrategyName)
{
	char szTemp[50];
	int iCount;
	strcpy(szTemp,strStrategyName.c_str());
	for(iCount=0;iCount<ST_COUNT;iCount++)
	{
		if(strcmp(szTemp,g_szStrategys[iCount])==0)
		{
			break;
		}
	}
	return iCount;
	
}
*/

int GetStrPart(int iPart,std::string strIn,std::string& strOut)
{
	int i=0,j,k=0;

	while(true)
	{
		j=  strIn.find(',',i);
		if(j==-1 && i)
		{
			k++;
			if(k!=iPart)
				return -1;
			strOut = strIn.substr(strIn.length() - i);
			return 1;
		}
		k++;
		if(k==iPart)
		{
			strOut = strIn.substr(i,j-i);
			return 1;
		}
		i=j+1;

	}
	return -1;
	
	
}


time_t ConvDate(std::string str)
{
	if(str.length()<8)
		return time_t(0);
	return MKTimestamp((char*)str.c_str(), NULL);
}

void LogOutput(char* i_lpszData, ...)
{
	time_t rawtime;
	struct tm * timeinfo;
	char szTemp[512];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	sprintf(szTemp,"[%02d:%02d:%02d]", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	va_list args;
	va_start(args, i_lpszData);
	vsprintf(szTemp+strlen(szTemp),i_lpszData, args);
	va_end(args);
	strcat(szTemp, "\n");

	std::ofstream out;

	// std::ios::app is the open mode "append" meaning
	// new data will be written to the end of the file.
	out.open("LogOutput.txt", std::ios::app);
	if(out.good())
		out << szTemp;
	out.close();
}

std::string GetDataPath()
{
	std::string ret;
#ifdef _WIN32
	ret = "c:\\fxData\\";
#else
	ret = "/home/ubuntu/fxData/";

#endif
	return ret;
}

void convert_trade_params(std::map < std::string, std::string> queue_dic,testParams& params)
{
	if (queue_dic.find("indicator") != queue_dic.end())
	{
		auto s = queue_dic["indicator"];
		if (s == "ema_cross")
			params.indIndex = IND_TYPE_EMA_CROSS;
		else
			if (s == "rsi")
				params.indIndex = IND_TYPE_RSI;
			else
				if (s == "stochastic")
					params.indIndex = IND_TYPE_STOCHASTIC;
				else
					if (s == "macd")
						params.indIndex = IND_TYPE_MACD;

	}

	if (queue_dic.find("from_date") != queue_dic.end())
	{
		params.beginDate = queue_dic["from_date"];

	}
	if (queue_dic.find("to_date") != queue_dic.end())
	{
		params.endDate = queue_dic["to_date"];
	}
	if (queue_dic.find("symbol") != queue_dic.end())
	{
		params.fileName = GetDataPath() + queue_dic["symbol"] + ".csv";
	}

}




