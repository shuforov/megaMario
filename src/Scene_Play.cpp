#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>

#include "../include/Action.h"
#include "../include/Assets.h"
#include "../include/Components.h"
#include "../include/GameEngine.h"
#include "../include/Scene_Menu.h"
#include "../include/Scene_Play.h"
#include "Physics.h"
#include "SFML//Window/Event.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "Vec2.h"

Scene_Play::Scene_Play(GameEngine *gameEngine, const std::string &levelPath)
    : Scene(gameEngine), m_levelPath(levelPath) {
  init(levelPath);
}

void Scene_Play::init(const std::string &levelPath) {
  registerAction(sf::Keyboard::P, "PAUSE");
  registerAction(sf::Keyboard::Escape, "QUIT");
  registerAction(sf::Keyboard::T,
                 "TOGGLE_TEXTURE"); // Toggle drawing (T)extures
  registerAction(sf::Keyboard::C,
                 "TOGGLE_COLLISION"); // Toggle drawing (C)ollision Boxes
  registerAction(sf::Keyboard::G, "TOGGLE_GRID"); // Toggle drawing (G)rid
  registerAction(sf::Keyboard::W, "JUMP");
  registerAction(sf::Keyboard::A, "LEFT");
  registerAction(sf::Keyboard::D, "RIGHT");
  registerAction(sf::Keyboard::S, "DOWN");
  registerAction(sf::Keyboard::J, "SHOOT");

  m_gridText.setCharacterSize(12);
  m_gridText.setFont(m_game->assets().getFont("Arial"));
  // m_gridText.setFont(m_game->assets().getFont("Tech"));

  loadLevel(levelPath);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY,
                                std::shared_ptr<Entity> entity) {
  // This function takes in a grid (x,y) position and an Entity
  //       Return a Vec2 indicating where the CENTER position of the Entity
  //       should be You must use the Entity's Animation size to position it
  //       correctly The size of the grid width and height is stored in
  //       m_gridSize.x and m_gridSize.y The bottom-left corner of the Animation
  //       should aligh with the bottom left of the grid cell
  sf::Vector2 windowSize = m_game->window().getSize();
  float positionByGridX =
      windowSize.x - (windowSize.x - (m_gridSize.x * gridX));
  float positionByGridY = windowSize.y - (m_gridSize.y * gridY);
  Vec2 spriteSize = entity->getComponent<CAnimation>().animation.getSize();
  Vec2 result = Vec2((positionByGridX + spriteSize.x / 2),
                     (positionByGridY - spriteSize.y / 2));
  return result;
}

void Scene_Play::loadLevel(const std::string &fileName) {
  // reset the entity manager every time we load a level
  m_entityManager = EntityManager();

  // Reading data in level file here
  std::ifstream fileInput(fileName);
  if (!fileInput.is_open()) {
    std::cerr << "Could not open config file: " << fileName << std::endl;
    exit(1);
  }
  std::string configName;
  std::string entityName;
  Vec2 gridPos;

  while (fileInput >> configName) {
    if (configName == "Tile") {
      fileInput >> entityName >> gridPos.x >> gridPos.y;
      auto tileNode = m_entityManager.addEntity("Tile");
      tileNode->addComponent<CAnimation>(
          m_game->assets().getAnimation(entityName), true);
      tileNode->addComponent<CTransform>(
          gridToMidPixel(gridPos.x, gridPos.y, tileNode));
      tileNode->getComponent<CTransform>().prevPos =
          tileNode->getComponent<CTransform>().pos;
      tileNode->addComponent<CBoundingBox>(
          m_game->assets().getAnimation(entityName).getSize());
    } else if (configName == "Dec") {
      fileInput >> entityName >> gridPos.x >> gridPos.y;
      auto decNode = m_entityManager.addEntity("Dec");
      decNode->addComponent<CAnimation>(
          m_game->assets().getAnimation(entityName), true);
      decNode->addComponent<CTransform>(
          gridToMidPixel(gridPos.x, gridPos.y, decNode));
    } else if (configName == "Player") {
      fileInput >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >>
          m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.JUMP >>
          m_playerConfig.MAX_SPEED >> m_playerConfig.GRAVITY >>
          m_playerConfig.WEAPON;
    }
  }

  spawnPlayer();

  // NOTE: THIS IS INCREDIBLY IMPORTANT PLEASE READ THIS EXAMPLE
  //       Components are now returned as references rather than pointers
  //       If you do not specify a reference variable type, it will COPY the
  //       component Here is an example:
  //
  //       This will COPY the transform into the variable 'transform1' - it
  //       is INCORRECT Any changes you make to transform1 will not be
  //       changed inside the entity auto transform1 =
  //       entity->get<CTransform>()
  //
  //       This will REFERENCE the transform with the variable 'transform2'
  //       - it is CORRECT Now any changes you make to transform2 will be
  //       changed inside the entity auto& transform2 =
  //       entity->get<CTransform>()
}

