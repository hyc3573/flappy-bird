#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <Windows.h>

#define SWIDTH 800
#define SHEIGHT 800

#define INITSPD 800

#define XMARGIN
#define GRAVITY 10

typedef int Square;

using namespace std;
using namespace sf;

class Bird : public Sprite
{
private:
    float speed;
    float acceleration;
    float mass;
    float gravity;
public:
    Bird(float X, float Y, float gravity, Texture& texture, IntRect rect) : gravity(gravity), speed(0), mass(1), acceleration(0)
    {
        this->setTexture(texture);
        this->setTextureRect(rect);
        this->setPosition(X, Y);
    }

    void Update(float dt, float force)
    {
        acceleration = gravity + (force/mass);
        speed += acceleration * dt;
        move(0, speed * dt);
    }
};

class Pipe
{
private:
    static float speed;
    
    float X;

    Sprite upper;
    Sprite lower;
    
public:
    Pipe(float X, float Y, float gap, Texture& texture, IntRect rect) : X(X)
    {
        speed = INITSPD;
        upper = Sprite(texture, rect);
        lower = Sprite(texture, rect);
        upper.setPosition(X, Y - gap / 2);
        lower.setPosition(X, Y + gap / 2);
    }
    void SetSpeed(float speed)
    {
        speed = speed;
    }
    void Update(float dt)
    {
        upper.move(-speed * dt, 0);
        lower.move(-speed * dt, 0);
    }
};

class Background
{
private:
    vector<Pipe> pipes;
    Sprite background1;
    Sprite background2;
    float speed;
    float pipeInterval;

public:
    Background(Texture& backTexture, IntRect rect, float speed, float pipeInterval) : background1(Sprite(backTexture, rect)), 
        background2(Sprite(backTexture, rect)), pipes(vector<Pipe>(0)), pipeInterval(pipeInterval)
    {
        background1.setPosition(0, 0);

        background2.setPosition(0 + SWIDTH, 0);

        pipes = vector<Pipe>(0);
    }
    void Update(float dt)
    {
        background1.move(-speed * dt, 0);
        background2.move(-speed * dt, 0);

        for (auto& elem : pipes)
        {
            elem.Update(dt);
        }

        if (background2.getPosition().x <= 0)
        {
            background1.setPosition(background2.getPosition().x + SWIDTH, 0);
            swap<Sprite>(background1, background2);
        }
    }

    void SetSpeed(float speed)
    {
        pipes[0].SetSpeed(speed);
    }

    vector<Pipe> GetPipes()
    {
        return pipes;
    }
};

int main()
{
    unsigned int t = GetTickCount64();
    srand(t);

    RenderWindow window(VideoMode(SWIDTH, SHEIGHT), L"크고♂아름다운♂게이ㅁ");

    Event event;

    Clock clock;

    Bird bird();
    Background back();

    while (window.isOpen()) 
    {
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed:
                window.close();
                break;
            }
        }

        window.clear();

        window.display();
    }
    return 0;
}