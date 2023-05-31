#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>
#include <stdexcept>
#include <istream>
#include <sstream>

using namespace std;

struct GameClock {
	vector<int> digits;
	float seconds = 0;
	float pausedSecs = 0;
	float tempSecs = 0;
	vector<int> LastDigitSeconds(float seconds) {
		int temp = int(seconds);
		int minutes = 0;
		vector<int> digits;
		digits.push_back(0);
		digits.push_back(0);
		digits.push_back(0);
		digits.push_back(0);
		if (seconds >= 60) {
			minutes = floor(temp / 60);
			temp %= 60;
		}
		digits.at(3) = temp % 10;
		digits.at(2) = floor(temp / 10);
		digits.at(1) = minutes % 10;
		digits.at(0) = floor(minutes / 10);

		return digits;
	}
	
};

struct Config {
	int columns;
	int rows;
	int mines;
	Config() {
		columns = 0;
		rows = 0;
		mines = 0;
	}
};

struct Player {   // Stores player info
	string name = "";
	string time = "59:59";
	string minutes;
	string seconds;
	bool starred = false;
};

bool compareTime(Player p1, Player p2) {
	return(p1.time < p2.time);
}

void writeToLeader(vector<Player> players, string filename) {
	fstream outFile(filename, ios_base::out);
	for (int i = 0; i < players.size() - 1; ++i) {
		outFile << players[i].time;
		outFile << ",";
		outFile << players[i].name;
		outFile << "\n";
	}
}

void LoadPlayer(string filename, vector<Player>& players) {
	fstream inFile(filename, ios_base::in);
	if (inFile.is_open()) {
		string line;
		while (getline(inFile, line)) {
			Player player;
			istringstream stream(line);
			getline(stream, player.time, ',');
			getline(stream, player.name);
			players.push_back(player);
		}
	}
}


void LoadConfig(string filename, Config& config) {   // Loads config data
	fstream readFile(filename, ios_base::in);
	if (readFile.is_open()) {
		string line;
		readFile >> config.columns;
		readFile >> config.rows;
		readFile >> config.mines;
	}
}

struct Tile {
	sf::Sprite tile;
	sf::Sprite flag;
	sf::Sprite mine;
	sf::FloatRect tilerect;
	sf::Vector2f coords;
	Tile* top = nullptr;
	Tile* topright = nullptr;
	Tile* topleft = nullptr;
	Tile* right = nullptr;
	Tile* left = nullptr;
	Tile* botright = nullptr;
	Tile* botleft = nullptr;
	Tile* bottom = nullptr;
	vector<Tile*> adjTiles;
	int mineCount = 0;
	bool tileRevealed = false;
	bool containsMine = false;
	bool flagged = false;
	bool revealMine = false;
};

struct Board {
	int height = 0;
	int width = 0;
	int flagsOnScreen = 0;
	vector<vector<Tile>> tiles;
};


void GenerateBoard(Config& config, Board& board, sf::Texture& tilehiddenData, sf::Sprite& mine) {
	for (int i = 0; i < config.rows; ++i) {
		vector<Tile> t;
		board.tiles.push_back(t);
		for (int j = 0; j < config.columns; ++j) {
			Tile hiddenTile;
			hiddenTile.tile.setTexture(tilehiddenData);
			hiddenTile.tile.setPosition(sf::Vector2f(j * 32.0f, i * 32.0f));
			hiddenTile.coords = sf::Vector2f(j * 32.0f, i * 32.0f);
			hiddenTile.tilerect = hiddenTile.tile.getGlobalBounds();
			board.tiles[i].push_back(hiddenTile);
		}
	}
	// Implementation help for generating random number from University of Alaska System
	srand((unsigned)time(NULL));
	 
	int count = 0;
	while (count < config.mines) {
		int random = 0;
		int randomHeight = rand() % (config.rows - 1);
		int randomWidth = rand() % (config.columns - 1);

		if (!board.tiles[randomHeight][randomWidth].containsMine) {
			board.tiles[randomHeight][randomWidth].containsMine = true;
			board.tiles[randomHeight][randomWidth].mine = mine;
			board.tiles[randomHeight][randomWidth].mine.setPosition(board.tiles[randomHeight][randomWidth].coords);
			count += 1;
		}
		else {
			continue;
		}
		
		
	}
	
}

