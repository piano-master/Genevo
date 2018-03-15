//============================================================================
// Name        : Genevo
// Author      : Samuel Goodwin
// Version     :
// Copyright   : 
// Description : Fighting game tournament simulator
//============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <math.h>

#include "Player.h"
#include "Character.h"
#include "Match.h"
#include "Matchup.h"
#include "Bracket.h"

#define NUM_TOTAL_CHARACTERS 13

using namespace std;

//TO-DO LIST
//make special player-character file (i.e. Armada beats IC, Mango beats Samus) - don't do this
	//subsitute - if new matchup, check if there are existing matchups with similarly seeded opponents who play same character as opponent
//let user either run tournament simulation, print specific matchups (that shows what tournaments the two players met and the results of their set each time)
	//allow user to override matchups before running tournament simulations
//add "byes" for non-powers-of-2 bracket sizes

//idea for incorpoarting best fantasy team
	//assign players scores based on predicted brackets
	//take weighted average scores for each player over 10? most likely brackets and determine which team generates maximum points within limited budget
		//most likely bracket weighted more heavily than 10th most likely bracket
		//add how much each player costs to player textfile

bool findPlayer(const string& player_tag, const vector<Player*>& players) {
	//returns true if a player of name player_tag exists
	for (unsigned int i = 0; i < players.size(); i++) {
		if (players[i]->getTag() == player_tag) return true;
	}
	return false;
}

void readMatchup(char c, ifstream& istr, int& wins, int& losses) {
	//reads a specific matchup and stores numbers of wins and losses
	c = istr.get();
	string s_wins, s_losses;
	while (c != '-') {
		s_wins += c;
		c = istr.get();
	}
	c = istr.get();
	while (c != '\n') {
		s_losses += c;
		c = istr.get();
	}
	wins = atoi(s_wins.c_str());
	losses = atoi(s_losses.c_str());
}

void printMatchup(Player* p1, Player* p2) {
	cout << p1->getMatchup(p2) << endl;
}

void switch_halves(vector<Player*>& losers, const vector<Player*>& temp) {
	//splits the losers vector in halves and switches the halves
	vector<Player*> first_half, second_half;
	for (unsigned int i = 0; i < losers.size(); i++) {
		if (i < losers.size()/2) first_half.push_back(temp[i]);
		else second_half.push_back(temp[i]);
	}
	losers.clear();
	for (unsigned int i = 0; i < second_half.size(); i++) {
		losers.push_back(second_half[i]);
	}
	for (unsigned int i = 0; i < first_half.size(); i++) {
		losers.push_back(first_half[i]);
	}
	first_half.clear();
	second_half.clear();
}

void advanceBracket(Bracket* tournament, const vector<Player*>& playersW, const vector<Player*>& playersL, int roundW, int roundL) {
	//creates new matches and adds them to the next round of the bracket
	int i = 0, j = 0;
	Player* p1;
	Player* p2;
	float p;
	Match match;

	//winners bracket
	for (i, j; i < playersW.size(); i += 2, j++) {
		p1 = playersW[i];
		p2 = playersW[i+1];
		p = p1->getMatchup(p2)->getPValue();
		match = Match(p1,p2,p,0);
		tournament->addToBracket(match,roundW,j);
	}

	//losers bracket
	if (roundL % 2 == 0) j = 0;
	else {
		bool searching = true;
		int k = 0;
		while (searching) {
			if (tournament->getMatch(roundL,k).getPlayer1() == NULL) {
				j = k;
				searching = false;
			}
			k++;
		}
	}
	for (i = 0, j; i < playersL.size(); i += 2, j++) {
		p1 = playersL[i];
		p2 = playersL[i+1];
		p = p1->getMatchup(p2)->getPValue();
		match = Match(p1,p2,p,1);
		tournament->addToBracket(match,roundL,j);
	}
}

