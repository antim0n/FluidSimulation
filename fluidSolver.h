#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

using namespace sf;
using namespace std;

const float H = 0.025f;
const float REST_DENSITY = 1.2f;
const float PRESSURE = 0.f;

const float STIFFNESS = 300.2f;
const float VISCOSITY = 0.02f;
const Vector2f GRAVITY = Vector2f(0.f, -9.81f);

const double PI = 3.14159265358979323846;
const float TIME_STEP = 0.0012f;

/* holds all important information needed at a particle */
struct Particle
{
    int index;
    float h; // on a scale from -1 to 1
    float density;  // in kg / m^3, 997 for water
    float pressure; // in N/m^2
    float mass;  // density * volume
    Vector2f position;
    Vector2f velocity;
    Vector2f acceleration;
    vector<Particle*> neighbors;    // pointers to all current neighbors
};

// function declarations

/* */
void initializeFluidParticles(Particle* particles, int numberOfFluidParticles, Vector2f offset);
/* */
void initializeBoundaryParticles(Particle* particles, int numberOfFluidParticles, int numberOfParticles, int sceneID);
/* */
float cubicSpline(Vector2f positionA, Vector2f positionB);
/* */
Vector2f cubicSplineDerivative(Vector2f positionA, Vector2f positionB);
/* */
void neighborSearchNN(Particle* particles, int numberOfFluidParticles, int numberOfParticles, float support);
/* */
void computeDensityAndPressure(Particle* particles, int numberOfFluidParticles);
/* */
void updatePositions(Particle* particles, int numberOfFluidParticles);
/* */
void computeAccelerations(Particle* particles, int numberOfFluidParticles);
/* */
Vector2f computeNonPAcc(Particle p);
/* */
Vector2f computePAcc(Particle p, int numberOfFluidParticles);
