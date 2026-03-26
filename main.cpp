#include <SFML/Graphics.hpp>
#include <cstdlib> // random
#include <ctime>  // time

int main()
{
    // 🪟 Window Setup
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Snake Game");

    // 🟩 Snake Head (Rectangle Shape)
    sf::RectangleShape snake(sf::Vector2f(20, 20)); // size (width, height)
    snake.setFillColor(sf::Color::Green);
    snake.setPosition({200, 200});

    // 🍎 Food
    sf::RectangleShape food(sf::Vector2f(20, 20));
    food.setFillColor(sf::Color::Red);

    // 🎲 Random seed
    srand(time(0));
    food.setPosition({(rand() % 40) * 20, (rand() % 30) *  20});

    // ➡️ Direction System (Start moving right)
    sf::Vector2f direction(20, 0);

    // ⏱️ Timing System (Auto movement speed control)
    sf::Clock clock;
    float delay = 0.15f; // lower = faster

    // 🎯 Score
    int score = 0;

    while (window.isOpen())
    {
        // ❌ Window Close Event
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // 🎮 Direction Control (ONLY change direction, no movement here)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && direction.x == 0)
            direction = {-20, 0};

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && direction.x == 0)
            direction = {20, 0};

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && direction.y == 0)
            direction = {0, -20};

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && direction.y == 0)
            direction = {0, 20};

        // ⚡ Auto Movement (Snake moves automatically after delay)
        if (clock.getElapsedTime().asSeconds() > delay)
        {
            snake.move(direction);
            clock.restart();
        }

        // 💥 Collision (Snake eats food)
        if (snake.getPosition() == food.getPosition())
        {
            score++; // score increase

            // 🎲 Respawn food at random position
            food.setPosition({(rand() % 40) * 20, (rand() % 30) * 20});
        }

        // 🚧 Boundary System (Keep snake inside screen)
        sf::Vector2f pos = snake.getPosition();
        float size = 20; // snake block size

        // Left boundary
        if (pos.x < 0)
            snake.setPosition({0, pos.y});

        // Right boundary
        if (pos.x + size > 800)
            snake.setPosition({800 - size, pos.y});

        // Top boundary
        if (pos.y < 0)
            snake.setPosition({pos.x, 0});

        // Bottom boundary
        if (pos.y + size > 600)
            snake.setPosition({pos.x, 600 - size});

        // 🖼️ Render Section
        window.clear();
        window.draw(snake);
        window.draw(food);
        window.display();
    }

    return 0;
}