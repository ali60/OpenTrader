#include "stdafx.h"
#include <fstream>
#include <memory>
#include <vector>
#include "indicator.h"
#include "util.h"

#include "position.h"
#include "optimizer.h"
#include "ind_backtest.h"

#ifdef _SUPPORT_CL_
#include <CL/cl.h>

#include "cl_optimizer.h"

#include "cl_context.h"
#include "optimizer_factory.h"
#endif


using namespace std;




void initTradeParams(int indIndex, tradeParameters& param)
{
	switch (indIndex)
	{
	case IND_TYPE_EMA_CROSS:
		param.counts = 2;
		param.params[0] = 1;
		param.params[1] = 2;
		break;
	case IND_TYPE_RSI:
		param.counts = 2;
		param.params[0] = 1;
		param.params[1] = 1;
		break;
	case IND_TYPE_MACD:
		param.counts = 3;
		param.params[0] = 1;
		param.params[1] = 2;
		param.params[2] = 1;
		break;
	case IND_TYPE_STOCHASTIC:
		param.counts = 4;
		param.params[0] = 1;
		param.params[1] = 2;
		param.params[2] = 1;
		param.params[3] = 5;
		break;
	}
}

bool getBruteForceNextParam(int indIndex, tradeParameters& param)
{
	switch (indIndex)
	{
	case IND_TYPE_EMA_CROSS:
		param.params[1]+=1;
		if (param.params[1] > 30) {
			param.params[0] += 1;
			param.params[1] = param.params[0]+1;
			if (param.params[0] > 30)
				return false;
		}
		break;
	case IND_TYPE_RSI:
		param.params[1] += 2;
		if (param.params[1] > 30) {
			param.params[1] = param.params[0];
			param.params[0] += 2;
			if (param.params[0] > 30)
				return false;
		}
		break;
	case IND_TYPE_MACD:
		param.params[1] += 2;
		if (param.params[1] > 30) {
			param.params[1] = param.params[0];
			param.params[0] += 2;
			if (param.params[0] > 30)
			{
				param.params[2] += 2;
				param.params[0] = 1;
				param.params[1] = 2;
				if (param.params[2] > 30)
					return false;
			}
		}
		break;
	case IND_TYPE_STOCHASTIC:
		param.params[1] += 2;
		if (param.params[1] > 30) {
			param.params[1] = param.params[0];
			param.params[0] += 2;
			if (param.params[0] > 30)
			{
				param.params[2] += 2;
				param.params[0] = 1;
				param.params[1] = 2;
				if (param.params[2] > 30)
				{
					param.params[2] = 1;
					param.params[0] = 1;
					param.params[1] = 2;
					param.params[3] += 5;
					if(param.params[3]>40)
						return false;
				}
			}
		}
		break;
	}
	return true;
}


bool runParamTest(int indIndex,CIndicator * pInd, tradeParameters tradeParam,positionsReport* report,bool bLog)
{
	switch (indIndex)
	{
	case IND_TYPE_EMA_CROSS:
		backTestMA(pInd, tradeParam, report,bLog);
		break;
	case IND_TYPE_RSI:
		break;
	case IND_TYPE_MACD:
		backTestMACD(pInd, tradeParam, report,bLog);
		break;
	case IND_TYPE_STOCHASTIC:
		backTestStochastic(pInd, tradeParam, report,bLog);
		break;
	}
	return true;
}


bool bruteForceOptimizer(testParams params, CIndicator* ind, tradeParameters* optResult,positionsReport* optReport)
{
	tradeParameters tradeParam = {0};
	initTradeParams(params.indIndex, tradeParam);
	positionsReport best = { 0 };
	tradeParameters bestParams = { 0 };
	//utility array for loop 
	bool result = true;
	do
	{
		positionsReport out;


		if (!runParamTest(params.indIndex, ind, tradeParam, &out,false))
		{
			result = false;
			break;
		}

		//store the best result
		if (out.lProfitPips > best.lProfitPips) {
			memcpy(&best, &out, sizeof(best));
			memcpy(&bestParams, &tradeParam, sizeof(bestParams));
		}

	} while (getBruteForceNextParam(params.indIndex, tradeParam));

	memcpy(optResult, &bestParams, sizeof(tradeParameters));
	memcpy(optReport, &best, sizeof(positionsReport));
	runParamTest(params.indIndex, ind, bestParams, &best, true);
	return result;
}

bool testOpenClOptimizer(testParams params, CIndicator* ind, tradeParameters* optResult,  positionsReport* optReport)
{
#ifdef _SUPPORT_CL_
	tradeParameters tradeParam = { 0 };
	tradeParameters bestParams = { 0 };
	//utility array for loop 
	bool result = true;
	Cl_optimizer* cl_opt= optimizer_factory::create_cl_optimizer(params.indIndex);
	Cl_context context;
	LOGLINE("context.SetOptimizer");
	context.SetOptimizer(cl_opt);
	return context.BeginTest(&params, ind, optResult,optReport)==CL_SUCCESS;
#else
	return -1;
#endif
}



bool RunTest(testParams params, tradeParameters* optResult, positionsReport* optReport)
{
	appUmf testData = { 0 };
	if (!LoadData(params.fileName.c_str(), &testData)) {
		LogError("LoadData failed");
		return false;
	}
	shared_ptr<CIndicator> ind(new CIndicator());
	ind->GetData(testData.pData, testData.rec_count);
	if (params.beginDate.size() == 8)
		params.beginDate += "0000";
	if (params.endDate.size() == 8)
		params.endDate += "0000";
	ind->PrepareIndicatorAlldata();
	ind->SetBeginDate(params.beginDate.c_str());
	ind->SetEndDate(params.endDate.c_str());
	ind->m_kValue = testData.kValue;
	ind->m_spread = (float)testData.spread/(float)ind->m_kValue;
	if (params.optType == OPT_TYPE_BRUTE_FORCE)
		return bruteForceOptimizer(params,&*ind,optResult, optReport);
	else if (params.optType == OPT_TYPE_BF_OPENCL) {
		return  testOpenClOptimizer(params, &*ind, optResult, optReport);
	}
	return true;
}

