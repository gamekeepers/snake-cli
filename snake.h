#ifndef SNAKE_H
#define SNAKE_H

#include <deque>
#include <vector>
#include <utility>
#include <random>
#include <algorithm>
#include <fstream>
#include <string>

/*
 Header-only refactor of the game logic into SnakeGame.
 All methods defined inline so there's no separate .cpp file required.
*/

class SnakeGame {
public:
    using Pos = std::pair<int,int>;
    enum class Status { OK, AteFood, AtePoison, SelfCollision };

    // Construct with grid size and optional RNG seed (useful for deterministic tests)
    explicit SnakeGame(int gridSize = 10, unsigned rngSeed = std::random_device{}())
        : gridSize_(gridSize),
          snake_(),
          food_(-1, -1),
          poison_(-1, -1),
          score_(0),
          foodEaten_(0),
          speed_(500),
          rng_(rngSeed),
          dist_(0, gridSize - 1)
    {
        snake_.push_back({0,0});
        food_ = generateUniqueFood(snake_, {-1,-1});
        poison_ = generateUniqueFood(snake_, food_);
    }

    // Compute next head with wrap-around
    Pos getNextHead(const Pos &current, char direction) const {
        int r = current.first;
        int c = current.second;
        if (direction == 'r') {
            c = (c + 1) % gridSize_;
        } else if (direction == 'l') {
            c = (c + gridSize_ - 1) % gridSize_;
        } else if (direction == 'd') {
            r = (r + 1) % gridSize_;
        } else if (direction == 'u') {
            r = (r + gridSize_ - 1) % gridSize_;
        }
        return {r, c};
    }

    // Generate a food position that is not overlapping the snake or 'other'
    Pos generateFood(const std::deque<Pos> &snake, const Pos &other) {
        return generateUniqueFood(snake, other);
    }

    // Single simulation step; returns status
    Status step(char direction) {
        Pos currentHead = snake_.back();
        Pos next = getNextHead(currentHead, direction);

        // Self collision?
        if (std::find(snake_.begin(), snake_.end(), next) != snake_.end()) {
            return Status::SelfCollision;
        }

        if (next == food_) {
            // grow
            snake_.push_back(next);
            score_ += 10;
            ++foodEaten_;
            // regenerate food and ensure poison doesn't collide
            food_ = generateUniqueFood(snake_, poison_);
            poison_ = generateUniqueFood(snake_, food_);
            if (foodEaten_ % 10 == 0 && speed_ > 100) speed_ -= 50;
            return Status::AteFood;
        } else if (next == poison_) {
            return Status::AtePoison;
        } else {
            // normal move
            snake_.push_back(next);
            snake_.pop_front();
            return Status::OK;
        }
    }

    // Accessors
    const std::deque<Pos>& getSnake() const { return snake_; }
    Pos getFood() const { return food_; }
    Pos getPoison() const { return poison_; }
    int getScore() const { return score_; }
    int getSpeed() const { return speed_; }
    int getGridSize() const { return gridSize_; }

    // Update highscores file and return top scores (descending)
    std::vector<int> updateHighScores(const std::string &path) const {
        std::vector<int> scores;
        std::ifstream infile(path);
        int s;
        while (infile >> s) scores.push_back(s);
        infile.close();

        scores.push_back(score_);
        std::sort(scores.begin(), scores.end(), std::greater<int>());
        if (scores.size() > 10) scores.resize(10);

        std::ofstream outfile(path);
        for (int sc : scores) outfile << sc << "\n";
        outfile.close();

        return scores;
    }

    // Test helpers (to set internal state deterministically in tests)
    void setFood(const Pos &p) { food_ = p; }
    void setPoison(const Pos &p) { poison_ = p; }
    void setSnake(const std::deque<Pos> &s) { snake_ = s; }
    void setScore(int sc) { score_ = sc; }
    void setSpeed(int sp) { speed_ = sp; }

private:
    int gridSize_;
    std::deque<Pos> snake_;
    Pos food_;
    Pos poison_;
    int score_;
    int foodEaten_;
    int speed_;

    // RNG for food/poison generation
    mutable std::mt19937 rng_;
    std::uniform_int_distribution<int> dist_;

    // internal helper to pick a free cell
    Pos generateUniqueFood(const std::deque<Pos> &snake, const Pos &other) const {
        // If grid is full, this would loop indefinitely; tests will avoid that case.
        Pos candidate;
        do {
            candidate = { dist_(rng_), dist_(rng_) };
        } while (std::find(snake.begin(), snake.end(), candidate) != snake.end() || candidate == other);
        return candidate;
    }
};

#endif // SNAKE_H
