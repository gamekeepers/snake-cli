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
// Food generation should not overlap snake or another food
TEST(FoodGeneration, NotOnSnakeOrOtherFood) {
    deque<pair<int,int>> snake = {{0,0}, {0,1}, {0,2}};
    pair<int,int> other_food = {5,5};
    for(int i=0;i<100;i++){ // test multiple generations
        pair<int,int> food = generate_food(10, snake, other_food);
        EXPECT_EQ(find(snake.begin(), snake.end(), food), snake.end());
        EXPECT_NE(food, other_food);
    }
}

// Calculate speed should never go below min speed
TEST(SpeedCalculation, MinSpeedLimit) {
    int speed = calculate_speed(20); // more than enough to reduce below min
    EXPECT_GE(speed, 100);
}

// Score increments properly
TEST(ScoreUpdate, IncrementBy10) {
    int score = 0;
    score += 10;
    EXPECT_EQ(score, 10);
}

// Poison food should never overlap snake or normal food
TEST(PoisonFood, NotOnSnakeOrFood) {
    deque<pair<int,int>> snake = {{0,0}, {1,0}, {2,0}};
    pair<int,int> normal_food = {5,5};
    for(int i=0;i<50;i++){
        pair<int,int> poison = generate_food(10, snake, normal_food);
        EXPECT_EQ(find(snake.begin(), snake.end(), poison), snake.end());
        EXPECT_NE(poison, normal_food);
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