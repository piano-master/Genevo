/*
 * Matchup.cpp
 *
 *  Created on: Jun 5, 2017
 *      Author: goodws
 */

#include "Matchup.h"

Matchup::Matchup(string aOpponent, int aWins, int aLosses) {
	opponent = aOpponent;
	wins = aWins;
	losses = aLosses;

	if (wins+losses == 0) {
		//change to finding p-value based on seeding and character matchups
		//if seeding difference significantly large, player with better seed wins
		//else p_value based on character matchup
		p = -1;
	}
	else {
		p = (float) ((float) wins/ (float) (wins+losses));
	}
}

Matchup::Matchup(string aOpponent, float aP) {
	opponent = aOpponent;
	p = aP;

	//not important values
	wins = 0;
	losses = 0;
}

Matchup::~Matchup() {
}