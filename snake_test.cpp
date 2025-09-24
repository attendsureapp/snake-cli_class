#include <gtest/gtest.h>
#include "snake.h"

// Movement Tests
TEST(SnakeTest, MoveUpWrapsToBottom) {
    SnakeGame g;
    g.set_direction(UP);
    for(int i=0;i<2;i++) g.step(); 
    auto head = g.next_head();
    EXPECT_EQ(head.first, BOARD_SIZE - 1); 
}

TEST(SnakeTest, MoveDownWrapsToTop) {
    SnakeGame g;
    g.set_direction(DOWN);
    for(int i=0;i<BOARD_SIZE; i++) g.step();
    auto head = g.next_head();
    EXPECT_EQ(head.first, 0); 
}

TEST(SnakeTest, MoveRightWrapsToLeft) {
    SnakeGame g;
    g.set_direction(RIGHT);
    for(int i=0;i<BOARD_SIZE; i++) g.step();
    auto head = g.next_head();
    EXPECT_EQ(head.second, 0);
}

TEST(SnakeTest, MoveLeftWrapsToRight) {
    SnakeGame g;
    g.set_direction(LEFT);
    for(int i=0;i<2;i++) g.step(); 
    auto head = g.next_head();
    EXPECT_EQ(head.second, BOARD_SIZE - 1); 
}

// Food Test
TEST(SnakeTest, FoodNotOnSnake) {
    SnakeGame g;
    auto snake = g.get_snake();
    auto food = g.get_food();
    EXPECT_EQ(find(snake.begin(), snake.end(), food), snake.end());
}

TEST(SnakeTest, EatFoodIncreasesScore) {
    SnakeGame g;
    auto food = g.get_food();
    while (g.next_head() != food) g.step();
    g.step(); 
    EXPECT_GT(g.get_score(), 0);
}

TEST(SnakeTest, SnakeGrowsOnFood) {
    SnakeGame g;
    int initial_len = g.get_snake().size();
    auto food = g.get_food();
    while (g.next_head() != food) g.step();
    g.step(); 
    EXPECT_GT(g.get_snake().size(), initial_len);
}

// Poison Tests
TEST(SnakeTest, PoisonNotOnFood) {
    SnakeGame g;
    EXPECT_NE(g.get_food(), g.get_poison());
}

TEST(SnakeTest, EatPoisonEndsGame) {
    SnakeGame g;
    auto poison = g.get_poison();
    while (g.next_head() != poison && g.step());
    EXPECT_FALSE(g.step()); 
}

// Collison Tests
TEST(SnakeTest, SelfCollisionEndsGame) {
    SnakeGame g;
    g.set_direction(RIGHT);
    for (int i=0; i<3; i++) g.step();
    g.set_direction(DOWN); g.step();
    g.set_direction(LEFT); g.step();
    g.set_direction(UP);
    EXPECT_FALSE(g.step()); 
}

// Score Management Tests
TEST(ScoreManagerTest, KeepsTop10) {
    ScoreManager sm;
    for (int i=0;i<20;i++) sm.add_score(i*10);
    auto scores = sm.get_scores();
    EXPECT_EQ(scores.size(), MAX_SCORES);
    EXPECT_TRUE(is_sorted(scores.begin(), scores.end(), greater<int>()));
}

TEST(ScoreManagerTest, PersistsScores) {
    ScoreManager sm;
    sm.add_score(100);
    sm.save();

    ScoreManager sm2;
    sm2.load();
    auto scores = sm2.get_scores();
    EXPECT_NE(find(scores.begin(), scores.end(), 100), scores.end());
}
