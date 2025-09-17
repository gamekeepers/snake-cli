#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <map>
#include <deque>
#include <algorithm>
#include <fstream>
#include <string>

// --- Platform-specific includes ---
#ifdef _WIN32
#include <conio.h> // For Windows
#else
#include <termios.h> // For Unix-like systems
#include <unistd.h> // for system clear
#endif

using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

// --- Global variables ---
char direction = 'r';
bool is_paused = false;
int score = 0;
int game_speed = 500;

// --- Function declarations ---
void input_handler();
void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poison);
pair<int, int> get_next_head(pair<int, int> current, char direction);
void game_play();
pair<int, int> generate_food(int size, deque<pair<int, int>> &snake);
void update_high_scores(int score);
void display_high_scores();

// --- Function definitions ---

// Handles user input (Cross-Platform)
void input_handler() {
#ifdef _WIN32
    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}, {'p', 'p'}};
    while (true) {
        if (_kbhit()) {
            char input = _getch();
            if (keymap.find(input) != keymap.end()) {
                if (input == 'p') {
                    is_paused = !is_paused;
                } else {
                    direction = keymap[input];
                }
            } else if (input == 'q') {
                exit(0);
            }
        }
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}, {'p', 'p'}};
    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            if (input == 'p') {
                is_paused = !is_paused;
            } else {
                direction = keymap[input];
            }
        } else if (input == 'q') {
            exit(0);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}

// Renders the game on the screen
void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poison) {
    cout << "\033[H"; // Move cursor to top-left
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (i == food.first && j == food.second) {
                cout << "🍎";
            } else if (i == poison.first && j == poison.second) {
                cout << "☠️ "; // Poison emoji
            } else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
                cout << "🐍";
            } else {
                cout << "⬜";
            }
        }
        cout << endl;
    }
    cout << "Score: " << score << endl;
    cout << "Length of snake: " << snake.size() << endl;
}

// Calculates the next position of the snake's head
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

// Generates food at a random position, avoiding the snake
pair<int, int> generate_food(int size, deque<pair<int, int>> &snake) {
    pair<int, int> food;
    while (true) {
        food = make_pair(rand() % size, rand() % size);
        if (find(snake.begin(), snake.end(), food) == snake.end()) {
            break;
        }
    }
    return food;
}

// Updates and stores high scores
void update_high_scores(int score) {
    vector<int> scores;
    ifstream infile("scores.txt");
    int s;
    while (infile >> s) {
        scores.push_back(s);
    }
    infile.close();

    scores.push_back(score);
    sort(scores.rbegin(), scores.rend());

    ofstream outfile("scores.txt");
    for (int i = 0; i < min((int)scores.size(), 10); ++i) {
        outfile << scores[i] << endl;
    }
    outfile.close();
}

// Displays the high scores
void display_high_scores() {
    cout << "--- High Scores ---" << endl;
    ifstream infile("scores.txt");
    int s;
    int rank = 1;
    while (infile >> s) {
        cout << rank++ << ". " << s << endl;
    }
    infile.close();
}

// Main game logic
void game_play() {
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food = generate_food(10, snake);
    pair<int, int> poison = generate_food(10, snake); // Initial poison position

    int poison_move_counter = 0;

    for (pair<int, int> head = make_pair(0, 1);; head = get_next_head(head, direction)) {
        if (is_paused) {
            // If paused, just sleep and continue to the next iteration
            sleep_for(100ms);
            continue;
        }

        // --- Game logic ---
        if (find(snake.begin(), snake.end(), head) != snake.end() || (head.first == poison.first && head.second == poison.second)) {
            system("clear");
            cout << "Game Over" << endl;
            cout << "Your score: " << score << endl;
            update_high_scores(score);
            display_high_scores();
            exit(0);
        } else if (head.first == food.first && head.second == food.second) {
            score += 10;
            food = generate_food(10, snake);
            snake.push_back(head);
            if (snake.size() % 10 == 0) {
                game_speed = max(100, game_speed - 100); // Increase speed, with a minimum delay
            }
        } else {
            snake.push_back(head);
            snake.pop_front();
        }

        // Move poison every 5 steps
        if (++poison_move_counter % 5 == 0) {
            poison = generate_food(10, snake);
        }

        render_game(10, snake, food, poison);
        sleep_for(chrono::milliseconds(game_speed));
    }
}
