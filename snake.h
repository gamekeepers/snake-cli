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
#include<set>
using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

char direction = 'r';
bool paused = false;
multiset<int, greater<int>> topscores;


void loadScores() {
    ifstream infile("scores.txt");
    int score;
    while (infile >> score) {
        topscores.insert(score);
    }
    infile.close();
}

void saveScores() {
    ofstream outfile("scores.txt");
    int count = 0;
    for (int score : topscores) {
        if (count++ == 10) break; 
        outfile << score << "\n";
    }
    outfile.close();
}

void showTopScores() {
    cout << "\n=== Top Scores ===\n";
    int count = 0;
    for (int s : topscores) {
        cout << ++count << ". " << s << endl;
        if (count == 10) break;
    }
    cout << "==================\n";
}

void input_handler() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}};
    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            direction = keymap[input];
        } else if (input == 'x') {
            paused = !paused;
        } else if (input == 'q') {
            saveScores();
            exit(0);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poisonfood) {
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (i == food.first && j == food.second) {
                cout << "🍎";
            } else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
                cout << "🐍";
            } else if (i == poisonfood.first && j == poisonfood.second) {
                cout << "💀";
            } else {
                cout << "⬜";
            }
        }
        cout << endl;
    }
}

pair<int, int> get_next_head(pair<int, int> current, char direction) {
    pair<int, int> next;
    if (direction == 'r') {
        next = make_pair(current.first, (current.second + 1) % 10);
    } else if (direction == 'l') {
        next = make_pair(current.first, current.second == 0 ? 9 : current.second - 1);
    } else if (direction == 'd') {
        next = make_pair((current.first + 1) % 10, current.second);
    } else if (direction == 'u') {
        next = make_pair(current.first == 0 ? 9 : current.first - 1, current.second);
    }
    return next;
}

void gameOver(int score, const string &reason) {
    system("clear");
    cout << "Game Over! " << reason << endl;
    cout << "Final Score: " << score << " points\n";

    topscores.insert(score);
    saveScores();

    showTopScores();
    exit(0);
}

void game_play() {
    system("clear");

    loadScores();
    showTopScores(); 

    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food;
    do {
        food = make_pair(rand() % 10, rand() % 10);
    } while (find(snake.begin(), snake.end(), food) != snake.end());

    pair<int, int> poisonfood = make_pair(-1, -1);
    pair<int, int> head = make_pair(0, 1);

    while (true) {
        cout << "\033[H";
        int score = snake.size() * 10;

        if (paused) {
            render_game(10, snake, food, poisonfood);
            cout << "Game paused. Press x to continue" << endl;
            cout << "Score: " << score << " points" << endl;
            sleep_for(std::chrono::milliseconds(200));
            continue;
        }
         
        head = get_next_head(head, direction);

        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            gameOver(score, "You hit yourself!");
        }

        else if (head.first == food.first && head.second == food.second) {
            do {
                food = make_pair(rand() % 10, rand() % 10);
            } while (find(snake.begin(), snake.end(), food) != snake.end());

            if (rand() % 3 == 0) {
                do {
                    poisonfood = make_pair(rand() % 10, rand() % 10);
                } while (find(snake.begin(), snake.end(), poisonfood) != snake.end() && food == poisonfood);
            } else {
                poisonfood = make_pair(-1, -1);
            }
            snake.push_back(head);
        }

        else if (head == poisonfood) {
            gameOver(score, "You ate poisonous food!");
        }

        else {
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(10, snake, food, poisonfood);
        cout << "length of snake: " << snake.size() << endl;
        cout << "Score: " << score << " points" << endl;

        int base_delay = 500;
        int min_delay = 100;
        int reduction = (snake.size() / 10) * 50;
        int current_delay = max(min_delay, base_delay - reduction);

        sleep_for(std::chrono::milliseconds(current_delay));
    }
}
