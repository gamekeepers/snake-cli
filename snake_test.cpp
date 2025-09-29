#include <gtest/gtest.h>
#include "snake.h"


TEST(SnakeBehaviour, NextHeadRight) {
    pair<int, int> current = make_pair(rand() % 10, rand() % 10);
    EXPECT_EQ(get_next_head(current, 'r'),make_pair(current.first,current.second+1));
    
}


TEST(SnakeBehaviour, NextHeadLeft) {
  pair<int, int> current = make_pair(rand() % 10, rand() % 10);
  EXPECT_EQ(get_next_head(current, 'l'),make_pair(current.first,current.second-1));
  
}

TEST(SnakeBehaviour, NextHeadUp) {
  pair<int, int> current = make_pair(rand() % 10, rand() % 10);
  EXPECT_EQ(get_next_head(current, 'u'),make_pair(current.first-1,current.second));
}

TEST(SnakeBehaviour, NextHeadDown) {
  pair<int, int> current = make_pair(rand() % 10, rand() % 10);
  EXPECT_EQ(get_next_head(current, 'd'),make_pair(current.first+1,current.second));
  
}

TEST(SnakeBehaviour, FoodNotOnSnake) {
    deque<pair<int,int>> snake = {
        {0,0}, {0,1}, {0,2}, {0,3}, {0,4}
    };

    // Run multiple times to increase confidence
    for (int i = 0; i < 100; i++) {
        pair<int,int> food = generate_food(snake, 10);
        EXPECT_EQ(find(snake.begin(), snake.end(), food), snake.end())
            << "Food spawned inside snake at " << food.first << "," << food.second;
    }
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