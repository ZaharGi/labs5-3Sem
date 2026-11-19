#pragma once
#include <list>
#include <memory>
#include "Entities.h"
#include "Observer.h"

using namespace std;

class GameField {
public:
    static constexpr int W = 20;
    static constexpr int H = 15;
};

class Game : public Subject {
public:
    Game();
    ~Game() override;
    void run();

private:
    void input();
    void update();
    void draw();
    void spawnAliens();
    void aliensShoot();
    void handleCollisions();
    void changeLives(int delta, const string& reason);

    list<Entities*> entities_;
    Entities* player_ = nullptr;
    int lives_ = 3, score_ = 0, turn_ = 0;
    bool gameOn_ = true;
};
