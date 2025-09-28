#pragma once
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
using namespace std::chrono_literals;

class SnakeGame {
private:
    char direction = 'r';
    std::chrono::milliseconds sleep_time = 500ms;
    int score = 10;
    bool is_paused = false;
    bool waiting_for_restart = false;
    std::deque<std::pair<int, int>> snake;
    std::pair<int, int> food;
    std::pair<int, int> poisonousFood;
    std::vector<int> top_scores;

    bool is_opposite_direction(char current, char next) {
        return (current == 'r' && next == 'l') ||
               (current == 'l' && next == 'r') ||
               (current == 'u' && next == 'd') ||
               (current == 'd' && next == 'u');
    }

    void setup_terminal(struct termios& oldt, struct termios& newt) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }

    void restore_terminal(struct termios& oldt) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    std::string render_cell(int i, int j) {
        if (i == food.first && j == food.second)
            return "🍎";
        else if (i == poisonousFood.first && j == poisonousFood.second)
            return "💀";
        else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end())
            return "🐍";
        else
            return "⬜";
    }

    void render_game(int size) {
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                cout << render_cell(i, j);
            }
            cout << endl;
        }
    }

    std::pair<int, int> get_next_head(std::pair<int, int> current, char dir) {
        if (dir == 'r')
            return {current.first, (current.second + 1) % 10};
        else if (dir == 'l')
            return {current.first, current.second == 0 ? 9 : current.second - 1};
        else if (dir == 'd')
            return {(current.first + 1) % 10, current.second};
        else // 'u'
            return {current.first == 0 ? 9 : current.first - 1, current.second};
    }

    void reduce_sleep_time() {
        if (snake.size() % 10 == 0 && sleep_time > 100ms)
            sleep_time -= 50ms;
    }

    std::pair<int, int> get_random_position(const std::pair<int, int>* avoid = nullptr) {
        std::pair<int, int> pos;
        do {
            pos = {rand() % 10, rand() % 10};
        } while (find(snake.begin(), snake.end(), pos) != snake.end() ||
                 (avoid && pos == *avoid));
        return pos;
    }

    std::pair<int, int> get_food() {
        return get_random_position();
    }

    void increase_score() {
        score += 10;
    }

    std::pair<int, int> get_poisonous_food(std::pair<int, int> food_pos) {
        return get_random_position(&food_pos);
    }

    void show_top_scores() {
        cout << "\n--- Top 10 Scores ---\n";
        for (size_t i = 0; i < top_scores.size(); ++i)
            cout << i + 1 << ". " << top_scores[i] << endl;
    }

    void update_top_scores(int new_score) {
        top_scores.push_back(new_score);
        sort(top_scores.rbegin(), top_scores.rend());
        if (top_scores.size() > 10) top_scores.resize(10);
    }

    void handle_pause() {
        cout << "Game Paused. Press 'p' to resume." << endl;
        std::this_thread::sleep_for(300ms);
    }

    bool is_collision(const std::pair<int, int>& head) {
        return find(snake.begin(), snake.end(), head) != snake.end() ||
               (head.first == poisonousFood.first && head.second == poisonousFood.second);
    }

    void handle_game_over() {
        waiting_for_restart = true;
        system("clear");
        cout << "Game Over" << endl;
        update_top_scores(score);
        show_top_scores();
        cout << "Press 'n' to start again or 'q' to quit." << endl;
        while (true) {
            char input = getchar();
            if (input == 'n') {
                score = 10;
                direction = 'r';
                waiting_for_restart = false;
                break;
            } else if (input == 'q') {
                restore_terminal(saved_oldt);
                exit(0);
            }
        }
    }

    void handle_food_eaten(const std::pair<int, int>& head) {
        food = get_food();
        poisonousFood = get_poisonous_food(food);
        snake.push_back(head);
        reduce_sleep_time();
        increase_score();
    }

    void move_snake(const std::pair<int, int>& head) {
        snake.push_back(head);
        snake.pop_front();
    }

    void display_status() {
        cout << "length of snake: " << snake.size() << "  -  score: " << score << endl;
    }

    struct termios saved_oldt; // For restoring terminal on exit

public:
    SnakeGame() {}

    void input_handler() {
        struct termios oldt, newt;
        setup_terminal(oldt, newt);
        saved_oldt = oldt;
        map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
        while (true) {
            if (waiting_for_restart) continue;
            char input = getchar();
            if (input == 'p') {
                is_paused = !is_paused;
            }
            else if (keymap.find(input) != keymap.end()) {
                char next_dir = keymap[input];
                if (!is_opposite_direction(direction, next_dir)) {
                    direction = next_dir;
                }
            }
            else if (input == 'q') {
                restore_terminal(oldt);
                exit(0);
            }
        }
    }

    void game_play() {
        system("clear");
        snake.clear();
        snake.push_back({0, 0});
        food = get_food();
        poisonousFood = get_poisonous_food(food);
        std::pair<int, int> head = {0, 1};

        while (true)
        {
            cout << "\033[H";
            if (is_paused) {
                handle_pause();
                continue;
            }
            if (is_collision(head)) {
                handle_game_over();
                // Restart game after 'n'
                snake.clear();
                snake.push_back({0, 0});
                food = get_food();
                poisonousFood = get_poisonous_food(food);
                head = {0, 1};
                score = 10;
                sleep_time = 500ms;
                continue;
            }
            else if (head.first == food.first && head.second == food.second) {
                handle_food_eaten(head);
            }
            else {
                move_snake(head);
            }
            render_game(10);
            display_status();

            std::this_thread::sleep_for(sleep_time);
            head = get_next_head(head, direction);
        }
    }
};
