#ifndef _CL_OPTIMIZER_H_
#define _CL_OPTIMIZER_H_

// Macros for OpenCL versions
#define OPENCL_VERSION_1_2  1.2f
#define OPENCL_VERSION_2_0  2.0f


class Cl_optimizer
{
public:
	Cl_optimizer();
	Cl_optimizer(CIndicator *,testParams* );
	virtual ~Cl_optimizer();
protected:
	// Regular OpenCL objects:
	cl_context       context_;           // hold the context_ handler
	cl_device_id     device_;            // hold the selected device_ handler
	cl_command_queue command_queue_;      // hold the commands-queue handler
	cl_program       program_;           // hold the program_ handler
	cl_kernel        kernel_;            // hold the kernel_ handler
	float            platform_version_;   // hold the OpenCL platform version (default 1.2)
	float            device_version_;     // hold the OpenCL device_ version (default. 1.2)
	float            compiler_version_;   // hold the device_ OpenCL C version (default. 1.2)

										  // Objects that are specific for algorithm implemented in this sample
	cl_mem           mem_ind_;              // hold first source buffer
	cl_mem			 mem_prices_;
	cl_mem			 mem_result_;
	cl_mem           mem_params_;            // hold destination buffer
	cl_mem           mem_options_;
	cl_int           result_size_;
	const char*		 TranslateOpenCLError(cl_int errorCode);
	char             kernel_name_[256];
	char             kernel_function_name_[256];
	CIndicator*      indicator_;
	testParams*      params_;
	cl_uint			 global_id_size_;
	cl_int			 test_option_count_;
	cl_int*			 outputResult_;
public:
	bool CheckPreferredPlatformMatch(cl_platform_id platform, const char* preferredPlatform);
	cl_platform_id FindOpenCLPlatform(const char* preferredPlatform, cl_device_type deviceType);
	int GetPlatformAndDeviceVersion(cl_platform_id platformId);
	int SetupOpenCL(cl_device_type deviceType);
	int CreateAndBuildProgram();
	cl_uint ExecuteAddKernel();

	
	void SetIndicator(CIndicator* ind)
	{
		indicator_ = ind;
	}

	void SetParams(testParams* params)
	{
		params_ = params;
	}

	int CreateKernel();
	int ExecuteKernel();

	int testOpenClOptimizer(testParams params, CIndicator* ind, tradeParameters* optResult);
	//virtual functions
public:
	virtual int CreateBufferArguments();
	virtual cl_uint SetKernelArguments();
	virtual long ReadAndVerify(tradeParameters* optResult);

};


#endif

