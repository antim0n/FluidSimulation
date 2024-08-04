#include "pch.h"
#include "../fluidSolver.cpp"
#include <iostream>

TEST(SimpleTest, TestCubicSpline)
{
    Vector2f pos1 = Vector2f(H, 2 * H);
    Vector2f pos2 = Vector2f(2 * H, 2 * H);
    float w = cubicSpline(pos1, pos2);
    EXPECT_FLOAT_EQ(w, 5.f / (14.f * PI * H * H));
}

TEST(SimpleTest, TestCubicSplineDerivative)
{
    Vector2f pos1 = Vector2f(H, 2 * H);
    Vector2f pos2 = Vector2f(2 * H, 2 * H);
    Vector2f deriv = cubicSplineDerivative(pos1, pos2);
    EXPECT_FLOAT_EQ(deriv.x, (5.f / (14.f * PI * H * H)) * 3 / H);
    EXPECT_FLOAT_EQ(deriv.y, 0.f);
}

TEST(SimpleTest, TestDensityAndPressureComputation)
{
    Particle* p = new Particle[9];

    initializeFluidParticles(p, 9, Vector2f(0, 0));

    p[1].position = Vector2f(H, H * 2);
    p[2].position = Vector2f(H * 2, H * 2);
    p[3].position = Vector2f(H * 3, H * 2);
    p[4].position = Vector2f(H, H * 3);
    p[0].position = Vector2f(H * 2, H * 3); // middle
    p[5].position = Vector2f(H * 3, H * 3);
    p[6].position = Vector2f(H, H * 4);
    p[7].position = Vector2f(H * 2, H * 4);
    p[8].position = Vector2f(H * 3, H * 4);

    p[0].neighbors.push_back(&p[0]);
    p[0].neighbors.push_back(&p[1]);
    p[0].neighbors.push_back(&p[2]);
    p[0].neighbors.push_back(&p[3]);
    p[0].neighbors.push_back(&p[4]);
    p[0].neighbors.push_back(&p[5]);
    p[0].neighbors.push_back(&p[6]);
    p[0].neighbors.push_back(&p[7]);
    p[0].neighbors.push_back(&p[8]);

    computeDensityAndPressure(p, 1);
    float sum = p[1].mass * cubicSpline(p[0].position, p[1].position) +
        p[2].mass * cubicSpline(p[0].position, p[2].position) + p[3].mass * cubicSpline(p[0].position, p[3].position) +
        p[4].mass * cubicSpline(p[0].position, p[4].position) + p[5].mass * cubicSpline(p[0].position, p[5].position) +
        p[6].mass * cubicSpline(p[0].position, p[6].position) + p[7].mass * cubicSpline(p[0].position, p[7].position) +
        p[8].mass * cubicSpline(p[0].position, p[8].position) + p[0].mass * cubicSpline(p[0].position, p[0].position);
    EXPECT_FLOAT_EQ(p[0].density, sum);
    // EXPECT_FLOAT_EQ(p[0].density, DENSITY); // not sure if this is supposed to be true == 1.201034199
    EXPECT_FLOAT_EQ(p[0].pressure, STIFFNESS * (p[0].density / DENSITY - 1.f));

    delete[] p;
}

