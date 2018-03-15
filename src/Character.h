/*
 * Character.h
 *
 *  Created on: Jun 4, 2017
 *      Author: Samuel Goodwin
 */

#ifndef CHARACTER_H_
#define CHARACTER_H_

#include <string>
#include <vector>
#include "Matchup.h"
using namespace std;

class Character {
public:
	Character(string aName);
	~Character();

	const string getName() const { return name; }
	const Matchup getMatchup(int i) const { return matchups[i]; }
	Matchup getMatchup(Character* ch);
	const unsigned int numMatchups() const { return matchups.size(); }

	void addMatchup(Matchup matchup);

private:
	string name;
	vector<Matchup> matchups;
};

#endif /* CHARACTER_H_ */