void simulateBracket(vector<Player*>& players) {
	// initialize the bracket
	//for non-powers of 2, can use "byes" to simulate power of 2 (advance players vs bye to next round)
	//cout << "players.size() = " << players.size() << endl;
	int size = players.size()/2;
	int max_rounds = ((log(size)/log(2))+1)*2;
	Bracket* tournament = new Bracket(size,max_rounds);

	// setup first round of the bracket - there's probably a math (recursive?) function I can use to generate array for n-sized bracket
	//to-do:  add "byes"
	vector<int> seating;
	if (players.size() == 4) seating = {0,1}; //4 players
	else if (players.size() == 8) seating = {0,3,2,1}; //8 players
	else if (players.size() == 16) seating = {0,7,4,3,2,5,6,1}; //16 players
	else if (players.size() == 32) seating = {0,8,12,4,6,14,10,2,3,11,15,7,5,13,9,1}; //32 players
	else if (players.size() == 64) seating = {0,16,24,8,15,31,23,7,4,20,28,12,11,27,19,3,2,18,26,10,13,29,21,5,6,22,30,14,9,25,17,1}; //64 players

	vector<int> order_2, order_4;
	if (players.size() == 32) {
		order_2 = {0,3,1,2};
	    order_4 = {0,2,1,3};
	}
	else if (players.size() == 64) {
		order_2 = {0,5,1,4,2,7,3,6};
		order_4 = {0,7,1,6,2,5,3,4};
	}

	for (unsigned int i = 0; i < players.size(); i++) {
		players[i]->clearLog();
	}

	Player* p1 = NULL;
	Player* p2 = NULL;
	for (int i = 0; i < size; i++) {
		p1 = players[i];
		p2 = players[players.size()-i-1];
		float p = 1.0;//p1->getMatchup(p2)->getPValue();
		Match match = Match(p1,p2,p,0);
		tournament->addToBracket(match,0,seating[i]);
	}

	// simulate all future rounds
	vector<Player*> winners;
	vector<Player*> losers;
	vector<Player*> elim;
	Player* top2[2];
	Match curr_match;
	int round_ = 0;
	int round_size = size;
	int inc = 1;
	string bt;
	while (round_ < max_rounds) {
		//cout << "Round " << round_ << endl;
		//cout << endl;
		//simulate who moves on to winners/losers/eliminated bracket
		if (round_ != 0 && round_ % 2 == 0) round_size /= 2;
		for (int i = 0; i < round_size; i++) {
			curr_match = tournament->getMatch(round_,i);
			Player* winner = NULL;
			Player* loser = NULL;
			if (curr_match.getPValue() >= 0.5) {
				// Player1 wins, Player2 loses 
				winner = curr_match.getPlayer1();
				loser = curr_match.getPlayer2();
			}
			else {
				// Player2 wins, Player1 loses 
				winner = curr_match.getPlayer2();
				loser = curr_match.getPlayer1();
			}
			//cout << "num_wins = " << winner->getMatchup(loser)->numWins() << " num_losses = " << winner->getMatchup(loser)->numLosses() << endl;
			//cout << "p = " << curr_match.getPValue() << endl;

			if (curr_match.getType() == 0) {
				//cout << "winners match" << endl;
				bt = "winners";
				winners.push_back(winner);
				losers.push_back(loser);
			}
			else if (curr_match.getType() == 1) {
				//cout << "losers match" << endl;
				bt = "losers";
				losers.push_back(winner);
				elim.push_back(loser);
				//cout << loser->getTag() << " is eliminated" << endl;
			}
			else {
				//grand finals
				//cout << "GRAND FINALS" << endl;
				bt = "GRAND FINALS";
				//order of players in elim indicates placing
				elim.push_back(loser);
				elim.push_back(winner);
			}
			if (round_ != 0) {
				string string_w = "beats " + loser->getTag() + " in " + bt;
				string string_l = "loses to " + winner->getTag() + " in " + bt;
				winner->addToLog(string_w);
				loser->addToLog(string_l);
			}
			//cout << winner->getTag() << " beats " << loser->getTag() << " in round " << round_ << endl;
			//cout << endl;
		}

		//reorganize losers bracket
		if ((round_+1) % 2 == 0 && losers.size() > players.size()/16 && players.size() == 32) {
			int orig_size = losers.size();
			vector<Player*> temp(orig_size);
			vector<int> order;
			if ((round_+1) % 4 == 0) order = order_4;
			else order = order_2;

			unsigned int i = 0, j = 0, k = 0;
			for (i = 0; i < players.size()/8; i++) {
				int stop = 8*losers.size()/players.size();
				if (i % 2 == 0) {
					for (j = 0, k = 0; j < stop*2; j += 2, k++) {
						temp[i*stop+j] = losers[order[i]*stop+j-k];
						temp[i*stop+j+1] = losers[order[i+1]*stop+j-k];
					}
				}	
			}

			if ((round_+1) % 4 != 0) switch_halves(losers,temp);
			else {
				losers.clear();
				for (unsigned int i = 0; i < orig_size; i++) {
					losers.push_back(temp[i]);
				}
			}
			order.clear();
		}
		else if ((round_+1) % 2 == 0 && players.size() < 32) {
			vector<Player*> temp = losers;
			int j = 1;
			for (unsigned int i = 1; i < losers.size(); i++) {
				//0 1 2 3 4 5 6 7 temp
				//0 7 1 6 2 5 3 4 losers
				if (i < losers.size()/2) losers[i*2] = temp[i];
				if (i % 2 != 0) losers[i] = temp[losers.size()-j];
				else j++;
			}
			temp = losers;
			switch_halves(losers,temp);
		}
		else if ((round_ == 0) && (players.size() == 64)) {
			for (unsigned int i = 0; i < winners.size(); i++) {
				if ( (i-4) % 8 == 0 ) {
					Player* temp0 = winners[i];
					Player* temp1 = winners[i+1];
					winners[i] = winners[i+2];
					winners[i+1] = winners[i+3];
					winners[i+2] = temp0;
					winners[i+3] = temp1;
				}
			}
			for (unsigned int i = 0; i < losers.size()/2; i++) {
				Player* temp = losers[i];
				losers[i] = losers[losers.size()-i-1];
				losers[losers.size()-1-i] = temp;
			}
			for (unsigned int i = 0; i < losers.size(); i++) {
				if (i % 8 == 0) {
					Player* temp0 = losers[i];
					Player* temp1 = losers[i+1];
					losers[i] = losers[i+2];
					losers[i+1] = losers[i+3];
					losers[i+2] = temp0;
					losers[i+3] = temp1;
				}
			}
		}
		else if ((round_+1) % 2 == 0 && players.size() == 64 && losers.size() > 2) {
			if (round_ == 5) {
				//2 3 0 1
				//0 1 2 3
				Player* temp = losers[2];
				losers[2] = losers[0];
				losers[0] = temp;
				temp = losers[1];
				losers[1] = losers[3];
				losers[3] = temp;
			}
			else if ((round_+1) % 4 != 0) {
				for (unsigned int i = 0; i < losers.size()/4; i++) {
					Player* temp = losers[i];
					losers[i] = losers[(losers.size()/2)-i-1];
					losers[(losers.size()/2)-i-1] = temp;
				}
			}
			else {
				unsigned int i, j;
				for (i = 0; i < losers.size()/8; i++) {
					Player* temp = losers[i];
					losers[i] = losers[(losers.size()/4)-i-1];
					losers[(losers.size()/4)-i-1] = temp;
				}
				for (i = losers.size()/4, j = 0; i < (losers.size()/2)-2; i++, j++) {
					Player* temp = losers[i];
					losers[i] = losers[(losers.size()/2)-j-1];
					losers[(losers.size()/2)-j-1] = temp;
				}
			}

			vector<Player*> first_half, second_half;
			for (unsigned int i = 0; i < losers.size(); i++) {
				if (i < losers.size()/2) first_half.push_back(losers[i]);
				else second_half.push_back(losers[i]);
			}
			int orig_size = losers.size();
			losers.clear();
			for (unsigned int i = 0; i < orig_size/2; i++) {
				losers.push_back(first_half[i]);
				losers.push_back(second_half[i]);
			}
		}
		
		// setup next round
		if (winners.size() == 1) top2[0] = winners[0];
		if (losers.size() == 1) {
			//add grand_finals match to the bracket
			top2[1] = losers[0];
			float p = top2[0]->getMatchup(top2[1])->getPValue();
			Match gf = Match(top2[0],top2[1],p,2);
			tournament->addToBracket(gf,round_+1,0);
		}
		else {
			advanceBracket(tournament,winners,losers,round_+inc,round_+1);
		}

		winners.clear();
		losers.clear();
		round_++;
		inc = 2;
	}	

	//output most probable brackets to a text file like so
	//i=				 0,1,2,3,4,5,6, 7, 8, 9,10,11,12,13
	//possible placings: 1,2,3,4,5,7,9,13,17,25,33,49,65,97
	int placing[12] = {1,2,3,4,5,7,9,13,17,25,33,49};
	int help[12] = {1,1,1,1,2,2,4,4,8,8,16,16};
	vector<Player*> over, under;
	cout << "\nTournament Results:\n" << endl;
	unsigned int j = 0;
	int k = 0;
	for (unsigned int i = 0, k = 0; i < elim.size(); i++, k++) {
		Player* player = elim[elim.size()-i-1];
		player->updatePlacing(placing[j]);
		int value = player->checkPerformance();
		if (value == 0) over.push_back(player);
		else if (value == 1) under.push_back(player);

		cout << placing[j] << ". " << player->getTag() << endl;
		if (k == help[j] - 1) {
			j++;
			k = -1;
		}
	}

	cout << "\nOverperforming Players:" << endl;
	for (unsigned int i = 0; i < over.size(); i++) {
		cout << "\t" << over[i]->getTag() << endl;
	}

	cout << "\nUnderperforming Players:" << endl;
	for (unsigned int i = 0; i < under.size(); i++) {
		cout << "\t" << under[i]->getTag() << endl;
	}
	cout << endl;
}

