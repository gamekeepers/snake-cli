#pragma once
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

constexpr int GRID_SIZE = 10;
constexpr int INITIAL_SPEED = 500; // milliseconds
constexpr int POISON_INTERVAL = 5000; // milliseconds
constexpr int MAX_TOP_SCORES = 10;

class Game {
private:
    char direction = 'r';
    bool paused = false;
    vector<int> top_scores;
    deque<pair<int,int>> snake;
    pair<int,int> food;
    pair<int,int> poison;
    int speed = INITIAL_SPEED;
    int score = 0;
    int food_eaten = 0;
    int poison_timer = 0;

public:
    Game() {
        snake.push_back({0,0});
        food = generate_food();
        poison = generate_poison();
        load_top_scores();
    }

    void load_top_scores() {
        ifstream infile("top_scores.txt");
        int s;
        top_scores.clear();
        while (infile >> s) top_scores.push_back(s);
        infile.close();
        sort(top_scores.rbegin(), top_scores.rend());
        if (top_scores.size() > MAX_TOP_SCORES) top_scores.resize(MAX_TOP_SCORES);
    }

    void save_top_scores() {
        ofstream outfile("top_scores.txt");
        for (int s : top_scores) outfile << s << endl;
        outfile.close();
    }

    pair<int,int> get_next_head(pair<int,int> current, char dir) {
        if(dir == 'r') return {current.first, (current.second + 1) % GRID_SIZE};
        if(dir == 'l') return {current.first, current.second == 0 ? GRID_SIZE - 1 : current.second - 1};
        if(dir == 'd') return {(current.first + 1) % GRID_SIZE, current.second};
        if(dir == 'u') return {current.first == 0 ? GRID_SIZE - 1 : current.first - 1, current.second};
        return current;
    }

    pair<int,int> generate_food() {
        pair<int,int> f;
        do { f = {rand() % GRID_SIZE, rand() % GRID_SIZE}; }
        while (find(snake.begin(), snake.end(), f) != snake.end());
        return f;
    }

    pair<int,int> generate_poison() {
        pair<int,int> p;
        do { p = {rand() % GRID_SIZE, rand() % GRID_SIZE}; }
        while (find(snake.begin(), snake.end(), p) != snake.end() || p == food);
        return p;
    }

    void render() {
        for(int i = 0; i < GRID_SIZE; ++i) {
            for(int j = 0; j < GRID_SIZE; ++j) {
                pair<int,int> cell = {i,j};
                if(cell == food) cout << "🍎";
                else if(cell == poison) cout << "☠️";
                else if(find(snake.begin(), snake.end(), cell) != snake.end()) cout << "🐍";
                else cout << "⬜";
            }
            cout << endl;
        }
        cout << "Length: " << snake.size() << " | Score: " << score << " | Food eaten: " << food_eaten
             << " | Speed: " << speed << "ms" << endl;
    }

    void handle_input() {
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        map<char,char> keymap = {{'d','r'}, {'a','l'}, {'w','u'}, {'s','d'}};

        while(true) {
            char input = getchar();
            if(keymap.find(input) != keymap.end()) direction = keymap[input];
            else if(input == 'p') paused = !paused;
            else if(input == 'q') exit(0);
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    void play() {
        pair<int,int> head = {0,1};

        while(true) {
            cout << "\033[H"; // move cursor to top

            while(paused) {
                system("clear");
                render();
                cout << "=== PAUSED ===\nPress 'p' to resume...\n";
                sleep_for(milliseconds(200));
            }

            head = get_next_head(head, direction);

            if(find(snake.begin(), snake.end(), head) != snake.end() || head == poison) {
                end_game(head == poison ? "Snake ate poison ☠️." : "Snake hit itself.");
            }

            if(head == food) {
                snake.push_back(head);
                food = generate_food();
                food_eaten++;
                score += 10;
                if(food_eaten % 10 == 0 && speed > 100) speed += 50;
            } else {
                snake.push_back(head);
                snake.pop_front();
            }

            poison_timer += speed;
            if(poison_timer >= POISON_INTERVAL) {
                poison = generate_poison();
                poison_timer = 0;
            }

            render();
            sleep_for(milliseconds(speed));
        }
    }

private:
    void end_game(const string& message) {
        system("clear");
        top_scores.push_back(score);
        sort(top_scores.rbegin(), top_scores.rend());
        if(top_scores.size() > MAX_TOP_SCORES) top_scores.resize(MAX_TOP_SCORES);
        save_top_scores();

        cout << "Game Over! " << message << "\nTop 10 Scores:\n";
        for(int s : top_scores) cout << s << endl;
        exit(0);
    }
};
