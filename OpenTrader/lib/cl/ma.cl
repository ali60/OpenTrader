/*****************************************************************************
 * Copyright (c) 2017 OpenTrader
 * All rights reserved.
 *
 *  Author : Ali Khalilvandi
 *****************************************************************************/

 #define BUY		1
 #define SELL		2

 #define _DBG


__kernel void ma_kernel(__global int* ma,__global int* prices,
					    __global int* params, __global int* result, 
					    int count,int ma_count)
{
    const int id     = get_global_id(0);
	int p1 = (params[id]) & 0xFF;
	int p2 = (params[id] >>  8) & 0xFF;

	int i;
	int spread=2;
	int curr_ma1,curr_ma2 ;
	int pre_ma1=0,pre_ma2=0 ;
	int pos_type=0,open_price;
	int total_profit=0,trades_count=0,profit;
	if(p1>=p2){ 
	   result[id]=0;
	   return;
	}
     int log_more=0;
	 if(p1==2 && p2==0)
	     log_more=1;
	for(i=0;i<count;i++){ 
	   curr_ma1 = ma [ ((p1-1)*count) + (i)];
	   curr_ma2 = ma [ ((p2-1)*count) + (i)];
/*	   if(i==3)
	      printf("mas %d,%d,%d\n",curr_ma1,curr_ma2,ma_count);*/
	   if(curr_ma1 >= curr_ma2 && pre_ma1<pre_ma2)
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

	   if(curr_ma1 <= curr_ma2 && pre_ma1>pre_ma2)
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
	pre_ma1=curr_ma1;  
	pre_ma2=curr_ma2;  
    }
#ifdef _DBG
    //printf("profit [%d,%d]=%d\n",p1,p2,total_profit);
#endif
    result[id] = total_profit;
	if(log_more)
		printf("result[%d] = %d\n",id,result[id]);

}
