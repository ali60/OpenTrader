/*****************************************************************************
 * Copyright (c) 2017 OpenTrader
 * All rights reserved.
 *
 *  Author : Ali Khalilvandi
 *****************************************************************************/

 #define BUY		1
 #define SELL		2

 #define _DBG


__kernel void macd_kernel(__global int* ma,__global int* prices,
					    __global int* params, __global int* result, 
					    int count,int ma_count)
{
    const int id     = get_global_id(0);
	int p_slowing = (params[id]) & 0xFF;
	int p_ma1 = (params[id] >>  16) & 0xFF;
	int p_ma2 = (params[id] >>  24) & 0xFF;
#ifdef _DBG
 //   if(p_slowing==4)
//    printf("macd [%d,%d,%d]\n",p_ma1,p_ma2,p_slowing);
#endif

	int i;
	int spread=2;
	int curr_macd_main,curr_macd_sig ;
	int pre_macd_main=0,pre_macd_sig=0 ;
	int pos_type=0,open_price;
	int total_profit=0,trades_count=0,profit;
	if(p_ma1>=p_ma2){ 
	   result[id]=0;
		printf("macd not valid [%d,%d,%d]\n",p_ma1,p_ma2,p_slowing);
	   return;
	}
	long sum_main=0,curr_ma1,curr_ma2;
	for(i=0;i<count;++i){ 
	   curr_ma1 = ma [ ((p_ma1-1)*count) + (i)];
	   curr_ma2 = ma [ ((p_ma2-1)*count) + (i)];
	   curr_macd_main = curr_ma2 - curr_ma1;
	   if(i < p_slowing){ 
	 	    sum_main += curr_macd_main;
	     	curr_macd_sig = (sum_main/(i+1));
			continue;
	   }
	   curr_macd_sig -= (curr_macd_sig/p_slowing);
	   curr_macd_sig += (curr_macd_main/p_slowing);
	//   if(i==15 && p_slowing==4)
	//      printf("macd res = %d,%d\n",curr_macd_main,curr_macd_sig);
		if (pre_macd_main <0 && curr_macd_main>curr_macd_sig 
		                     && pre_macd_main<=pre_macd_sig)
	   { 
	      if(pos_type == SELL)
		  { 
		     trades_count++;
			 profit = open_price - prices[i];
		     total_profit += profit;
			 total_profit -= spread; 
		  }
	      pos_type = BUY;
		  open_price = prices[i];
	   }

		if (pre_macd_main >0 && curr_macd_main<curr_macd_sig 
		                     && pre_macd_main>=pre_macd_sig)
	   { 
	      if(pos_type == BUY)
		  { 
		     trades_count++;
			 profit =  prices[i] - open_price;
		     total_profit += profit; 
			 total_profit -= spread; 
		  }
	      pos_type = SELL;
		  open_price = prices[i];
	   }
		pre_macd_main=curr_macd_main;  
		pre_macd_sig=curr_macd_sig;  
    }
#ifdef _DBG
    //printf("profit [%d,%d]=%d\n",p_ma1,p_ma2,total_profit);
#endif
    result[id] = total_profit;

}
