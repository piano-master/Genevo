/*
 * Bracket.cpp
 *
 *  Created on: Jun 18, 2017
 *      Author: goodws
 */

#include "Bracket.h"
#include <iostream>

Bracket::Bracket(int aNumSets, int aMaxRounds) {
	// TODO Auto-generated constructor stub
	num_sets = aNumSets;
	max_rounds = aMaxRounds;
	int round_size = num_sets;
	this->bracket = new Match*[max_rounds];
	this->bracket[0] = new Match[round_size];
	for (int i = 1; i < max_rounds; i++) {
		bracket[i] = new Match[round_size];
		//if (i % 2 != 0) round_size /= 2;
	}
}

Bracket::~Bracket() {
	// TODO Auto-generated destructor stub
}

const Match Bracket::getMatch(int round_, int set) {
	if (round_ >= max_rounds || set >= num_sets) {
		cerr << "retrieving from invalid round or set number" << endl;
	}
	else {
		return bracket[round_][set];
	}
}

void Bracket::addToBracket(Match match, int round_, int set) {
	if (round_ >= max_rounds || set >= num_sets) {
		cerr << "adding using invalid round or set number" << endl;
		cout << "set = " << set << endl;
		cout << "num_sets = " << num_sets << endl;
	}
	else {
		bracket[round_][set] = match;
	}
}