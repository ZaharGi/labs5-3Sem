#pragma once
#include <memory>

using namespace std;

enum class Direction { LEFT = -1, NONE = 0, RIGHT = 1 };
enum class EntityType { PLAYER, ALIEN, PLAYER_BULLET, ALIEN_BULLET };

class Entities;

//Strategy
class IMovementStrategy {
public:
    virtual ~IMovementStrategy() = default;
    virtual void move(Entities& e, int w, int h) = 0;
};

//Entities
class Entities {
public:
    Entities(int x, int y, char sym, EntityType t, int sp = 1)
        : x_(x), y_(y), symbol_(sym), type_(t), speed_(sp) {}
    virtual ~Entities() = default;

    virtual void update() = 0;

    int  x() const { return x_; }
    int  y() const { return y_; }
    int  speed() const { return speed_; }
    char symbol() const { return symbol_; }
    EntityType type() const { return type_; }
    Direction direction() const { return dir_; }
    bool alive() const { return alive_; }

    void setX(int x) { x_ = x; }
    void setY(int y) { y_ = y; }
    void setDir(Direction d) { dir_ = d; }
    void setAlive(bool a) { alive_ = a; }
    void setMovement(unique_ptr<IMovementStrategy> s) { mv_ = move(s); }

protected:
    int x_, y_;
    char symbol_;
    EntityType type_;
    Direction dir_ = Direction::NONE;
    int speed_;
    bool alive_ = true;
    unique_ptr<IMovementStrategy> mv_;
};

//Strategies
class LinearMovement : public IMovementStrategy {
public:
    void move(Entities& e, int, int h) override;
};

class AlienMovement : public IMovementStrategy {
public:
    void move(Entities& e, int w, int) override;
};

// Concrete entities
class Player : public Entities {
public:
    Player(int x, int y) : Entities(x, y, 'A', EntityType::PLAYER) {}
    void update() override {}
};

class Alien : public Entities {
public:
    Alien(int x, int y);
    void update() override;
};

class Bullet : public Entities {
public:
    Bullet(int x, int y, EntityType t);
    void update() override;
};

//Factory Method
class EntityFactory {
public:
    static unique_ptr<Entities> createPlayer(int x, int y);
    static unique_ptr<Entities> createAlien(int x, int y);
    static unique_ptr<Entities> createBullet(int x, int y, EntityType t);
};
