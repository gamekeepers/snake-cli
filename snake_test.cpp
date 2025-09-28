#include <gtest/gtest.h>
#include "snake.h"

// Create a test fixture to access the SnakeGame class
class SnakeGameTest : public ::testing::Test {
protected:
    SnakeGame game;
};

TEST_F(SnakeGameTest, NextHeadRight) {
    pair<int, int> current = make_pair(7, 9);
    EXPECT_EQ(game.get_next_head(current, 'r'), make_pair(7, 0));
}

TEST_F(SnakeGameTest, NextHeadLeft) {
    pair<int, int> current = make_pair(7, 0);
    EXPECT_EQ(game.get_next_head(current, 'l'), make_pair(7, 9));
}

TEST_F(SnakeGameTest, NextHeadUp) {
    pair<int, int> current = make_pair(0, 2);
    EXPECT_EQ(game.get_next_head(current, 'u'), make_pair(9, 2));
}

TEST_F(SnakeGameTest, NextHeadDown) {
    pair<int, int> current = make_pair(9, 2);
    EXPECT_EQ(game.get_next_head(current, 'd'), make_pair(0, 2));
}

TEST_F(SnakeGameTest, IsSnakeSegment) {
    // Snake starts at (0,0)
    EXPECT_TRUE(game.is_snake_segment(0, 0));
    EXPECT_FALSE(game.is_snake_segment(1, 1));
}

TEST_F(SnakeGameTest, CheckFoodCollision) {
    // This test would require access to private members or a different approach
    // For now, we'll focus on what we can test with the public interface
}

TEST_F(SnakeGameTest, CheckPoisonCollision) {
    // This test would require access to private members or a different approach
    // For now, we'll focus on what we can test with the public interface
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