TEST(SimpleTest, TestComputeNonPAcc)
{
    Particle* p = new Particle[9];

    initializeFluidParticles(p, 9, Vector2f(0, 0));

    p[1].position = Vector2f(H, H * 2);
    p[2].position = Vector2f(H * 2, H * 2);
    p[3].position = Vector2f(H * 3, H * 2);
    p[4].position = Vector2f(H, H * 3);
    p[0].position = Vector2f(H * 2, H * 3); // middle
    p[5].position = Vector2f(H * 3, H * 3);
    p[6].position = Vector2f(H, H * 4);
    p[7].position = Vector2f(H * 2, H * 4);
    p[8].position = Vector2f(H * 3, H * 4);

    p[1].velocity = GRAVITY;
    p[2].velocity = GRAVITY;
    p[3].velocity = GRAVITY;
    p[4].velocity = GRAVITY;
    p[0].velocity = Vector2f(0, 0);
    p[5].velocity = GRAVITY;
    p[6].velocity = GRAVITY;
    p[7].velocity = GRAVITY;
    p[8].velocity = GRAVITY;

    p[0].neighbors.push_back(&p[0]);
    p[0].neighbors.push_back(&p[1]);
    p[0].neighbors.push_back(&p[2]);
    p[0].neighbors.push_back(&p[3]);
    p[0].neighbors.push_back(&p[4]);
    p[0].neighbors.push_back(&p[5]);
    p[0].neighbors.push_back(&p[6]);
    p[0].neighbors.push_back(&p[7]);
    p[0].neighbors.push_back(&p[8]);

    Vector2f res = computeNonPAcc(p[0]);

    EXPECT_FLOAT_EQ(res.x, 0); // H = 0.017
    EXPECT_FLOAT_EQ(res.y, -14374.3691f);

    delete[] p;
}

TEST(SimpleTest, TestComputePAcc)
{
    Particle* p = new Particle[9];

    initializeFluidParticles(p, 9, Vector2f(0, 0));

    p[1].position = Vector2f(H, H * 2);
    p[2].position = Vector2f(H * 2, H * 2);
    p[3].position = Vector2f(H * 3, H * 2);
    p[4].position = Vector2f(H, H * 3);
    p[0].position = Vector2f(H * 2, H * 3); // middle
    p[5].position = Vector2f(H * 3, H * 3);
    p[6].position = Vector2f(H, H * 4);
    p[7].position = Vector2f(H * 2, H * 4);
    p[8].position = Vector2f(H * 3, H * 4);

    p[1].pressure = 1.1;
    p[2].pressure = 1.1;
    p[3].pressure = 1.1;
    p[4].pressure = 1.1;
    p[0].pressure = 2.1; // middle
    p[5].pressure = 2.1;
    p[6].pressure = 2.1;
    p[7].pressure = 2.1;
    p[8].pressure = 2.1;

    p[0].neighbors.push_back(&p[0]);
    p[0].neighbors.push_back(&p[1]);
    p[0].neighbors.push_back(&p[2]);
    p[0].neighbors.push_back(&p[3]);
    p[0].neighbors.push_back(&p[4]);
    p[0].neighbors.push_back(&p[5]);
    p[0].neighbors.push_back(&p[6]);
    p[0].neighbors.push_back(&p[7]);
    p[0].neighbors.push_back(&p[8]);

    Vector2f res = computePAcc(p[0], 9);

    EXPECT_FLOAT_EQ(res.x, 0); // H = 0.017
    EXPECT_FLOAT_EQ(res.y, 0);

    delete[] p;
}

