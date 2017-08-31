#pragma once
class optimizer_factory
{
public:
	optimizer_factory();
	~optimizer_factory();
	static Cl_optimizer* create_cl_optimizer(int ind_type);
};

