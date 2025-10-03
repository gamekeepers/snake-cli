#include <gtest/gtest.h>
#include "snake.h"

TEST(SnakeTest, MovementCollision) {
    Snake s;
    auto h = s.head();
    EXPECT_FALSE(s.collides({h.first+1,h.second}));
    s.grow({1,0});
    EXPECT_TRUE(s.collides({1,0}));
}

TEST(GameTest, NextHeadWrapAround) {
    Game g;
    EXPECT_EQ(g.getNextHead({5,14},'r'), std::make_pair(5,0));
    EXPECT_EQ(g.getNextHead({0,5},'u'), std::make_pair(14,5));
}

TEST(ScoreManagerTest, Top10Scores) {
    ScoreManager sm;
    for(int i=1;i<=15;i++) sm.add(i*10);
    sm.load();
    EXPECT_LE(sm.getScores().size(), 10);
    EXPECT_GE(sm.getScores()[0], sm.getScores()[9]);
}
