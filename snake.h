#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <termios.h>
#include <unistd.h> 
#include <map>
#include <deque>
#include <algorithm>
#include <fstream>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

    
char direction = 'r';
bool paused = false;

void input_handler() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);   
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {
        {'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}
    };

    while (true) {
        char input = getchar();
        if (keymap.count(input)) {
            direction = keymap[input];
        } else if (input == 'p') {
            paused = !paused;
        } else if (input == 'q') {
            exit(0);
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); 
}


void render_game(int size, const deque<pair<int, int>>& snake,
                 const pair<int, int>& food, const pair<int, int>& poison) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == food.first && j == food.second) {
                cout << "🍎";
            } else if (i == poison.first && j == poison.second) {
                cout << "☠️";
            } else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end()) {
                cout << "🐍";
            } else {
                cout << "⬜";
            }
        }
        cout << "\n";
    }
}


pair<int, int> get_next_head(const pair<int, int>& current, char dir) {
    int x = current.first;
    int y = current.second;

    if (dir == 'r') return {x, (y + 1) % 10};
    if (dir == 'l') return {x, y == 0 ? 9 : y - 1};
    if (dir == 'd') return {(x + 1) % 10, y};
    if (dir == 'u') return {x == 0 ? 9 : x - 1, y};

    return current; 
}

pair<int, int> generate_food(const deque<pair<int, int>>& snake,
                             const pair<int, int>& avoid) {
    pair<int, int> pos;
    do {
        pos = {rand() % 10, rand() % 10};
    } while (find(snake.begin(), snake.end(), pos) != snake.end() || pos == avoid);
    return pos;
}


void update_highscores(int score) {
    vector<int> scores;
    ifstream infile("highscores.txt");
    int s;
    while (infile >> s) scores.push_back(s);
    infile.close();

    scores.push_back(score);
    sort(scores.begin(), scores.end(), greater<int>());
    if (scores.size() > 10) scores.resize(10);

    ofstream outfile("highscores.txt");
    for (int sc : scores) outfile << sc << "\n";
    outfile.close();

    cout << "\n🏆 Top 10 High Scores 🏆\n";
    for (size_t i = 0; i < scores.size(); i++) {
        cout << i + 1 << ". " << scores[i] << "\n";
    }
}


void game_play() {
    system("clear");
    deque<pair<int, int>> snake = {{0, 0}};
    pair<int, int> food = generate_food(snake, {-1, -1});
    pair<int, int> poison = generate_food(snake, food);

    int score = 0, foodEaten = 0, speed = 500;

    for (pair<int, int> head = {0, 1};; head = get_next_head(head, direction)) {
        cout << "\033[H"; 
        if (paused) {
            cout << "Game Paused - Press 'p' to Resume\n";
            sleep_for(200ms);
            continue;
        }

        // Collisions
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over! You ate yourself 🐍\n";
            cout << "Final Score: " << score << "\n";
            update_highscores(score);
            exit(0);
        }
        if (head == food) {
            food = generate_food(snake, poison);
            poison = generate_food(snake, food);
            snake.push_back(head);
            foodEaten++;
            score += 10;
            if (foodEaten % 10 == 0 && speed > 100) speed -= 50;
        } else if (head == poison) {
            system("clear");
            cout << "Game Over! You ate poison ☠️\n";
            cout << "Final Score: " << score << "\n";
            update_highscores(score);
            exit(0);
        } else {
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(10, snake, food, poison);
        cout << "Score: " << score << "\n";
        cout << "Length of snake: " << snake.size() << "\n";
        cout << "Speed: " << speed << "ms\n";

        sleep_for(milliseconds(speed));
    }
}
