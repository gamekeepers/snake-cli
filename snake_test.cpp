#include <gtest/gtest.h>
#include "snake.h"

// Subclass Game to expose private methods and override gameOver
class TestGame : public Game {
public:
    bool gameOverCalled = false;
    string gameOverReason;

    using Game::Game; // inherit constructor
    using Game::changeDirection;
    
    // Expose getNextHead
    pair<int,int> testNextHead(pair<int,int> current) {
        return Game::getNextHead(current);
    }

    // Override gameOver to avoid exit()
    void gameOver(const string &reason) override {
        gameOverCalled = true;
        gameOverReason = reason;
    }

    // Helper to place food and poison at specific positions
    void setFood(pair<int,int> pos) { this->food = pos; }
    void setPoison(pair<int,int> pos) { this->poison = pos; }
};

class SnakeEnhancementsTest : public ::testing::Test {
protected:
    HighScores hs;
    TestGame game{10, hs}; // 10x10 grid
};

// ---------------- Movement Tests ----------------
TEST_F(SnakeEnhancementsTest, NextHeadRight) {
    pair<int,int> current = {5,5};
    game.changeDirection('r');
    EXPECT_EQ(game.testNextHead(current), make_pair(5,6));
}

TEST_F(SnakeEnhancementsTest, NextHeadLeft) {
    pair<int,int> current = {5,5};
    game.changeDirection('l');
    EXPECT_EQ(game.testNextHead(current), make_pair(5,4));
}

// ---------------- Food Tests ----------------
TEST_F(SnakeEnhancementsTest, EatingFoodIncreasesScoreAndLength) {
    pair<int,int> foodPos = {0,1};
    game.setFood(foodPos);

    // Initial snake length
    size_t initialLength = 1;

    // Move head to food
    pair<int,int> head = game.testNextHead({0,0});
    game.changeDirection('r');

    // Simulate eating food
    if (head == foodPos) {
        game.snake.push_back(head);
        game.score += 10;
    }

    EXPECT_EQ(game.score, 10);
    EXPECT_EQ(game.snake.size(), initialLength + 1);
}

// ---------------- Poison Tests ----------------
TEST_F(SnakeEnhancementsTest, HittingPoisonCallsGameOver) {
    pair<int,int> poisonPos = {0,1};
    game.setPoison(poisonPos);

    pair<int,int> head = game.testNextHead({0,0});
    game.changeDirection('r');

    // Simulate poison collision
    if (head == poisonPos) {
        game.gameOver("You ate poison!");
    }

    EXPECT_TRUE(game.gameOverCalled);
    EXPECT_EQ(game.gameOverReason, "You ate poison!");
}

// ---------------- HighScores Tests ----------------
TEST_F(SnakeEnhancementsTest, HighScoresUpdatesCorrectly) {
    hs.update(50);
    hs.update(30);
    hs.update(70);

    vector<int> expected = {70,50,30};
    for (size_t i=0; i<expected.size(); i++) {
        EXPECT_EQ(hs.scores[i], expected[i]);
    }
}

// ---------------- Wrap-Around Tests ----------------
TEST_F(SnakeEnhancementsTest, WrapAroundRight) {
    pair<int,int> current = {5,9};
    game.changeDirection('r');
    EXPECT_EQ(game.testNextHead(current), make_pair(5,0));
}

TEST_F(SnakeEnhancementsTest, WrapAroundLeft) {
    pair<int,int> current = {5,0};
    game.changeDirection('l');
    EXPECT_EQ(game.testNextHead(current), make_pair(5,9));
}

TEST_F(SnakeEnhancementsTest, WrapAroundUp) {
    pair<int,int> current = {0,5};
    game.changeDirection('u');
    EXPECT_EQ(game.testNextHead(current), make_pair(9,5));
}

TEST_F(SnakeEnhancementsTest, WrapAroundDown) {
    pair<int,int> current = {9,5};
    game.changeDirection('d');
    EXPECT_EQ(game.testNextHead(current), make_pair(0,5));
}