TEST(SimpleTest, TestComputeAccelerations)
{
    Particle* p = new Particle[1];
    Particle p1;
    Particle p2;
    Particle p3;
    Particle p4;
    Particle p5;
    Particle p6;
    Particle p7;
    Particle p8;

    p1.position = Vector2f(H, H * 2);
    p2.position = Vector2f(H * 2, H * 2);
    p3.position = Vector2f(H * 3, H * 2);
    p4.position = Vector2f(H, H * 3);
    p[0].position = Vector2f(H * 2, H * 3); // middle
    p5.position = Vector2f(H * 3, H * 3);
    p6.position = Vector2f(H, H * 4);
    p7.position = Vector2f(H * 2, H * 4);
    p8.position = Vector2f(H * 3, H * 4);

    p[0].neighbors.push_back(&p[0]);
    p[0].neighbors.push_back(&p1);
    p[0].neighbors.push_back(&p2);
    p[0].neighbors.push_back(&p3);
    p[0].neighbors.push_back(&p4);
    p[0].neighbors.push_back(&p5);
    p[0].neighbors.push_back(&p6);
    p[0].neighbors.push_back(&p7);
    p[0].neighbors.push_back(&p8);

    computeDensityAndPressure(p, 1);
    computeAccelerations(p, 1);

    float term1 = ((p[0].velocity - p1.velocity).x * (p[0].position - p1.position).x + (p[0].velocity - p1.velocity).y * (p[0].position - p1.position).y) / (pow((p[0].position - p1.position).x, 2.f) + pow((p[0].position - p1.position).y, 2.f) + 0.01 * H * H);
    float term2 = ((p[0].velocity - p2.velocity).x * (p[0].position - p2.position).x + (p[0].velocity - p2.velocity).y * (p[0].position - p2.position).y) / (pow((p[0].position - p2.position).x, 2.f) + pow((p[0].position - p2.position).y, 2.f) + 0.01 * H * H);
    float term3 = ((p[0].velocity - p3.velocity).x * (p[0].position - p3.position).x + (p[0].velocity - p3.velocity).y * (p[0].position - p3.position).y) / (pow((p[0].position - p3.position).x, 2.f) + pow((p[0].position - p3.position).y, 2.f) + 0.01 * H * H);
    float term4 = ((p[0].velocity - p4.velocity).x * (p[0].position - p4.position).x + (p[0].velocity - p4.velocity).y * (p[0].position - p4.position).y) / (pow((p[0].position - p4.position).x, 2.f) + pow((p[0].position - p4.position).y, 2.f) + 0.01 * H * H);
    float term5 = ((p[0].velocity - p5.velocity).x * (p[0].position - p5.position).x + (p[0].velocity - p5.velocity).y * (p[0].position - p5.position).y) / (pow((p[0].position - p5.position).x, 2.f) + pow((p[0].position - p5.position).y, 2.f) + 0.01 * H * H);
    float term6 = ((p[0].velocity - p6.velocity).x * (p[0].position - p6.position).x + (p[0].velocity - p6.velocity).y * (p[0].position - p6.position).y) / (pow((p[0].position - p6.position).x, 2.f) + pow((p[0].position - p6.position).y, 2.f) + 0.01 * H * H);
    float term7 = ((p[0].velocity - p7.velocity).x * (p[0].position - p7.position).x + (p[0].velocity - p7.velocity).y * (p[0].position - p7.position).y) / (pow((p[0].position - p7.position).x, 2.f) + pow((p[0].position - p7.position).y, 2.f) + 0.01 * H * H);
    float term8 = ((p[0].velocity - p8.velocity).x * (p[0].position - p8.position).x + (p[0].velocity - p8.velocity).y * (p[0].position - p8.position).y) / (pow((p[0].position - p8.position).x, 2.f) + pow((p[0].position - p8.position).y, 2.f) + 0.01 * H * H);
    float term9 = ((p[0].velocity - p[0].velocity).x * (p[0].position - p[0].position).x + (p[0].velocity - p[0].velocity).y * (p[0].position - p[0].position).y) / (pow((p[0].position - p[0].position).x, 2.f) + pow((p[0].position - p[0].position).y, 2.f) + 0.01 * H * H);

    float nonPressureX = GRAVITY.x + 2.f * STIFFNESS * (
        (p1.mass / p1.density) * term1 * cubicSplineDerivative(p[0].position, p1.position).x +
        (p2.mass / p2.density) * term2 * cubicSplineDerivative(p[0].position, p2.position).x +
        (p3.mass / p3.density) * term3 * cubicSplineDerivative(p[0].position, p3.position).x +
        (p4.mass / p4.density) * term4 * cubicSplineDerivative(p[0].position, p4.position).x +
        (p5.mass / p5.density) * term5 * cubicSplineDerivative(p[0].position, p5.position).x +
        (p6.mass / p6.density) * term6 * cubicSplineDerivative(p[0].position, p6.position).x +
        (p7.mass / p7.density) * term7 * cubicSplineDerivative(p[0].position, p7.position).x +
        (p8.mass / p8.density) * term8 * cubicSplineDerivative(p[0].position, p8.position).x +
        (p[0].mass / p[0].density) * term9 * cubicSplineDerivative(p[0].position, p[0].position).x);

    float pressureX = -(p1.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p1.pressure / (p1.density * p1.density))) * cubicSplineDerivative(p[0].position, p1.position).x +
        p2.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p2.pressure / (p2.density * p2.density))) * cubicSplineDerivative(p[0].position, p2.position).x +
        p3.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p3.pressure / (p3.density * p3.density))) * cubicSplineDerivative(p[0].position, p3.position).x +
        p4.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p4.pressure / (p4.density * p4.density))) * cubicSplineDerivative(p[0].position, p4.position).x +
        p5.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p5.pressure / (p5.density * p5.density))) * cubicSplineDerivative(p[0].position, p5.position).x +
        p6.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p6.pressure / (p6.density * p6.density))) * cubicSplineDerivative(p[0].position, p6.position).x +
        p7.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p7.pressure / (p7.density * p7.density))) * cubicSplineDerivative(p[0].position, p7.position).x +
        p8.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p8.pressure / (p8.density * p8.density))) * cubicSplineDerivative(p[0].position, p8.position).x +
        p[0].mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p[0].pressure / (p[0].density * p[0].density))) * cubicSplineDerivative(p[0].position, p[0].position).x);

    float sum1 = nonPressureX + pressureX;

    float nonPressureY = GRAVITY.y + 2.f * STIFFNESS * (
        (p1.mass / p1.density) * term1 * cubicSplineDerivative(p[0].position, p1.position).y +
        (p2.mass / p2.density) * term2 * cubicSplineDerivative(p[0].position, p2.position).y +
        (p3.mass / p3.density) * term3 * cubicSplineDerivative(p[0].position, p3.position).y +
        (p4.mass / p4.density) * term4 * cubicSplineDerivative(p[0].position, p4.position).y +
        (p5.mass / p5.density) * term5 * cubicSplineDerivative(p[0].position, p5.position).y +
        (p6.mass / p6.density) * term6 * cubicSplineDerivative(p[0].position, p6.position).y +
        (p7.mass / p7.density) * term7 * cubicSplineDerivative(p[0].position, p7.position).y +
        (p8.mass / p8.density) * term8 * cubicSplineDerivative(p[0].position, p8.position).y +
        (p[0].mass / p[0].density) * term9 * cubicSplineDerivative(p[0].position, p[0].position).y);

    float pressureY = -(p1.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p1.pressure / (p1.density * p1.density))) * cubicSplineDerivative(p[0].position, p1.position).y +
        p2.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p2.pressure / (p2.density * p2.density))) * cubicSplineDerivative(p[0].position, p2.position).y +
        p3.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p3.pressure / (p3.density * p3.density))) * cubicSplineDerivative(p[0].position, p3.position).y +
        p4.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p4.pressure / (p4.density * p4.density))) * cubicSplineDerivative(p[0].position, p4.position).y +
        p5.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p5.pressure / (p5.density * p5.density))) * cubicSplineDerivative(p[0].position, p5.position).y +
        p6.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p6.pressure / (p6.density * p6.density))) * cubicSplineDerivative(p[0].position, p6.position).y +
        p7.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p7.pressure / (p7.density * p7.density))) * cubicSplineDerivative(p[0].position, p7.position).y +
        p8.mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p8.pressure / (p8.density * p8.density))) * cubicSplineDerivative(p[0].position, p8.position).y +
        p[0].mass * ((p[0].pressure / (p[0].density * p[0].density)) + (p[0].pressure / (p[0].density * p[0].density))) * cubicSplineDerivative(p[0].position, p[0].position).y);

    float sum2 = nonPressureY + pressureY;

    EXPECT_FLOAT_EQ(p[0].acceleration.x, sum1);
    EXPECT_FLOAT_EQ(p[0].acceleration.y, sum2);
    EXPECT_FLOAT_EQ(p[0].acceleration.x, GRAVITY.x);
    EXPECT_FLOAT_EQ(p[0].acceleration.y, GRAVITY.y);

    delete[] p;
}

