#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>

#include "util.h"


#include "optimizer.h"
#include "indicator.h"

#include "cl_optimizer.h"



#include "cl_stochastic_optimizer.h"



Cl_stochastic_optimizer::Cl_stochastic_optimizer()
{
	input_ind_array_ = NULL;
	input_price_ = NULL;
	input_params_list_ = NULL;
	outputResult_ = NULL;

	strcpy(kernel_name_, "stochastic.cl");
	strcpy(kernel_function_name_, "stochastic_kernel");

}


Cl_stochastic_optimizer::~Cl_stochastic_optimizer()
{
	if (input_ind_array_)
		_aligned_free(input_ind_array_);
	if (input_price_)
		_aligned_free(input_price_);
	if (input_params_list_)
		_aligned_free(input_params_list_);
	if (outputResult_)
		_aligned_free(outputResult_);
}


int Cl_stochastic_optimizer::CreateAndInitBuffers()
{
	cl_uint kCount = 40,dCount=40,slowing_count=1;
	array_size_ = indicator_->GetCount();
	// allocate working buffers. 
	// the buffer should be aligned with 4K page and size should fit 64-byte cached line
	array_byte_size_ = ((sizeof(cl_int) * array_size_ - 1) / 64 + 1) * 64;
	ind_array_size_ = array_size_*kCount*dCount;
	ind_array_byte_size_ = ((sizeof(cl_int) * (ind_array_size_)-1) / 64 + 1) * 64;
	input_ind_array_ = (cl_int*)_aligned_malloc(ind_array_byte_size_, 4096);
	input_price_ = (cl_int*)_aligned_malloc(array_byte_size_, 4096);
	if (NULL == input_ind_array_ || NULL == input_ind_array_)
	{
		LogError("Error: _aligned_malloc failed to allocate buffers.\n");
		return -1;
	}

	for (cl_uint i = 0; i < array_size_; ++i)
	{
		input_price_[i] = static_cast<int>(indicator_->GetPrice(i, PRICE_OPEN)*10000.00);
	}

	for (cl_uint k = 1; k < kCount; ++k)
			for (cl_uint j = 0; j < array_size_; ++j)
		{
			int idx = ((k)*array_size_) + j;
			 int temp = static_cast<int>(indicator_->GetStochastic_K(j, k + 1)*100);
			 input_ind_array_[idx] = temp;
		}

	test_option_count_ = 0;
	for (cl_uint k = 0; k < kCount; ++k)
		for (cl_uint d = 0; d < dCount; ++d)
			for (cl_uint s = 0; s < slowing_count; s++)
			{
				cl_int item = MAKELONG(s, MAKEWORD(k, d));
				ma_pair_list.push_back(item);
				test_option_count_++;
			}
	params_byte_size_ = ((sizeof(cl_int) * test_option_count_ - 1) / 64 + 1) * 64;;
	input_params_list_ = (cl_int*)_aligned_malloc(params_byte_size_, 4096);
	for (size_t i = 0; i < ma_pair_list.size(); i++)
		input_params_list_[i] = ma_pair_list[i];

	outputResult_ = (cl_int*)_aligned_malloc(params_byte_size_, 4096);
	for (size_t i = 0; i < ma_pair_list.size(); i++)
		outputResult_[i] = 0;
	return CL_SUCCESS;
}




/*
* Create OpenCL buffers from host memory
* These buffers will be used later by the OpenCL kernel_
*/
int  Cl_stochastic_optimizer::CreateBufferArguments()
{

	cl_int err = CL_SUCCESS;


	err = CreateAndInitBuffers();
	if (CL_SUCCESS != err)
	{
		LogError("Error: CreateAndInitBuffers returned \n");
		return err;
	}

	// Create new OpenCL buffer objects
	// As these buffer are used only for read by the kernel, you are recommended to create it with flag CL_MEM_READ_ONLY.
	// Always set minimal read/write flags for buffers, it may lead to better performance because it allows runtime
	// to better organize data copying.
	// You use CL_MEM_COPY_HOST_PTR here, because the buffers should be populated with bytes at inputA and inputB.

	mem_ind_ = clCreateBuffer(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uint) * ind_array_byte_size_, input_ind_array_, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateBuffer for inputMA1 returned %s\n", TranslateOpenCLError(err));
		return err;
	}


	mem_prices_ = clCreateBuffer(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uint) * array_byte_size_, input_price_, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateBuffer for inputPrice returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	mem_options_ = clCreateBuffer(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_uint) * params_byte_size_, input_params_list_, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateBuffer for inputPrice returned %s\n", TranslateOpenCLError(err));
		return err;
	}


	// If the output buffer is created directly on top of output buffer using CL_MEM_USE_HOST_PTR,
	// then, depending on the OpenCL runtime implementation and hardware capabilities, 
	// it may save you not necessary data copying.
	// As it is known that output buffer will be write only, you explicitly declare it using CL_MEM_WRITE_ONLY.
	mem_result_ = clCreateBuffer(context_, CL_MEM_READ_WRITE, sizeof(cl_uint) * params_byte_size_, NULL, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateBuffer for dstMem returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	return CL_SUCCESS;
}

