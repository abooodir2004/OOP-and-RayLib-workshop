#ifndef GAME_ENTITIES_H
#define GAME_ENTITIES_H

#include "shapes.h"
#include <format>
#define GRAVITY Vector2{0, 200}
#define JUMP_COOLDOWN 3
#define PLAYER_JUMP_VEL Vector2{0, -250}
#define ENEMY_SPEED 200.f

class Entity {
public:
  virtual void Update() = 0;

  std::shared_ptr<Shape> entity_shape_;
};

class Player : public Entity {
public:
  Player(std::shared_ptr<Shape> player_shape) {
    entity_shape_ = player_shape;

    // set a constant gravity
    entity_shape_->acceleration_ = GRAVITY;
  }

  void Jump() {
    if (can_jump_) {
      entity_shape_->velocity_ = PLAYER_JUMP_VEL;
    }
  }

  void Update() { entity_shape_->Update(); }

  bool can_jump_ = false;
};

/*
    This class is used to define the movements of
    non-player-controlled entites.
*/

class Enemy : public Entity {
public:
  Enemy(std::shared_ptr<Shape> enemy_shape, float distance_marching) {
    entity_shape_ = enemy_shape;
    initial_x_ = entity_shape_->position_.x;

    distance_marching_ = distance_marching;

    // set a constant gravity
    entity_shape_->acceleration_ = GRAVITY;
    entity_shape_->enable_screen_edge_collision_ = false;

    entity_shape_->velocity_.x = ENEMY_SPEED;
  }

  void Update() {
    // update the position using distance marching
    if (entity_shape_->position_.x > initial_x_ + distance_marching_ ||
        entity_shape_->position_.x < initial_x_ - distance_marching_) {
      entity_shape_->velocity_.x = -entity_shape_->velocity_.x;
    }

    entity_shape_->Update();
  }

  float distance_marching_;
  float initial_x_;
};

class Platform : public Entity {
public:
  Platform(std::shared_ptr<Shape> platform_shape) {
    entity_shape_ = platform_shape;
  }

  void Update() {
    // update our shape's physics
    entity_shape_->Update();
  }
};

class WinPlatform : public Platform {
public:
  WinPlatform(std::shared_ptr<Shape> platform_shape)
      : Platform(platform_shape) {
    platform_shape->shape_color_ = GOLD;
  }
};

class Coin : public Entity {
public:
  Coin(std::shared_ptr<Shape> coin_shape) { entity_shape_ = coin_shape; }

  void Update() { entity_shape_->Update(); }
};

#endif // GAME_ENTITIES_H