TEST(SimpleTest, TestUpdatePositions)
{
    Particle* p = new Particle[1];
    p[0].position = Vector2f(0.5f, 0.5f);
    p[0].velocity = Vector2f(1.3f, 4.79f);
    p[0].acceleration = Vector2f(0.34f, 0.61f);

    updatePositions(p, 1);
    EXPECT_FLOAT_EQ(p[0].velocity.x, 1.3f + TIME_STEP * 0.34f);
    EXPECT_FLOAT_EQ(p[0].velocity.y, 4.79f + TIME_STEP * 0.61f);
    EXPECT_FLOAT_EQ(p[0].position.x, 0.5f + TIME_STEP * p[0].velocity.x);
    EXPECT_FLOAT_EQ(p[0].position.y, 0.5f + TIME_STEP * p[0].velocity.y);

    delete[] p;
}

// ImportantTest: tests that are recommended in the slides

TEST(ImportantTest, TestNeighborSearchNN)
{
    Particle* p = new Particle[13];

    initializeFluidParticles(p, 13, Vector2f(0, 0));

    p[1].position = Vector2f(H, H * 2);
    p[2].position = Vector2f(H * 2, H * 2);
    p[3].position = Vector2f(H * 3, H * 2);
    p[4].position = Vector2f(H, H * 3);
    p[0].position = Vector2f(H * 2, H * 3); // middle
    p[5].position = Vector2f(H * 3, H * 3);
    p[6].position = Vector2f(H, H * 4);
    p[7].position = Vector2f(H * 2, H * 4);
    p[8].position = Vector2f(H * 3, H * 4);

    p[9].position = Vector2f(H * 2, H);
    p[10].position = Vector2f(H * 4, H * 3);
    p[11].position = Vector2f(H * 2, H * 5);
    p[12].position = Vector2f(0, H * 3);

    neighborSearchNN(p, 13, 13, 1.9);

    EXPECT_EQ(p[0].neighbors.size(), 9);

    neighborSearchNN(p, 13, 13, 2.1);

    EXPECT_EQ(p[0].neighbors.size(), 13);

    delete[] p;
}
TEST(ImportantTest, TestCubicSpline) {
    Vector2f pos1 = Vector2f(H, H * 2);
    Vector2f pos2 = Vector2f(H * 2, H * 2);
    Vector2f pos3 = Vector2f(H * 3, H * 2);
    Vector2f pos4 = Vector2f(H, H * 3);
    Vector2f pos5 = Vector2f(H * 2, H * 3); // middle
    Vector2f pos6 = Vector2f(H * 3, H * 3);
    Vector2f pos7 = Vector2f(H, H * 4);
    Vector2f pos8 = Vector2f(H * 2, H * 4);
    Vector2f pos9 = Vector2f(H * 3, H * 4);

    float w1 = cubicSpline(pos5, pos5);
    float w2 = cubicSpline(pos5, pos1);
    float w3 = cubicSpline(pos5, pos2);
    float w4 = cubicSpline(pos5, pos3);
    float w5 = cubicSpline(pos5, pos4);
    float w6 = cubicSpline(pos5, pos6);
    float w7 = cubicSpline(pos5, pos7);
    float w8 = cubicSpline(pos5, pos8);
    float w9 = cubicSpline(pos5, pos9);

    float w10 = cubicSpline(pos9, pos5);

    EXPECT_FLOAT_EQ(w9, w10);

    float sum = w1 + w2 + w3 + w4 + w5 + w6 + w7 + w8 + w9;

    EXPECT_FLOAT_EQ(sum, 1.f / (H * H));
}

