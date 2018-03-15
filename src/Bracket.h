/*
 * Bracket.h
 *
 *  Created on: Jun 18, 2017
 *      Author: goodws
 */

#ifndef BRACKET_H_
#define BRACKET_H_

#include <vector>
#include "Match.h"
using namespace std;

class Bracket {
public:
	Bracket(int aNumSets, int aMaxRounds);
	//Bracket(Bracket aBracket);
	virtual ~Bracket();

	const Match getMatch(int round_, int set);

	void addToBracket(Match match, int round, int set);

private:
	int num_sets;
	int max_rounds;
	Match** bracket;
};

#endif /* BRACKET_H_ */
