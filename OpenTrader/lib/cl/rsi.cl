/*****************************************************************************
 * Copyright (c) 2017 OpenTrader
 * All rights reserved.
 *
 *  Author : Ali Khalilvandi
 *****************************************************************************/

 #define BUY		1
 #define SELL		2

 #define _DBG


__kernel void rsi_kernel(__global int* ind_arr,__global int* prices,
					    __global int* params, __global int* result, 
					    int count,int ma_count)
{
    const int id     = get_global_id(0);
	int rsi = (params[id]) & 0xFF;
	int level1 = (params[id] >>  16) & 0xFF;
	int level2 = (params[id] >>  24) & 0xFF;
    int log_more=0;
	if(rsi==4 && level1==20 && level2==80 )
	    log_more=1;
	level1*=10000;
	level2*=10000;

#ifdef _DBG
    if(log_more)
    printf("rsi [%d,%d,%d]\n",rsi,level1,level2);
#endif

	int i;
	int spread=2;
	int curr_rsi,prev_rsi ;
	int pos_type=0,open_price;
	int total_profit=0,trades_count=0,profit;


	for(i=0;i<count;++i){ 
	   curr_rsi = ind_arr [ ((rsi-1)*count) + (i)];
	   if(i < rsi){ 
			continue;
	   }
			 if(log_more)
			    printf("curr_rsi[%d]=%d,%d\n",i,curr_rsi,prev_rsi);
		if (prev_rsi<level1 && curr_rsi>=level1)
	   { 
	      if(pos_type == SELL)
		  { 
		     trades_count++;
			 profit = open_price - prices[i];
		     total_profit += profit;
			 total_profit -= spread; 
			 if(log_more)
			    printf("close sell @%d, total profit=%d,%d\n",prices[i],profit,total_profit);
		  }
	      pos_type = BUY;
		  open_price = prices[i];
		  if(log_more)
			printf("buy  @%d\n",open_price);
	   }

		if (prev_rsi>level2 && curr_rsi<=level2)
	   { 
	      if(pos_type == BUY)
		  { 
		     trades_count++;
			 profit =  prices[i] - open_price;
		     total_profit += profit; 
			 total_profit -= spread; 
			 if(log_more)
			    printf("close buy @%d, total profit=%d,%d\n",prices[i],profit,total_profit);
		  }
	      pos_type = SELL;
		  open_price = prices[i];
		  if(log_more)
			printf("sell @%d\n",open_price);
	   }
		prev_rsi=curr_rsi;  
    }
    result[id] = total_profit;
#ifdef _DBG
	if(log_more)
	    printf("profit [%d,%d]=%d\n",rsi,id,total_profit);
#endif
}
