#include "../include/Physics.h"
#include "Vec2.h"
#include <cmath>

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
  // Returning the overlap rectangle size of the bounding boxes of entity a
  //  and b

  // A entity data
  Vec2 aEntityPosition = a->getComponent<CTransform>().pos;
  Vec2 aEntityBBHalfSizes = a->getComponent<CBoundingBox>().halfSize;

  // B entity data
  Vec2 bEntityPosition = b->getComponent<CTransform>().pos;
  Vec2 bEntityBBHalfSizes = b->getComponent<CBoundingBox>().halfSize;

  // Calculate overlap of two objects
  // differences between center of two rectangles
  Vec2 delta = bEntityPosition - aEntityPosition;
  float overlapX =
      (aEntityBBHalfSizes.x + bEntityBBHalfSizes.x) - std::abs(delta.x);
  float overlapY =
      (aEntityBBHalfSizes.y + bEntityBBHalfSizes.y) - std::abs(delta.y);
  if (overlapX > 0 && overlapY > 0) {
    return Vec2((delta.x > 0 ? -overlapX : overlapX),
                (delta.y > 0 ? -overlapY : overlapY));
  }
  return Vec2(0, 0);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a,
                                 std::shared_ptr<Entity> b) {
  // Returning the previous overlap rectangle size of the bounding boxes of
  // entity a and b
  //       previous overlap uses the entity's previous position

  // A entity data
  Vec2 aEntityPPosition = a->getComponent<CTransform>().prevPos;
  Vec2 aEntityBBHalfSizes = a->getComponent<CBoundingBox>().halfSize;

  // B entity data
  Vec2 bEntityPPosition = b->getComponent<CTransform>().prevPos;
  Vec2 bEntityBBHalfSizes = b->getComponent<CBoundingBox>().halfSize;

  // Calculate overlap of two objects
  // differences between center of two rectangles
  Vec2 delta = bEntityPPosition - aEntityPPosition;
  float overlapX =
      (aEntityBBHalfSizes.x + bEntityBBHalfSizes.x) - std::abs(delta.x);
  float overlapY =
      (aEntityBBHalfSizes.y + bEntityBBHalfSizes.y) - std::abs(delta.y);
  return Vec2((delta.x > 0 ? -overlapX : overlapX),
              (delta.y > 0 ? -overlapY : overlapY));
}
