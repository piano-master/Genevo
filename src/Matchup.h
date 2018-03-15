/*
 * Matchup.h
 *
 *  Created on: Jun 5, 2017
 *      Author: goodws
 */

#ifndef MATCHUP_H_
#define MATCHUP_H_

#include <string>
using namespace std;

class Matchup {
public:
	Matchup(string aOpponent, int aWins, int aLosses);
	Matchup(string aOpponent, float aP);
	virtual ~Matchup();

	const string getOpponent() const { return opponent; }
	const int numWins() const { return wins; }
	const int numLosses() const { return losses; }
	const float getPValue() const { return p; }

	void incrementWins(int amount) { wins += amount; }
	void incrementLosses(int amount) { losses += amount; }
	void setP(float aP) { p = aP; }
	void updatePValue() { p = ((float)wins) / ((float)(wins+losses)); }

protected:
	string opponent;
	int wins;
	int losses;
	float p;
};

#endif /* MATCHUP_H_ */
