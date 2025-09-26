#include <gtest/gtest.h>
#include "snake.h"

// The global 'direction' variable must be defined for the linker, 
// even though get_next_head doesn't use it directly in its logic.
char direction = 'r'; 

// --- Next Head Calculation Tests ---

// Test non-wrapping movement to the right
TEST(NextHeadTest, NextHeadRightNonWrap)
{
    pair<int, int> pos = {5, 5};
    auto next = get_next_head(pos, 'r');
    EXPECT_EQ(next, make_pair(5, 6));
}

// Test wrapping movement to the right (from BOARD_SIZE-1 to 0)
TEST(NextHeadTest, NextHeadRightWrap)
{
    pair<int, int> pos = {0, BOARD_SIZE - 1}; // e.g., (0, 9)
    auto next = get_next_head(pos, 'r');
    EXPECT_EQ(next, make_pair(0, 0));
}

// Test non-wrapping movement to the left
TEST(NextHeadTest, NextHeadLeftNonWrap)
{
    pair<int, int> pos = {5, 5};
    auto next = get_next_head(pos, 'l');
    EXPECT_EQ(next, make_pair(5, 4));
}

// Test wrapping movement to the left (from 0 to BOARD_SIZE-1)
TEST(NextHeadTest, NextHeadLeftWrap)
{
    pair<int, int> pos = {0, 0};
    auto next = get_next_head(pos, 'l');
    EXPECT_EQ(next, make_pair(0, BOARD_SIZE - 1)); // e.g., (0, 9)
}

// Test non-wrapping movement down
TEST(NextHeadTest, NextHeadDownNonWrap)
{
    pair<int, int> pos = {5, 5};
    auto next = get_next_head(pos, 'd');
    EXPECT_EQ(next, make_pair(6, 5));
}

// Test wrapping movement down (from BOARD_SIZE-1 to 0)
TEST(NextHeadTest, NextHeadDownWrap)
{
    pair<int, int> pos = {BOARD_SIZE - 1, 0}; // e.g., (9, 0)
    auto next = get_next_head(pos, 'd');
    EXPECT_EQ(next, make_pair(0, 0));
}

// Test non-wrapping movement up
TEST(NextHeadTest, NextHeadUpNonWrap)
{
    pair<int, int> pos = {5, 5};
    auto next = get_next_head(pos, 'u');
    EXPECT_EQ(next, make_pair(4, 5));
}

// Test wrapping movement up (from 0 to BOARD_SIZE-1)
TEST(NextHeadTest, NextHeadUpWrap)
{
    pair<int, int> pos = {0, 0};
    auto next = get_next_head(pos, 'u');
    EXPECT_EQ(next, make_pair(BOARD_SIZE - 1, 0)); // e.g., (9, 0)
}