/*
 * Match.h
 *
 *  Created on: Jun 4, 2017
 *      Author: goodws
 */

#ifndef MATCH_H_
#define MATCH_H_

#include "Player.h"

class Match {
public:
	Match();
	Match(Player* p1, Player* p2, float p, int type);
	~Match();

	Player* getPlayer1() const { return player1; }
	Player* getPlayer2() const { return player2; }
	const float getPValue() const { return p_value; }
	const int getType() const { return bracket_type; }

	void addPlayer(Player* p2, float p);

	//void setWinners(Match* match) { winners = match; } //?
	//void setLosers(Match* match) { losers = match; } //?

private:
	Player* player1;
	Player* player2;
	int bracket_type; //winners==0 and losers==1
	float p_value; //probability that player1 will win

	//Match* winners;//?
	//Match* losers;//?
};

#endif /* MATCH_H_ */