int main(int argc, char** argv) {
	if (argc < 4) {
		std::cerr << "Please provide data for the tournament predictor" << std::endl;
		std::cerr << "[executable] [players] [character_matchups] [tournament data]" << std::endl;
		return 1;
	}
	/*string player_file = "\"/Smash/Tournament Predictor/bin/";
	//player_file += argv[1];
	player_file += '\"';
	string character_file = "/Smash/Tournament Predictor/bin/";
	//character_file += argv[2];*/

	vector<Player*> players;
	vector<Character*> characters_all; //all characters present

	vector<Character*> characters_one; //a particular player's characters
	ifstream player_data(argv[1]);
	char c;
	string player;

//=============================== Player File ================================================
	int seed = 1;
	while (player_data.good()) {
		c = player_data.get();
		player.clear();
		for (unsigned int i = 0; i < 4; i++) { c = player_data.get(); }
		while (c != ' ') {
			//retrieve name of player
			player += c;
			c = player_data.get();
		}
		for (unsigned int i = 0; i < 3; i++) { c = player_data.get(); }
		bool more_characters = true;
		while (more_characters) {
			//retrieve all characters that player plays
			string char_name;
			while (c != ',') {
				char_name += c;
				c = player_data.get();
				if (c == '\r' || c == '\n') break;
				if (!player_data.good()) break;
			}
			bool found = false;
			for (unsigned int i = 0; i < characters_all.size(); i++) {
				if (characters_all[i]->getName() == char_name) {
					//this character is already accounted for
					found = true;
					characters_one.push_back(characters_all[i]);
					break;
				}
			}
			if (!found) {
				//new character not accounted for so add Character to vector
				Character* ch = new Character(char_name);
				characters_all.push_back(ch);
				characters_one.push_back(ch);
			}
			if (c == ',') {
				for (unsigned int i = 0; i < 1; i++) { c = player_data.get(); }
			}
			else { more_characters = false; }
		}
		Player* person = new Player(player,characters_one,seed);
		players.push_back(person);
		characters_one.clear();
		c = player_data.get();
		seed++;
	}
	player_data.close();
	//cout << "finished reading player file\n" << endl;

//============================== Character Matchups ==========================================
	ifstream character_data(argv[2]);
	string char1;
	string char2;
	Character* chara;
	while (character_data.good()) {
		c = character_data.get();
		char1.clear();
		char2.clear();
		while (c != '\r') {
			if (c != '\n') char1 += c;
			c = character_data.get();
		}
		bool found = false;
		for (unsigned int i = 0; i < characters_all.size(); i++) {
			if (characters_all[i]->getName() == char1) {
				found = true;
				chara = characters_all[i];
				break;
			}
		}
		if (found) {
			for (unsigned int i = 0; i < 2; i++) { c = character_data.get(); }
			for (unsigned int j = 0; j < NUM_TOTAL_CHARACTERS-1; j++) {
				char2.clear();
				while (c != ' ') {
					if (c != '\t') char2 += c;
					c = character_data.get();
				}
				bool found2 = false;
				for (unsigned int i = 0; i < characters_all.size(); i++) {
					if (characters_all[i]->getName() == char2) {
						found2 = true;
						break;
					}
				}
				if (found2) {
					int wins, losses;
					readMatchup(c,character_data,wins,losses);
					Matchup matchup = Matchup(char2,wins,losses);
					chara->addMatchup(matchup);
					for (unsigned int i = 0; i < 2; i++) { c = character_data.get(); }
				}
				else {
					for (unsigned int i = 0; i < 9; i++) { c = character_data.get(); }
					char2.clear();
				}
			}
			int wins=50,losses=50;
			Matchup mirror_match = Matchup(chara->getName(),wins,losses);
			chara->addMatchup(mirror_match);
		}
		else {
			int line = 0;
			while (line < 12 && c != -1) {
				c = character_data.get();
				if (c == '\r') line++;
			}
		}
	}
	character_data.close();
	//cout << "finished reading character data\n" << endl;
	//cout << "characters_all.size() = " << characters_all.size() << endl;

//================================= Tournament Data ================================================
	//example of a tournament data file
	//Player1
	//	Player2 2-0
	//	Player3 0-1
	//
	//cout << "reading tournament data" << endl;

	string player1;
	string player2;
	for (unsigned int i = 3; i < argc; i++) {
		ifstream tournament_data(argv[i]);
		c = tournament_data.get();
		bool end = false;
		while (tournament_data.good()) {
			player1.clear();
			player2.clear();
			while (c != '\r') {
				//retrieve player name to look at their record
				player1 += c;
				c = tournament_data.get();
			}
			bool found1 = findPlayer(player1,players);
			if (found1) {
				for (unsigned int j = 0; j < 3; j++) { c = tournament_data.get(); }
				bool more_players = true;
				while (more_players) {
					player2.clear();
					while (c != ' ') {
						//retrieve tag of player2
						player2 += c;
						c = tournament_data.get();
					}
					bool found2 = findPlayer(player2,players);
					if (found2) {
						Player* person = NULL;
						for (unsigned int j = 0; j < players.size(); j++) {
							if (players[j]->getTag() == player1) {
								person = players[j];
								break;
							}
						}
						Matchup* fight = NULL;
						for (unsigned int j = 0; j < person->numMatchups(); j++) {
							if (person->getMatchup(j)->getOpponent() == player2) {
								fight = person->getMatchup(j);
								break;
							}
						}
						int wins, losses;
						readMatchup(c,tournament_data,wins,losses);
						if (fight) {
							//player1/player2 matchup already exists
							fight->incrementWins(wins);
							fight->incrementLosses(losses);
							fight->updatePValue();
						}
						else {
							//matchup doesn't exist, need to add it to player1's
							//vector of player matchups
							fight = new Matchup(player2,wins,losses);
							person->addMatchup(fight);
						}
						c = tournament_data.get();
						if (c == -1) {
							more_players = false;
							break;
						}
						else if (c != '\t') {
							more_players = false;
						}
						else {
							c = tournament_data.get();
						}
					}
					else {
						//"player2" is a player not at the current tournament
						while (c != '\n') {
							c = tournament_data.get();
							if (c == -1) {
								end = true;
								break;
							}
						}
						if (!end) {
							c = tournament_data.get();
							if (c != '\t')  {
								more_players = false;
							}
							else { c = tournament_data.get(); }
						}
						else {
							more_players = false;
						}
					}
				}
			}
			else {
				//"player1" isn't at current tournament so skip them
				for (unsigned int j = 0; j < 2; j++) { c = tournament_data.get(); }
				while (c == '\t') {
					while (c != '\n') {
						c = tournament_data.get();
					}
					c = tournament_data.get();
				}
			}
		}
		tournament_data.close();
	}
	//cout << "finished reading tournament data\n" << endl;

	//============================== Tournament Simulator ==========================================

	simulateBracket(players);

	//ask user if they want to override any particular matches
	bool change = true;
	while (change) {
		string answer;
		string match_s = "";
		cout << "\nOverride Matches OR Print Player Logs OR Update Simulation" << endl;
		cin >> answer;
		if (answer == "override") {
			//cout << "[player1]>[player2]\n" << endl;
			//while (match_s != "q") {
				cin >> match_s;
				//if (match_s == "q") continue;

				//read match input
				c = match_s[0];
				int i = 0;
				string player1 = "";
				string player2 = "";
				while (c != '>') {
					player1 += c;
					c = match_s[++i];
				}
				i++;
				for (i; i < match_s.size(); i++) {
					c = match_s[i];
					player2 += c;
				}

				//find matchup from user input and override it
				Player* p1 = NULL;
				Player* p2 = NULL;
				unsigned int j,k;
				for (j = 0; j < players.size(); j++) {
					if (players[j]->getTag() == player1) {
						p1 = players[j];
						break;
					}
				}
				for (k = 0; k < players.size(); k++) {
					if (players[k]->getTag() == player2) {
						p2 = players[k];
						break;
					}
				}
				if (p1 == NULL || p2 == NULL) {
					cout << "You made a spelling mistake\n" << endl;
					continue;
				}
				p1->changeMatchup(p2,1.0);
				p2->changeMatchup(p1,0.0);
				cout << "\n" << player1 << " will beat " << player2 << " if they meet in bracket" << endl;
				//cout << "\nNext matchup: (Press q to stop overriding matches)\n" << endl;
			//}
		}
		else if (answer == "print") {
			string name;
			//cout << "\nSelect a player:" << endl;
			cin >> name;
			for (unsigned int i = 0; i < players.size(); i++) {
				if (players[i]->getTag() == name) {
					cout << endl;
					players[i]->printLog();
					break;
				}
			}
		}
		else if (answer == "update") {
			simulateBracket(players);
		}
		else if (answer == "q") {
			change = false;
		}
		else {
			cout << "Cannot recognize input. Please try again.\n" << endl;
		}
	}
	

	//for debugging purposes===============================================================================
	/*
	cout << endl;
	for (unsigned int i = 0; i < players.size(); i++) {
		cout << players[i]->getTag() << " - ";
		for (unsigned int j = 0; j < players[i]->numCharacters(); j++) {
			cout << players[i]->getCharacterName(j) << " ";
		}
		cout << endl;
	}

	cout << endl;
	cout << "There are " << characters_all.size() << " characters at this tournament." << endl;
	cout << endl;

	for (unsigned int i = 0; i < characters_all.size(); i++) {
		cout << characters_all[i]->getName() << " has the following matchups:" << endl;
		for (unsigned int j = 0; j < characters_all[i]->numMatchups(); j++) {
			cout << "\t" << characters_all[i]->getMatchup(j).getOpponent() <<
					" " << characters_all[i]->getMatchup(j).numWins() << " " <<
					characters_all[i]->getMatchup(j).numLosses() << endl;
		}
	}

	cout << "\nTournament History:" << endl;
	cout << endl;

	for (unsigned int i = 0; i < players.size(); i++) {
		cout << players[i]->getTag() << ":" << endl;
		for (unsigned int j = 0; j < players[i]->numMatchups(); j++) {
			cout << "\t" << players[i]->getMatchup(j)->getOpponent() << " "
					<< players[i]->getMatchup(j)->numWins() << "-"
					<< players[i]->getMatchup(j)->numLosses() << endl;
		}
	}*/

	cout << "\nThank you for using Genevo" << endl;
	return 0;
}
