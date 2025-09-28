#include "snake.h"
#include <gtest/gtest.h>

char direction = 'r';

// ---------------- Movement Tests ----------------
TEST(SnakeTest, NextHeadRight)
{
  pair<int, int> pos = {0, 0};
  auto next = get_next_head(pos, 'r');
  EXPECT_EQ(next, make_pair(0, 1));
}

TEST(SnakeTest, NextHeadWrapLeft)
{
  pair<int, int> pos = {0, 0};
  auto next = get_next_head(pos, 'l');
  EXPECT_EQ(next, make_pair(0, BOARD_SIZE - 1));
}

TEST(SnakeTest, NextHeadDown)
{
  pair<int, int> pos = {0, 0};
  auto next = get_next_head(pos, 'd');
  EXPECT_EQ(next, make_pair(1, 0));
}

TEST(SnakeTest, NextHeadUpWrap)
{
  pair<int, int> pos = {0, 0};
  auto next = get_next_head(pos, 'u');
  EXPECT_EQ(next, make_pair(BOARD_SIZE - 1, 0));
}

TEST(SnakeTest, FoodNotInSnake)
{
  deque<pair<int, int>> snake = {{0, 0}, {0, 1}, {0, 2}};
  auto food = spawn_food(snake);
  EXPECT_TRUE(find(snake.begin(), snake.end(), food) == snake.end());
}

// Poison food spawn & collision
TEST(SnakeTest, PoisonNotInSnake)
{
  deque<pair<int, int>> snake = {{0, 0}, {0, 1}};
  auto poison = spawn_food(snake);
  EXPECT_TRUE(find(snake.begin(), snake.end(), poison) == snake.end());
}
// play/pause functionality test
TEST(SnakeTest, PauseStopsMovement)
{
  deque<pair<int, int>> snake = {{0, 0}};
  char pauseDir = 'P';
  auto next = get_next_head(snake.back(), pauseDir);
  EXPECT_EQ(next, snake.back());
}
TEST(SnakeTest, SaveAndLoadScores)
{
  save_score(100);
  save_score(50);
  auto scores = load_top_scores();
  EXPECT_TRUE(find(scores.begin(), scores.end(), 100) != scores.end());
  EXPECT_TRUE(find(scores.begin(), scores.end(), 50) != scores.end());
}
// ---------------- Difficulty Level Tests ----------------
TEST(SnakeTest, DelayDecreasesWithLevel)
{
  int baseDelay = 500;
  int level1 = max(50, baseDelay - (1 - 1) * 50 - 1 * 5);
  int level5 = max(50, baseDelay - (5 - 1) * 50 - 10 * 5);

  EXPECT_LT(level5, level1); // delay should reduce at higher levels
}

// ---------------- Snake Class Tests ----------------
TEST(SnakeTest, SnakeGrowsOnFood)
{
  Snake s;
  int oldSize = s.getSize();
  s.grow({0, 1}); // add a new head
  EXPECT_EQ(s.getSize(), oldSize + 1);
}

TEST(SnakeTest, SnakeMovesCorrectly)
{
  Snake s;
  int oldSize = s.getSize();
  s.move({0, 1});                  // head moves forward
  EXPECT_EQ(s.getSize(), oldSize); // size remains same after move
  EXPECT_EQ(s.getHead(), make_pair(0, 1));
}
/**
 * g++ -o my_tests snake_test.cpp -lgtest -lgtest_main -pthread;
 * This command is a two-part shell command. Let's break it down.

  The first part is the compilation:
  g++ -o my_tests hello_gtest.cpp -lgtest -lgtest_main -pthread


   * g++: This invokes the GNU C++ compiler.
   * -o my_tests: This tells the compiler to create an executable file named
     my_tests.
   * hello_gtest.cpp: This is the C++ source file containing your tests.
   * -lgtest: This links the Google Test library, which provides the core testing
     framework.
   * -lgtest_main: This links a pre-compiled main function provided by Google
     Test, which saves you from writing your own main() to run the tests.
   * -pthread: This links the POSIX threads library, which is required by Google
     Test for its operation.
 *
*/