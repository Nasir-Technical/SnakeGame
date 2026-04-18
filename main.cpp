#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// Reset Game Function
void resetGame(std::vector<sf::Vector2f> &snake,
               sf::Vector2f &direction,
               sf::RectangleShape &food,
               bool &gameOver,
               bool &grow,
               int &score)
{
    // Reset Snake
    snake.clear();
    snake.push_back({200, 200}); // head

    // Reset Direction
    direction = {20, 0};

    // Reset Game State
    gameOver = false;

    // Reset Grow State
    grow = false;

    // Reset Game Over Sound State
    // gameOverSoundPlayed = false;

    // Reset Score
    score = 0;

    // Respawn food safely
    bool valid = false;
    while (!valid)
    {
        sf::Vector2f newPos = {
            static_cast<float>((rand() % 40) * 20),
            static_cast<float>((rand() % 30) * 20)};

        valid = true;

        for (auto &segment : snake)
        {
            if (segment == newPos)
            {
                valid = false;
                break;
            }
        }

        if (valid)
            food.setPosition(newPos);
    }
}

int main()
{
    // 🪟 Window Setup
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Snake Game");

    // 🟩 Snake body (positions)
    std::vector<sf::Vector2f> snake;
    snake.push_back({200, 200}); // head

    // Score Variable
    int score = 0;

    // GAME OVER SOUND VARIABLE
    bool gameOverSoundPlayed = false;

    sf::Font font;
    if (!font.openFromFile("Super Youth.ttf"))
    {
        return -1; // agar font load fail ho
    }

    sf::Text scoreText(font, "", 24);
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({10, 10});

    // sound effects
    sf::SoundBuffer eatBuffer;
    sf::SoundBuffer gameOverBuffer;

    if (!eatBuffer.loadFromFile("eat.wav") || !gameOverBuffer.loadFromFile("gameover.wav"))
    {
        return -1; // agar sound load fail ho
    }

    // create sound objects
    sf::Sound eatSound(eatBuffer);
    sf::Sound gameOverSound(gameOverBuffer);

    // 🍎 Food Setup
    sf::RectangleShape food(sf::Vector2f(20, 20));
    food.setFillColor(sf::Color::Red);

    srand(time(0));

    // 🎲 Food Spawn Logic (avoid snake body)
    bool valid = false;
    while (!valid)
    {
        sf::Vector2f newPos = {
            static_cast<float>((rand() % 40) * 20),
            static_cast<float>((rand() % 30) * 20)};

        valid = true;

        for (auto &segment : snake)
        {
            if (segment == newPos)
            {
                valid = false;
                break;
            }
        }

        if (valid)
            food.setPosition(newPos);
    }

    // ➡️ Direction System
    sf::Vector2f direction(20, 0);

    // ⏱️ Timing System
    sf::Clock clock;
    float delay = 0.15f;

    // 💀 Game Over System
    bool gameOver = false;

    // 🌱 Grow System (IMPORTANT)
    bool grow = false;

    while (window.isOpen())
    {
        // ❌ Window Close Event
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // 🎮 Input Direction Control (no reverse)
        if (!gameOver)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && direction.x == 0)
                direction = {-20, 0};

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && direction.x == 0)
                direction = {20, 0};

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && direction.y == 0)
                direction = {0, -20};

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && direction.y == 0)
                direction = {0, 20};
        }

        // Restart on R press
        if (gameOver && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
        {
            resetGame(snake, direction, food, gameOver, grow, score);
            gameOverSoundPlayed = false; // Reset game over sound state
        }

        // if (gameOver)
        // {
        //     scoreText.setString("Game Over! Press R to Restart\nScore" + std::to_string(score));
        // }

        // ⚡ Auto Movement System
        if (!gameOver && clock.getElapsedTime().asSeconds() > delay)
        {
            // 🌱 Grow Logic (add new segment before move)
            if (grow)
            {
                snake.push_back(snake.back());
                grow = false;
            }

            // 🧠 Move body from back
            for (int i = snake.size() - 1; i > 0; i--)
            {
                snake[i] = snake[i - 1];
            }

            // 🟩 Move head
            snake[0] += direction;

            clock.restart();
        }

        // 💥 Food Collision System
        if (!gameOver && snake[0] == food.getPosition())
        {
            // 🌱 Activate grow
            grow = true;
            score++;
            eatSound.play();

            // 🎲 Respawn food (safe position)
            bool valid = false;
            while (!valid)
            {
                sf::Vector2f newPos = {
                    static_cast<float>((rand() % 40) * 20),
                    static_cast<float>((rand() % 30) * 20)};

                valid = true;

                for (auto &segment : snake)
                {
                    if (segment == newPos)
                    {
                        valid = false;
                        break;
                    }
                }

                if (valid)
                    food.setPosition(newPos);
            }
        }

        // 💀 Wall Collision System
        float size = 20;
        if (!gameOver && (snake[0].x < 0 || snake[0].x + size > 800 ||
                          snake[0].y < 0 || snake[0].y + size > 600))
        {
            gameOver = true;
        }

        // 💀 Self Collision System
        if (!gameOver)
        {
            for (int i = 1; i < snake.size(); i++)
            {
                if (snake[0] == snake[i])
                {
                    gameOver = true;
                }
            }
        }

        scoreText.setString("Score: " + std::to_string(score));

        if (gameOver && !gameOverSoundPlayed)
        {
            gameOverSound.play();
            gameOverSoundPlayed = true;
        }

        // 🖼️ Render System
        window.clear();
        // Draw Score
        window.draw(scoreText);
        // 🟩 Draw Snake
        for (auto &segment : snake)
        {
            sf::RectangleShape block(sf::Vector2f(20, 20));
            block.setFillColor(sf::Color::Green);
            block.setPosition(segment);
            window.draw(block);
        }

        // 🍎 Draw Food
        window.draw(food);

        // 💀 Game Over UI
        if (gameOver)
        {
            window.setTitle("Game Over!");
        }

        window.display();
    }

    return 0;
}