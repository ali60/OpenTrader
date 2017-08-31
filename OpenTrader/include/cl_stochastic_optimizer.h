#ifndef _CL_STOCHASTIC_OPTIMIZER_H__
#define _CL_STOCHASTIC_OPTIMIZER_H__

class Cl_stochastic_optimizer :
	public Cl_optimizer
{
public:
	Cl_stochastic_optimizer();
	virtual ~Cl_stochastic_optimizer();
	int CreateBufferArguments();

	cl_uint SetKernelArguments();

	long ReadAndVerify(tradeParameters* optResult);
private:

	int CreateAndInitBuffers();
	std::vector<cl_int> ma_pair_list;


	cl_int* input_ind_array_;
	cl_int *input_price_;
	cl_int *input_params_list_;
	cl_uint array_byte_size_;
	cl_uint array_size_;
	cl_uint ind_array_size_;
	cl_uint ind_array_byte_size_;
	cl_int  params_size_;
	cl_int  params_byte_size_;

};


#endif

