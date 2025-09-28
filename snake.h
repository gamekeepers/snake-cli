#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <deque>
#include <algorithm>
#include <fstream> // ✅ file handling

using namespace std;
using namespace std::this_thread;

char direction = 'r';
int score = 0;
pair<int, int> poisonous_food;
bool is_paused = false;

void update_high_scores() {
    vector<int> high_scores;
    ifstream infile("high_scores.txt");
    int s;
    while (infile >> s) high_scores.push_back(s);
    infile.close();

    high_scores.push_back(score);
    sort(high_scores.rbegin(), high_scores.rend());

    ofstream outfile("high_scores.txt");
    for (size_t i = 0; i < high_scores.size() && i < 10; ++i) {
        outfile << high_scores[i] << endl;
    }
    outfile.close();

    cout << "\n--- Top 10 High Scores ---" << endl;
    for (int hs : high_scores) cout << hs << endl;
}

void input_handler() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (true) {
        char input = getchar();
        if (input == 'd') direction = 'r';
        else if (input == 'a') direction = 'l';
        else if (input == 'w') direction = 'u';
        else if (input == 's') direction = 'd';
        else if (input == 'p') is_paused = !is_paused;
        else if (input == 'q') exit(0);
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

pair<int, int> get_next_head(pair<int, int> current, char direction) {
    if (direction == 'r') return {current.first, (current.second + 1) % 10};
    if (direction == 'l') return {current.first, current.second == 0 ? 9 : current.second - 1};
    if (direction == 'd') return {(current.first + 1) % 10, current.second};
    if (direction == 'u') return {current.first == 0 ? 9 : current.first - 1, current.second};
    return current;
}

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == food.first && j == food.second) cout << "🍎";
            else if (i == poisonous_food.first && j == poisonous_food.second) cout << "💀";
            else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end()) cout << "🐍";
            else cout << "⬜";
        }
        cout << endl;
    }
}

pair<int, int> generate_food(deque<pair<int, int>> &snake, pair<int, int> avoid) {
    pair<int, int> food;
    do {
        food = {rand() % 10, rand() % 10};
    } while (find(snake.begin(), snake.end(), food) != snake.end() || food == avoid);
    return food;
}

void game_play() {
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back({0, 0});

    pair<int, int> food = generate_food(snake, {-1, -1});
    poisonous_food = generate_food(snake, food);

    chrono::milliseconds sleep_duration(500);

    for (pair<int, int> head = {0, 1};; head = get_next_head(head, direction)) {
        cout << "\033[H";

        if (is_paused) {
            cout << "Game Paused. Press 'p' to resume." << endl;
            sleep_for(200ms);
            continue;
        }

        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over (Hit yourself)" << endl;
            cout << "Final Score: " << score << endl;
            update_high_scores(); // ✅ store high score
            exit(0);
        }

        if (head == poisonous_food) {
            system("clear");
            cout << "Game Over (Ate poisonous food)" << endl;
            cout << "Final Score: " << score << endl;
            update_high_scores(); // ✅ store high score
            exit(0);
        }

        if (head == food) {
            snake.push_back(head);
            score += 10;
            food = generate_food(snake, poisonous_food);

            if (sleep_duration.count() > 50) sleep_duration -= chrono::milliseconds(10);
        } else {
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(10, snake, food);
        cout << "Score: " << score << endl;
        sleep_for(sleep_duration);
    }
}

int main() {
    thread t(input_handler);
    t.detach();
    game_play();
    return 0;
}
