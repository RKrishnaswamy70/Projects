/*
 * Knapsack.cpp
 *
 *  Created on: Nov 8, 2022
 *      Author: Family
 */
#include "Knapsack.h"

#include <vector>
#include <iostream>

// We use a two dimensional map each of whose entries is a Choice.
// The (x,y)'th entry is the most optimum choice of items to pick
// for a knapsack whose weight limit is x, and which picks from
// elements at indices 0..y.  The number of rows is  the maximum
// weight.  The number of columns is the number is the number of
// entries in the arrays wt and val arrays of Knapsack::Run().

// The class Choice specifies the choice being made to add an entry
// (x,y) to the choice map.
class Choice {
public:
	// The preferred weight-limit and item-index which was used to determine
	// the current choice.
	int m_fromWtLimit;
	int m_fromItemLimit;

	// This is true if the item is chosen or false otherwise.
	bool m_isChosen;

	// The total weight of the current choice.
	int m_totalWt;
	int m_totalVal;

	Choice()
	{
		m_fromWtLimit = -1;
		m_fromItemLimit = -1;
		m_isChosen = false;
		m_totalWt = -1;
		m_totalVal = -1;
	}
};


void Knapsack::Run(int maxWt, const Values& val, const Weights& wt)
{
	const int count = val.size();

	if (count != (int) wt.size()) {
		std::cout << "Sorry ... val and wt must be the same size"
				  << std::endl;
		return;
	}

	// We need a map of choices.  The x'th row, and y'th column entry
	// optimal[x][y] is written as optimal[x,y] in comments.  It is
	// the optimal Choice for a knapsack with maximum weight x, and which
	// picks items indexed upto y (i.e. from 0 to y).
	//
	// The declaration below declares the two dimensional map optimal
	std::vector< std::vector<Choice> > optimal(maxWt + 1, std::vector<Choice>(count));

	// Now we are ready to run the algorithm.  Recall that optimal[x,y]
	// gives information for the optimal choices for a knapsack whose
	// weight-limit is x, and which can choose items at indices 0..y.
	// So how is optimal[x,y] to be computed?  Well, we have 2 cases
	// to consider:
	//    a. The element at index y is chosen.
	//    b. The element at index y is not chosen.
	//
    // Case a: If the element at index y is chosen, then we refer to
	// element optimal[x-wt[y],y-1] and set m_isChosen to true.
	//
	// Case b: If the element at index y is not chosen, then how do we
	// best pack a knapsack of weight-limit x using elements 0..y-1? Well,
	// just refer to optimal[x,y-1] and set m_isChosen to false.
	//
	// First we need to setup the first row and first column.

	// The first row corresponds to a backpack of weight-limit 0.
	// Nothing can be chosen.
	for (int y = 0; y < count; y++) {
		Choice& currentChoice = optimal[0][y];
		currentChoice.m_fromItemLimit = -1; // Undefined
		currentChoice.m_fromWtLimit = -1; // Undefined
		currentChoice.m_isChosen = false;
		currentChoice.m_totalVal = 0;
		currentChoice.m_totalWt = 0;
	}

	// The first column corresponds to a backpack that can only choose or not-choose
	// item 0.  If the weight of this item is wt0, then the choice can be made for
	// weight-limit >= wt0.
	int wt0 = wt[0];
	for (int x = 0; x < maxWt + 1; x++) {
		Choice& currentChoice = optimal[x][0];
		currentChoice.m_fromItemLimit = -1; // Undefined
		currentChoice.m_fromWtLimit = -1; // Undefined

		if (x < wt0) {
			currentChoice.m_isChosen = false;
			currentChoice.m_totalVal = 0;
			currentChoice.m_totalWt = 0;
		} else {
			currentChoice.m_isChosen = true;
			currentChoice.m_totalVal = wt0;
			currentChoice.m_totalWt = wt0;
		}
	}

	// Now that the first row and first column are filled, do the rest of the array.
	// The outermost 2 loops are to loop over the entire optimal array,
	// filling in the x,y values.
	for (int x = 1; x < maxWt + 1; x++) {
		for (int y = 1; y < count; y++) {
			// Ready to fill in optimal[x,y]

			// Case a. The element y is chosen.
			int valIfYIsChosen = -1;
			int wtIfYIsChosen = -1;

			if (x >= wt[y]) {
				valIfYIsChosen = optimal[x-wt[y]][y-1].m_totalVal + val[y];
				wtIfYIsChosen = optimal[x-wt[y]][y-1].m_totalWt + wt[y];
			}
			// Interesting variation:  If one is allowed to choose multiple
			// copies of the element y, then we would use optimal[x-wt[y]][y].
			// That is to say, in this iteration, we would choose y, but y
			// could have been chosen before when the weight limit was
			// x-wt[y] and items were 0..y in optimal[x-wt[y]][y].

			// Case b. The element y is not chosen.
			int valIfYNotChosen = optimal[x][y-1].m_totalVal;
			int wtIfYNotChosen = optimal[x][y-1].m_totalWt;

			bool possibleToChooseY = (wtIfYIsChosen > -1) && (wtIfYIsChosen <= x);
			bool profitableToChooseY = valIfYIsChosen > valIfYNotChosen;

			Choice& currentChoice = optimal[x][y];
			if (possibleToChooseY & profitableToChooseY) {
				// It is possible and profitable to choose y.
				currentChoice.m_fromWtLimit = x - wt[y];
				currentChoice.m_fromItemLimit = y - 1;
				currentChoice.m_isChosen = true;
				currentChoice.m_totalVal = valIfYIsChosen;
				currentChoice.m_totalWt = wtIfYIsChosen;
			} else {
				// It is either not possible or not profitable to choose y.
				currentChoice.m_fromWtLimit = x;
				currentChoice.m_fromItemLimit = y - 1;
				currentChoice.m_isChosen = false;
				currentChoice.m_totalVal = valIfYNotChosen;
				currentChoice.m_totalWt = wtIfYNotChosen;
			}
		} // end of y-loop for items
	} // end of x-loop for weights

	// Now to print the solution.
	int x,y;
	x = maxWt;
	y = count - 1;
	while (x > 0) {
		Choice& choice = optimal[x][y];
		if (choice.m_isChosen) {
			std::cout << "Item " << y
					  << ", Weight-limit " << x
					  << ", Item-limit " << y
					  << std::endl;
		}

		x = choice.m_fromWtLimit;
		y = choice.m_fromItemLimit;
	}
}


