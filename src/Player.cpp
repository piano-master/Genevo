/*
 * Player.cpp
 *
 *  Created on: Jun 4, 2017
 *      Author: Samuel Goodwin
 */

#include "Player.h"
#include <iostream>
#define SKILL_DIFFERENCE 10

Player::Player(string aTag, vector<Character*> aCharacters, int aSeed) {
	tag = aTag;
	characters = aCharacters;
	seed = aSeed;
}

Player::~Player() {
}

Matchup* Player::getMatchup(int i) {
	if (p_matchups[i]->getPValue() == -1) {

	}
	return p_matchups[i];
}

float newP(Player* player2, int seed, const vector<Character*>& characters) {
	//modify p
	if (seed - player2->getSeed() > SKILL_DIFFERENCE) {
		return 0;
	}
	else if (player2->getSeed() - seed > SKILL_DIFFERENCE) {
		return 1;
	}
	else {
		//cout << "characters = " << player2->numCharacters() << endl;
		Character* c1 = NULL;
		Character* c2 = NULL;
		float max_p=0.0, min_p=1.0;
		for (unsigned int j = 0; j < characters.size(); j++) {
			c1 = characters[j];
			for (unsigned int k = 0; k < player2->numCharacters(); k++) {
				c2 = player2->getCharacter(k);
				Matchup matchup = c1->getMatchup(c2);
				//cout << "p_c = " << matchup.getPValue() << endl;
				if (matchup.getPValue() > max_p) max_p = matchup.getPValue();
				if (matchup.getPValue() < min_p) min_p = matchup.getPValue();
			}
		}
		//cout << "keep going" << endl;
		if (max_p == min_p == 0.5) {
			if (seed < player2->getSeed()) return 0.51;
			else return 0.49;
		}
		else if (max_p - 0.5 > 0.5 - min_p) { //review Westballz vs Mew2King
			if (max_p != 0.5) return max_p;
			else {
				if (seed < player2->getSeed()) return 0.51;
				else return 0.49;
			}
		}
		else {
			if (min_p != 0.5) return min_p;
			else {
				if (seed < player2->getSeed()) return 0.51;
				else return 0.49;
			}
		}
	}
}

Matchup* Player::getMatchup(Player* player) {
	for (unsigned int i = 0; i < p_matchups.size(); i++) {
		if (p_matchups[i]->getOpponent() == player->getTag()) {
			if (p_matchups[i]->getPValue() == -1) {
				p_matchups[i]->setP( newP(player,seed,characters) );
			}
			return p_matchups[i];
		}
	}
	Matchup* first = new Matchup( player->getTag(),newP(player,seed,characters) );
	return first;
}

void Player::changeMatchup(Player* player, float p) {
	for (unsigned int i = 0; i < p_matchups.size(); i++) {
		if (p_matchups[i]->getOpponent() == player->getTag()) {
			p_matchups[i]->setP(p);
			return;
		}
	}
	Matchup* first = new Matchup(player->getTag(),p);
	p_matchups.push_back(first);
}

const void Player::printLog() const {
	cout << tag << ":" << endl;
	for (unsigned int i = 0; i < log_.size(); i++) {
		cout << "\t" << log_[i] << endl;
	}
}

int Player::checkPerformance() {
	//overperform == 0
	//underperform == 1
	//neither == 2
	int expected_placing = 0;
	int placing_help[12] = {1,2,3,4,5,7,9,13,17,25,33,49};
	for (unsigned int i = 0; i < 12; i++) {
		if (seed < placing_help[i]) {
			expected_placing = placing_help[i-1];
			break;
		}
	}
	if (expected_placing == 0) expected_placing = 49;

	//cout << tag << ": " << seed << " " << expected_placing << " " << placing << endl;
	if (expected_placing - placing > 0) return 0;
	else if (placing - expected_placing > 0) return 1;
	else return 2;
}
