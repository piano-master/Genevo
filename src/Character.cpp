/*
 * Character.cpp
 *
 *  Created on: Jun 4, 2017
 *      Author: goodws
 */

#include <iostream>
#include "Character.h"

Character::Character(string aName) {
	name = aName;
}

Character::~Character() {
}

Matchup Character::getMatchup(Character* ch) {
	for (unsigned int i = 0; i < matchups.size(); i++) {
		if (matchups[i].getOpponent() == ch->getName()) return matchups[i];
	}
}

void Character::addMatchup(Matchup matchup) {
	matchups.push_back(matchup);
}
