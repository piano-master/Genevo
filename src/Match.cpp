/*
 * Match.cpp
 *
 *  Created on: Jun 4, 2017
 *      Author: goodws
 */

#include "Match.h"

Match::Match() {
	player1 = NULL;
	player2 = NULL;
	p_value = -1;
	bracket_type = -1;
}

Match::Match(Player* p1, Player* p2, float p, int type) {
	player1 = p1;
	player2 = p2;
	p_value = p;
	bracket_type = type;
	/*
	if (bracket_type == 1) {
		losers = NULL;
	}
	winners = NULL;
	losers = NULL;
	*/
}

Match::~Match() {
}

void Match::addPlayer(Player* p2, float p) {
	player2 = p2;
	p_value = p;
}