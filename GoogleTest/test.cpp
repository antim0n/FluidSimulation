#include "pch.h"
#include "../fluidSolver.cpp"

TEST(TestCaseName, TestName) {
	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
	Vector2f temp1 = Vector2f(H, 2 * H);
	Vector2f temp2 = Vector2f(H, H);
	EXPECT_EQ(1, cubicSpline(temp1, temp2));
}