void MineCount(Board& board, Config& config) {
	for (int i = 0; i < config.rows; ++i) {
		for (int j = 0; j < config.columns; ++j) {
			if (i != 0) {
				if (j != 0 && board.tiles[i - 1][j - 1].containsMine) { // Checks top left
					board.tiles[i][j].mineCount += 1;
				}
				if (board.tiles[i - 1][j].containsMine) {  // Checks top
					board.tiles[i][j].mineCount += 1;
				}
				
				if ((j != config.columns - 1) && board.tiles[i - 1][j + 1].containsMine) {   // Checks top right
					board.tiles[i][j].mineCount += 1;
				}
			}

			if (i != (config.rows - 1)) {
				if (board.tiles[i + 1][j].containsMine) {  // Checks bottom
					board.tiles[i][j].mineCount += 1;
				}
				if ((j != config.columns - 1) && board.tiles[i + 1][j + 1].containsMine) {  // checks bottom right
					board.tiles[i][j].mineCount += 1;
				}
			}

			if (j != 0) {
				if (board.tiles[i][j - 1].containsMine) {   // Checks left
					board.tiles[i][j].mineCount += 1;
				}
				if ((i != config.rows - 1) && board.tiles[i + 1][j - 1].containsMine) {  // Checks bottom left
					board.tiles[i][j].mineCount += 1; 
				}
			}

			if (j != config.columns - 1) {
				if (board.tiles[i][j + 1].containsMine) {                 // checks right
					board.tiles[i][j].mineCount += 1;
				}
			}
		}
	}
}


void AssignAdjTiles(Board& board, Config& config) {
	for (int i = 0; i < config.rows; ++i) {
		for (int j = 0; j < config.columns; ++j) {
			if (i != 0) {
				if (j != 0) { // adds top left
					board.tiles[i][j].topleft = &board.tiles[i - 1][j - 1];
					board.tiles[i][j].adjTiles.push_back(board.tiles[i][j].topleft);
					board.tiles[i][j].top = &board.tiles[i - 1][j];
					board.tiles[i][j].adjTiles.push_back(board.tiles[i][j].top);  // Adds top
				}
				
				if (j != config.columns - 1) {   // Checks top right
					board.tiles[i][j].topright = &board.tiles[i - 1][j + 1];
					board.tiles[i][j].adjTiles.push_back(board.tiles[i][j].topright);
				}
			}
			if (i != 0 && j == 0) {
				board.tiles[i][j].top = &board.tiles[i - 1][j];  // Adds top for left edge
				board.tiles[i][j].adjTiles.push_back(board.tiles[i][j].top);
			}
			if (i != (config.rows - 1)) {
				// Checks bottom
				board.tiles[i][j].bottom = &board.tiles[i + 1][j];
				board.tiles[i][j].adjTiles.push_back(board.tiles[i][j].bottom);
		
				if (j != config.columns - 1) {  // checks bottom right
					board.tiles[i][j].botright = &board.tiles[i + 1][j + 1];
					board.tiles[i][j].adjTiles.push_back(board.tiles[i][j].botright);
				}
			}

			if (j != 0) {
				// Checks left
				board.tiles[i][j].left = &board.tiles[i][j - 1];
				board.tiles[i][j].adjTiles.push_back(board.tiles[i][j].left);
				
				if (i != config.rows - 1) {  // Checks bottom left
					board.tiles[i][j].botleft = &board.tiles[i + 1][j - 1];
					board.tiles[i][j].adjTiles.push_back(board.tiles[i][j].botleft);
				}
			}

			if (j != config.columns - 1) {
				// checks right
				board.tiles[i][j].right = &board.tiles[i][j + 1];
				board.tiles[i][j].adjTiles.push_back(board.tiles[i][j].right);
			}
		}
	}
}

void RecursiveRevealTile(vector<Tile*>& adjTiles) {
	for (int i = 0; i < adjTiles.size(); ++i) {
		// BASE CASE:
		if (adjTiles[i]->containsMine || adjTiles[i]->flagged || adjTiles[i]->tileRevealed) {
			continue;
		}
		// IF ADJ MINECOUNT IS 0
		else if (adjTiles[i]->mineCount > 0) {
			adjTiles[i]->tileRevealed = true;
		}
		// RESCURSIVE STEP
		else {
			adjTiles[i]->tileRevealed = true;
			RecursiveRevealTile(adjTiles[i]->adjTiles);
		}
	}
}

vector<sf::Sprite> CheckIfMine(Board& board, Config& config) {
	vector<sf::Sprite> mines;
	for (int i = 0; i < config.rows; ++i) {
		for (int j = 0; j < config.columns; ++j) {
			if (board.tiles[i][j].revealMine) {
				mines.push_back(board.tiles[i][j].mine);
			}
		}
	}
	return mines;
}

vector<int> LastDigitMine(int mines) {
	bool numWasNeg = false;
	if (mines < 0) {
		numWasNeg = true;
	}
	int temp = abs(mines);
	int temp2 = 0;
	vector<int> digits;
	while (temp > 0) {
		temp2 = temp % 10;
		digits.push_back(temp2);
		temp = floor(temp / 10);
	}
	if (digits.size() < 2) {
		digits.push_back(0);
	}
	if (digits.size() < 3) {
		digits.push_back(0);
	}
	if (mines == 0) {
		digits.push_back(0);
	}
	if (numWasNeg) {
		digits.push_back(69);
	}
	reverse(digits.begin(), digits.end());
	return digits;
}