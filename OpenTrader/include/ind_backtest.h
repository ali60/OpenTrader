#pragma once

void backTestMA(CIndicator * pInd, tradeParameters tradeParam, positionsReport* report, bool bLog = false);
void backTestMACD(CIndicator * pInd, tradeParameters tradeParam, positionsReport* report, bool bLog = false);
void backTestStochastic(CIndicator * pInd, tradeParameters tradeParam, positionsReport* report, bool bLog = false);

