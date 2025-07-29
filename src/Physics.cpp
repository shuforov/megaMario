#include "../include/Physics.h"
#include "Vec2.h"
#include <cmath>

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
  // Returning the overlap rectangle size of the bounding boxes of entity a
  //  and b

  // A entity data
  Vec2 aEntityPosition = a->getComponent<CTransform>().pos;
  Vec2 aEntityBBSize = a->getComponent<CBoundingBox>().size;
  Vec2 aEntityBBHalfSizes = a->getComponent<CBoundingBox>().halfSize;

  // B entity data
  Vec2 bEntityPosition = b->getComponent<CTransform>().pos;
  Vec2 bEntityBBSize = b->getComponent<CBoundingBox>().size;
  Vec2 bEntityBBHalfSizes = b->getComponent<CBoundingBox>().halfSize;

  // Calculate overlap of two objects
  // differences between center of two rectangles
  Vec2 delta = Vec2(std::fabs(aEntityPosition.x - bEntityPosition.x),
                    std::fabs(aEntityPosition.y - bEntityPosition.y));
  float overlapX = aEntityBBHalfSizes.x + bEntityBBHalfSizes.x - delta.x;
  float overlapY = aEntityBBHalfSizes.y + bEntityBBHalfSizes.y - delta.y;
  Vec2 overlapResult = Vec2(
      overlapX, overlapY); // if one of those value negative it's not ovelpa
  return overlapResult;
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a,
                                 std::shared_ptr<Entity> b) {
  // Returning the previous overlap rectangle size of the bounding boxes of
  // entity a and b
  //       previous overlap uses the entity's previous position

  // A entity data
  Vec2 aEntityPPosition = a->getComponent<CTransform>().prevPos;
  Vec2 aEntityBBSize = a->getComponent<CBoundingBox>().size;
  Vec2 aEntityBBHalfSizes = a->getComponent<CBoundingBox>().halfSize;

  // B entity data
  Vec2 bEntityPPosition = b->getComponent<CTransform>().prevPos;
  Vec2 bEntityBBSize = b->getComponent<CBoundingBox>().size;
  Vec2 bEntityBBHalfSizes = b->getComponent<CBoundingBox>().halfSize;

  // Calculate overlap of two objects
  // differences between center of two rectangles
  Vec2 delta = Vec2(std::fabs(aEntityPPosition.x - bEntityPPosition.x),
                    std::fabs(aEntityPPosition.y - bEntityPPosition.y));
  float overlapX = aEntityBBHalfSizes.x + bEntityBBHalfSizes.x - delta.x;
  float overlapY = aEntityBBHalfSizes.y + bEntityBBHalfSizes.y - delta.y;
  Vec2 overlapResult = Vec2(
      overlapX, overlapY); // if one of those value negative it's not ovelpa

  return overlapResult;
}
