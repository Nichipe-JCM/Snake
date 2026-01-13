#include <iostream>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include <random>

using namespace std;

enum TileType {
	EMPTY = 0,
	APPLE = 1,
	HEAD = 2,
	BODY = 3,
	WALL = 4
};

struct Point {
	int x, y;
};

int getRandomInt(int min, int max) {
	static random_device rd;
	static mt19937 gen(rd());
	uniform_int_distribution<> dis(min, max);
	return dis(gen);
}

Point getRandomEmptyPosition(const vector<vector<int>>& map, int& score) {
	while (true) {
		int y = getRandomInt(1, map.size() - 1);
		int x = getRandomInt(1, map[0].size() - 1);
		if (map[y][x] == EMPTY) {
			return { x, y };
		}
	}
}

Point DirectionToPoint(int direction) {
	switch (direction) {
	case 0: return { 0, -1 }; // Up
	case 1: return { 0, 1 };  // Down
	case 2: return { -1, 0 }; // Left
	case 3: return { 1, 0 };  // Right
	default: return { 0, 0 };
	}
}

void setnextPos(Point& nextPos, int& direction, vector<Point>& snakeBody, const vector<vector<int>>& map) {
	Point temp = DirectionToPoint(direction);
	nextPos = { snakeBody[0].x + temp.x, snakeBody[0].y + temp.y };
}

void hideCursor(HANDLE handle) {
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 1;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(handle, &info);
}


int main() {

	int height = 12;
	int width = 22;

	vector<vector<int>> map(height, vector<int>(width, EMPTY));

	vector<Point> snakeBody;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (y == 0 || y == height - 1 || x == 0 || x == width - 1) {
				map[y][x] = WALL;
			}
			else {
				map[y][x] = EMPTY;
			}
		}
	}

	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	int tick = 5; // 입력 보정용 틱
	int speedDelay = 100; // 게임 스피드 조절
	int bufferTick = 0;
	int score = 0;

	int direction = 3; // 0: Up, 1: Down, 2: Left, 3: Right
	int lastdir = 3;

	int defaultLength = 3;
	for (int i = 0; i < defaultLength; i++) {
		snakeBody.push_back({ 5 - i, 5 });
	}
	for (int i = 0; i < snakeBody.size(); i++) {
		Point p = snakeBody[i];
		if (i == 0)
			map[p.y][p.x] = HEAD;
		else
			map[p.y][p.x] = BODY;
	}

	Point applePos = getRandomEmptyPosition(map, score);
	map[applePos.y][applePos.x] = APPLE;

	bool isGameOver = false;
	Point nextPos = { 0,0 };
	setnextPos(nextPos, direction, snakeBody, map);


	cout << "스네이크 게임" << endl;
	cout << "WASD 키로 조작하세요." << endl;
	cout << "사과를 최대한 많이 먹으세요" << endl;
	system("pause");


	hideCursor(consoleHandle);



	while (!isGameOver) {
		if (_kbhit()) {
			int key = _getch();
			switch (key) {
			case 'w':
				if (direction != 1 && lastdir != 1) direction = 0;
				break;
			case 's':
				if (direction != 0 && lastdir != 0) direction = 1;
				break;
			case 'a':
				if (direction != 3 && lastdir != 3) direction = 2;
				break;
			case 'd':
				if (direction != 2 && lastdir != 2) direction = 3;
				break;
			}
		}
		Sleep(tick);
		bufferTick += tick;
		if (bufferTick < speedDelay) continue;
		setnextPos(nextPos, direction, snakeBody, map);

		bufferTick = 0;
		if (nextPos.y >= height || nextPos.y < 0 || nextPos.x >= width || nextPos.x < 0) {
			isGameOver = true;
			cout << "Game Over!" << endl;
			break;
		}
		if (map[nextPos.y][nextPos.x] == WALL || map[nextPos.y][nextPos.x] == BODY) {
			isGameOver = true;
			cout << "Game Over!" << endl;
			break;
		}
		if (map[nextPos.y][nextPos.x] == APPLE) {
			if (score == (map.size() - 2) * (map[0].size() - 2) - defaultLength) {
				score++;
				cout << "You Win!" << endl;
				Sleep(1000);
				system("pause");
				exit(0);
			}
			snakeBody.insert(snakeBody.begin(), nextPos);
			map[nextPos.y][nextPos.x] = HEAD;
			map[snakeBody[1].y][snakeBody[1].x] = BODY;
			applePos = getRandomEmptyPosition(map, score);
			map[applePos.y][applePos.x] = APPLE;
			score++;
			lastdir = direction;
		}
		else {
			Point tail = snakeBody.back();
			map[tail.y][tail.x] = EMPTY;
			snakeBody.pop_back();
			snakeBody.insert(snakeBody.begin(), nextPos);
			map[nextPos.y][nextPos.x] = HEAD;
			map[snakeBody[1].y][snakeBody[1].x] = BODY;
			lastdir = direction;
		}

		SetConsoleCursorPosition(consoleHandle, { 0,0 });
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				switch (map[y][x]) {
				case EMPTY:
					cout << "  ";
					break;

				case APPLE:
					cout << "● ";
					break;

				case HEAD:
					switch (direction) {
					case 0:
						cout << "▲ ";
						break;

					case 1:
						cout << "▼ ";
						break;

					case 2:
						cout << "◀ ";
						break;

					case 3:
						cout << "▶ ";
						break;

					}
					break;

				case BODY:
					cout << "■ ";
					break;

				case WALL:
					cout << "# ";
					break;
				}
			}
			cout << endl;
		}
		cout << "Score: " << score << endl;
	}
	cout << "Final Score: " << score << endl;
	cout << "Thanks for playing!" << endl;
	Sleep(1000);
	system("pause");
	return 0;
}