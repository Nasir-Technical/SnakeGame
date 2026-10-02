#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cstdint>

// ============================================================
// UTILITY
// ============================================================
static float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (max - min));
}

// ============================================================
// PARTICLE SYSTEM
// ============================================================
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    sf::Color color;
};

class ParticleSystem {
public:
    std::vector<Particle> particles;

    void spawnBurst(sf::Vector2f position, int count) {
        particles.reserve(particles.size() + static_cast<size_t>(count));
        for (int i = 0; i < count; ++i) {
            float angle = randomFloat(0.0f, 6.283185f);
            float speed = randomFloat(60.0f, 220.0f);
            Particle p;
            p.position = position;
            p.velocity = { std::cos(angle) * speed, std::sin(angle) * speed };
            p.maxLifetime = randomFloat(0.25f, 0.65f);
            p.lifetime = p.maxLifetime;
            p.color = sf::Color(255, static_cast<std::uint8_t>(randomFloat(180.0f, 255.0f)), 30, 255);
            particles.push_back(p);
        }
    }

    void update(float dt) {
        for (auto it = particles.begin(); it != particles.end(); ) {
            it->lifetime -= dt;
            if (it->lifetime <= 0.0f) {
                it = particles.erase(it);
            } else {
                it->position += it->velocity * dt;
                it->velocity *= 0.94f;
                ++it;
            }
        }
    }

    void draw(sf::RenderWindow& window) const {
        for (const auto& p : particles) {
            float alpha = static_cast<float>(p.lifetime / p.maxLifetime) * 255.0f;
            sf::CircleShape circle(3.0f);
            circle.setFillColor(sf::Color(p.color.r, p.color.g, p.color.b, static_cast<std::uint8_t>(alpha)));
            circle.setPosition({ p.position.x - 3.0f, p.position.y - 3.0f });
            window.draw(circle);
        }
    }

    void clear() { particles.clear(); }
};

// ============================================================
// FOOD
// ============================================================
class Food {
public:
    sf::RectangleShape shape;
    sf::Vector2f position;
    static constexpr float SIZE = 20.0f;

    Food() {
        shape.setSize({ SIZE, SIZE });
        shape.setFillColor(sf::Color::Red);
        position = { 200.0f, 100.0f };
        shape.setPosition(position);
    }

    void spawn(const std::vector<sf::Vector2f>& snake, int width, int height) {
        bool valid = false;
        int cols = width / static_cast<int>(SIZE);
        int rows = height / static_cast<int>(SIZE);
        while (!valid) {
            float x = static_cast<float>((rand() % cols) * static_cast<int>(SIZE));
            float y = static_cast<float>((rand() % rows) * static_cast<int>(SIZE));
            position = { x, y };
            valid = true;
            for (const auto& seg : snake) {
                if (seg == position) {
                    valid = false;
                    break;
                }
            }
        }
        shape.setPosition(position);
    }

    void draw(sf::RenderWindow& window) const {
        sf::CircleShape foodGlow(10.0f);
        foodGlow.setFillColor(sf::Color(255, 50, 50, 80));
        foodGlow.setPosition({ position.x, position.y });
        window.draw(foodGlow);

        sf::CircleShape apple(6.0f);
        apple.setFillColor(sf::Color::Red);
        apple.setPosition({ position.x + 4.0f, position.y + 4.0f });
        window.draw(apple);
    }
};

// ============================================================
// SNAKE
// ============================================================
class Snake {
public:
    std::vector<sf::Vector2f> segments;
    sf::Vector2f direction;
    bool grow;
    static constexpr float SIZE = 20.0f;
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    Snake() {
        segments.push_back({ 200.0f, 200.0f });
        direction = { SIZE, 0.0f };
        grow = false;
    }

    void reset() {
        segments.clear();
        segments.push_back({ 200.0f, 200.0f });
        direction = { SIZE, 0.0f };
        grow = false;
    }

    void update() {
        if (grow) {
            segments.push_back(segments.back());
            grow = false;
        }
        for (int i = static_cast<int>(segments.size()) - 1; i > 0; --i) {
            segments[i] = segments[i - 1];
        }
        segments[0] += direction;
    }

    bool checkWallCollision() const {
        return segments[0].x < 0 || segments[0].x >= WIDTH ||
               segments[0].y < 0 || segments[0].y >= HEIGHT;
    }

    bool checkSelfCollision() const {
        for (size_t i = 1; i < segments.size(); ++i) {
            if (segments[0] == segments[i]) return true;
        }
        return false;
    }

    bool checkFoodCollision(const sf::Vector2f& foodPos) const {
        return segments[0] == foodPos;
    }

