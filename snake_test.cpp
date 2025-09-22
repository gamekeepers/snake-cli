#include <gtest/gtest.h>
#include "snake.h"
#include <fstream>
#include <cstdio>

using Pos = SnakeGame::Pos;
using Status = SnakeGame::Status;

TEST(NextHead, MovesAndWrapsRightLeftUpDown) {
    SnakeGame g(10, 1);
    EXPECT_EQ(g.getNextHead(Pos{3,3}, 'r'), Pos{3,4});
    EXPECT_EQ(g.getNextHead(Pos{3,9}, 'r'), Pos{3,0});
    EXPECT_EQ(g.getNextHead(Pos{0,0}, 'l'), Pos{0,9});
    EXPECT_EQ(g.getNextHead(Pos{0,0}, 'u'), Pos{9,0});
    EXPECT_EQ(g.getNextHead(Pos{9,9}, 'd'), Pos{0,9});
}

TEST(GenerateFood, AvoidsSnakeAndOther) {
    // 3x3 grid, fill all except (2,2)
    SnakeGame g(3, 42);
    std::deque<Pos> snake;
    for (int r=0;r<3;r++){
        for (int c=0;c<3;c++){
            if (r == 2 && c == 2) continue;
            snake.push_back({r,c});
        }
    }
    Pos other = {-1,-1};
    Pos food = g.generateFood(snake, other);
    EXPECT_EQ(food, Pos{2,2});
}

TEST(Step, EatFoodGrowsAndIncreasesScore) {
    SnakeGame g(4, 77);
    std::deque<Pos> snake = {{0,0}};
    g.setSnake(snake);
    g.setFood({0,1});
    auto res = g.step('r');
    EXPECT_EQ(res, Status::AteFood);
    EXPECT_EQ(g.getScore(), 10);
    EXPECT_EQ(g.getSnake().back(), Pos{0,1});
    EXPECT_EQ(g.getSnake().size(), 2);
}

TEST(Step, EatPoisonEndsGame) {
    SnakeGame g(4, 77);
    std::deque<Pos> snake = {{0,0}};
    g.setSnake(snake);
    g.setPoison({0,1});
    auto res = g.step('r');
    EXPECT_EQ(res, Status::AtePoison);
}

TEST(Step, SelfCollisionDetected) {
    SnakeGame g(4, 99);
    // snake positions such that head at (1,1) and moving left collides with (1,0)
    std::deque<Pos> snake = {{0,1},{0,0},{1,0},{1,1}};
    g.setSnake(snake);
    auto res = g.step('l');
    EXPECT_EQ(res, Status::SelfCollision);
}

TEST(Step, SpeedDecreasesAfterTenFood) {
    SnakeGame g(5, 123);
    // Prepare snake so we can repeatedly set food directly and step
    g.setSnake({{0,0}});
    g.setSpeed(500);
    for (int i = 0; i < 9; ++i) {
        // place food at next
        Pos next = g.getNextHead(g.getSnake().back(), 'r');
        g.setFood(next);
        EXPECT_EQ(g.step('r'), Status::AteFood);
        // put snake back so next iteration can reuse pattern
    }
    // 10th eat: speed should reduce by 50 (from 500 to 450)
    // place food at next
    Pos next = g.getNextHead(g.getSnake().back(), 'r');
    g.setFood(next);
    EXPECT_EQ(g.step('r'), Status::AteFood);
    EXPECT_EQ(g.getSpeed(), 450);
}

TEST(UpdateHighScores, WritesTopSorted) {
    SnakeGame g(4, 111);
    std::string path = "tmp_highscores_test.txt";
    {
        std::ofstream f(path);
        f << 100 << "\n" << 50 << "\n" << 200 << "\n";
    }
    g.setScore(150);
    auto top = g.updateHighScores(path);
    ASSERT_FALSE(top.empty());
    // top should be sorted descending and include new score 150
    EXPECT_TRUE(std::is_sorted(top.begin(), top.end(), std::greater<int>()));
    EXPECT_TRUE(std::find(top.begin(), top.end(), 150) != top.end());
    std::remove(path.c_str());
}
