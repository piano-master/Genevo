/*
 * Player.h
 *
 *  Created on: Jun 4, 2017
 *      Author: Samuel Goodwin
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <string>
#include <vector>
#include "Character.h"
#include "Matchup.h"
using namespace std;

class Player {
public:
	Player(string aTag, vector<Character*> aCharacters, int aSeed);
	~Player();

	const string getTag() const { return tag; }
	Character* getCharacter(int i) const { return characters[i]; }
	Matchup* getMatchup(int i);
	Matchup* getMatchup(Player* player);
	const unsigned int numMatchups() const { return p_matchups.size(); }
	const int getSeed() const { return seed; }
	const string getCharacterName(int i) const { return characters[i]->getName(); }
	const unsigned int numCharacters() const { return characters.size(); }
	const void printLog() const;

	void addMatchup(Matchup* matchup) { p_matchups.push_back(matchup); }
	void changeMatchup(Player* player, float p);

	void addToLog(string& entry) { log_.push_back(entry); }
	void clearLog() { log_.clear(); }

	void updatePlacing(int aPlacing) { placing = aPlacing; }
	int checkPerformance();

private:
	string tag;
	int seed;
	int placing;
	vector<Character*> characters;
	vector<Matchup*> p_matchups;
	vector<Matchup*> c_matchups;
	vector<string> log_;

	vector<string> wins; 
	vector<string> losses; 
};

#endif /* PLAYER_H_ */
