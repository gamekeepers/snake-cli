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
using namespace std;
using namespace std::this_thread;

class HighScores {
    vector<int> scores;
public:
    void update(int score) {
        scores.push_back(score);
        sort(scores.begin(), scores.end(), greater<int>());
        if (scores.size() > 10) scores.resize(10);
    }
    void show() const {
        cout << "\n=== TOP 10 HIGH SCORES ===\n";
        if (scores.empty()) {
            cout << "No scores yet.\n";
            return;
        }
        for (size_t i = 0; i < scores.size(); i++) {
            cout << i + 1 << ". " << scores[i] << endl;
        }
    }
};

class Game {
    int size;
    deque<pair<int, int>> snake;
    pair<int, int> food;
    pair<int, int> poison;
    char direction;
    bool paused;
    int score;
    int steps;
    int foodsEaten;
    int speedMs;
    HighScores &highScores; // reference to shared highscore tracker

    pair<int, int> randomCellExcluding(const pair<int, int> &other = {-1, -1}) {
        pair<int, int> p;
        do {
            p = make_pair(rand() % size, rand() % size);
        } while (find(snake.begin(), snake.end(), p) != snake.end() || p == other);
        return p;
    }

    pair<int, int> getNextHead(pair<int, int> current) {
        if (direction == 'r') {
            return make_pair(current.first, (current.second + 1) % size);
        } else if (direction == 'l') {
            return make_pair(current.first, current.second == 0 ? size - 1 : current.second - 1);
        } else if (direction == 'd') {
            return make_pair((current.first + 1) % size, current.second);
        } else if (direction == 'u') {
            return make_pair(current.first == 0 ? size - 1 : current.first - 1, current.second);
        }
        return current;
    }

    void render() {
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                if (i == food.first && j == food.second) {
                    cout << "🍎";
                } else if (i == poison.first && j == poison.second) {
                    cout << "💀";
                } else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
                    cout << "🐍";
                } else {
                    cout << "⬜";
                }
            }
            cout << endl;
        }
        cout << "Length: " << snake.size()
             << " | Score: " << score
             << (paused ? " | PAUSED (press 'p' to resume)" : "") << endl;
    }

    void gameOver(const string &reason) {
        system("clear");
        cout << "Game Over - " << reason << "\nFinal Score: " << score << endl;
        highScores.update(score);
        highScores.show();
        exit(0);
    }

public:
    Game(int gridSize, HighScores &hs)
        : size(gridSize), direction('r'), paused(false), score(0), steps(0),
          foodsEaten(0), speedMs(500), highScores(hs) {
        snake.push_back(make_pair(0, 0));
        food = randomCellExcluding();
        poison = randomCellExcluding(food);
    }

    void togglePause() { paused = !paused; }

    void changeDirection(char newDir) { direction = newDir; }

    //this is the loop wherein snake goes about doing its job
    void loop() {
        for (pair<int, int> head = make_pair(0, 1);; head = getNextHead(head, direction)) {
            cout << "\033[H";
            if (paused) {
                render();
                sleep_for(200ms);
                continue;
            }

            // self collision - feature
            if (find(snake.begin(), snake.end(), head) != snake.end()) {
                gameOver("You hit yourself!");
            }

            // poison collision - feature
            if (head == poison) {
                gameOver("You ate poison!");
            }
 
            // normal food  - feature
            if (head == food) {
                snake.push_back(head);
                score += 10;
                foodsEaten++;
                if (foodsEaten % 10 == 0 && speedMs > 100) speedMs -= 50;
                food = randomCellExcluding(poison);
            } else {
                snake.push_back(head);
                snake.pop_front();
            }

            // move poison - feature
            steps++;
            if (steps % 10 == 0) poison = randomCellExcluding(food);

            render();
            sleep_for(std::chrono::milliseconds(speedMs));
        }
    }
};

// Separate input handler function but pass Game to it -> this is to pass the game to it and separate input handler, to make it easy
void inputHandler(Game &game) {
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
            game.changeDirection(keymap[input]);
        } else if (input == 'q') {
            exit(0);
        } else if (input == 'p') {
            game.togglePause();
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}


