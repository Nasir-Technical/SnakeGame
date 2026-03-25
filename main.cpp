#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "My First Game");

    sf::CircleShape circle(50); // radius
    circle.setFillColor(sf::Color::Green);
    circle.setPosition({200, 200});

    float speed = 0.5f; // movement speed

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Movement logic
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            circle.move({-speed, 0});

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            circle.move({speed, 0});

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            circle.move({0, -speed});

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            circle.move({0, speed});

        // Boundary logic
        sf::Vector2f pos = circle.getPosition();

        float radius = circle.getRadius();

        // Left
        if (pos.x < 0)
            circle.setPosition({0, pos.y});

        // Right
        if (pos.x + radius * 2 > 800)
            circle.setPosition({800 - radius * 2, pos.y});

        // Top
        if (pos.y < 0)
            circle.setPosition({pos.x, 0});

        // Bottom
        if (pos.y + radius * 2 > 600)
            circle.setPosition({pos.x, 600 - radius * 2});

        window.clear();
        window.draw(circle); // 👈 draw ho raha hai
        window.display();
    }

    return 0;
}