#include "Entities.h"
#include "Game.h"

void LinearMovement::move(Entities& e, int, int h) {
    e.setY(e.y() + (e.type() == EntityType::PLAYER_BULLET ? -e.speed() : e.speed()));
    if (e.y() < 0 || e.y() >= h) e.setAlive(false);
}

void AlienMovement::move(Entities& e, int w, int) {
    int nx = e.x() + int(e.direction()) * e.speed();
    if (nx <= 0)               { e.setX(0);     e.setY(e.y() + 1); e.setDir(Direction::RIGHT); }
    else if (nx >= w - 1)      { e.setX(w - 1); e.setY(e.y() + 1); e.setDir(Direction::LEFT);  }
    else                       { e.setX(nx); }
}

Alien::Alien(int x, int y) : Entities(x, y, 'W', EntityType::ALIEN) {
    setDir(Direction::RIGHT);
    setMovement(make_unique<AlienMovement>());
}

void Alien::update() {
    if (mv_) mv_->move(*this, GameField::W, GameField::H);
    if (y_ >= GameField::H - 1) alive_ = false;   // дошёл до земли
}

Bullet::Bullet(int x, int y, EntityType t)
    : Entities(x, y, t == EntityType::PLAYER_BULLET ? '|' : '!', t) {
    setMovement(make_unique<LinearMovement>());
}

void Bullet::update() {
    if (mv_) mv_->move(*this, GameField::W, GameField::H);
}

unique_ptr<Entities> EntityFactory::createPlayer(int x, int y)          { return make_unique<Player>(x, y); }
unique_ptr<Entities> EntityFactory::createAlien(int x, int y)           { return make_unique<Alien>(x, y); }
unique_ptr<Entities> EntityFactory::createBullet(int x, int y, EntityType t) { return make_unique<Bullet>(x, y, t); }
