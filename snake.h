#pragma once
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <fstream>
#include <atomic>
#include <conio.h>
#include <thread>
#include <cstdlib>
#include <ctime>

// === Console Colors ===
const std::string GREEN_BG  = "\033[42m"; // food
const std::string RED_BG    = "\033[41m"; // poison
const std::string YELLOW_BG = "\033[43m"; // snake
const std::string BLUE_BG   = "\033[44m"; // background
const std::string RESET     = "\033[0m";

const int BOARD_SIZE = 15;

std::atomic<char> direction('r');
std::atomic<bool> paused(false);

// ================= SCORE MANAGER =================
class ScoreManager {
    std::vector<int> scores;

public:
    void load(const std::string& file = "highscores.txt") {
        scores.clear();
        std::ifstream in(file);
        int s;
        while (in >> s) scores.push_back(s);
        sort(scores.rbegin(), scores.rend());
        if (scores.size() > 10) scores.resize(10);
    }

    void save(const std::string& file = "highscores.txt") {
        sort(scores.rbegin(), scores.rend());
        if (scores.size() > 10) scores.resize(10);
        std::ofstream out(file);
        for (int s : scores) out << s << "\n";
    }

    void add(int score) {
        scores.push_back(score);
        save();
    }

    void display() {
        std::cout << "Top 10 Highest Scores:\n";
        for (size_t i = 0; i < scores.size(); i++)
            std::cout << (i + 1) << ". " << scores[i] << "\n";
    }

    const std::vector<int>& getScores() const { return scores; }
};

// ================= SNAKE =================
class Snake {
    std::deque<std::pair<int,int>> body;

public:
    Snake() { body.push_back({0,0}); }

    const std::deque<std::pair<int,int>>& getBody() const { return body; }
    std::pair<int,int> head() const { return body.back(); }

    void grow(std::pair<int,int> h) { body.push_back(h); }
    void move(std::pair<int,int> h) {
        body.push_back(h);
        body.pop_front();
    }

    bool collides(std::pair<int,int> pos) const {
        return std::find(body.begin(), body.end(), pos) != body.end();
    }
};

// ================= GAME =================
class Game {
    Snake snake;
    ScoreManager scoreManager;
    std::pair<int,int> food, poison;
    int score = 0;

public:
    Game() { srand((unsigned)time(NULL)); }

    void init() {
        scoreManager.load();
        spawnFood();
        spawnPoison();
    }

    std::pair<int,int> randomCell() {
        return { rand() % BOARD_SIZE, rand() % BOARD_SIZE };
    }

    void spawnFood() {
        do { food = randomCell(); }
        while (snake.collides(food) || food == poison);
    }

    void spawnPoison() {
        do { poison = randomCell(); }
        while (snake.collides(poison) || poison == food);
    }

    std::pair<int,int> getNextHead(std::pair<int,int> current, char dir) {
        if (dir == 'r') return {current.first, (current.second + 1) % BOARD_SIZE};
        if (dir == 'l') return {current.first, current.second == 0 ? BOARD_SIZE-1 : current.second-1};
        if (dir == 'd') return {(current.first+1) % BOARD_SIZE, current.second};
        return {current.first == 0 ? BOARD_SIZE-1 : current.first-1, current.second}; // 'u'
    }

    void update(char dir, bool& game_over) {
        auto head = getNextHead(snake.head(), dir);

        if (snake.collides(head) || head == poison) {
            game_over = true;
            scoreManager.add(score);
            return;
        }

        if (head == food) {
            snake.grow(head);
            score += 10;
            spawnFood();
            spawnPoison();
        } else {
            snake.move(head);
        }
    }

    void render() {
        std::cout << "\033[2J\033[H"; // clear
        for (int i=0;i<BOARD_SIZE;i++) {
            for (int j=0;j<BOARD_SIZE;j++) {
                if (i==food.first && j==food.second) std::cout << GREEN_BG << "  " << RESET;
                else if (i==poison.first && j==poison.second) std::cout << RED_BG << "  " << RESET;
                else if (snake.collides({i,j})) std::cout << YELLOW_BG << "  " << RESET;
                else std::cout << BLUE_BG << "  " << RESET;
            }
            std::cout << "\n";
        }
        std::cout << "Score: " << score << " (ESC quit, SPACE pause)\n";
    }

    void run(std::atomic<bool>& running) {
    bool game_over = false;
    while (running) {
        update(direction, game_over);
        if (game_over) break;

        render();

        while (paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (!running) break;
        }
        if (!running) break;

        int sleep_ms = 400 - score*5;
        if (sleep_ms < 100) sleep_ms = 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
    }

    // ✅ Always save score whether quit or crash
    if (!game_over) {
        // quit with ESC
        scoreManager.add(score);
    }

    std::cout << "\033[2J\033[H";
    std::cout << (game_over ? "==== GAME OVER ====\n" : "==== GAME QUIT ====\n");
    std::cout << "Your Score: " << score << "\n\n";
    scoreManager.display();
    running = false;
}

};

// ================= INPUT HANDLER =================
inline void input_handler(std::atomic<bool>& running) {
    while (running) {
        char ch = _getch(); // blocking, always reads every key

        // Arrow keys (224) or function keys (0)
        if (ch == 224 || ch == 0) {
            ch = _getch();
            switch (ch) {
                case 72: if (direction != 'd') direction = 'u'; break;
                case 80: if (direction != 'u') direction = 'd'; break;
                case 75: if (direction != 'r') direction = 'l'; break;
                case 77: if (direction != 'l') direction = 'r'; break;
            }
        }
        else {
            if (ch == 27) running = false;               // ESC
            else if (ch == ' ') paused = !paused.load(); // SPACE
            else if (ch == 'w' || ch == 'W') if (direction != 'd') direction = 'u';
            else if (ch == 's' || ch == 'S') if (direction != 'u') direction = 'd';
            else if (ch == 'a' || ch == 'A') if (direction != 'r') direction = 'l';
            else if (ch == 'd' || ch == 'D') if (direction != 'l') direction = 'r';
        }
    }
}
