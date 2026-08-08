/*
 * Knapsack.h
 *
 *  Created on: Nov 8, 2022
 *      Author: Family
 */

#ifndef KNAPSACK_H_
#define KNAPSACK_H_

#include <vector>

// This class implements a solution to the 0/1 Knapsack dynamic programming
// problem.  The problem is that a thief has a backpack which can take a
// maximum weight of W.  The residence has n items, each of which has a
// weight wt[i] and a value val[i].  He can either choose to take or not
// take the i'th item (hence the name 0/1).  The total weight cannot exceed
// the weight W of his backpack, and he needs to maximize the total value.
//
// The input is two arrays wt[i] and val[i] of the same length.  The result
// is a list of the indices of the objects he does take.
//
// So the input are two vectors wt[i] and val[i], and a value maxWt that is
// the weight the backpack can carry.  The output is a list of the indices
// i which he does pick.

// The weights vector is just an array of integers
typedef std::vector<int> Values;

// The values vector is also just an array of integers
typedef std::vector<int> Weights;

class Knapsack {
public:
	Knapsack()
	{}

	void Run(int maxWt, const Values& val, const Weights& wt);
};




#endif /* KNAPSACK_H_ */
