#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>
#include <CL/cl.h>
#include "stdafx.h"


#include "util.h"






#include "optimizer.h"
#include "indicator.h"

#include "cl_context.h"

Cl_context::Cl_context():optimizer_(NULL)
{

}

Cl_context::~Cl_context()
{
	if (optimizer_)
		delete optimizer_;
}



int Cl_context::BeginTest(testParams* params, CIndicator* ind, tradeParameters* optResult, positionsReport* optReport)
{
	cl_device_type deviceType = CL_DEVICE_TYPE_GPU;

#ifdef _WIN32
	deviceType = CL_DEVICE_TYPE_CPU;
#endif

#ifdef PERFORMANCE_CHEKING
	LARGE_INTEGER perfFrequency;
	LARGE_INTEGER performanceCountNDRangeStart;
	LARGE_INTEGER performanceCountNDRangeStop;
#endif
	optimizer_->SetIndicator(ind);
	optimizer_->SetParams(params);
	//initialize Open CL objects (context_, queue, etc.)
	LOGLINE("SetupOpenCL");
	if (CL_SUCCESS != optimizer_->SetupOpenCL(deviceType))
	{
		return -1;
	}


	// Create OpenCL buffers from host memory
	// These buffers will be used later by the OpenCL kernel_
	LOGLINE("CreateBufferArguments");
	if (CL_SUCCESS != optimizer_->CreateBufferArguments())
	{
		return -1;
	}

	// Create and build the OpenCL program
	LOGLINE("CreateAndBuildProgram");
	if (CL_SUCCESS != optimizer_->CreateAndBuildProgram())
	{
		return -1;
	}

	LOGLINE("CreateKernel");
	if (CL_SUCCESS != optimizer_->CreateKernel())
	{
		return -1;
	}

	// Passing arguments into OpenCL kernel.
	LOGLINE("SetKernelArguments");
	if (CL_SUCCESS != optimizer_->SetKernelArguments())
	{
		return -1;
	}

	// Regularly you wish to use OpenCL in your application to achieve greater performance results
	// that are hard to achieve in other ways.
	// To understand those performance benefits you may want to measure time your application spent in OpenCL kernel_ execution.
	// The recommended way to obtain this time is to measure interval between two moments:
	//   - just before clEnqueueNDRangeKernel is called, and
	//   - just after clFinish is called
	// clFinish is necessary to measure entire time spending in the kernel, measuring just clEnqueueNDRangeKernel is not enough,
	// because this call doesn't guarantees that kernel_ is finished.
	// clEnqueueNDRangeKernel is just enqueue new command in OpenCL command queue and doesn't wait until it ends.
	// clFinish waits until all commands in command queue are finished, that suits your need to measure time.
	bool queueProfilingEnable = true;
#ifdef PERFORMANCE_CHEKING
	if (queueProfilingEnable)
		QueryPerformanceCounter(&performanceCountNDRangeStart);
#endif
	// Execute (enqueue) the kernel_
	LOGLINE("ExecuteAddKernel");
	if (CL_SUCCESS != optimizer_->ExecuteAddKernel())
	{
		return -1;
	}
#ifdef PERFORMANCE_CHEKING
	if (queueProfilingEnable)
		QueryPerformanceCounter(&performanceCountNDRangeStop);
#endif
	// The last part of this function: getting processed results back.
	// use map-unmap sequence to update original memory area with output buffer.
	LOGLINE("ReadAndVerify");
	int best =optimizer_->ReadAndVerify(optResult);

	LogInfo("best profit for %d,%d=%d\n", optResult->params[0], optResult->params[1], best);
	optReport->lProfitPips = best;
	// retrieve performance counter frequency
#ifdef PERFORMANCE_CHEKING
	if (queueProfilingEnable)
	{
		QueryPerformanceFrequency(&perfFrequency);
		LogInfo("NDRange performance counter time %f ms.\n",
			1000.0f*(float)(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / (float)perfFrequency.QuadPart);
	}
#endif

	return 0;

}

