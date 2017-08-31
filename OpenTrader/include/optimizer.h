#ifndef _OPTIMIZER_H__
#define _OPTIMIZER_H__

#include "position.h"

#define OPT_TYPE_BRUTE_FORCE	1
#define OPT_TYPE_GENETIC		2
#define OPT_TYPE_BF_OPENCL		3
#define OPT_TYPE_GENETIC_OPENCL 4

#define IND_TYPE_EMA_CROSS		1
#define IND_TYPE_STOCHASTIC		2
#define IND_TYPE_MACD			3
#define IND_TYPE_RSI			4


struct testParams
{
	std::string fileName;
	std::string beginDate;
	std::string endDate;
	int			optType;
	int			indIndex;
};

struct tradeParameters
{
	int			counts;
	int			params[50];
	char		szParamName[50][60];
};

struct tradeOptimizationParams
{
	int			counts;
	int			paramRange[50];
	int			paramStep;
};

bool RunTest(testParams params, tradeParameters* optResult, positionsReport* optReport);

#endif
