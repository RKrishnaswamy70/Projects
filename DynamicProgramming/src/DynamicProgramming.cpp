//============================================================================
// Name        : DynamicProgramming.cpp
// Author      : R. Krishnaswamy
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;
#include "Knapsack.h"

int main() {
	Knapsack knapsack;

	int maxWt = 20;
	Values val = {1,2,3,4,5,6,7,8,9,10};
	Weights wt = {10,9,8,7,6,5,4,3,2,1};

	knapsack.Run(maxWt, val, wt);
}