/*
* Set kernel_ arguments
*/
cl_uint Cl_stochastic_optimizer::SetKernelArguments()
{
	cl_int err = CL_SUCCESS;
	int index = 0;

	err = clSetKernelArg(kernel_, index++, sizeof(cl_mem), (void *)&mem_ind_);
	if (CL_SUCCESS != err)
	{
		LogError("error: Failed to set argument ma1, returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	err = clSetKernelArg(kernel_, index++, sizeof(cl_mem), (void *)&mem_prices_);
	if (CL_SUCCESS != err)
	{
		LogError("Error: Failed to set argument prices_, returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	err = clSetKernelArg(kernel_, index++, sizeof(cl_mem), (void *)&mem_options_);
	if (CL_SUCCESS != err)
	{
		LogError("Error: Failed to set argument options_, returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	err = clSetKernelArg(kernel_, index++, sizeof(cl_mem), (void *)&mem_result_);
	if (CL_SUCCESS != err)
	{
		LogError("Error: Failed to set argument prices_, returned %s\n", TranslateOpenCLError(err));
		return err;
	}
	err = clSetKernelArg(kernel_, index++, sizeof(size_t), (void *)&array_size_);
	if (CL_SUCCESS != err)
	{
		LogError("Error: Failed to set argument size, returned %s\n", TranslateOpenCLError(err));
		return err;
	}
	err = clSetKernelArg(kernel_, index++, sizeof(size_t), (void *)&ind_array_size_);
	if (CL_SUCCESS != err)
	{
		LogError("Error: Failed to set argument size, returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	return err;
}


/*
* "Read" the result_ buffer (mapping the buffer to the host memory address)
*/
long Cl_stochastic_optimizer::ReadAndVerify(tradeParameters* optResult)
{
	cl_int err = CL_SUCCESS;
	bool ret = true;

	// Enqueue a command to map the buffer object (dstMem) into the host address space and returns a pointer to it
	// The map operation is blocking
	//	cl_int *resultPtr = (cl_int *)clEnqueueMapBuffer(command_queue_, this->mem_result_,
	//		true, CL_MAP_READ, 0, sizeof(cl_uint) * result_size_, 0, NULL, NULL, &err);

	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueMapBuffer returned %s\n", TranslateOpenCLError(err));
		return false;
	}

	// Call clFinish to guarantee that output region is updated
	err = clFinish(command_queue_);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clFinish returned %s\n", TranslateOpenCLError(err));
	}


	// Unmapped the output buffer before releasing it
	cl_int *resultPtr = (cl_int*)_aligned_malloc(sizeof(cl_int) * test_option_count_, 4096);
	err = clEnqueueReadBuffer(command_queue_, mem_result_, CL_TRUE, 0, sizeof(cl_int) * test_option_count_, resultPtr, 0, NULL, NULL);
	//	err = clEnqueueUnmapMemObject(command_queue_, mem_result_, resultPtr, 0, NULL, NULL);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueUnmapMemObject returned %s\n", TranslateOpenCLError(err));
	}

	long temp = 0;
	long best = 0;
	for (cl_int i = 0; i < test_option_count_; ++i)
	{
		temp = resultPtr[i];
		int slowing = (ma_pair_list[i] & 0xFF);
		int K = ((ma_pair_list[i] >> 16) & 0xFF);
		int D = ((ma_pair_list[i] >> 24) & 0xFF);
	//	LogInfo("Stochastic -profit [%d,%d,%d]=%d\n", K, D, slowing, temp);
		if (best < temp) {
			optResult->params[0] = K;
			optResult->params[1] = D;
			optResult->params[2] = slowing;
			best = temp;
		}
	}

	optResult->counts = 3;

	return best;
}




