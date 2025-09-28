#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h> // for system clear
#include <map>
#include <deque>
#include <algorithm>
using namespace std;
using namespace std::this_thread;

char direction = 'r';
int game_speed = 500; // default in ms (easy)
int score = 0;        // score tracker

// --- Input Handler ---
void input_handler() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            direction = keymap[input];
        } else if (input == 'q') {
            exit(0);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// --- Render Function ---
void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poison) {
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (i == food.first && j == food.second) {
                cout << "🍎"; // normal food
            } else if (i == poison.first && j == poison.second) {
                cout << "🍄"; // poisonous food
            } else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
                cout << "🐍";
            } else {
                cout << "⬜";
            }
        }
        cout << endl;
    }
}

// --- Snake Movement ---
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

// --- Food Generator ---
pair<int, int> generate_item(deque<pair<int, int>> &snake, pair<int, int> other = {-1, -1}) {
    pair<int, int> f;
    do {
        f = make_pair(rand() % 10, rand() % 10);
    } while (find(snake.begin(), snake.end(), f) != snake.end() || f == other);
    return f;
}

// --- Game Play ---
void game_play() {
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food = generate_item(snake);
    pair<int, int> poison = generate_item(snake, food);

    for (pair<int, int> head = make_pair(0, 1);; head = get_next_head(head, direction)) {
        cout << "\033[H"; // move cursor to top
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over! You hit yourself!\n";
            cout << "Final Score: " << score << endl;
            exit(0);
        } else if (head == food) {
            // Eat normal food
            snake.push_back(head);
            score += 10;
            if (game_speed > 80) game_speed -= 20;

            // generate both food and poison again, ensuring they never overlap
            food = generate_item(snake);
            poison = generate_item(snake, food);

        } else if (head == poison) {
            // Eat poisonous food → game ends
            system("clear");
            cout << "☠️ Game Over! You ate poisonous food!\n";
            cout << "Final Score: " << score << endl;
            exit(0);
        } else {
            // Normal move
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(10, snake, food, poison);
        cout << "Score: " << score << endl;
        cout << "Length: " << snake.size() << endl;
        cout << "Speed: " << game_speed << "ms" << endl;
        sleep_for(chrono::milliseconds(game_speed));
    }
}

// --- Difficulty Selection ---
void select_difficulty() {
    cout << "Choose Difficulty:\n";
    cout << "1. Easy   (500ms)\n";
    cout << "2. Medium (300ms)\n";
    cout << "3. Hard   (150ms)\n";
    cout << "Enter choice: ";
    int choice;
    cin >> choice;

    switch (choice) {
        case 1: game_speed = 500; break;
        case 2: game_speed = 300; break;
        case 3: game_speed = 150; break;
        default: game_speed = 500; break;
    }
}
