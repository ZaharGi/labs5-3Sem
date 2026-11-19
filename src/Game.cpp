#include "Game.h"
#include "Entities.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <termios.h>
#include <unistd.h>

// Читает одну клавишу без Enter (Linux).
// 'a' — влево, 'd' — вправо, ' ' — выстрел, 'q' — выход, 0 — прочее
static char readKey() {
    termios oldt{}, newt{};
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char c = 0;
    if (read(STDIN_FILENO, &c, 1) == 1 && c == '\033') {
        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) == 1 &&
            read(STDIN_FILENO, &seq[1], 1) == 1) {
            if (seq[1] == 'D') c = 'a';       // Left
            else if (seq[1] == 'C') c = 'd';  // Right
            else c = 0;
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
}

Game::Game() {
    player_ = EntityFactory::createPlayer(GameField::W / 2, GameField::H - 1).release();
    entities_.push_back(player_);
    spawnAliens();
    attach(make_unique<ConsoleHUD>());
    notify(lives_, score_, "Start");
}

Game::~Game() {
    for (auto* e : entities_) delete e;
}

void Game::spawnAliens() {
    for (int r = 0; r < 2; ++r)
        for (int c = 2; c < GameField::W - 2; c += 3)
            entities_.push_back(EntityFactory::createAlien(c, r + 1).release());
}

void Game::run() {
    while (gameOn_) { draw(); input(); update(); }
    cout << "\n=== GAME OVER === Score: " << score_ << "\n";
}

void Game::input() {
    cout << "\n[<-][->] move  [space] shoot  [q] quit > ";
    cout.flush();

    char c = readKey();
    if (c == 'a') {
        player_->setX(max(0, player_->x() - 1));
    } else if (c == 'd') {
        player_->setX(min(GameField::W - 1, player_->x() + 1));
    } else if (c == ' ') {
        entities_.push_back(EntityFactory::createBullet(
            player_->x(), player_->y() - 1, EntityType::PLAYER_BULLET).release());
    } else if (c == 'q') {
        gameOn_ = false;
    }
}

void Game::update() {
    ++turn_;
    for (auto* e : entities_) if (e->alive()) e->update();
    aliensShoot();
    handleCollisions();

    for (auto it = entities_.begin(); it != entities_.end(); ) {
        if (!(*it)->alive()) { delete *it; it = entities_.erase(it); }
        else ++it;
    }
    if (player_ && !player_->alive()) { player_ = nullptr; gameOn_ = false; }
}

void Game::aliensShoot() {
    if (turn_ % 2) return;
    vector<Entities*> aliens;
    for (auto* e : entities_)
        if (e->alive() && e->type() == EntityType::ALIEN) aliens.push_back(e);
    if (aliens.empty()) return;
    Entities* a = aliens[rand() % aliens.size()];
    entities_.push_back(EntityFactory::createBullet(a->x(), a->y() + 1,
                                                    EntityType::ALIEN_BULLET).release());
}

void Game::handleCollisions() {
    for (auto* b : entities_) {
        if (!b->alive() || b->type() != EntityType::PLAYER_BULLET) continue;
        for (auto* a : entities_) {
            if (a->alive() && a->type() == EntityType::ALIEN &&
                a->x() == b->x() && a->y() == b->y()) {
                a->setAlive(false); b->setAlive(false);
                score_ += 10; notify(lives_, score_, "Alien destroyed");
                break;
            }
        }
    }
    for (auto* b : entities_) {
        if (b->alive() && b->type() == EntityType::ALIEN_BULLET &&
            player_ && b->x() == player_->x() && b->y() == player_->y()) {
            b->setAlive(false);
            changeLives(-1, "You were shot!");
        }
    }
    for (auto* a : entities_)
        if (!a->alive() && a->type() == EntityType::ALIEN && a->y() >= GameField::H - 1)
            changeLives(-1, "Alien reached the ground!");
    for (auto* a : entities_)
        if (a->alive() && a->type() == EntityType::ALIEN && player_ &&
            a->x() == player_->x() && a->y() == player_->y()) {
            a->setAlive(false);
            changeLives(-1, "Alien collided with you!");
        }
}

void Game::changeLives(int delta, const string& reason) {
    lives_ += delta;
    notify(lives_, score_, reason);
    if (lives_ <= 0) gameOn_ = false;
}

void Game::draw() {
    vector<string> grid(GameField::H, string(GameField::W, '.'));
    for (auto* e : entities_) {
        if (!e->alive()) continue;
        int x = e->x(), y = e->y();
        if (x >= 0 && x < GameField::W && y >= 0 && y < GameField::H)
            grid[y][x] = e->symbol();
    }
    cout << "\n";
    for (auto& r : grid) cout << r << "\n";
    cout << "Lives: " << lives_ << " | Score: " << score_ << "\n";

    bool anyAlien = false;
    for (auto* e : entities_)
        if (e->alive() && e->type() == EntityType::ALIEN) { anyAlien = true; break; }
    if (!anyAlien) { cout << "\nYou win!\n"; gameOn_ = false; }
}
