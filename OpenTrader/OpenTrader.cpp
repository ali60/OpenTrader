#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>
#include <map>
#include <sstream>
#include "stdafx.h"

#include "optimizer.h"
#include "util.h"
#include "Job_queue.h"


 

void convert_trade_result_to_map(positionsReport repport,
	std::map < std::string, std::string>& result_dic)
{
	std::stringstream strstream;
	strstream << repport.lProfitPips;
	std::string s;
	strstream >> s;
	result_dic.insert(std::make_pair("profit", s));
	strstream << repport.iSuccessRaio;
	strstream >> s;
	result_dic.insert(std::make_pair("success_rate", s));
	strstream << repport.iTotalTrades;
	strstream >> s;
	result_dic.insert(std::make_pair("trades", s));
	strstream << repport.iSuccess;
	strstream >> s;
	result_dic.insert(std::make_pair("success_trades", s));
}


void sqs_main()
{
#ifdef	_SUPPORT_SQS
	Job_queue fifo;
	while (1) {
		if (fifo.get_next_job())
		{
			testParams params;
			tradeParameters optResult;
			positionsReport rep;
			convert_trade_params( fifo.get_message_dic(), params);
			params.optType = OPT_TYPE_BF_OPENCL;
			if (RunTest(params, &optResult, &rep)) {
				std::map < std::string, std::string> result_dic;
					
				convert_trade_result_to_map(rep, result_dic);
				fifo.store_result_db(result_dic);

			}
		}
		
	}
#endif
}



int main(int argc, char *argv[])
{
	if (argc == 1) {
		sqs_main();
		return 0;
	}
	else
	{
		testParams params;
		tradeParameters optResult;
		positionsReport rep;
		memset(&optResult, 0, sizeof(optResult));
		params.beginDate = "20170101";
		params.endDate = "20170401";
		params.optType = OPT_TYPE_BF_OPENCL;
		params.indIndex = IND_TYPE_STOCHASTIC;
		params.fileName = GetDataPath() + std::string("EURUSD1440.csv");
		LOGLINE("RunTest");
		RunTest(params, &optResult, &rep);
		char param_str[200] = { 0 };
		GenerateParamsStr(optResult, param_str);
		LogInfo("result = %s=%d\n", param_str, rep.lProfitPips);
#ifdef _WIN32
		getchar();
#endif
	}
	return 0;
}

