#include <gtest/gtest.h>
#include "snake.h"

TEST(SnakeBehaviour, NextHeadRightWraps) {
    pair<int, int> currentPos = make_pair(3, 9);
    EXPECT_EQ(Snake::computeNextHead(currentPos, 'r', 10), make_pair(3, 0));
}

TEST(SnakeBehaviour, NextHeadLeftWraps) {
    pair<int, int> currentPos = make_pair(4, 0);
    EXPECT_EQ(Snake::computeNextHead(currentPos, 'l', 10), make_pair(4, 9));
}

TEST(SnakeBehaviour, NextHeadUpWraps) {
    pair<int, int> currentPos = make_pair(0, 7);
    EXPECT_EQ(Snake::computeNextHead(currentPos, 'u', 10), make_pair(9, 7));
}

TEST(SnakeBehaviour, NextHeadDownWraps) {
    pair<int, int> currentPos = make_pair(9, 2);
    EXPECT_EQ(Snake::computeNextHead(currentPos, 'd', 10), make_pair(0, 2));
}

TEST(SnakeClass, GrowAndMoveBehaviour) {
    Snake testSnake(10);
    // Start with one segment at (0,0); grow to (0,1), then move to (0,2)
    pair<int,int> nextPos = Snake::computeNextHead(make_pair(0,0), 'r', 10);
    testSnake.growTo(nextPos);
    EXPECT_EQ(testSnake.body().size(), 2u);
    
    nextPos = Snake::computeNextHead(testSnake.head(), 'r', 10);
    testSnake.moveTo(nextPos);
    EXPECT_EQ(testSnake.body().size(), 2u);
    EXPECT_EQ(testSnake.head(), make_pair(0,2));
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
 */