    void draw(sf::RenderWindow& window) const {
        for (size_t i = 0; i < segments.size(); ++i) {
            const auto& seg = segments[i];

            sf::CircleShape glow(12.0f);
            glow.setFillColor(sf::Color(0, 255, 150, 60));
            glow.setPosition({ seg.x - 2.0f, seg.y - 2.0f });
            window.draw(glow);

            if (i == 0) {
                sf::RectangleShape head({ SIZE, SIZE });
                head.setFillColor(sf::Color(0, 255, 180));
                head.setPosition(seg);
                window.draw(head);

                sf::CircleShape eye(2.0f);
                eye.setFillColor(sf::Color::Black);
                if (direction.x > 0) {
                    eye.setPosition({ seg.x + 12.0f, seg.y + 5.0f }); window.draw(eye);
                    eye.setPosition({ seg.x + 12.0f, seg.y + 12.0f }); window.draw(eye);
                } else if (direction.x < 0) {
                    eye.setPosition({ seg.x + 4.0f, seg.y + 5.0f }); window.draw(eye);
                    eye.setPosition({ seg.x + 4.0f, seg.y + 12.0f }); window.draw(eye);
                } else if (direction.y > 0) {
                    eye.setPosition({ seg.x + 5.0f, seg.y + 12.0f }); window.draw(eye);
                    eye.setPosition({ seg.x + 12.0f, seg.y + 12.0f }); window.draw(eye);
                } else if (direction.y < 0) {
                    eye.setPosition({ seg.x + 5.0f, seg.y + 4.0f }); window.draw(eye);
                    eye.setPosition({ seg.x + 12.0f, seg.y + 4.0f }); window.draw(eye);
                }
            } else {
                sf::RectangleShape block({ 18.0f, 18.0f });
                block.setFillColor(sf::Color(0, 255, 150));
                block.setPosition({ seg.x + 1.0f, seg.y + 1.0f });
                window.draw(block);
            }
        }
    }
};

// ============================================================
// GAME
// ============================================================
class Game {
public:
    enum class State { MENU, PLAYING, PAUSED, GAME_OVER };

    sf::RenderWindow window;
    sf::Font font;
    sf::SoundBuffer eatBuffer, gameOverBuffer;
    sf::Sound eatSound, gameOverSound;
    sf::Text title, playText, exitText, pauseText, resumeText, gameOverText, restartText;
    sf::Text scoreText, highScoreText;

    Snake snake;
    Food food;
    ParticleSystem particles;

    State state;
    int score;
    int highScore;
    bool gameOverSoundPlayed;

    sf::Clock tickClock;
    float tickDelay;

    Game()
        : window(sf::VideoMode({ 800, 600 }), "Snake Game")
        , font()
        , eatBuffer(), gameOverBuffer()
        , eatSound(eatBuffer), gameOverSound(gameOverBuffer)
        , title(font, "SNAKE GAME", 40)
        , playText(font, "ENTER = PLAY", 24)
        , exitText(font, "ESC = EXIT", 24)
        , pauseText(font, "PAUSED", 40)
        , resumeText(font, "P = RESUME | ESC = MENU", 24)
        , gameOverText(font, "GAME OVER", 40)
        , restartText(font, "R = RESTART | ESC = MENU", 24)
        , scoreText(font, "", 24)
        , highScoreText(font, "", 24)
        , snake()
        , food()
        , particles()
        , state(State::MENU)
        , score(0)
        , highScore(0)
        , gameOverSoundPlayed(false)
        , tickDelay(0.15f) {
        srand(static_cast<unsigned>(time(nullptr)));

        if (!font.openFromFile("Super Youth.ttf")) {
        }

        if (!eatBuffer.loadFromFile("eat.wav")) {
        }
        if (!gameOverBuffer.loadFromFile("gameover.wav")) {
        }

        centerText(title, 400.0f, 150.0f);
        centerText(playText, 400.0f, 250.0f);
        centerText(exitText, 400.0f, 300.0f);
        centerText(pauseText, 400.0f, 220.0f);
        centerText(resumeText, 400.0f, 300.0f);
        centerText(gameOverText, 400.0f, 200.0f);
        centerText(restartText, 400.0f, 300.0f);
        scoreText.setPosition({ 10.0f, 10.0f });

        loadHighScore();
        reset();
    }

