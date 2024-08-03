#include "fluidSolver.h"


void initializeFluidParticles(Particle* particles, int numberOfFluidParticles, Vector2f offset)
{
    for (size_t i = 0; i < numberOfFluidParticles; i++)
    {
        particles[i].h = H;
        particles[i].density = DENSITY;
        particles[i].pressure = PRESSURE;
        particles[i].mass = DENSITY * (H * H);
        particles[i].velocity = Vector2f(0, 0);
        particles[i].acceleration = Vector2f(0, 0);

        float temp1 = H * (i % 25 + offset.x) - 1.f;
        float temp2 = H * (i / 25 + offset.y) - 1.f;
        particles[i].index = i;
        particles[i].position = Vector2f(temp1, temp2); // distribute the particles
    }
}

void initializeBoundaryParticles(Particle* particles, int numberOfFluidParticles, int numberOfParticles)
{
    for (size_t i = numberOfFluidParticles; i < numberOfParticles; i++)
    {
        float temp1 = 0.f;
        float temp2 = 0.f;
        if (i - numberOfFluidParticles < 120)
        {
            // right
            temp1 = H * ((i - numberOfFluidParticles) % 2 + 60) - 1.f;
            temp2 = (H * ((i - numberOfFluidParticles) / 2 + 5) - 1.f);
        }
        else if (i - numberOfFluidParticles < 240)
        {
            // left
            temp1 = H * ((i - numberOfFluidParticles - 120) % 2 + 2) - 1.f;
            temp2 = (H * ((i - numberOfFluidParticles - 120) / 2 + 5) - 1.f);
        }
        else
        {
            // bottom
            temp1 = H * ((i - numberOfFluidParticles - 240) % 110 + 2) - 1.f;
            temp2 = (H * ((i - numberOfFluidParticles - 240) / 110 + 3) - 1.f);
        }
        particles[i].h = H;
        particles[i].pressure = PRESSURE;
        particles[i].velocity = Vector2f(0, 0);
        particles[i].acceleration = Vector2f(0, 0);

        particles[i].index = i;
        particles[i].position = Vector2f(temp1, temp2); // distribute the particles
        particles[i].density = 1.7f;
        particles[i].mass = 1.7f * H * H;
    }
}

float cubicSpline(Vector2f positionA, Vector2f positionB) // just slightly too much * 0.999138886f for alpha
{
    Vector2f temp = positionA - positionB;
    float distance = sqrt(temp.x * temp.x + temp.y * temp.y);
    float d = distance / H;

    float alpha = 5.f / (14.f * PI * H * H); // 2D

    float t1 = max(1.f - d, 0.f);
    float t2 = max(2.f - d, 0.f);

    float w = alpha * (t2 * t2 * t2 - 4.f * t1 * t1 * t1);

    return w;
}

Vector2f cubicSplineDerivative(Vector2f positionA, Vector2f positionB)
{
    Vector2f temp = positionA - positionB;
    float distance = sqrt(temp.x * temp.x + temp.y * temp.y);
    if (distance == 0.f)
    {
        return Vector2f(0.f, 0.f);
    }
    float d = distance / H;

    float alpha = 5.f / (14.f * PI * H * H); // 2D

    float t1 = max(1 - d, 0.f);
    float t2 = max(2 - d, 0.f);

    float v = alpha * (-3.f * t2 * t2 + 12.f * t1 * t1);
    Vector2f w1 = Vector2f((temp.x / (d * H * H)) * v, (temp.y / (d * H * H)) * v); // whyy * H

    return w1;
}

void neighbourSearchNN(Particle* particles, int numberOfFluidParticles, int numberOfParticles)
{
    for (size_t i = 0; i < numberOfFluidParticles; i++)
    {
        particles[i].neighbors.clear();
        for (size_t j = 0; j < numberOfParticles; j++)
        {
            Vector2f d = particles[i].position - particles[j].position;
            float distance = sqrt(d.x * d.x + d.y * d.y);
            if (distance < 2 * H)
            {
                particles[i].neighbors.push_back(&particles[j]);
            }
        }
    }
}

void computeDensityAndPressure(Particle* particles, int numberOfFluidParticles)
{
    for (size_t i = 0; i < numberOfFluidParticles; i++)
    {
        float temp = 0;
        // sum over all neighbors
        for (size_t j = 0; j < particles[i].neighbors.size(); j++)
        {
            temp += particles[i].neighbors[j]->mass * cubicSpline(particles[i].position, particles[i].neighbors[j]->position); // multiply with kernel
        }
        particles[i].density = temp;
        particles[i].pressure = STIFFNESS * (max((temp / DENSITY) - 1.f, 0.f)); // somwhere max? // problem with dividing by 0?
    }
}

void updatePositions(Particle* particles, int numberOfFluidParticles)
{
    for (size_t i = 0; i < numberOfFluidParticles; i++)
    {
        particles[i].velocity += TIME_STEP * particles[i].acceleration;
        particles[i].position += TIME_STEP * particles[i].velocity; // updated velocity
    }
}

Vector2f computeNonPAcc(Particle p)
{
    Vector2f SPH = Vector2f(0.f, 0.f);
    for (size_t i = 0; i < p.neighbors.size(); i++)
    {
        Vector2f velD = p.velocity - p.neighbors[i]->velocity;
        Vector2f posD = p.position - p.neighbors[i]->position;
        float val = (velD.x * posD.x + velD.y * posD.y) / (posD.x * posD.x + posD.y * posD.y + 0.01 * H * H);
        Vector2f kernel = cubicSplineDerivative(p.position, p.neighbors[i]->position);
        SPH += (p.neighbors[i]->mass / p.neighbors[i]->density) * val * kernel;
    }
    return 2.f * VISCOSITY * SPH + GRAVITY;
}

Vector2f computePAcc(Particle p)
{
    Vector2f SPH = Vector2f(0.f, 0.f);
    for (size_t i = 0; i < p.neighbors.size(); i++)
    {
        float val = p.pressure / (p.density * p.density) + p.neighbors[i]->pressure / (p.neighbors[i]->density * p.neighbors[i]->density);
        Vector2f kernel = cubicSplineDerivative(p.position, p.neighbors[i]->position);
        SPH += p.neighbors[i]->mass * val * kernel;
    }
    return -SPH;
}

void computeAccelerations(Particle* particles, int numberOfFluidParticles)
{
    for (size_t i = 0; i < numberOfFluidParticles; i++)
    {
        Vector2f aNonP = computeNonPAcc(particles[i]);
        Vector2f aP = computePAcc(particles[i]);

        particles[i].acceleration = aNonP + aP;
    }
}