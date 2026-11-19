#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

int main() {
    srand(unsigned(time(nullptr)));
    Game g;
    g.run();
    return 0;
}