void Scene_Play::spawnPlayer() {
  // here is a sample player entity which you can use to construct other
  // entities
  m_player = m_entityManager.addEntity("player");
  m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"),
                                     true);
  m_player->addComponent<CTransform>(
      gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
  m_player->addComponent<CBoundingBox>(
      Vec2(m_playerConfig.CX, m_playerConfig.CY));
  m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity) {
  // This spawn a bullet at the given entity, going in the
  // direction the entity is facing
  auto bulletNode = m_entityManager.addEntity("Bullet");
  auto entityPosition = entity->getComponent<CTransform>().pos;
  bulletNode->addComponent<CAnimation>(
      m_game->assets().getAnimation(m_playerConfig.WEAPON), true);
  bulletNode->addComponent<CTransform>(entityPosition);
  bulletNode->addComponent<CBoundingBox>(
      m_game->assets().getAnimation(m_playerConfig.WEAPON).getSize());
  if (m_playerLookDiraction == "left") {
    bulletNode->getComponent<CTransform>().velocity.x = -1;
  } else if (m_playerLookDiraction == "right") {
    bulletNode->getComponent<CTransform>().velocity.x = 1;
  }
}

void Scene_Play::update() {
  m_entityManager.update();

  // TODO: implement pause functionality

  sMovement();
  sLifespan();
  sCollision();
  sAnimation();
  sRender();
}

void Scene_Play::sMovement() {
  auto &transform = m_player->getComponent<CTransform>();
  auto &velocity = transform.velocity;
  auto gravity = m_player->getComponent<CGravity>().gravity;
  auto &input = m_player->getComponent<CInput>();

  // Track previous position before any changes
  transform.prevPos = transform.pos;

  //
  // === JUMP LOGIC ===
  //
  if (input.up && m_playerOnGround && !m_isJumping) {
    // Start jump
    velocity.y = m_playerConfig.JUMP;
    m_isJumping = true;
    m_jumpTime = 0.0f;
    m_playerOnGround = false;
    // std::cout << "Jump Start! Velocity.y = " << velocity.y << "\n";
  }

  // Holding jump: allow extended jump height while going upward
  if (input.up && m_isJumping) {
    m_jumpTime += 1.0f / m_game->m_frameLimit; // assume 60 FPS
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Air"),
                                       true);
    if (m_jumpTime < m_maxJumpTime) {
      // Optional: slightly reduce gravity during hold
      velocity.y += -gravity * 0.5f;
    } else {
      m_isJumping = false; // max hold time reached
    }
  }

  // Jump key released early: stop extending
  if (!input.up && m_isJumping) {
    m_isJumping = false;

    // Interrupt upward velocity if player was moving up
    if (velocity.y < 0) {
      velocity.y = 0;
    }
  }
  //
  // === APPLY GRAVITY ===
  //
  velocity.y += gravity;

  if (velocity.y > m_playerConfig.MAX_SPEED) {
    velocity.y = m_playerConfig.MAX_SPEED;
  }

  //
  // === HORIZONTAL INPUT ===
  //
  if (input.left) {
    velocity.x = -m_playerConfig.SPEED;
    if (m_playerOnGround) {
      m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"),
                                         true);
    }
    m_player->getComponent<CAnimation>().animation.setFlipped(true);
    m_playerLookDiraction = "left";
  } else if (input.right) {
    velocity.x = m_playerConfig.SPEED;
    if (m_playerOnGround) {
      m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"),
                                         true);
    }
    m_player->getComponent<CAnimation>().animation.setFlipped(false);
    m_playerLookDiraction = "right";
  } else {
    velocity.x = 0;
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"),
                                       true);
    if (m_playerLookDiraction == "left") {
      m_player->getComponent<CAnimation>().animation.setFlipped(true);
    } else {
      m_player->getComponent<CAnimation>().animation.setFlipped(false);
    }
  }

  //
  // === APPLY MOVEMENT ===
  //
  transform.pos += velocity;

  // BULLETS MOVEMENT UPDATE
  for (auto &entityNode : m_entityManager.getEntities("Bullet")) {
    Vec2 &entityPosition = entityNode->getComponent<CTransform>().pos;
    Vec2 &entityVelocity = entityNode->getComponent<CTransform>().velocity;
    entityPosition.x += entityVelocity.x * 10; // 10 is speed of bullet
  }
}

