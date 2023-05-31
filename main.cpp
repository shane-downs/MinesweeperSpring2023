#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <istream>
#include <map>
#include <random>
#include <ctime>
#include "methods.h"
#include <SFML/Graphics.hpp>


using namespace std;

int main() {
	Config config;   // Config Object
	Player player;    // Player object
	bool shouldOpenGame = true;  // Boolean to determine if game window should open or not
	Board board;
	vector<Player> players;
	string currentName;

	LoadConfig("files/board_config.cfg", config);
	board.width = config.columns * 32;
	board.height = (config.rows * 32) + 100;
	sf::RenderWindow welcomeWindow(sf::VideoMode(board.width, board.height), "Minesweeper", sf::Style::Close);

	sf::Font font;  // FONT CONST.
	font.loadFromFile("files/font.ttf");

	sf::Text welcome;   // Welcome text construction
	welcome.setFont(font);
	welcome.setString("WELCOME TO MINESWEEPER!");
	welcome.setCharacterSize(24);
	welcome.setFillColor(sf::Color::White);
	welcome.setStyle(sf::Text::Bold | sf::Text::Underlined);

	//---------------------------------------------------------------//
	const float welcomeX = float(board.width / 2.0f);
	const float welcomeY = float((board.height / 2.0f) - 150);
	const float promptX = float(board.width / 2.0f);
	const float promptY = float(board.height / 2.0f - 75);           // Positioning for drawable objects
	const float inputX = float(board.width / 2.0f);
	const float inputY = float(board.height / 2.0f - 45);
	const float happyX = float((((config.columns) / 2.0) * 32) - 32);
	const float happyY = float(32 * ((config.rows) + 0.5f));
	const float debugX = float(((config.columns) * 32) - 304);
	const float debugY = float(32 * ((config.rows) + 0.5f));
	const float pausePlayX = float(((config.columns) * 32) - 240);
	const float pausePlayY = float(32 * ((config.rows) + 0.5f));
	const float leaderX = float(((config.columns) * 32) - 176);
	const float leaderY = float(32 * ((config.rows) + 0.5f));
	const int wholeDigitsStartX = 33;
	const int wholeDigitsStartY = int(32 * ((config.rows) + 0.5f) + 16);
	const int clockMinX = (((config.columns) * 32) - 97);
	const int clockMinY = 32 * ((config.rows) + 0.5f) + 16;
	const int clockSecX = (((config.columns) * 32) - 54);
	const int clockSecY = 32 * ((config.rows) + 0.5f) + 16;
	const float leaderWidth = 16 * config.columns;
	const float leaderHeight = (config.rows * 16) + 50;
	//---------------------------------------------------------------//

	sf::FloatRect welcomeRect;
	welcomeRect = welcome.getLocalBounds();
	welcome.setOrigin(welcomeRect.left + welcomeRect.width / 2.0f, welcomeRect.top + welcomeRect.height / 2.0f);
	welcome.setPosition(welcomeX, welcomeY);

	sf::Text prompt;           // Prompt for name construction
	prompt.setFont(font);
	prompt.setString("Enter your name:");
	prompt.setCharacterSize(20);
	prompt.setFillColor(sf::Color::White);
	prompt.setStyle(sf::Text::Bold);
	sf::FloatRect promptRect;
	promptRect = prompt.getLocalBounds();
	prompt.setOrigin(promptRect.left + promptRect.width / 2.0f, promptRect.top + promptRect.height / 2.0f);
	prompt.setPosition(promptX, promptY);

	string inputStr = "";      // Input text stuff
	sf::Text inputText;
	inputText.setFont(font);
	inputText.setCharacterSize(18);
	inputText.setFillColor(sf::Color::Yellow);
	inputText.setStyle(sf::Text::Bold);

	while (welcomeWindow.isOpen()) {
		//EVENT
		sf::Event event;

		while (welcomeWindow.pollEvent(event)) {
			if (event.type == sf::Event::Closed)  // Close window if user clicks 'X'
				welcomeWindow.close();
			shouldOpenGame = false;


			if (event.type == sf::Event::TextEntered) {   // Input text block
				if (inputStr.size() >= 10) {
					continue;
				}

				if (isalpha(event.text.unicode)) {
					inputStr += event.text.unicode;
					inputText.setString(inputStr + "|");

				}

			}

			if (event.type == sf::Event::KeyPressed) {        // Checks for backspace and enter keys
				if (event.key.code == sf::Keyboard::Backspace) {
					if (inputStr.size() == 0) {
						continue;
					}
					else {
						inputStr.pop_back();
						inputText.setString(inputStr + "|");
					}
				}

				if (event.key.code == sf::Keyboard::Enter) {
					if (inputStr.size() == 0) {
						continue;
					}
					string toLower = inputStr.substr(1, (inputStr.size() - 1));
					string temp = "";
					temp += toupper(inputStr[0]);
					for (int i = 0; i < toLower.size(); ++i) {
						temp += tolower(toLower[i]);
					}
					player.name = temp;
					currentName = temp;
					players.push_back(player);
					welcomeWindow.close();  // Close window when data is stored
					shouldOpenGame = true;
				}
			}

		}

		sf::FloatRect inputRect;   // Create input rectangle for display
		inputRect = inputText.getLocalBounds();
		inputText.setOrigin(inputRect.left + inputRect.width / 2.0f, inputRect.top + inputRect.height / 2.0f);
		inputText.setPosition(inputX, inputY);
		welcomeWindow.clear(sf::Color::Blue);
		welcomeWindow.draw(welcome);
		welcomeWindow.draw(prompt);
		if (inputStr.size() == 0) {  // Displays "|" on the start of the program, deletes after
			inputStr += "|";
			inputText.setString(inputStr);
			welcomeWindow.draw(inputText);
			inputStr.pop_back();
		}
		else {
			welcomeWindow.draw(inputText);
		}
		welcomeWindow.display();

	}


	// GAME WINDOW


	sf::RenderWindow gameWindow(sf::VideoMode(board.width, board.height), "Minesweeper", sf::Style::Close);  // Create game window

	sf::Texture tilereveal;
	tilereveal.loadFromFile("files/images/tile_revealed.png");
	sf::Texture tilehiddenData;
	tilehiddenData.loadFromFile("files/images/tile_hidden.png");


	sf::Texture happyFaceTexture;
	happyFaceTexture.loadFromFile("files/images/face_happy.png");
	sf::Texture sadFaceTexture;
	sadFaceTexture.loadFromFile("files/images/face_lose.png");

	sf::Texture winFaceText;
	winFaceText.loadFromFile("files/images/face_win.png");
	sf::Sprite faceWin;


	sf::Sprite happyFace;
	happyFace.setTexture(happyFaceTexture);        // Initialize Happy Face Sprite
	happyFace.setPosition(happyX, happyY);
	sf::FloatRect happyrect;
	happyrect = happyFace.getGlobalBounds();

	sf::Texture flagTexture;
	flagTexture.loadFromFile("files/images/flag.png");

	sf::Texture debugText;
	debugText.loadFromFile("files/images/debug.png");

	sf::Sprite debug;         // Create Debug sprite
	debug.setTexture(debugText);
	debug.setPosition(debugX, debugY);
	sf::FloatRect debugrect;
	debugrect = debug.getGlobalBounds();

	sf::Texture pauseText;
	pauseText.loadFromFile("files/images/pause.png");
	sf::Texture playText;
	playText.loadFromFile("files/images/play.png");

	sf::Sprite pausePlay;    // Create Pause button Sprite
	pausePlay.setTexture(pauseText);
	pausePlay.setPosition(pausePlayX, pausePlayY);
	sf::FloatRect pauseRect;
	pauseRect = pausePlay.getGlobalBounds();



	sf::Texture leaderText;
	leaderText.loadFromFile("files/images/leaderboard.png");

	sf::Sprite leaderboard;
	leaderboard.setTexture(leaderText);
	leaderboard.setPosition(leaderX, leaderY);
	sf::FloatRect leaderRect;
	leaderRect = leaderboard.getGlobalBounds();

	sf::Texture digitsText;
	digitsText.loadFromFile("files/images/digits.png");

	sf::Sprite digits;
	digits.setTexture(digitsText);


	sf::Texture mineText;
	mineText.loadFromFile("files/images/mine.png");

	sf::Sprite mine;
	mine.setTexture(mineText);



	map<int, sf::IntRect> counterMap;   // Map of counter digits, ***69 = neg sign
	counterMap.emplace(0, sf::IntRect(0, 0, 21, 32));
	counterMap.emplace(1, sf::IntRect(21, 0, 21, 32));
	counterMap.emplace(2, sf::IntRect(42, 0, 21, 32));
	counterMap.emplace(3, sf::IntRect(63, 0, 21, 32));
	counterMap.emplace(4, sf::IntRect(84, 0, 21, 32));
	counterMap.emplace(5, sf::IntRect(105, 0, 21, 32));
	counterMap.emplace(6, sf::IntRect(126, 0, 21, 32));
	counterMap.emplace(7, sf::IntRect(147, 0, 21, 32));
	counterMap.emplace(8, sf::IntRect(168, 0, 21, 32));
	counterMap.emplace(9, sf::IntRect(189, 0, 21, 32));
	counterMap.emplace(69, sf::IntRect(210, 0, 21, 32));


	map<int, sf::Texture> numsText;

	sf::Texture num1Text;
	num1Text.loadFromFile("files/images/number_1.png");
	numsText.emplace(1, num1Text);

	sf::Texture num2Text;
	num2Text.loadFromFile("files/images/number_2.png");
	numsText.emplace(2, num2Text);

	sf::Texture num3Text;
	num3Text.loadFromFile("files/images/number_3.png");
	numsText.emplace(3, num3Text);

	sf::Texture num4Text;
	num4Text.loadFromFile("files/images/number_4.png");
	numsText.emplace(4, num4Text);

	sf::Texture num5Text;
	num5Text.loadFromFile("files/images/number_5.png");
	numsText.emplace(5, num5Text);

	sf::Texture num6Text;
	num6Text.loadFromFile("files/images/number_6.png");
	numsText.emplace(6, num6Text);

	sf::Texture num7Text;
	num7Text.loadFromFile("files/images/number_7.png");
	numsText.emplace(7, num7Text);

	sf::Texture num8Text;
	num8Text.loadFromFile("files/images/number_8.png");
	numsText.emplace(8, num8Text);



	GenerateBoard(config, board, tilehiddenData, mine); // GENERATE BOARD
	AssignAdjTiles(board, config);
	MineCount(board, config);

	bool pause = false;
	bool play = true;
	bool printMines = false;         // Game Temps and Bools
	bool debugOn = false;
	bool debugOff = true;
	bool gameWin = false;
	bool gameLose = false;
	bool openLeader = false;
	bool winLeader = true;
	bool noRestart = false;
	bool accumTime = false;

	int mines = config.mines;

	sf::Texture wholeDigitsText;
	sf::Sprite wholeDigits;

	sf::Sprite pausedTile;
	pausedTile.setTexture(tilereveal);

	sf::Clock gameClock;
	sf::Clock pausedClock;
	sf::Clock leaderClock;
	sf::Time mainTime;
	sf::Time pausedTime;
	sf::Time leaderTime;

	GameClock gClock;
	

	LoadPlayer("files/leaderboard.txt", players);

	sf::Text leaderTitle;
	leaderTitle.setFont(font);
	leaderTitle.setString("LEADERBOARD");
	leaderTitle.setCharacterSize(20);
	leaderTitle.setFillColor(sf::Color::White);
	leaderTitle.setStyle(sf::Text::Bold | sf::Text::Underlined);
	sf::FloatRect leaderTitleRect;
	leaderTitleRect = leaderTitle.getLocalBounds();
	leaderTitle.setOrigin(leaderTitleRect.left + leaderTitleRect.width / 2.0f, leaderTitleRect.top + leaderTitleRect.height / 2.0f);
	leaderTitle.setPosition(leaderWidth / 2.0f, leaderHeight / 2.0f - 120);

	sf::Text leaderName;
	leaderName.setFont(font);
	leaderName.setString("1.\t" + players[0].time + "\t" + players[0].name + "\n\n" + "2.\t" + players[1].time + "\t" + players[1].name + "\n\n" + "3.\t" + players[2].time + "\t" + players[2].name + "\n\n" + "4.\t" + players[3].time + "\t" + players[3].name + "\n\n" + "5.\t" + players[4].time + "\t" + players[4].name);
	leaderName.setCharacterSize(18);
	leaderName.setFillColor(sf::Color::White);
	leaderName.setStyle(sf::Text::Bold);
	sf::FloatRect leaderNameRect;
	leaderNameRect = leaderName.getLocalBounds();
	leaderName.setOrigin(leaderNameRect.left + leaderNameRect.width / 2.0f, leaderNameRect.top + leaderNameRect.height / 2.0f);
	leaderName.setPosition(leaderWidth / 2.0f, leaderHeight / 2.0f + 20);
	
	

	while (gameWindow.isOpen() && shouldOpenGame) {                     // Minesweeper Game loop
		sf::Event gameEvent;  //Game Event Object
		gameWindow.clear(sf::Color::White);

		if (play) {
			if (accumTime) {
				gClock.pausedSecs += gClock.tempSecs;
			}
			mainTime = gameClock.getElapsedTime();       
			gClock.seconds = mainTime.asSeconds() - gClock.pausedSecs;
			gClock.digits = gClock.LastDigitSeconds(gClock.seconds);
			accumTime = false;
		}

		if (pause) {
			if (noRestart) {
				pausedClock.restart();
			}
			pausedTime = pausedClock.getElapsedTime();
			gClock.tempSecs = pausedTime.asSeconds();
			noRestart = false;
		}

		for (int i = 0; i < gClock.digits.size(); ++i) {
			int temp = gClock.digits.at(i);
			wholeDigitsText.loadFromFile("files/images/digits.png", counterMap.at(temp));
			wholeDigits.setTexture(wholeDigitsText);

			if (i == 0) {
				wholeDigits.setPosition(clockMinX, clockMinY);
				gameWindow.draw(wholeDigits);
			}
			if (i == 1) {
				wholeDigits.setPosition(clockMinX + 21, clockMinY);
				gameWindow.draw(wholeDigits);
			}

			if (i == 2) {
				wholeDigits.setPosition(clockSecX, clockSecY);
				gameWindow.draw(wholeDigits);
			}
			if (i == 3) {
				wholeDigits.setPosition(clockSecX + 21, clockSecY);
				gameWindow.draw(wholeDigits);
			}
		}


		while (gameWindow.pollEvent(gameEvent)) {   // Event loop
			
			if (openLeader) {
				continue;
			}

			if (gameEvent.type == sf::Event::Closed) {  // Close game window
				gameWindow.close();
			}
			
			
			if (gameEvent.type == sf::Event::MouseButtonPressed) {
				if (gameEvent.mouseButton.button == sf::Mouse::Left) {
					auto mouse = sf::Mouse::getPosition(gameWindow);   // Reveal Tile after left click
					if ((happyrect.contains(mouse.x, mouse.y) && (pause || play)) || (happyrect.contains(mouse.x, mouse.y) && gameWin) || (happyrect.contains(mouse.x, mouse.y) && gameLose)) {    //TODO: Implement debug feature
						board.tiles.clear();
						GenerateBoard(config, board, tilehiddenData, mine); // GENERATE NEW BOARD
						AssignAdjTiles(board, config);
						MineCount(board, config);
						happyFace.setTexture(happyFaceTexture);
						pause = false;
						play = true;
						pausePlay.setTexture(pauseText);
						printMines = false;
						debugOn = false;
						debugOff = true;
						gameWin = false;
						gameLose = false;
						mines = config.mines;
						winLeader = true;
						gameClock.restart();
						pausedClock.restart();
						gClock.seconds = 0;
						gClock.pausedSecs = 0;
						gClock.tempSecs = 0;
					}

					if (pauseRect.contains(mouse.x, mouse.y)) {
						if (play) {
							pausePlay.setTexture(playText);
							pause = true;
							play = false;
							noRestart = true;
						}

						else if (pause) {
							pausePlay.setTexture(pauseText);
							pause = false;
							play = true;
							accumTime = true;
						}

					}

					if (debugrect.contains(mouse.x, mouse.y) && play) {
						if (debugOff) {
							printMines = true;
							debugOn = true;
							debugOff = false;
						}

						else if (debugOn) {
							printMines = false;
							debugOff = true;
							debugOn = false;
						}

					}

			
					if ((leaderRect.contains(mouse.x, mouse.y) && (play || pause)) || leaderRect.contains(mouse.x, mouse.y) && gameLose || leaderRect.contains(mouse.x, mouse.y) && gameWin) {
						for (int i = 0; i < config.rows; ++i) {
							for (int j = 0; j < config.columns; ++j) {
								pausedTile.setPosition(board.tiles[i][j].coords);
								gameWindow.draw(pausedTile);
							}
						}
						vector<int> indDigits = LastDigitMine(mines);
						for (int i = 0; i < indDigits.size(); ++i) {
							int temp = indDigits.at(i);
							wholeDigitsText.loadFromFile("files/images/digits.png", counterMap.at(temp));
							wholeDigits.setTexture(wholeDigitsText);

							if (i > 0) {
								wholeDigits.setPosition(wholeDigitsStartX + (21 * i), wholeDigitsStartY);
								gameWindow.draw(wholeDigits);
							}
							else {
								wholeDigits.setPosition(wholeDigitsStartX, wholeDigitsStartY);
								gameWindow.draw(wholeDigits);
							}
						}
						gameWindow.draw(happyFace);
						gameWindow.draw(debug);
						gameWindow.draw(pausePlay);
						gameWindow.draw(leaderboard);
						gameWindow.display();
						int count = 0;
						players.resize(1);
						LoadPlayer("files/leaderboard.txt", players);
						leaderName.setString("1.\t" + players[1].time + "\t" + players[1].name + "\n\n" + "2.\t" + players[2].time + "\t" + players[2].name + "\n\n" + "3.\t" + players[3].time + "\t" + players[3].name + "\n\n" + "4.\t" + players[4].time + "\t" + players[4].name + "\n\n" + "5.\t" + players[5].time + "\t" + players[5].name);
						openLeader = true;
						sf::RenderWindow leaderWindow(sf::VideoMode(leaderWidth, leaderHeight), "Minesweeper", sf::Style::Close);
						while (leaderWindow.isOpen()) {
							sf::Event leaderEvent;
							if (count == 0) {
								leaderClock.restart();
							}
							accumTime = true;
							leaderTime = leaderClock.getElapsedTime();
							gClock.tempSecs = leaderTime.asSeconds();
							count += 1;
							leaderWindow.clear(sf::Color::Blue);
							while (leaderWindow.pollEvent(leaderEvent)) {
								if (leaderEvent.type == sf::Event::Closed) {
									leaderWindow.close();
								}
							}
							
							leaderWindow.draw(leaderTitle);
							leaderWindow.draw(leaderName);
							leaderWindow.display();
						}

					}

					for (int i = 0; i < config.rows; ++i) {
						for (int j = 0; j < config.columns; ++j) {
							if (board.tiles[i][j].tilerect.contains(mouse.x, mouse.y) && !board.tiles[i][j].flagged && play) {
								board.tiles[i][j].tileRevealed = true;
								if (board.tiles[i][j].mineCount == 0 && !board.tiles[i][j].containsMine) {
									RecursiveRevealTile(board.tiles[i][j].adjTiles);
								}
							}

							if (board.tiles[i][j].tilerect.contains(mouse.x, mouse.y) && board.tiles[i][j].containsMine && !board.tiles[i][j].flagged && play) {
								gameLose = true;
								board.tiles[i][j].tileRevealed = false;
								printMines = true;
							}

						}

					}
				}
			}

			if (gameEvent.type == sf::Event::MouseButtonPressed) {
				if (gameEvent.mouseButton.button == sf::Mouse::Right && play) {
					auto mouse = sf::Mouse::getPosition(gameWindow);   // Allows user to right click and place flags on screen
					for (int i = 0; i < config.rows; ++i) {
						for (int j = 0; j < config.columns; ++j) {
							if (board.tiles[i][j].tilerect.contains(mouse.x, mouse.y) && (board.tiles[i][j].tileRevealed == false && board.tiles[i][j].flagged == false)) {
								board.tiles[i][j].flag.setTexture(flagTexture);
								board.tiles[i][j].flag.setPosition(board.tiles[i][j].coords);
								board.tiles[i][j].flagged = true;
								mines -= 1;
							}

							else if (board.tiles[i][j].tilerect.contains(mouse.x, mouse.y) && (board.tiles[i][j].tileRevealed == false && board.tiles[i][j].flagged)) {
								board.tiles[i][j].flagged = false;
								mines += 1;
							}
						}
					}

				}
			}
		}
		openLeader = false;

		if (pause) {
			for (int i = 0; i < config.rows; ++i) {
				for (int j = 0; j < config.columns; ++j) {
					pausedTile.setPosition(board.tiles[i][j].coords);
					gameWindow.draw(pausedTile);
				}
			}
		}

		else {
			long count = 0;
			for (int i = 0; i < config.rows; ++i) {
				for (int j = 0; j < config.columns; ++j) {
					if (board.tiles[i][j].tileRevealed || board.tiles[i][j].containsMine) {
						count += 1;
					}
				}
			}
			if (count == (config.rows * config.columns)) {
				gameWin = true;
			}

			
			if (gameWin) {
				printMines = false;
				for (int i = 0; i < config.rows; ++i) {
					for (int j = 0; j < config.columns; ++j) {
						if (board.tiles[i][j].containsMine) {
							board.tiles[i][j].flag.setTexture(flagTexture);
							board.tiles[i][j].flag.setPosition(board.tiles[i][j].coords);
							board.tiles[i][j].flagged = true;
						}
					}
				}
				players[0].minutes = to_string(int(floor(int(gClock.seconds) / 60)));
				if (players[0].minutes.size() < 2) {
					players[0].minutes = "0" + players[0].minutes;
				}
				players[0].seconds = to_string(int(gClock.seconds) % 60);
				if (players[0].seconds.size() < 2) {
					players[0].seconds = "0" + players[0].seconds;
				}
				players[0].time = players[0].minutes + ":" + players[0].seconds;
				sort(players.begin(), players.end(), compareTime);

				happyFace.setTexture(winFaceText);
				play = false;
				pause = false;
			}

			if (gameLose) {
				happyFace.setTexture(sadFaceTexture);
				play = false;
				pause = false;
			}


			for (int i = 0; i < config.rows; ++i) {
				for (int j = 0; j < config.columns; ++j) {
					gameWindow.draw(board.tiles[i][j].tile);
					if (board.tiles[i][j].flagged && board.tiles[i][j].tileRevealed == false) {
						gameWindow.draw(board.tiles[i][j].flag);
					}

					if (board.tiles[i][j].tileRevealed) {  // Assigns number to tiles (num = amount of mines adj to that tile)
						if (1 <= board.tiles[i][j].mineCount <= 8) {
							board.tiles[i][j].tile.setTexture(tilereveal);
							gameWindow.draw(board.tiles[i][j].tile);
							sf::Sprite num;
							num.setTexture(numsText[board.tiles[i][j].mineCount]);
							num.setPosition(board.tiles[i][j].coords);
							gameWindow.draw(num);
						}
						else{
							board.tiles[i][j].tile.setTexture(tilereveal);
							gameWindow.draw(board.tiles[i][j].tile);
						}
					}
					if (printMines) {
						if (board.tiles[i][j].containsMine) {
							gameWindow.draw(board.tiles[i][j].mine);
						}
					}
					
				}

			}

		}
		

		vector<int> indDigits = LastDigitMine(mines);
		for (int i = 0; i < indDigits.size(); ++i) {
			int temp = indDigits.at(i);
			wholeDigitsText.loadFromFile("files/images/digits.png", counterMap.at(temp));
			wholeDigits.setTexture(wholeDigitsText);

			if (i > 0) {
				wholeDigits.setPosition(wholeDigitsStartX + (21 * i), wholeDigitsStartY);
				gameWindow.draw(wholeDigits);
			}
			else {
				wholeDigits.setPosition(wholeDigitsStartX, wholeDigitsStartY);
				gameWindow.draw(wholeDigits);
			}
		}
		

		gameWindow.draw(happyFace);
		gameWindow.draw(debug);
		gameWindow.draw(pausePlay);
		gameWindow.draw(leaderboard);
		gameWindow.display();

		if (gameWin && winLeader) {
			writeToLeader(players, "files/leaderboard.txt");
			LoadPlayer("files/leaderboard.txt", players);
			players.resize(5);
			for (int i = 0; i < players.size(); ++i) {
				if ((players[i].name.find(currentName) != string::npos) && i == 0) {
					players[i].name += "*";
				}
			}
			leaderName.setString("1.\t" + players[0].time + "\t" + players[0].name + "\n\n" + "2.\t" + players[1].time + "\t" + players[1].name + "\n\n" + "3.\t" + players[2].time + "\t" + players[2].name + "\n\n" + "4.\t" + players[3].time + "\t" + players[3].name + "\n\n" + "5.\t" + players[4].time + "\t" + players[4].name);
			sf::RenderWindow leaderWindow(sf::VideoMode(leaderWidth, leaderHeight), "Minesweeper", sf::Style::Close);
			while (leaderWindow.isOpen()) {
				sf::Event leaderEvent;
				leaderWindow.clear(sf::Color::Blue);
				while (leaderWindow.pollEvent(leaderEvent)) {
					if (leaderEvent.type == sf::Event::Closed) {
						leaderWindow.close();
					}
				}
				leaderWindow.draw(leaderTitle);
				leaderWindow.draw(leaderName);
				for (int i = 0; i < players.size(); ++i) {
					if (players[i].name.find("*") != string::npos) {
						players[i].name.pop_back();
					}
				}
				leaderWindow.display();
			}
			winLeader = false;
		}
	}
	return 0;
}