#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// 🎯 Center Align Function
void centerText(sf::Text &text, float x, float y)
{
    sf::FloatRect bounds = text.getLocalBounds();

    text.setOrigin({bounds.position.x + bounds.size.x / 2,
                    bounds.position.y + bounds.size.y / 2});

    text.setPosition({x, y});
}

// 🔁 Reset Game Function
void resetGame(std::vector<sf::Vector2f> &snake,
               sf::Vector2f &direction,
               sf::RectangleShape &food,
               bool &grow,
               int &score)
{
    snake.clear();
    snake.push_back({200, 200});
    direction = {20, 0};
    grow = false;
    score = 0;

    // 🍎 Safe food spawn
    bool valid = false;
    while (!valid)
    {
        sf::Vector2f newPos = {
            static_cast<float>((rand() % 40) * 20),
            static_cast<float>((rand() % 30) * 20)};

        valid = true;
        for (auto &seg : snake)
        {
            if (seg == newPos)
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
    // 🪟 Window
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Snake Game");
    srand(time(0));

    // 🎮 GAME STATES
    enum GameState
    {
        MENU,
        PLAYING,
        PAUSED,
        GAME_OVER
    };

    GameState state = MENU;

    // 🟩 Snake
    std::vector<sf::Vector2f> snake = {{200, 200}};
    sf::Vector2f direction(20, 0);

    // 🍎 Food
    sf::RectangleShape food(sf::Vector2f(20, 20));
    food.setFillColor(sf::Color::Red);
    food.setPosition({200, 100});

    // ⏱️ Timing
    sf::Clock clock;
    float delay = 0.15f;

    // 🎯 Score + flags
    int score = 0;
    bool grow = false;
    bool gameOverSoundPlayed = false;

    // 🔤 Font
    sf::Font font;
    if (!font.openFromFile("Super Youth.ttf"))
        return -1;

    // =========================
    // 🧾 UI TEXTS (CENTERED)
    // =========================
    sf::Text title(font, "SNAKE GAME", 40);
    centerText(title, 400, 150);

    sf::Text playText(font, "ENTER = PLAY", 24);
    centerText(playText, 400, 250);

    sf::Text exitText(font, "ESC = EXIT", 24);
    centerText(exitText, 400, 300);

    sf::Text pauseText(font, "PAUSED", 40);
    centerText(pauseText, 400, 220);

    sf::Text resumeText(font, "P = RESUME | ESC = MENU", 24);
    centerText(resumeText, 400, 300);

    sf::Text gameOverText(font, "GAME OVER", 40);
    centerText(gameOverText, 400, 200);

    sf::Text restartText(font, "R = RESTART | ESC = MENU", 24);
    centerText(restartText, 400, 300);

    sf::Text scoreText(font, "", 24);
    scoreText.setPosition({10, 10});

    // 🔊 Sounds
    sf::SoundBuffer eatBuffer, gameOverBuffer;
    eatBuffer.loadFromFile("eat.wav");
    gameOverBuffer.loadFromFile("gameover.wav");

    sf::Sound eatSound(eatBuffer);
    sf::Sound gameOverSound(gameOverBuffer);

    // =========================
    // 🔁 GAME LOOP
    // =========================
    while (window.isOpen())
    {
        // =========================
        // 🎮 INPUT SYSTEM (ALL KEYS HERE)
        // =========================
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::KeyPressed>())
            {
                auto key = event->getIf<sf::Event::KeyPressed>()->code;

                // ===== MENU =====
                if (state == MENU)
                {
                    if (key == sf::Keyboard::Key::Enter)
                        state = PLAYING;

                    else if (key == sf::Keyboard::Key::Escape)
                        window.close();
                }

                // ===== PLAYING =====
                else if (state == PLAYING)
                {
                    if (key == sf::Keyboard::Key::Escape)
                        state = MENU;

                    else if (key == sf::Keyboard::Key::P)
                        state = PAUSED;

                    // Direction
                    if (key == sf::Keyboard::Key::Left && direction.x == 0)
                        direction = {-20, 0};

                    if (key == sf::Keyboard::Key::Right && direction.x == 0)
                        direction = {20, 0};

                    if (key == sf::Keyboard::Key::Up && direction.y == 0)
                        direction = {0, -20};

                    if (key == sf::Keyboard::Key::Down && direction.y == 0)
                        direction = {0, 20};
                }

                // ===== PAUSED =====
                else if (state == PAUSED)
                {
                    if (key == sf::Keyboard::Key::P)
                        state = PLAYING;

                    else if (key == sf::Keyboard::Key::Escape)
                        state = MENU;
                }

                // ===== GAME OVER =====
                else if (state == GAME_OVER)
                {
                    if (key == sf::Keyboard::Key::R)
                    {
                        resetGame(snake, direction, food, grow, score);
                        state = PLAYING;
                        gameOverSoundPlayed = false;
                    }

                    else if (key == sf::Keyboard::Key::Escape)
                        state = MENU;
                }
            }
        }

        // =========================
        // ⚡ GAMEPLAY LOGIC
        // =========================
        if (state == PLAYING && clock.getElapsedTime().asSeconds() > delay)
        {
            // Grow
            if (grow)
            {
                snake.push_back(snake.back());
                grow = false;
            }

            // Move body
            for (int i = snake.size() - 1; i > 0; i--)
                snake[i] = snake[i - 1];

            // Move head
            snake[0] += direction;
            clock.restart();

            // Food collision
            if (snake[0] == food.getPosition())
            {
                grow = true;
                score++;
                eatSound.play();

                food.setPosition({static_cast<float>((rand() % 40) * 20),
                                  static_cast<float>((rand() % 30) * 20)});
            }

            // Wall collision
            if (snake[0].x < 0 || snake[0].x >= 800 ||
                snake[0].y < 0 || snake[0].y >= 600)
            {
                state = GAME_OVER;
            }

            // Self collision
            for (int i = 1; i < snake.size(); i++)
            {
                if (snake[0] == snake[i])
                    state = GAME_OVER;
            }
        }

        // 💀 Game Over Sound
        if (state == GAME_OVER && !gameOverSoundPlayed)
        {
            gameOverSound.play();
            gameOverSoundPlayed = true;
        }

        // 🎯 Score update
        scoreText.setString("Score: " + std::to_string(score));

        // =========================
        // 🖼️ RENDER SYSTEM (FULL UI)
        // =========================
        window.clear(sf::Color(15, 15, 15));

        // =========================
        // 🧱 GRID BACKGROUND
        // =========================
        for (int x = 0; x < 800; x += 20)
        {
            for (int y = 0; y < 600; y += 20)
            {
                sf::RectangleShape cell(sf::Vector2f(20, 20));
                cell.setPosition({(float)x, (float)y});
                cell.setFillColor(sf::Color(20, 20, 20));
                cell.setOutlineThickness(1);
                cell.setOutlineColor(sf::Color(35, 35, 35));
                window.draw(cell);
            }
        }

        // =========================
        // 🟦 TOP BAR (GLASS)
        // =========================
        sf::RectangleShape topBar(sf::Vector2f(800, 40));
        topBar.setFillColor(sf::Color(20, 20, 20, 200));
        window.draw(topBar);

        // =========================
        // 🟫 GAME PANEL
        // =========================
        sf::RectangleShape panel(sf::Vector2f(760, 560));
        panel.setPosition({20, 20});
        panel.setFillColor(sf::Color(25, 25, 25, 180));
        panel.setOutlineThickness(2);
        panel.setOutlineColor(sf::Color(80, 80, 80));
        window.draw(panel);

        // =========================
        // 🎮 STATE RENDERING
        // =========================

        // ========= MENU =========
        if (state == MENU)
        {
            window.draw(title);
            window.draw(playText);
            window.draw(exitText);
        }

        // ========= PLAYING =========
        else if (state == PLAYING)
        {
            // 📊 SCORE
            scoreText.setPosition({20, 8});
            scoreText.setFillColor(sf::Color::White);
            window.draw(scoreText);

            // =========================
            // 🐍 SNAKE (WITH HEAD + GLOW)
            // =========================
            for (int i = 0; i < snake.size(); i++)
            {
                auto &seg = snake[i];

                // 🔥 Glow
                sf::CircleShape glow(12);
                glow.setFillColor(sf::Color(0, 255, 150, 60));
                glow.setPosition({seg.x - 2, seg.y - 2});
                window.draw(glow);

                // 🧠 HEAD
                if (i == 0)
                {
                    sf::RectangleShape head(sf::Vector2f(20, 20));
                    head.setFillColor(sf::Color(0, 255, 180));
                    head.setPosition(seg);
                    window.draw(head);

                    // 👀 Eyes
                    sf::CircleShape eye(2);
                    eye.setFillColor(sf::Color::Black);

                    if (direction.x > 0) // RIGHT
                    {
                        eye.setPosition({seg.x + 12, seg.y + 5});
                        window.draw(eye);
                        eye.setPosition({seg.x + 12, seg.y + 12});
                        window.draw(eye);
                    }
                    else if (direction.x < 0) // LEFT
                    {
                        eye.setPosition({seg.x + 4, seg.y + 5});
                        window.draw(eye);
                        eye.setPosition({seg.x + 4, seg.y + 12});
                        window.draw(eye);
                    }
                    else if (direction.y > 0) // DOWN
                    {
                        eye.setPosition({seg.x + 5, seg.y + 12});
                        window.draw(eye);
                        eye.setPosition({seg.x + 12, seg.y + 12});
                        window.draw(eye);
                    }
                    else if (direction.y < 0) // UP
                    {
                        eye.setPosition({seg.x + 5, seg.y + 4});
                        window.draw(eye);
                        eye.setPosition({seg.x + 12, seg.y + 4});
                        window.draw(eye);
                    }
                }
                else
                {
                    // 🟩 BODY
                    sf::RectangleShape block(sf::Vector2f(18, 18));
                    block.setFillColor(sf::Color(0, 255, 150));
                    block.setPosition({seg.x + 1, seg.y + 1});
                    window.draw(block);
                }
            }

            // =========================
            // 🍎 FOOD (GLOW)
            // =========================
            sf::CircleShape foodGlow(10);
            foodGlow.setFillColor(sf::Color(255, 50, 50, 80));
            foodGlow.setPosition({food.getPosition().x, food.getPosition().y});
            window.draw(foodGlow);

            sf::CircleShape apple(6);
            apple.setFillColor(sf::Color::Red);
            apple.setPosition({food.getPosition().x + 4, food.getPosition().y + 4});
            window.draw(apple);
        }

        // ========= PAUSED =========
        else if (state == PAUSED)
        {
            window.draw(pauseText);
            window.draw(resumeText);
        }

        // ========= GAME OVER =========
        else if (state == GAME_OVER)
        {
            window.draw(gameOverText);
            window.draw(restartText);
        }

        // =========================
        // 🖥️ DISPLAY
        // =========================
        window.display();
    }

        return 0;
    }