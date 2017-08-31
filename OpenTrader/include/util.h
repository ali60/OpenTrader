#ifndef _FXTRADER_UTIL_H__
#define _FXTRADER_UTIL_H__

#include "optimizer.h"

struct appUmf
{
	int spread;
	bool  jpb;
	bool  stock;
	std::string name;
	int kValue;
	int time_frame;
	BYTE* pData;
	int datalen;
	int rec_count;
};

int  LoadData(const char* i_lpszFileName, appUmf* umf);

// Print useful information to the default output. Same usage as with printf
void LogInfo(const char* str, ...);

// Print error notification to the default output. Same usage as with printf
void LogError(const char* str, ...);

// Read OpenCL source code from fileName and store it in source. The number of read bytes returns in sourceSize
int ReadSourceFromFile(const char* fileName, char** source, size_t* sourceSize);

std::string GetDataPath();

void GenerateParamsStr(tradeParameters param, char* out_str);

void convert_trade_params(std::map < std::string, std::string> queue_dic, testParams& params);

#ifdef WIN32 
#define LOGLINE(x)  LogInfo("execution %d=%s\n",__LINE__,x)
#define PERFORMANCE_CHEKING

#else
#define LOGLINE(x)  LogInfo("execution %d=%s\n",__LINE__,x)
//#define LOGLINE(x)    
void* _aligned_malloc(size_t _Size,size_t _Alignment);

#define _aligned_free   free

#define MAKEWORD(a, b)      ((unsigned short)(((unsigned char)(((a)) & 0xff)) | ((unsigned short)((unsigned char)(((b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((unsigned long)(((unsigned short)(((a)) & 0xffff)) | ((unsigned long)((unsigned short)(((b)) & 0xffff))) << 16))

#endif


#endif
