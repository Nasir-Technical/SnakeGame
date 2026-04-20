#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// 🔁 Reset Game Function
void resetGame(std::vector<sf::Vector2f> &snake,
               sf::Vector2f &direction,
               sf::RectangleShape &food,
               bool &gameOver,
               bool &grow,
               int &score)
{
    // 🟩 Reset Snake
    snake.clear();
    snake.push_back({200, 200});

    // ➡️ Reset Direction
    direction = {20, 0};

    // 💀 Reset Game State
    gameOver = false;

    // 🌱 Reset Grow
    grow = false;

    // 🎯 Reset Score
    score = 0;

    // 🍎 Respawn Food (safe)
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

    srand(time(0));

    // 🎮 Game State System
    enum GameState
    {
        MENU,
        PLAYING,
        GAME_OVER
    };

    GameState gameState = MENU;

    // 🟩 Snake Data
    std::vector<sf::Vector2f> snake = {{200, 200}};

    // ➡️ Direction
    sf::Vector2f direction(20, 0);

    // 🍎 Food
    sf::RectangleShape food(sf::Vector2f(20, 20));
    food.setFillColor(sf::Color::Red);

    // 🎲 Initial Food Spawn
    food.setPosition({200, 100});

    // ⏱️ Timing
    sf::Clock clock;
    float delay = 0.15f;

    // 💀 Game Flags
    bool gameOver = false;
    bool grow = false;
    bool gameOverSoundPlayed = false;

    // 🎯 Score
    int score = 0;

    // 🔤 Font
    sf::Font font;
    if (!font.openFromFile("Super Youth.ttf"))
        return -1;

    // 🧾 UI Texts
    sf::Text scoreText(font, "", 24);
    scoreText.setPosition({10, 10});

    sf::Text title(font, "SNAKE GAME", 40);
    title.setPosition({220, 150});

    sf::Text playText(font, "Press ENTER to Play", 24);
    playText.setPosition({240, 250});

    sf::Text exitText(font, "Press ESC to Exit", 24);
    exitText.setPosition({250, 300});

    sf::Text gameOverText(font, "GAME OVER", 40);
    gameOverText.setPosition({250, 200});

    sf::Text restartText(font, "Press R to Restart", 24);
    restartText.setPosition({250, 300});

    // 🔊 Sounds
    sf::SoundBuffer eatBuffer, gameOverBuffer;
    eatBuffer.loadFromFile("eat.wav");
    gameOverBuffer.loadFromFile("gameover.wav");

    sf::Sound eatSound(eatBuffer);
    sf::Sound gameOverSound(gameOverBuffer);

    // 🔁 GAME LOOP
    while (window.isOpen())
    {
        // ❌ Window Events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // =========================
        // 🎮 MENU INPUT SYSTEM
        // =========================
        if (gameState == MENU)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
            {
                gameState = PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            {
                window.close();
            }
        }

        // =========================
        // 🔁 GAMEPLAY SYSTEM
        // =========================
        if (gameState == PLAYING)
        {
            // 🎮 Direction Input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && direction.x == 0)
                direction = {-20, 0};

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && direction.x == 0)
                direction = {20, 0};

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && direction.y == 0)
                direction = {0, -20};

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && direction.y == 0)
                direction = {0, 20};

            // ⚡ Movement
            if (clock.getElapsedTime().asSeconds() > delay)
            {
                // 🌱 Grow Logic
                if (grow)
                {
                    snake.push_back(snake.back());
                    grow = false;
                }

                // 🧠 Move Body
                for (int i = snake.size() - 1; i > 0; i--)
                    snake[i] = snake[i - 1];

                // 🟩 Move Head
                snake[0] += direction;

                clock.restart();
            }

            // 💥 Food Collision
            if (snake[0] == food.getPosition())
            {
                grow = true;
                score++;
                eatSound.play();

                // 🍎 Respawn Food
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

            // 💀 Wall Collision
            if (snake[0].x < 0 || snake[0].x >= 800 ||
                snake[0].y < 0 || snake[0].y >= 600)
            {
                gameState = GAME_OVER;
                gameOver = true;
            }

            // 💀 Self Collision
            for (int i = 1; i < snake.size(); i++)
            {
                if (snake[0] == snake[i])
                {
                    gameState = GAME_OVER;
                    gameOver = true;
                }
            }
        }

        // =========================
        // 💀 GAME OVER SYSTEM
        // =========================
        if (gameState == GAME_OVER)
        {
            if (!gameOverSoundPlayed)
            {
                gameOverSound.play();
                gameOverSoundPlayed = true;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
            {
                resetGame(snake, direction, food, gameOver, grow, score);
                gameState = PLAYING;
                gameOverSoundPlayed = false;
            }
        }

        // 🎯 Score Update
        scoreText.setString("Score: " + std::to_string(score));

        // =========================
        // 🖼️ RENDER SYSTEM
        // =========================
        window.clear();

        // 📜 MENU SCREEN
        if (gameState == MENU)
        {
            window.draw(title);
            window.draw(playText);
            window.draw(exitText);
        }

        // 🐍 GAMEPLAY SCREEN
        else if (gameState == PLAYING)
        {
            window.draw(scoreText);

            for (auto &segment : snake)
            {
                sf::RectangleShape block(sf::Vector2f(20, 20));
                block.setFillColor(sf::Color::Green);
                block.setPosition(segment);
                window.draw(block);
            }

            window.draw(food);
        }

        // 💀 GAME OVER SCREEN
        else if (gameState == GAME_OVER)
        {
            window.draw(gameOverText);
            window.draw(restartText);
        }

        window.display();
    }

    return 0;
}