#include "stdafx.h"
#include "indicator.h"
#include "optimizer.h"
#include "position.h"
#include "ind_backtest.h"
#include "util.h"

void backTestMA(CIndicator * pInd, tradeParameters tradeParam, positionsReport* report, bool bLog )
{
	CPosition order(pInd, bLog);
	order.m_bStock = pInd->m_bStock;


	bool bGoodOne = false;
	float dTP = 0, dSL = 0;


	int iOperations = 0;


	int iCount = 0;

	int ma1 = (int)tradeParam.params[iCount++];//0
	int ma2 = (int)tradeParam.params[iCount++];//1
	int iTP1 = (int)tradeParam.params[iCount++];//2
	int iTP2 = (int)tradeParam.params[iCount++];//3

	bool bSell = true, bBuy = true;


	bool bReverse = false;
	if (iTP1<0)
	{
		iTP1 = -1 * iTP1;
		bReverse = true;

	}


	int profit = 0;
	bool bBegin = false;

	int len = pInd->GetCount(), item;

	bool bTradeNoMore = false;
	float nMab3 = 0, lastMab2 = 0;
	float nMas3 = 0, lastMas2 = 0;
	float dRSI = 0;

	order.reset();
	float lastr = 0, nMa1 = 0, nMa2 = 0, oMa1 = 0, oMa2=0, k = 0, dMaSt = 0;
	float dPrice = 0;
	iOperations++;

	item = -1;
	if (ma1>ma2)
		std::swap(ma1, ma2);
	//initialize MAs
//	if (pInd->IsValid(item))
//	{
//		nMa1 = pInd->GetMa(item, ma1, PRICE_OPEN);
//		oMa1 = pInd->GetMa(item, ma2, PRICE_OPEN);
//	}

	int iTrades = 0, iPOSType, iTradeFrame = 0;
	int iTP = 0, iSL = 0, i;


	//the main loop
	for (i = 0; i<len; i++)
	{
		item = i;

		dPrice = pInd->GetPrice(item, PRICE_OPEN);

		nMa1 = pInd->GetMa(item, ma1, PRICE_OPEN);
		nMa2 = pInd->GetMa(item, ma2, PRICE_OPEN);




		if (nMa1 >= nMa2 && oMa1 < oMa2 //if cross for buy
			&& bBuy
			)
		{
			for (int cnt = 1; cnt <= order.OrdersTotal(); cnt++)
			{
				iTrades++;
				order.ClosePosition(cnt, i, PRICE_OPEN);
			}
			if (iTP1)
				dTP = dPrice + (((float)iTP1) / (float)pInd->m_kValue);
			if (iSL)
				dSL = dPrice - (((float)iSL) / (float)pInd->m_kValue);
			iPOSType = POS_BUY;
			iTradeFrame = item;
			order.OpenPosition(iPOSType, item, dPrice, dTP, dSL);

		}
		if (nMa1 <= nMa2 && oMa1 > oMa2 //if cross for sell
			&& bSell
			)
		{
			for (int cnt = 1; cnt <= order.OrdersTotal(); cnt++)
			{
				iTrades++;
				order.ClosePosition(cnt, i, PRICE_OPEN);
			}

			if (iTP2)
				dTP = dPrice - (((float)iTP2) / pInd->m_kValue);
			if (iSL)
				dSL = dPrice + (((float)iSL) / pInd->m_kValue);
			iPOSType = POS_SELL;
			iTradeFrame = item;
			order.OpenPosition(iPOSType, item, dPrice, dTP, dSL);
		}

		if (order.Tick(item))
		{
			for (int iTkt = 1; iTkt <= order.OrdersTotal(); iTkt++)
				order.ClosePosition(iTkt, item, PRICE_OPEN);
		}

		oMa1 = nMa1;
		oMa2 = nMa2;
	}
//	for (int cnt = 1; cnt <= order.OrdersTotal(); cnt++)
//		order.ClosePosition(cnt, i - 1, PRICE_OPEN);
	bGoodOne = false;
	memset(report, 0, sizeof(positionsReport));
	order.GetReport(report);
	LogInfo("test %d,%d = %d\n", ma1,ma2,report->lProfitPips);

}


