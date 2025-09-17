#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <map>
#include <deque>
#include <algorithm>
#include <fstream>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

const int BOARD_SIZE = 10;
char direction = 'r';
bool paused = false;
int score = 0;
vector<int> highScores;   // top 10 scores
pair<int, int> poisonousFood = {-1, -1}; // init with invalid pos

// ------------------ Utility ------------------
pair<int, int> random_food(const deque<pair<int, int>>& snake) {
    pair<int, int> pos;
    do {
        pos = make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
    } while (find(snake.begin(), snake.end(), pos) != snake.end() || pos == poisonousFood);
    return pos;
}

pair<int, int> random_poison(const deque<pair<int, int>>& snake, pair<int, int> food) {
    pair<int, int> pos;
    do {
        pos = make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
    } while (find(snake.begin(), snake.end(), pos) != snake.end() || pos == food);
    return pos;
}

// ------------------ Input ------------------
void input_handler() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {
        {'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}
    };

    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            direction = keymap[input];
        } else if (input == 'q') {
            exit(0);
        } else if (input == 'p') {  // toggle pause
            paused = !paused;
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// ------------------ Rendering ------------------
void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == food.first && j == food.second) {
                cout << "🍎"; // normal food
            } else if (i == poisonousFood.first && j == poisonousFood.second) {
                cout << "💀"; // poisonous food
            } else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end()) {
                cout << "🐍";
            } else {
                cout << "⬜";
            }
        }
        cout << endl;
    }
    cout << "Score: " << score << endl;
}

// ------------------ Game Mechanics ------------------
pair<int,int> get_next_head(pair<int,int> current, char dir) {
    pair<int, int> next;
    if (dir == 'r') {
        next = make_pair(current.first, (current.second + 1) % BOARD_SIZE);
    } else if (dir == 'l') {
        next = make_pair(current.first, current.second == 0 ? BOARD_SIZE - 1 : current.second - 1);
    } else if (dir == 'd') {
        next = make_pair((current.first + 1) % BOARD_SIZE, current.second);
    } else if (dir == 'u') {
        next = make_pair(current.first == 0 ? BOARD_SIZE - 1 : current.first - 1, current.second);
    }
    return next;
}

void save_score(int s) {
    highScores.push_back(s);
    sort(highScores.rbegin(), highScores.rend());
    if (highScores.size() > 10) highScores.pop_back();

    ofstream file("scores.txt");
    for (int sc : highScores) file << sc << endl;
    file.close();
}

void load_scores() {
    ifstream file("scores.txt");
    int s;
    while (file >> s) highScores.push_back(s);
    file.close();
    sort(highScores.rbegin(), highScores.rend());
    if (highScores.size() > 10) highScores.resize(10);
}

void game_play() {
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));
    score = 0;
    load_scores();

    pair<int, int> food = random_food(snake);
    poisonousFood = random_poison(snake, food);

    for (pair<int, int> head = make_pair(0, 1); ; head = get_next_head(head, direction)) {
        if (paused) {
            this_thread::sleep_for(200ms);
            continue;
        }

        cout << "\033[H";

        // Collision with self
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over (hit yourself)\nFinal Score: " << score << endl;
            save_score(score);
            exit(0);
        }

        // Eating normal food
        if (head == food) {
            score += 10;
            food = random_food(snake);
            poisonousFood = random_poison(snake, food);
            snake.push_back(head);
        }
        // Eating poisonous food
        else if (head == poisonousFood) {
            system("clear");
            cout << "Game Over (ate poison)\nFinal Score: " << score << endl;
            save_score(score);
            exit(0);
        }
        // Just move
        else {
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(BOARD_SIZE, snake, food);

        // Difficulty increases with score
        int speed = max(100, 500 - score * 10);
        sleep_for(milliseconds(speed));
    }
}
