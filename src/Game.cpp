#include "../include/Game.h"

Game::Game(const std::string &config) { init(config); }

void Game::init(const std::string &path) {
  // TODO
  // read in config file here

  // set up default window parameters
  m_window.create(sf::VideoMode(1280, 720), "Assignment 2");
  m_window.setFramerateLimit(60);

  spawnPlayer();
}

void Game::run() {
  // TODO: add pause functionality in here
  while (m_running) {
    m_entities.update();
    sEnemySpawner();
    sMovement();
    sCollision();
    sUserInput();
    sRender();
  }
}

void Game::setPaused(bool paused) {
  // TODO
}

// respawn the player in the middle of the screen
void Game::spawnPlayer() {
  // TODO: Finish adding all properties of the player with the correct values
  // from the config

  // We create every entity by calling EntityManager.addEntity(tag)
  // This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
  auto entity = m_entities.addEntity("player");
  // Give this entity a Transform so it spawns at (200,200) with speed (1,1) and
  // angle 0
  entity->addComponent<CTransform>(Vec2(200, 200), Vec2(0, 0), Vec2(1, 1), 0);
  // entity->cTransform = new CTransform(Vec2(200, 200), Vec2(1, 1), 0);
  // The entity's shape will have radius 32, 8 sides, dark grey fill, and red
  // outline of thickness 4
  entity->addComponent<CShape>(32, 8, sf::Color(10, 10, 10),
                               sf::Color(255, 0, 0), 4);

  // entity->cShape =
  //     new CShape(32, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4);
  // Since we want this Entity to be our player, set our Game's player variable
  // to be this Entity
  m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy() {
  // TODO
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
  // TODO
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &target) {
  // TODO
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
  // TODO
}

void Game::sMovement() {
  // TODO

  // Sample movement speed update
  // m_player->cTransform->pos.x += m_player->cTransform->speed.x;
  // m_player->cTransform->pos.y += m_player->cTransform->speed.y;
}

void Game::sCollision() {
  // TODO
}

void Game::sEnemySpawner() {
  // TODO
}

void Game::sRender() {
  // TODO
  // sample drawing of the temporary Entity that we have created
  m_window.clear();

  // set the position of the shape based on the entity's transform->pos
  // m_player->cShape->circle.setPosition(m_player->cTransform->pos.x,
  //                                      m_player->cTransform->pos.y);
  //
  // set the rotation of the shape based on the entity's transform->angle
  // m_player->cTransform->angle += 1.0f;
  // m_player->cShape->circle.setRotation(m_player->cTransform->angle);

  // draw the entity's sf::CircleShape
  m_window.draw(m_player->getComponent<CShape>()->circle);

  m_window.display();
}

void Game::sUserInput() {
  // TODO
  sf::Event event;
  while (m_window.pollEvent(event)) {
    // this event triggers when the window is closed
    if (event.type == sf::Event::Closed) {
      m_running = false;
    }

    // this event is triggered when a key is pressed
    if (event.type == sf::Event::KeyPressed) {
      switch (event.key.code) {
      case sf::Keyboard::W:
        std::cout << "W Key Pressed\n";
        break;
      }
    }

    if (event.type == sf::Event::KeyReleased) {
      switch (event.key.code) {
      case sf::Keyboard::W:
        std::cout << "W Key Released\n";
        break;
      }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
      if (event.mouseButton.button == sf::Mouse::Left) {
        std::cout << "Left Mouse Button Clicked at (" << event.mouseButton.x
                  << "," << event.mouseButton.y << ")\n";
      }

      if (event.mouseButton.button == sf::Mouse::Right) {
        std::cout << "Right Mouse Button Clicked at (" << event.mouseButton.x
                  << "," << event.mouseButton.y << ")\n";
      }
    }
  }
}