void backTestMACD(CIndicator * pInd, tradeParameters tradeParam, positionsReport* report, bool bLog )
{
	CPosition order(pInd, bLog);
	order.m_bStock = pInd->m_bStock;

	bool bGoodOne = false;
	float dTP = 0, dSL = 0;

	int iCount = 0;

	int ma1 = (int)tradeParam.params[iCount++];//0
	int ma2 = (int)tradeParam.params[iCount++];//1
	int ma3 = (int)tradeParam.params[iCount++];//2

	bool bSell = true, bBuy = true;

	bool bReverse = false;


	int profit = 0;
	bool bBegin = false;
	pInd->ResetIndicator();

	int len = pInd->GetCount(), item;

	bool bTradeNoMore = false;

	order.reset();
	float MacdCurrent = 0, MacdPrevious = 0, SignalCurrent = 0, SignalPrevious = 0;
	float dPrice = 0;

	item = -1;
	if (ma1>ma2)
		len = 0;

	int iTrades = 0, iPOSType, iTradeFrame = 0;
	int iTP = 0, iSL = 0, i;


	//the main loop
	for (i = 0; i<len; i++)
	{
		item = i;

		dPrice = pInd->GetPrice(item, PRICE_OPEN);

		MacdCurrent = pInd->iMACD(item, ma1, ma2, ma3, PRICE_OPEN, METHOD_MAIN);
		SignalCurrent = pInd->iMACD(item, ma1, ma2, ma3, PRICE_OPEN, METHOD_SIGNAL);

		if (MacdPrevious <0 && MacdCurrent>SignalCurrent && MacdPrevious<=SignalPrevious)
		{
			for (int cnt = 1; cnt <= order.OrdersTotal(); cnt++)
			{
				iTrades++;
				order.ClosePosition(cnt, i, PRICE_OPEN);
			}
			if (iTP)
				dTP = dPrice + (((float)iTP) / pInd->m_kValue);
			if (iSL)
				dSL = dPrice - (((float)iSL) / pInd->m_kValue);
			iPOSType = POS_BUY;
			iTradeFrame = item;
			order.OpenPosition(iPOSType, item, dPrice, dTP, dSL);
		}
		if (MacdPrevious >0 && MacdCurrent<SignalCurrent && MacdPrevious>=SignalPrevious)
		{
			for (int cnt = 1; cnt <= order.OrdersTotal(); cnt++)
			{
				iTrades++;
				order.ClosePosition(cnt, i, PRICE_OPEN);
			}
			if (iTP)
				dTP = dPrice + (((float)iTP) / pInd->m_kValue);
			if (iSL)
				dSL = dPrice - (((float)iSL) / pInd->m_kValue);
			iPOSType = POS_SELL;
			iTradeFrame = item;
			order.OpenPosition(iPOSType, item, dPrice, dTP, dSL);
		}

		MacdPrevious = MacdCurrent;
		SignalPrevious = SignalCurrent;

		if (order.Tick(item))
		{
			for (int iTkt = 1; iTkt <= order.OrdersTotal(); iTkt++)
				order.ClosePosition(iTkt, item, PRICE_OPEN);
		}

	}
	for (int cnt = 1; cnt <= order.OrdersTotal(); cnt++)
		order.ClosePosition(cnt, i - 1, PRICE_OPEN);
	bGoodOne = false;
	memset(report, 0, sizeof(positionsReport));
	order.GetReport(report);

}


void backTestStochastic(CIndicator * pInd, tradeParameters tradeParam, positionsReport* report, bool bLog)
{
	CPosition order(pInd, bLog);
	order.m_bStock = pInd->m_bStock;

	bool bGoodOne = false;
	float dTP = 0, dSL = 0;

	int iCount = 0;

	int iK = (int)tradeParam.params[iCount++];//0
	int iD = (int)tradeParam.params[iCount++];//1
	int iSlowing = (int)tradeParam.params[iCount++];//1
	int ilevel1 = (int)tradeParam.params[iCount++];//2

	int profit = 0;
	bool bBegin = false;

	int len = pInd->GetCount(), item;
	pInd->ResetIndicator();

	bool bTradeNoMore = false;

	order.reset();
	float dK = 0, dD = 0;
	float dLastK = 0, dLastD = 0;
	float dPrice = 0;

	item = -1;

	int iTrades = 0, iPOSType, iTradeFrame = 0;
	int iTP = 0, iSL = 0, i;


	//the main loop
	for (i = 0; i<len; i++)
	{
		item = i;

		dPrice = pInd->GetPrice(item, PRICE_OPEN);

		dK = pInd->StochasticK(item, iK, iSlowing);
		dD = pInd->StochasticD(item, iK, iD, iSlowing);


		if (dK < ilevel1 && dLastK<dLastD && dK >= dD)
		{
			for (int cnt = 1; cnt <= order.OrdersTotal(); cnt++)
			{
				iTrades++;
				order.ClosePosition(cnt, i, PRICE_OPEN);
			}
			if (iTP)
				dTP = dPrice + (((float)iTP) / pInd->m_kValue);
			if (iSL)
				dSL = dPrice - (((float)iSL) / pInd->m_kValue);
			iPOSType = POS_BUY;
			iTradeFrame = item;
			order.OpenPosition(iPOSType, item, dPrice, dTP, dSL);

		}

		if (dK > (100-ilevel1) && dLastK>dLastD && dK <= dD)
		{
			for (int cnt = 1; cnt <= order.OrdersTotal(); cnt++)
			{
				iTrades++;
				order.ClosePosition(cnt, i, PRICE_OPEN);
			}
			if (iTP)
				dTP = dPrice + (((float)iTP) / pInd->m_kValue);
			if (iSL)
				dSL = dPrice - (((float)iSL) / pInd->m_kValue);
			iPOSType = POS_SELL;
			iTradeFrame = item;
			order.OpenPosition(iPOSType, item, dPrice, dTP, dSL);
		}

		if (order.Tick(item))
		{
			for (int iTkt = 1; iTkt <= order.OrdersTotal(); iTkt++)
				order.ClosePosition(iTkt, item, PRICE_OPEN);
		}
		dLastK = dK;
		dLastD = dD;

	}
	for (int cnt = 1; cnt <= order.OrdersTotal(); cnt++)
		order.ClosePosition(cnt, i - 1, PRICE_OPEN);
	bGoodOne = false;
	memset(report, 0, sizeof(positionsReport));
	order.GetReport(report);

}
