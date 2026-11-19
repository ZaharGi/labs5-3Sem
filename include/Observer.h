#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class IGameObserver {
public:
    virtual ~IGameObserver() = default;
    virtual void onStateChanged(int lives, int score, const string& msg) = 0;
};

class Subject {
public:
    virtual ~Subject() = default;
    void attach(unique_ptr<IGameObserver> o) { obs_.push_back(move(o)); }
protected:
    void notify(int lives, int score, const string& msg) {
        for (auto& o : obs_) o->onStateChanged(lives, score, msg);
    }
    vector<unique_ptr<IGameObserver>> obs_;
};

class ConsoleHUD : public IGameObserver {
public:
    void onStateChanged(int lives, int score, const string& msg) override {
        cout << "\n[HUD] Lives: " << lives << "  Score: " << score
             << (msg.empty() ? "" : "  " + msg) << "\n";
    }
};
