#ifndef _CL_MA_OPTIMIZER_H_
#define _CL_MA_OPTIMIZER_H_

class Cl_ma_optimizer : public Cl_optimizer
{
public:
	Cl_ma_optimizer();
	virtual ~Cl_ma_optimizer();
protected:


public:

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

