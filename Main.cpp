#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "fluidSolver.h"
#include "savePlot.h"

using namespace sf;
using namespace std;

const int NUMBER_OF_FLUID_PARTICLES = 600;
const int NUMBER_OF_BOUNDARY_PARTICLES = 460;
const int NUMBER_OF_PARTICLES = NUMBER_OF_FLUID_PARTICLES + NUMBER_OF_BOUNDARY_PARTICLES;

const int WINDOW_WIDTH = 1400;
const int WINDOW_HEIGHT = 800;

float myTime = 0;
float deltaTime = 0.001;

Shader shader;
const std::string fragmentShader;

// states
bool useShader = false;
bool useColor = false;
bool stopSimulation = false;

Font font;
Text text;

int main()
{
    /* setup window */
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "FluidSolver", Style::Default);
    window.setFramerateLimit(60);   // prevent too much work for GPU
    window.setPosition(Vector2i(10, 10));

    /* are shaders available? */
    if (!sf::Shader::isAvailable())
    {
        cout << "shaders are not available...\n";
    }

    /* load shaders */
    if (!shader.loadFromFile("frag_shader.glsl", sf::Shader::Fragment))
    {
        cout << "error...\n";
    }

    /* seperate texture for water smoothing/shader effects */
    RenderTexture texture;
    if (!texture.create(WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        cout << "texture error";
    }

    /* load font */
    if (!font.loadFromFile("arial.ttf"))
    {
        cout << "font not loaded";
    }
    text.setFont(font);
    text.setString("SHORTCUTS   >>   stop: X | restart: left mouse | color: C | shader: S");
    text.setCharacterSize(15);
    text.setFillColor(Color::Green);

    /* allocate memory for the particles and their shapes */
    Particle* particles = new Particle[NUMBER_OF_PARTICLES];
    CircleShape* drawingCircles = new CircleShape[NUMBER_OF_PARTICLES];

    if (!particles || !drawingCircles)
    {
        cout << "Memory allocation failed.\n";
    }

    /* initialize all fluid particles */
    initializeFluidParticles(particles, NUMBER_OF_FLUID_PARTICLES, Vector2f(4, 5));

    /* initialize all boundary particles */
    initializeBoundaryParticles(particles, NUMBER_OF_FLUID_PARTICLES, NUMBER_OF_PARTICLES);

    /* one time plot */
    /*for (size_t i = 0; i < 101; i++)
    {
        savePoint("cubicSpline", - H - H + (i / 100.f) * 4 * H, cubicSpline(Vector2f(0, H), Vector2f(-H - H + (i / 100.f) * 4 * H, H)));
        savePoint("x-cubicSplineDerivative", -H - H + (i / 100.f) * 4 * H, cubicSplineDerivative(Vector2f(0, H), Vector2f(-H - H + (i / 100.f) * 4 * H, H)).x);
        savePoint("y-cubicSplineDerivative", -H - H + (i / 100.f) * 4 * H, cubicSplineDerivative(Vector2f(H, 0), Vector2f(H, -H - H + (i / 100.f) * 4 * H)).y);
    }*/

    /* simulation and rendering loop */
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event)) // check for user inputs (only for closing the window)
        {
            switch (event.type)
            {
            case Event::Closed:
                window.close();
                break;

            case Event::KeyPressed:
                if (event.key.scancode == sf::Keyboard::Scan::C)
                {
                    useColor = !useColor;
                }
                else if (event.key.scancode == sf::Keyboard::Scan::S)
                {
                    useShader = !useShader;
                }
                else if (event.key.scancode == sf::Keyboard::Scan::X)
                {
                    stopSimulation = !stopSimulation;
                }
                break;

            case Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    Vector2i mousePos = Mouse::getPosition(window);
                    initializeFluidParticles(particles, NUMBER_OF_FLUID_PARTICLES, Vector2f((float)mousePos.x / WINDOW_WIDTH * 2 / H, ((float)WINDOW_HEIGHT - (float)mousePos.y) / WINDOW_WIDTH * 2 / H));
                }
                break;

            default:
                break;
            }
        }


        if (!stopSimulation)
        {
            /* Update (SPH Fluid Solver) */
            neighbourSearchNN(particles, NUMBER_OF_FLUID_PARTICLES, NUMBER_OF_PARTICLES);
            computeDensityAndPressure(particles, NUMBER_OF_FLUID_PARTICLES);
            computeAccelerations(particles, NUMBER_OF_FLUID_PARTICLES);
            updatePositions(particles, NUMBER_OF_FLUID_PARTICLES);
        }
        
        /* plot over time */
        //if (myTime < 1) // 1000 points per graph
        //{
        //    savePoint("y-velocity", myTime, particles[0].velocity.y);
        //    savePoint("y-acceleration", myTime, particles[0].acceleration.y);
        //    savePoint("density", myTime, particles[0].density);
        //    savePoint("pressure", myTime, particles[0].pressure);
        //    savePoint("position", (particles[0].position.x + 1.f) * WINDOW_WIDTH / 2.f, (particles[0].position.y + 1.f) * WINDOW_WIDTH / 2.f);
        //}

        /* Draw */
        texture.clear();
        texture.display();

        window.clear(); // don't draw on top of the previous frame

        for (size_t i = 0; i < NUMBER_OF_PARTICLES; i++)
        {
            drawingCircles[i].setRadius(particles[i].h / 2.f * WINDOW_WIDTH / 2.f);    // h is defined as the "diameter"
            drawingCircles[i].setPosition(Vector2f((particles[i].position.x + 1.f) * WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - (particles[i].position.y + 1.f) * WINDOW_WIDTH / 2.f));   // the shapes to be drawn have to be updated independently, scale
            if (i < NUMBER_OF_FLUID_PARTICLES)
            {
                drawingCircles[i].setFillColor(Color::Blue);
                if (useShader)
                {
                    drawingCircles[i].setFillColor(Color::Transparent);
                }
                if(useColor && !useShader)
                {
                    Color color1;
                    Color color2;
                    Color c;
                    float t = min(1.f, sqrt(particles[i].velocity.x * particles[i].velocity.x + particles[i].velocity.y * particles[i].velocity.y) / 4.f);
                    if (t < 0.11)
                    {
                        t = (t - 0.0f) / 0.11f;
                        color1 = Color::Blue;
                        color2 = Color::Cyan;
                    }
                    else if (t < 0.27)
                    {
                        t = (t - 0.11f) / 0.16f;
                        color1 = Color::Cyan;
                        color2 = Color::Green;
                    }
                    else if (t < 0.36f)
                    {
                        t = (t - 0.27f) / 0.09f;
                        color1 = Color::Green;
                        color2 = Color::Yellow;
                    }
                    else
                    {
                        t = (t - 0.36f) / 0.64f;
                        color1 = Color::Yellow;
                        color2 = Color::Red;
                    }

                    c.r = min(color1.r + ((color2.r - color1.r) * t), 255.f);
                    c.g = min(color1.g + ((color2.g - color1.g) * t), 255.f);
                    c.b = min(color1.b + ((color2.b - color1.b) * t), 255.f);

                    drawingCircles[i].setFillColor(c);
                }
            }
            //if (i == 0) // one is red
            //{
            //    drawingCircles[0].setFillColor(Color::Red);
            //}
            window.draw(drawingCircles[i]);
        }

        if (useShader == true)
        {
            Sprite sprite(texture.getTexture());
            vector<Vector2f> p_pos;
            vector<Vector2f> p_vel;
            for (size_t i = 0; i < NUMBER_OF_FLUID_PARTICLES; i++) {
                p_pos.push_back(drawingCircles[i].getPosition() + Vector2f(drawingCircles[i].getRadius(), drawingCircles[i].getRadius()));
                p_vel.push_back(particles[i].velocity);
            }
            shader.setUniformArray("particle_positions", p_pos.data(), NUMBER_OF_FLUID_PARTICLES);
            shader.setUniformArray("particle_velocities", p_vel.data(), NUMBER_OF_FLUID_PARTICLES);
            shader.setUniform("threshold", 0.03f);
            shader.setUniform("radius", 30.f);
            shader.setUniform("with_color", useColor);
            window.draw(sprite, &shader);
        }

        /* instructions */
        window.draw(text);

        /* Display */
        window.display();

        myTime += deltaTime;
    }

    writeToFile();

    /* deallocate memory */
    delete[] particles;
    delete[] drawingCircles;

    return EXIT_SUCCESS;
}