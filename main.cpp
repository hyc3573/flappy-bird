#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <deque>
#include "texture.hpp"

#define SWIDTH 800
#define SHEIGHT 800

#define INITSPD 700

#define XMARGIN 50
#define GRAVITY 400

#define PIPEGAP 70

#define PUSHF -160

#define PIPEHRANGE 400 

#define PIPEINTERVAL 1000

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

    void Update(float dt, bool pushed)
    {
        acceleration = gravity;

        if (pushed)
        {
            speed = PUSHF;
        }

        speed += acceleration * dt;
        move(0, speed * dt);
    }
};

class Pipe
{
private:
    float X;
public:
    Pipe(float X, float Y, float gap, Texture& texture, IntRect rect) : X(X)
    {
        upper = Sprite(texture, IntRect(rect.left, rect.top + rect.height, rect.width, -rect.height));
        lower = Sprite(texture, rect);
        
        upper.setPosition(X, Y - gap / 2 - rect.height);
        lower.setPosition(X, Y + gap / 2);
    }
    void Update(float dt, float speed)
    {
        upper.move(-speed * dt, 0);
        lower.move(-speed * dt, 0);
    }

    float getX()
    {
        return upper.getPosition().x;
    }

    Sprite upper;
    Sprite lower;
};

class Background
{
private:
    deque<Pipe> pipes;
    Sprite background1;
    Sprite background2;
    float speed;
    float pipeInterval;
    Texture& texture;
    IntRect rect;

    float randY()
    {
        return rand() % PIPEHRANGE + SHEIGHT / 2 - PIPEHRANGE/2;
    }

public:
    Background(Texture& backTexture, Texture& texture, IntRect backRect, IntRect rect, float speed, float pipeInterval) : background1(Sprite(backTexture, backRect)), 
        background2(Sprite(backTexture, backRect)), pipeInterval(pipeInterval), texture(texture), rect(rect), speed(speed)
    {
        background1.setPosition(0, 0);

        background2.setPosition(0 + SWIDTH, 0);

        pipes = deque<Pipe>(1, Pipe(SWIDTH, randY(), PIPEGAP, texture, rect));
    }
    Background& operator=(const Background &rhs)
    {
        pipes = deque<Pipe>(rhs.pipes);
        background1 = rhs.background1;
        background2 = rhs.background2;
        speed = rhs.speed;
        pipeInterval = rhs.pipeInterval;
        texture = rhs.texture;
        rect = rhs.rect;
        return *this;
    }

    void Update(float dt)
    {
        background1.move(-speed * dt, 0);
        background2.move(-speed * dt, 0);

        for (auto& elem : pipes)
        {
            elem.Update(dt, speed);
        }

        if (pipes.empty() || (SHEIGHT - pipes.back().getX() >= pipeInterval))
        {
            pipes.push_back(Pipe(SWIDTH, randY(), PIPEGAP, texture, rect));
        }

        if (pipes.front().getX() <= 0)
        {
            pipes.pop_front();
        }

        if (background2.getPosition().x <= 0)
        {
            background1.setPosition(background2.getPosition().x + SWIDTH, 0);
            swap<Sprite>(background1, background2);
        }
    }

    void SetSpeed(float speed)
    {
        speed = speed;
    }

    deque<Pipe> GetPipes()
    {
        return pipes;
    }

    pair<Sprite, Sprite> GetBackground()
    {
        return pair<Sprite, Sprite>(background1, background2);
    }
};

int main()
{
    unsigned int t = GetTickCount64();
    srand(t);

    RenderWindow window(VideoMode(SWIDTH, SHEIGHT), L"크고♂아름다운♂게이ㅁ");

    Event event;

    Clock clock;
    Clock timer;

    Texture texture;
    if (!texture.loadFromMemory(texture_png, texture_png_len))
    {
        cout << "Error!" << endl;
    }

    Bird bird(XMARGIN, SHEIGHT/2, GRAVITY, texture, IntRect(0, 0, 25, 25));
    Background back(texture, texture, IntRect(26, 0, 800, 800), IntRect(0, 26, 25, 1600), INITSPD, PIPEINTERVAL);

    float dt;

    bool push = false;

    auto reset = [&]()
    {
        bird = Bird(XMARGIN, SHEIGHT / 2, GRAVITY, texture, IntRect(0, 0, 25, 25));
        back = Background(texture, texture, IntRect(26, 0, 800, 800), IntRect(0, 26, 25, 1600), INITSPD, PIPEINTERVAL);
        back.SetSpeed(INITSPD);
        timer.restart();
    };

    reset();

    while (window.isOpen()) 
    {
        push = false;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed:
                window.close();
                break;
            case Event::KeyPressed:
                switch (event.key.code)
                {
                case Keyboard::Space:
                    push = true;
                    break;
                }
                break;
            }
        }

        dt = clock.getElapsedTime().asSeconds();
        clock.restart();

        bird.Update(dt, push);
        back.Update(dt);

        for (auto& elem : back.GetPipes())
        {
            if (elem.upper.getGlobalBounds().intersects(bird.getGlobalBounds()) || 
                elem.lower.getGlobalBounds().intersects(bird.getGlobalBounds()))
            {
                reset();
            }
        }

        if (bird.getPosition().y > SHEIGHT || bird.getPosition().y < 0)
        {
            reset();
        }

        window.clear();

        window.draw(back.GetBackground().first);
        window.draw(back.GetBackground().second);

        window.draw(bird);

        for (auto &elem : back.GetPipes())
        {
            window.draw(elem.upper);
            window.draw(elem.lower);
        }

        window.display();
    }
    return 0;
}