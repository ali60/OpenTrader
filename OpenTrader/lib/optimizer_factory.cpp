#include <vector>
#include <CL/cl.h>

#include "stdafx.h"

#include "util.h"

//for perf. counters
//#include <Windows.h>

#include "optimizer.h"
#include "indicator.h"

#include "cl_optimizer.h"
#include "cl_ma_optimizer.h"
#include "cl_macd_optimizer.h"
#include "cl_rsi_optimizer.h"
#include "cl_stochastic_optimizer.h"
#include "optimizer_factory.h"


optimizer_factory::optimizer_factory()
{
}


optimizer_factory::~optimizer_factory()
{
}


Cl_optimizer* optimizer_factory::create_cl_optimizer(int ind_type)
{
	Cl_optimizer* cl_opt=nullptr;
	if (ind_type == IND_TYPE_EMA_CROSS)
		cl_opt = new Cl_ma_optimizer();
	else if (ind_type == IND_TYPE_MACD)
		cl_opt = new Cl_macd_optimizer();
	else if (ind_type == IND_TYPE_RSI)
		cl_opt = new Cl_rsi_optimizer();
	else if (ind_type == IND_TYPE_STOCHASTIC)
		cl_opt = new Cl_stochastic_optimizer();
	return cl_opt;
}