void Scene_Play::sLifespan() {
  // TODO: Check lifespan of entities the have them, and destroy them if the
  // go over
}

void Scene_Play::sCollision() {
  // REMEMBER: SFML's (0,0) position is in the TOP-LEFT corner
  //           This means jumping will have a negative y-component
  //           and gravity will have a positive y-component
  //           Also, something BELOW something else will hava a y value
  //           GREATER than it Also, something ABOVE something else will
  //           hava a y value LESS than it

  //
  // Collisions of tile with player BEGIN
  //
  m_playerOnGround = false;
  Vec2 &playerPosition = m_player->getComponent<CTransform>().pos;
  for (auto &entityNode : m_entityManager.getEntities("Tile")) {
    Vec2 overlap = m_worldPhysics.GetOverlap(m_player, entityNode);
    if (overlap.x != 0 && overlap.y != 0) {
      Vec2 previousOverlap =
          m_worldPhysics.GetPreviousOverlap(m_player, entityNode);
      auto &velocity = m_player->getComponent<CTransform>().velocity;
      auto entityName =
          entityNode->getComponent<CAnimation>().animation.getName();
      if (entityName == "Flag" || entityName == "Pole" ||
          entityName == "PoleTop") {
        m_player->addComponent<CTransform>(
            gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
      }
      if (std::abs(overlap.x) < std::abs(overlap.y)) {
        playerPosition.x += overlap.x;
      } else {
        playerPosition.y += overlap.y;

        if (overlap.y < 0) {
          // Landed on top of tile
          m_playerOnGround = true;
          velocity.y = 0;
          if (m_player->getComponent<CAnimation>().animation.getName() ==
              "Air") {
            m_player->addComponent<CAnimation>(
                m_game->assets().getAnimation("Stand"), true);
          }
        } else if (overlap.y > 0 && velocity.y < 0) {
          // Hit head on bottom of tile while jumping
          velocity.y = 0;
          if (entityName == "Brick") {
            entityNode->destroy();
          } else if (entityName == "Question") {
            entityNode->addComponent<CAnimation>(
                m_game->assets().getAnimation("Question2"), true);
          }
        }
      }
    }
  }

  //
  // Collisions of tile with player END
  //

  //
  // Block blayer to walk off the left side of the map BEGIN
  //
  if ((playerPosition.x - 32) < 0) {
    playerPosition.x = m_player->getComponent<CTransform>().prevPos.x;
    m_player->getComponent<CTransform>().velocity.x = 0;
  }
  //
  // Block blayer to walk off the left side of the map END
  //

  //
  // Player has fallen down BEGIN
  //
  if ((playerPosition.y) > m_game->window().getSize().y) {
    m_player->addComponent<CTransform>(
        gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
  }
  //
  // Player has fallen down END
  //

  //
  // Bullet collision BEGIN
  //
  for (auto bulletNode : m_entityManager.getEntities("Bullet")) {
    for (auto entityNode : m_entityManager.getEntities("Tile")) {
      Vec2 overlap = m_worldPhysics.GetOverlap(bulletNode, entityNode);
      if (overlap.x != 0 && overlap.y != 0) {
        bulletNode->destroy();
        auto entityName =
            entityNode->getComponent<CAnimation>().animation.getName();
        if (entityName == "Brick") {
          entityNode->destroy();
        }
      }
    }
  }
  //
  // Bullet collision END
  //

  // TODO: Implement bullet/tile collisions
  //       Destroy the tile if it has a Brick animation
}

void Scene_Play::sDoAction(const Action &action) {
  if (action.type() == "START") {
    if (action.name() == "TOGGLE_TEXTURE") {
      m_drawTextures = !m_drawTextures;
    } else if (action.name() == "TOGGLE_COLLISION") {
      m_drawCollision = !m_drawCollision;
    } else if (action.name() == "TOGGLE_GRID") {
      m_drawGrid = !m_drawGrid;
    } else if (action.name() == "PAUSE") {
      setPaused(!m_paused);
    } else if (action.name() == "QUIT") {
      onEnd();
    } else if (action.name() == "JUMP") {
      m_jumpActive = true;
      m_player->getComponent<CInput>().up = true;
    } else if (action.name() == "LEFT") {
      m_player->getComponent<CInput>().left = true;
    } else if (action.name() == "RIGHT") {
      m_player->getComponent<CInput>().right = true;
    } else if (action.name() == "DOWN") {
      m_player->getComponent<CInput>().down = true;
    } else if (action.name() == "SHOOT") {
      if (m_player->getComponent<CInput>().canShoot) {
        spawnBullet(m_player);
      }
      m_player->getComponent<CInput>().canShoot = false;
    }
  } else if (action.type() == "END") {
    if (action.name() == "JUMP") {
      m_jumpActive = false;
      m_player->getComponent<CInput>().up = false;
    } else if (action.name() == "LEFT") {
      m_player->getComponent<CInput>().left = false;
    } else if (action.name() == "RIGHT") {
      m_player->getComponent<CInput>().right = false;
    } else if (action.name() == "DOWN") {
      m_player->getComponent<CInput>().down = false;
    } else if (action.name() == "SHOOT") {
      m_player->getComponent<CInput>().canShoot = true;
    }
  }
}

void Scene_Play::sAnimation() {
  // TODO: Complete the Animation class code first
}

void Scene_Play::onEnd() {
  // When the scene ends, change back to the MENU scene
  m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}

void Scene_Play::sRender() {
  // color the background darker, so you know that the game is paused
  if (!m_paused) {
    m_game->window().clear(sf::Color(100, 100, 255));
  } else {
    m_game->window().clear(sf::Color(50, 50, 150));
  }

  // set the viewport of the window to be centered on the player if it's far
  // enough right
  auto &pPos = m_player->getComponent<CTransform>().pos;
  float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
  sf::View view = m_game->window().getView();
  view.setCenter(windowCenterX,
                 m_game->window().getSize().y - view.getCenter().y);
  m_game->window().setView(view);

  // draw all Entity textures / animations
  if (m_drawTextures) {
    for (const auto &e : m_entityManager.getEntities()) {
      auto &transform = e->getComponent<CTransform>();
      if (e->hasComponent<CAnimation>()) {
        auto &animation = e->getComponent<CAnimation>().animation;
        animation.getSprite().setRotation(transform.angle);
        animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
        animation.getSprite().setScale(transform.scale.x, transform.scale.y);
        m_game->window().draw(animation.getSprite());
      }
    }
  }

  // draw all Entity collision bounding boxes with a rectangle shape
  if (m_drawCollision) {
    for (const auto &e : m_entityManager.getEntities()) {
      if (e->hasComponent<CBoundingBox>()) {
        auto &box = e->getComponent<CBoundingBox>();
        auto &transform = e->getComponent<CTransform>();
        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
        rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
        rect.setPosition(transform.pos.x, transform.pos.y);
        rect.setFillColor(sf::Color(0, 0, 0, 0));
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(1);
        m_game->window().draw(rect);
      }
    }
  }

  // draw the grid so that can easily debug
  if (m_drawGrid) {
    float leftX = m_game->window().getView().getCenter().x - width() / 2.0;
    float rightX = leftX + width() + m_gridSize.x;
    float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

    for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
      drawLine(Vec2(x, 0), Vec2(x, height()));
    }

    for (float y = 0; y < height(); y += m_gridSize.y) {
      drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

      for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
        std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
        std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
        m_gridText.setString("(" + xCell + "," + yCell + ")");
        m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
        m_game->window().draw(m_gridText);
      }
    }
  }
}
