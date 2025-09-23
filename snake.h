#pragma once
#include <iostream>
#include <vector>
#include <deque>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <conio.h>
#include <windows.h>
using namespace std;

class Position {
public:
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
};

class Snake {
private:
    deque<Position> body;
    char direction;
    
public:
    Snake() : direction('r') { body.push_back(Position(0, 0)); }
    
    void setDirection(char dir) { direction = dir; }
    char getDirection() const { return direction; }
    const deque<Position>& getBody() const { return body; }
    size_t getLength() const { return body.size(); }
    
    Position getNextHead() const {
        Position head = body.back();
        switch(direction) {
            case 'r': return Position(head.x, (head.y + 1) % 10);
            case 'l': return Position(head.x, head.y == 0 ? 9 : head.y - 1);
            case 'd': return Position((head.x + 1) % 10, head.y);
            case 'u': return Position(head.x == 0 ? 9 : head.x - 1, head.y);
        }
        return head;
    }
    
    void move() {
        body.push_back(getNextHead());
        body.pop_front();
    }
    
    void grow() { body.push_back(getNextHead()); }
    
    bool checkSelfCollision() const {
        Position head = getNextHead();
        return find(body.begin(), body.end(), head) != body.end();
    }
};

class Food {
private:
    Position pos;
    
public:
    void spawn(const Snake& snake) {
        do {
            pos = Position(rand() % 10, rand() % 10);
        } while (find(snake.getBody().begin(), snake.getBody().end(), pos) != snake.getBody().end());
    }
    
    Position getPosition() const { return pos; }
};

class ScoreManager {
private:
    int currentScore;
    vector<int> highScores;
    
public:
    ScoreManager() : currentScore(0) { loadScores(); }
    
    void addScore(int points) { currentScore += points; }
    int getScore() const { return currentScore; }
    
    void saveScore() {
        highScores.push_back(currentScore);
        sort(highScores.rbegin(), highScores.rend());
        if (highScores.size() > 10) highScores.resize(10);
        
        ofstream file("scores.txt");
        for (int s : highScores) file << s << "\n";
    }
    
    void showScores() const {
        cout << "\nTOP 10 SCORES:\n";
        for (size_t i = 0; i < highScores.size(); i++) {
            cout << i + 1 << ". " << highScores[i] << "\n";
        }
    }
    
private:
    void loadScores() {
        ifstream file("scores.txt");
        int s;
        while (file >> s && highScores.size() < 10) {
            highScores.push_back(s);
        }
    }
};

class Game {
private:
    Snake snake;
    Food food, poison;
    ScoreManager scoreManager;
    int gameSpeed;
    bool paused;
    
public:
    Game() : gameSpeed(500), paused(false) {
        srand(time(0));
        food.spawn(snake);
        poison.spawn(snake);
    }
    
    void handleInput() {
        if (_kbhit()) {
            char input = _getch();
            if (input == 'p') {
                paused = !paused;
            } else if (input == 'q') {
                exit(0);
            } else if (!paused) {
                switch(input) {
                    case 'd': snake.setDirection('r'); break;
                    case 'a': snake.setDirection('l'); break;
                    case 'w': snake.setDirection('u'); break;
                    case 's': snake.setDirection('d'); break;
                }
            }
        }
    }
    
    void update() {
        if (paused) return;
        
        Position nextHead = snake.getNextHead();
        
        if (snake.checkSelfCollision()) {
            gameOver("Game Over!");
        } else if (nextHead == poison.getPosition()) {
            gameOver("Poisoned!");
        } else if (nextHead == food.getPosition()) {
            scoreManager.addScore(10);
            gameSpeed = max(100, gameSpeed - 20);
            snake.grow();
            food.spawn(snake);
            if (rand() % 3 == 0) poison.spawn(snake);
        } else {
            snake.move();
        }
    }
    
    void render() const {
        COORD coord = {0, 0};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        
        if (paused) {
            coord = {0, 12};
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
            cout << "PAUSED - Press 'p' to resume";
            return;
        }
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                Position current(i, j);
                if (current == food.getPosition()) {
                    cout << "🍎";
                } else if (current == poison.getPosition()) {
                    cout << "☠️";
                } else if (find(snake.getBody().begin(), snake.getBody().end(), current) != snake.getBody().end()) {
                    cout << "🐍";
                } else {
                    cout << "⬜";
                }
            }
            cout << endl;
        }
        cout << "Score: " << scoreManager.getScore() << " | Length: " << snake.getLength() 
             << " | Speed: " << (600 - gameSpeed) << " | Press 'p' to pause" << endl;
    }
    
    void run() {
        system("cls");
        while (true) {
            handleInput();
            update();
            render();
            Sleep(gameSpeed);
        }
    }
    
private:
    void gameOver(const string& message) {
        system("cls");
        cout << message << " Final Score: " << scoreManager.getScore() << endl;
        scoreManager.saveScore();
        scoreManager.showScores();
        exit(0);
    }
};

// Legacy functions for test compatibility
pair<int, int> get_next_head(pair<int, int> current, char direction) {
    Position pos(current.first, current.second);
    Snake tempSnake;
    tempSnake.setDirection(direction);
    Position next = tempSnake.getNextHead();
    return make_pair(next.x, next.y);
}

pair<int, int> spawn_food(const deque<pair<int, int>>& snake) {
    pair<int, int> food;
    do {
        food = make_pair(rand() % 10, rand() % 10);
    } while (find(snake.begin(), snake.end(), food) != snake.end());
    return food;
}

vector<int> high_scores;
void save_score(int score) {
    high_scores.push_back(score);
    sort(high_scores.rbegin(), high_scores.rend());
    if (high_scores.size() > 10) high_scores.resize(10);
}

void game_play() {
    Game game;
    game.run();
}