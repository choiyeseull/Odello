#include <bangtal>
using namespace bangtal;
using namespace std;

ObjectPtr board[8][8], black_turn, white_turn;
ScenePtr scene;
ObjectPtr Black_S, Black_s, White_S, White_s; 
ObjectPtr game_over;

enum class State {
	BLANK,
	POSSIBLE,
	BLACK,
	WHITE
};
State board_state[8][8];	

enum class Turn {
	BLACK,
	WHITE
};
Turn turn = Turn::BLACK;	

void setState(int x, int y, State state) {
	switch (state) {
	case State::BLANK: board[y][x]->setImage("images/blank.png"); break;
	case State::POSSIBLE: board[y][x]->setImage(turn == Turn::BLACK ? "images/black possible.png" : "images/white possible.png"); break;
	case State::BLACK: board[y][x]->setImage("images/black.png"); break;
	case State::WHITE: board[y][x]->setImage("images/white.png"); break;
	}
	board_state[y][x] = state;
}

bool checkPossible(int x, int y, int dx, int dy) {
	State self;
	State other;
	if (turn == Turn::BLACK) {
		self = State::BLACK;
		other = State::WHITE;
	}
	else if (turn == Turn::WHITE) {
		self = State::WHITE;
		other = State::BLACK;
	}

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {	
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			return possible;						
		}
		else {
			return false;
		}
	}
	return false;
}

bool checkPossible(int x, int y) {
	if (board_state[y][x] == State::BLACK) return false;	
	if (board_state[y][x] == State::WHITE) return false;
	setState(x, y, State::BLANK);
	int delta[8][2] = {
		{0,1},
		{1,1},
		{1,0},
		{1,-1},
		{0,-1},
		{-1,-1},
		{-1,0},
		{-1,1},

	};
	bool possible = false;
	for (auto d : delta) {
		if (checkPossible(x, y, d[0], d[1])) possible = true;
	}
	return possible;
}

void reverse(int x, int y, int dx, int dy) {
	State self;
	State other;
	if (turn == Turn::BLACK) {
		self = State::BLACK;
		other = State::WHITE;
	}
	else if (turn == Turn::WHITE) {	
		self = State::WHITE;
		other = State::BLACK;
	}	
	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			if (possible) {

				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {	
					if (board_state[y][x] == other) {	
						setState(x, y, self);	
					}
					else {
						return;
					}
				}
			}
		}
		else {
			return;
		}
	}
}

void reverse(int x, int y) {
	int delta[8][2] = {
		{0,1},
		{1,1},
		{1,0},
		{1,-1},
		{0,-1},
		{-1,-1},
		{-1,0},
		{-1,1},

	};
	bool possible = false;
	for (auto d : delta) {
		reverse(x, y, d[0], d[1]);	
	}
}

bool setPossible() {
	bool possible = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {	
			if (checkPossible(x, y)) {	
				setState(x, y, State::POSSIBLE);	
				possible = true;
			}
		}
	}
	return possible;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int count_reverse(int x, int y, int dx, int dy) {
	State self = State::WHITE;	
	State other = State::BLACK;

	int count = 0;
	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			if (possible) {

				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {	
					if (board_state[y][x] == other) {	
						count++;			
					}
					else {	
						return count;
					}
				}
			}
		}
		else {	
			return count;
		}
	}
	return 0; //시발......이거때문이었음
}

struct change {
	int x;
	int y;
	int count = 0;
} choice;

void count_reverse(int x, int y) {
	int delta[8][2] = {
		{0,1},
		{1,1},
		{1,0},
		{1,-1},
		{0,-1},
		{-1,-1},
		{-1,0},
		{-1,1},

	};
	int howmany = 0;	
	for (int i = 0; i < 8; i++) {	
		howmany += count_reverse(x, y, delta[i][0], delta[i][1]);		
	}
	if (choice.count <=  howmany){
		choice.x = x;
		choice.y = y;
		choice.count = howmany;
	}
}
void calcul(int x, int y) {
	count_reverse(x, y);
}

void change_score() {
	
	int black_count = 0;
	int white_count = 0;
	string filename;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board_state[i][j] == State::BLACK) {
				black_count++;
			}
			else if (board_state[i][j] == State::WHITE) {
				white_count++;
			}
		}
	}

	if (black_count + white_count == 64) {
		game_over->show();
		if (black_count > white_count) {
			showMessage("검은돌이 이겼습니다.");
		}
		else if (black_count < white_count) {
			showMessage("흰돌이 이겼습니다.");
		}
		else if (black_count == white_count) {
			showMessage("무승부");
		}
	}

	filename = "images/L" + to_string(black_count/10) + ".png";
	Black_S->setImage(filename);
	filename = "images/L" + to_string(black_count % 10) + ".png";
	Black_s->setImage(filename);

	filename = "images/L" + to_string(white_count / 10) + ".png";
	White_S->setImage(filename);
	filename = "images/L" + to_string(white_count % 10) + ".png";
	White_s->setImage(filename);


}

void computer() {
	auto timer = Timer::create(2.f);
	timer->start();
	timer->setOnTimerCallback([&](TimerPtr)->bool {
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				if (board_state[y][x] == State::POSSIBLE) {

					calcul(x, y);

				}
			}
		}
		setState(choice.x, choice.y, State::WHITE);
		reverse(choice.x, choice.y);
		change_score();

		choice.count = 0;
		turn = Turn::BLACK;
		white_turn->hide();
		black_turn->show();

		if (!setPossible()) {
			//showMessage("흰돌 놓을 자리가 없습니다.");
			turn = Turn::BLACK;
			white_turn->hide();
			black_turn->show();
		}
		return true;
		});
}

int main() {
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	scene = Scene::create("Othello", "images/background.png");
	black_turn = Object::create("images/black turn.png", scene, 770, 360, true);
	white_turn = Object::create("images/white turn.png", scene, 1100, 360, false);
	game_over = Object::create("images/game end.png", scene, 935, 350, false);

	Black_S = Object::create("images/L0.png", scene, 750, 230, true);
	Black_s = Object::create("images/L2.png", scene, 820, 230, true);
	White_S = Object::create("images/L0.png", scene, 1070, 230, true);
	White_s = Object::create("images/L2.png", scene, 1140, 230, true);

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = Object::create("images/blank.png", scene, 40 + x * 80, 40 + y * 80);
			board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr object, int, int, MouseAction)->bool {
				if (board_state[y][x] == State::POSSIBLE) {
						setState(x, y, State::BLACK);
						reverse(x, y);
						change_score();

						turn = Turn::WHITE;	
						white_turn->show();
						black_turn->hide();

						if (!setPossible()) {
							//showMessage("검은돌 놓을 자리가 없습니다");
							turn = Turn::WHITE;
							white_turn->show();
							black_turn->hide();
						}
						computer();
				}
				return true;
				});
			board_state[y][x] = State::BLANK;
		}
	}
	setState(3, 3, State::BLACK);
	setState(4, 4, State::BLACK);
	setState(3, 4, State::WHITE);
	setState(4, 3, State::WHITE);
	setPossible();

	startGame(scene);
}