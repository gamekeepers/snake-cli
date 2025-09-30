#include <gtest/gtest.h>
#include "snake.h"


TEST(SnakeMove, NextHeadRight) {
    pair<int, int> current = make_pair(5, 5);
    Snake snake = Snake(current);
    snake.set_direction('r');
    
    snake.move();
    Cell next = snake.get_head();
    
    EXPECT_EQ(next,make_pair(current.first,current.second+1));
}

TEST(SnakeMove, NextHeadLeft) {
    Cell current = make_pair(5, 5);
    Snake snake = Snake(current);
    snake.set_direction('l');
    
    snake.move();
    Cell next = snake.get_head();
    
    EXPECT_EQ(next,make_pair(current.first, current.second-1));
}

TEST(SnakeMove, NextHeadUp) {
    Cell current = make_pair(5, 5);
    Snake snake = Snake(current);
    snake.set_direction('u');
    
    snake.move();
    Cell next = snake.get_head();
    
    EXPECT_EQ(next,make_pair(current.first-1, current.second));
}

TEST(SnakeMove, NextHeadDown) {
    Cell current = make_pair(5, 5);
    Snake snake = Snake(current);
    snake.set_direction('d');
    
    snake.move();
    Cell next = snake.get_head();
    
    EXPECT_EQ(next,make_pair(current.first+1,current.second));
}


TEST(SnakeGrow, Size){
  Cell current = make_pair(5, 5);
  Snake snake = Snake(current);
  snake.set_direction('r');

  snake.grow();
  
  EXPECT_EQ(snake.getSize(),2);
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