#ifndef _CL_CONTEXT_H_
#define _CL_CONTEXT_H_

#include "cl_optimizer.h"

class Cl_context
{
private:
	Cl_optimizer* optimizer_;
public:
	Cl_context();
	~Cl_context();
	void SetOptimizer(Cl_optimizer* optimizer)
	{
		optimizer_ = optimizer;
    }

	int BeginTest(testParams* params, CIndicator* ind, tradeParameters* optResult, positionsReport* optReport);

};


#endif

