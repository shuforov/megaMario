#include "../include/GameEngine.h"
#include <SFML/Graphics.hpp>

int main() {
  GameEngine g("../bin/assets.txt");
  g.run();
}
