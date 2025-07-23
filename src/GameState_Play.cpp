#include "../include/GameState_Play.h"
#include "../include/Assets.h"
#include "../include/Common.h"
#include "../include/Components.h"
#include "../include/GameEngine.h"
#include "../include/Physics.h"
#include <cmath>

GameState_Play::GameState_Play(GameEngine &game, const std::string &levelPath)
    : GameState(game), m_levelPath(levelPath) {
  init(m_levelPath);
}

void GameState_Play::init(const std::string &levelPath) {
  loadLevel(levelPath);

  spawnPlayer();

  // some sample entities
  auto brick = m_entityManager.addEntity("tile");
  brick->addComponent<CTransform>(Vec2(100, 400));
  brick->addComponent<CAnimation>(m_game.getAssets().getAnimation("Brick"),
                                  true);

  if (brick->getComponent<CAnimation>()->animation.getName() == "Brick") {
    std::cout
        << "This could be a good way of identifying if a tile is a brick!\n";
  }

  auto block = m_entityManager.addEntity("tile");
  block->addComponent<CTransform>(Vec2(200, 400));
  block->addComponent<CAnimation>(m_game.getAssets().getAnimation("Block"),
                                  true);
  // add a bounding box, this will now show up if we press the 'F' key
  block->addComponent<CBoundingBox>(
      m_game.getAssets().getAnimation("Block").getSize());

  auto question = m_entityManager.addEntity("tile");
  question->addComponent<CTransform>(Vec2(300, 400));
  question->addComponent<CAnimation>(
      m_game.getAssets().getAnimation("Question"), true);
}

void GameState_Play::loadLevel(const std::string &filename) {
  // reset the entity manager every time we load a level
  m_entityManager = EntityManager();

  // TODO: read in the level file and add the appropriate entities
  //       use the PlayerConfig struct m_playerConfig to store player properties
  //       this struct is defined at the top of GameState_Play.h
}

void GameState_Play::spawnPlayer() {
  // here is a sample player entity which you can use to construct other
  // entities
  m_player = m_entityManager.addEntity("player");
  m_player->addComponent<CTransform>(Vec2(200, 200));
  m_player->addComponent<CBoundingBox>(Vec2(48, 48));
  m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("Stand"),
                                     true);

  // TODO: be sure to add the remaining components to the player
}

void GameState_Play::spawnBullet(std::shared_ptr<Entity> entity) {
  // TODO: this should spawn a bullet at the given entity, going in the
  // direction the entity is facing
}

void GameState_Play::update() {
  m_entityManager.update();

  // TODO: implement pause functionality

  sMovement();
  sLifespan();
  sCollision();
  sAnimation();
  sUserInput();
  sRender();
}

void GameState_Play::sMovement() {
  // TODO: Implement player movement / jumping based on its CInput component
  // TODO: Implement gravity's effect on the player
  // TODO: Implement the maxmimum player speed in both X and Y directions
}

void GameState_Play::sLifespan() {
  // TODO: Check lifespawn of entities that have them, and destroy them if they
  // go over
}

void GameState_Play::sCollision() {
  // TODO: Implement Physics::GetOverlap() function, use it inside this function

  // TODO: Implement bullet / tile collisions
  //       Destroy the tile if it has a Brick animation
  // TODO: Implement player / tile collisions and resolutions
  //       Update the CState component of the player to store whether
  //       it is currently on the ground or in the air. This will be
  //       used by the Animation system
  // TODO: Check to see if the player has fallen down a hole (y < 0)
  // TODO: Don't let the player walk off the left side of the map
}

void GameState_Play::sUserInput() {
  // TODO: implement the rest of the player input

  sf::Event event;
  while (m_game.window().pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      m_game.quit();
    }

    if (event.type == sf::Event::KeyPressed) {
      switch (event.key.code) {
      case sf::Keyboard::Escape: {
        m_game.popState();
        break;
      }
      case sf::Keyboard::Z: {
        init(m_levelPath);
        break;
      }
      case sf::Keyboard::R: {
        m_drawTextures = !m_drawTextures;
        break;
      }
      case sf::Keyboard::F: {
        m_drawCollision = !m_drawCollision;
        break;
      }
      case sf::Keyboard::P: {
        setPaused(!m_paused);
        break;
      }
      }
    }

    if (event.type == sf::Event::KeyReleased) {
      switch (event.key.code) {
      case sf::Keyboard::Escape: {
        break;
      }
      }
    }
  }
}

void GameState_Play::sAnimation() {
  // TODO: set the animation of the player based on its CState component
  // TODO: for each entity with an animation, call
  // entity->getComponent<CAnimation>()->animation.update()
  //       if the animation is not repeated, and it has ended, destroy the
  //       entity
}

// this function has been completed for you
void GameState_Play::sRender() {
  // color the background darker so you know that the game is paused
  if (!m_paused) {
    m_game.window().clear(sf::Color(100, 100, 255));
  } else {
    m_game.window().clear(sf::Color(50, 50, 150));
  }

  // set the viewport of the window to be centered on the player if it's far
  // enough right
  auto pPos = m_player->getComponent<CTransform>()->pos;
  float windowCenterX = fmax(m_game.window().getSize().x / 2.0f, pPos.x);
  sf::View view = m_game.window().getView();
  view.setCenter(windowCenterX,
                 m_game.window().getSize().y - view.getCenter().y);
  m_game.window().setView(view);

  // draw all Entity textures / animations
  if (m_drawTextures) {
    for (auto e : m_entityManager.getEntities()) {
      auto transform = e->getComponent<CTransform>();

      if (e->hasComponent<CAnimation>()) {
        auto animation = e->getComponent<CAnimation>()->animation;
        animation.getSprite().setRotation(transform->angle);
        animation.getSprite().setPosition(
            transform->pos.x, m_game.window().getSize().y - transform->pos.y);
        animation.getSprite().setScale(transform->scale.x, transform->scale.y);
        m_game.window().draw(animation.getSprite());
      }
    }
  }

  // draw all Entity collision bounding boxes with a rectangleshape
  if (m_drawCollision) {
    for (auto e : m_entityManager.getEntities()) {
      if (e->hasComponent<CBoundingBox>()) {
        auto box = e->getComponent<CBoundingBox>();
        auto transform = e->getComponent<CTransform>();
        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(box->size.x - 1, box->size.y - 1));
        rect.setOrigin(sf::Vector2f(box->halfSize.x, box->halfSize.y));
        rect.setPosition(transform->pos.x,
                         m_game.window().getSize().y - transform->pos.y);
        rect.setFillColor(sf::Color(0, 0, 0, 0));
        rect.setOutlineColor(sf::Color(255, 255, 255, 255));
        rect.setOutlineThickness(1);
        m_game.window().draw(rect);
      }
    }
  }

  m_game.window().display();
}