    void centerText(sf::Text& text, float x, float y) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                         bounds.position.y + bounds.size.y / 2.0f });
        text.setPosition({ x, y });
    }

    void loadHighScore() {
        std::ifstream file("highscore.txt");
        if (file.is_open()) {
            file >> highScore;
            file.close();
        }
    }

    void saveHighScore() {
        if (score > highScore) {
            highScore = score;
            std::ofstream file("highscore.txt");
            if (file.is_open()) {
                file << highScore;
                file.close();
            }
        }
    }

    void reset() {
        snake.reset();
        food.spawn(snake.segments, 800, 600);
        particles.clear();
        score = 0;
        gameOverSoundPlayed = false;
        tickDelay = 0.15f;
    }

    void handleEvent(const sf::Event& event) {
        if (event.is<sf::Event::Closed>()) {
            saveHighScore();
            window.close();
        }

        if (event.is<sf::Event::KeyPressed>()) {
            auto key = event.getIf<sf::Event::KeyPressed>()->code;

            switch (state) {
                case State::MENU:
                    if (key == sf::Keyboard::Key::Enter) {
                        reset();
                        state = State::PLAYING;
                    } else if (key == sf::Keyboard::Key::Escape) {
                        saveHighScore();
                        window.close();
                    }
                    break;

                case State::PLAYING:
                    if (key == sf::Keyboard::Key::Escape) {
                        state = State::MENU;
                    } else if (key == sf::Keyboard::Key::P) {
                        state = State::PAUSED;
                    } else {
                        if (key == sf::Keyboard::Key::Left && snake.direction.x == 0)
                            snake.direction = { -Snake::SIZE, 0.0f };
                        else if (key == sf::Keyboard::Key::Right && snake.direction.x == 0)
                            snake.direction = { Snake::SIZE, 0.0f };
                        else if (key == sf::Keyboard::Key::Up && snake.direction.y == 0)
                            snake.direction = { 0.0f, -Snake::SIZE };
                        else if (key == sf::Keyboard::Key::Down && snake.direction.y == 0)
                            snake.direction = { 0.0f, Snake::SIZE };
                    }
                    break;

                case State::PAUSED:
                    if (key == sf::Keyboard::Key::P)
                        state = State::PLAYING;
                    else if (key == sf::Keyboard::Key::Escape)
                        state = State::MENU;
                    break;

                case State::GAME_OVER:
                    if (key == sf::Keyboard::Key::R) {
                        reset();
                        state = State::PLAYING;
                    } else if (key == sf::Keyboard::Key::Escape) {
                        state = State::MENU;
                    }
                    break;
            }
        }
    }

    void update(float dt) {
        if (state == State::PLAYING && tickClock.getElapsedTime().asSeconds() > tickDelay) {
            snake.update();

            if (snake.checkWallCollision() || snake.checkSelfCollision()) {
                state = State::GAME_OVER;
                saveHighScore();
            }

            if (snake.checkFoodCollision(food.position)) {
                snake.grow = true;
                ++score;
                eatSound.play();
                particles.spawnBurst(food.position, 20);
                food.spawn(snake.segments, 800, 600);
            }

            tickClock.restart();
        }

        if (state == State::GAME_OVER && !gameOverSoundPlayed) {
            gameOverSound.play();
            gameOverSoundPlayed = true;
        }

        particles.update(dt);
    }

    void drawBackground() {
        for (int x = 0; x < 800; x += 20) {
            for (int y = 0; y < 600; y += 20) {
                sf::RectangleShape cell(sf::Vector2f(20.0f, 20.0f));
                cell.setPosition({ static_cast<float>(x), static_cast<float>(y) });
                cell.setFillColor(sf::Color(20, 20, 20));
                cell.setOutlineThickness(1);
                cell.setOutlineColor(sf::Color(35, 35, 35));
                window.draw(cell);
            }
        }
    }

    void drawUI() {
        sf::RectangleShape topBar(sf::Vector2f(800.0f, 40.0f));
        topBar.setFillColor(sf::Color(20, 20, 20, 200));
        window.draw(topBar);

        sf::RectangleShape panel(sf::Vector2f(760.0f, 560.0f));
        panel.setPosition({ 20.0f, 20.0f });
        panel.setFillColor(sf::Color(25, 25, 25, 180));
        panel.setOutlineThickness(2);
        panel.setOutlineColor(sf::Color(80, 80, 80));
        window.draw(panel);
    }

    void render() {
        window.clear(sf::Color(15, 15, 15));

        drawBackground();
        drawUI();

        switch (state) {
            case State::MENU:
                window.draw(title);

                highScoreText.setString("HIGH SCORE: " + std::to_string(highScore));
                highScoreText.setFillColor(sf::Color::Yellow);
                centerText(highScoreText, 400.0f, 350.0f);
                window.draw(highScoreText);

                window.draw(playText);
                window.draw(exitText);
                break;

            case State::PLAYING:
                scoreText.setString("Score: " + std::to_string(score));
                scoreText.setPosition({ 20.0f, 8.0f });
                scoreText.setFillColor(sf::Color::White);
                window.draw(scoreText);

                snake.draw(window);
                food.draw(window);
                particles.draw(window);
                break;

            case State::PAUSED:
                scoreText.setString("Score: " + std::to_string(score));
                scoreText.setPosition({ 20.0f, 8.0f });
                scoreText.setFillColor(sf::Color::White);
                window.draw(scoreText);

                window.draw(pauseText);
                window.draw(resumeText);
                break;

            case State::GAME_OVER: {
                std::string detailStr = "Score: " + std::to_string(score) + "    High Score: " + std::to_string(highScore);
                sf::Text detail(font, detailStr, 24);
                detail.setFillColor(sf::Color::Yellow);
                centerText(detail, 400.0f, 240.0f);
                window.draw(detail);

                window.draw(gameOverText);
                window.draw(restartText);
                break;
            }
        }

        window.display();
    }

    void run() {
        sf::Clock deltaClock;
        while (window.isOpen()) {
            while (const std::optional<sf::Event> event = window.pollEvent()) {
                handleEvent(*event);
            }

            float dt = deltaClock.restart().asSeconds();
            update(dt);
            render();
        }
    }
};

// ============================================================
// MAIN
// ============================================================
int main() {
    Game game;
    game.run();
    return 0;
}