TEST(ImportantTest, TestCubicSplineDerivative)
{
    Vector2f pos1 = Vector2f(H, H * 2);
    Vector2f pos2 = Vector2f(H * 2, H * 2);
    Vector2f pos3 = Vector2f(H * 3, H * 2);
    Vector2f pos4 = Vector2f(H, H * 3);
    Vector2f pos5 = Vector2f(H * 2, H * 3); // middle
    Vector2f pos6 = Vector2f(H * 3, H * 3);
    Vector2f pos7 = Vector2f(H, H * 4);
    Vector2f pos8 = Vector2f(H * 2, H * 4);
    Vector2f pos9 = Vector2f(H * 3, H * 4);

    Vector2f diff1 = pos5 - pos1;
    Vector2f diff2 = pos5 - pos2;
    Vector2f diff3 = pos5 - pos3;
    Vector2f diff4 = pos5 - pos4;
    Vector2f diff5 = pos5 - pos5;
    Vector2f diff6 = pos5 - pos6;
    Vector2f diff7 = pos5 - pos7;
    Vector2f diff8 = pos5 - pos8;
    Vector2f diff9 = pos5 - pos9;

    Vector2f kernel1 = cubicSplineDerivative(pos5, pos1);
    Vector2f kernel2 = cubicSplineDerivative(pos5, pos2);
    Vector2f kernel3 = cubicSplineDerivative(pos5, pos3);
    Vector2f kernel4 = cubicSplineDerivative(pos5, pos4);
    Vector2f kernel5 = cubicSplineDerivative(pos5, pos5);
    Vector2f kernel6 = cubicSplineDerivative(pos5, pos6);
    Vector2f kernel7 = cubicSplineDerivative(pos5, pos7);
    Vector2f kernel8 = cubicSplineDerivative(pos5, pos8);
    Vector2f kernel9 = cubicSplineDerivative(pos5, pos9);

    EXPECT_FLOAT_EQ(kernel5.x, 0); // same position
    EXPECT_FLOAT_EQ(kernel5.y, 0);

    Vector2f kernel10 = cubicSplineDerivative(pos9, pos5);

    EXPECT_FLOAT_EQ(kernel9.x, -kernel10.x); // symmteric (kind of)
    EXPECT_FLOAT_EQ(kernel9.y, -kernel10.y);

    Vector2f kernelSum = kernel1 + kernel2 + kernel3 + kernel4 + kernel5 + kernel6 + kernel7 + kernel8 + kernel9;

    EXPECT_FLOAT_EQ(kernelSum.x, 0); // as kernel values are huge, very small error happens?
    EXPECT_FLOAT_EQ(kernelSum.y, 0);

    float matrix00 = diff1.x * kernel1.x + diff2.x * kernel2.x + diff3.x * kernel3.x + diff4.x * kernel4.x + diff5.x * kernel5.x + diff6.x * kernel6.x + diff7.x * kernel7.x + diff8.x * kernel8.x + diff9.x * kernel9.x;
    float matrix01 = diff1.x * kernel1.y + diff2.x * kernel2.y + diff3.x * kernel3.y + diff4.x * kernel4.y + diff5.x * kernel5.y + diff6.x * kernel6.y + diff7.x * kernel7.y + diff8.x * kernel8.y + diff9.x * kernel9.y;
    float matrix10 = diff1.y * kernel1.x + diff2.y * kernel2.x + diff3.y * kernel3.x + diff4.y * kernel4.x + diff5.y * kernel5.x + diff6.y * kernel6.x + diff7.y * kernel7.x + diff8.y * kernel8.x + diff9.y * kernel9.x;
    float matrix11 = diff1.y * kernel1.y + diff2.y * kernel2.y + diff3.y * kernel3.y + diff4.y * kernel4.y + diff5.y * kernel5.y + diff6.y * kernel6.y + diff7.y * kernel7.y + diff8.y * kernel8.y + diff9.y * kernel9.y;

    EXPECT_FLOAT_EQ(matrix00, -1.f / (H * H)); // should be correct, difference comes from small kernel errors?
    EXPECT_FLOAT_EQ(matrix01, 0);
    EXPECT_FLOAT_EQ(matrix10, 0);
    EXPECT_FLOAT_EQ(matrix11, -1.f / (H * H));
}