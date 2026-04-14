#ifndef GAME_H
#define GAME_H
#include "game_entities.h"
#include "raylib.h"
#include "raymath.h"
#include <format>
#include <map>
#include <memory>
#include <vector>

/*
    Level class will help us to store all the game enitites and the
    player starting position.
*/
class Level {
public:
  Level(std::shared_ptr<Camera2D> camera, Vector2 player_starting_position) {
    camera_ = camera;
    player_starting_point_ = player_starting_position;
  }

  std::vector<std::shared_ptr<Entity>> level_entities_;
  std::shared_ptr<Camera2D> camera_;
  Vector2 player_starting_point_;
};

// game state can help us to keep track of the state of the game
enum GameState {
  kDead = 0, // the player died so can handle on restart
  kPlaying =
      1, // the player is playing so we just run the current level continously
  kWin = 2, // the player won if there is a next level lined up then we run it
            // else we show the win screen
};

class Game {
public:
  Game(std::shared_ptr<Player> player,
       std::vector<std::shared_ptr<Level>> levels) {
    player_ = player;
    levels_ = levels;
    current_level_index_ = 0;
    current_level_index_ = 0;
    total_game_time_ = 0.0f;
    coin_count_ = 0;

    if (!levels_.empty()) {
      current_level_ = levels_[0];
      current_level_->level_entities_.push_back(player);
    }
  }

  void StartGame() {
    if (current_level_) {
      current_level_->camera_->target = current_level_->player_starting_point_;
      player_->entity_shape_->position_ =
          current_level_->player_starting_point_;
      current_game_state_ = kPlaying;
      current_level_time_ = 0.0f;
    }
  }

  void NextLevel() {
    current_level_index_++;
    if (current_level_index_ < levels_.size()) {
      // Remove player from old level to avoid duplication if we ever go back
      // (though we don't here) Actually, we just add to the new one. Better:
      // just set the new level.
      current_level_ = levels_[current_level_index_];

      // Add player to the new level's entities so collision works
      current_level_->level_entities_.push_back(player_);

      // Reset player position
      StartGame();
    } else {
      // No more levels
      current_game_state_ = kWin;
    }
  }

  // utilities
  struct Text {
    const char *text_;
    int font_size_;
    Color text_color_;

    int GetTextWidth() { return MeasureText(text_, font_size_); }
    void DrawTextOnScreen(float x, float y) {
      DrawText(text_, x - GetTextWidth() / 2.f, y - font_size_ / 2.f,
               font_size_, text_color_);
    }
  };

  /*
      Where the core game logic sits and to be updated on every frame.
  */
  void Update() {
    // this is the state machine running our game
    switch (current_game_state_) {
    case kPlaying: {
      current_level_time_ += GetFrameTime();
      total_game_time_ += GetFrameTime();

      //-------------------------------------------------------------------------------------------------
      // collision handling
      //-------------------------------------------------------------------------------------------------
      for (int i = 0; i < current_level_->level_entities_.size(); i++) {
        for (int j = i + 1; j < current_level_->level_entities_.size(); j++) {
          // first we need to know which entity is the one we are using first
          {

            auto player = std::dynamic_pointer_cast<Player>(
                current_level_->level_entities_[i]);
            auto platform = std::dynamic_pointer_cast<Platform>(
                current_level_->level_entities_[j]);

            if (!player || !platform) {
              player = std::dynamic_pointer_cast<Player>(
                  current_level_->level_entities_[j]);
              platform = std::dynamic_pointer_cast<Platform>(
                  current_level_->level_entities_[i]);
            }

            // Now check if we have a valid player-platform pair
            if (player && platform) {
              auto prev_platform_position = platform->entity_shape_->position_;
              if (player->entity_shape_->Collides(platform->entity_shape_)) {
                // Platform stays fixed
                platform->entity_shape_->velocity_ = {0, 0};
                platform->entity_shape_->position_ = prev_platform_position;
                player_->can_jump_ = true;
              }
            }
          }

          {

            auto enemy = std::dynamic_pointer_cast<Enemy>(
                current_level_->level_entities_[i]);
            auto platform = std::dynamic_pointer_cast<Platform>(
                current_level_->level_entities_[j]);

            if (!enemy || !platform) {
              enemy = std::dynamic_pointer_cast<Enemy>(
                  current_level_->level_entities_[j]);
              platform = std::dynamic_pointer_cast<Platform>(
                  current_level_->level_entities_[i]);
            }

            if (enemy && platform) {
              auto prev_platform_position = platform->entity_shape_->position_;
              if (enemy->entity_shape_->Collides(platform->entity_shape_)) {
                // Platform stays fixed
                platform->entity_shape_->velocity_ = {0, 0};
                platform->entity_shape_->position_ = prev_platform_position;
              }
            }
          }

          {
            // collision with enemy
            auto enemy = std::dynamic_pointer_cast<Enemy>(
                current_level_->level_entities_[j]);
            auto player = std::dynamic_pointer_cast<Player>(
                current_level_->level_entities_[i]);
            // swap if neccessary
            if (!enemy || !player) {
              enemy = std::dynamic_pointer_cast<Enemy>(
                  current_level_->level_entities_[i]);
              player = std::dynamic_pointer_cast<Player>(
                  current_level_->level_entities_[j]);
            }

            if (enemy && player) {
              if (enemy->entity_shape_->Collides(player->entity_shape_)) {
                current_game_state_ = kDead;
              }
            }
          }

          {
            // colision with win platform
            auto win_platform = std::dynamic_pointer_cast<WinPlatform>(
                current_level_->level_entities_[i]);
            auto player = std::dynamic_pointer_cast<Player>(
                current_level_->level_entities_[j]);

            if (!win_platform || !player) {
              auto win_platform = std::dynamic_pointer_cast<WinPlatform>(
                  current_level_->level_entities_[j]);
              auto player = std::dynamic_pointer_cast<Player>(
                  current_level_->level_entities_[i]);
            }

            if (win_platform && player) {
              auto prev_platform_position =
                  win_platform->entity_shape_->position_;
              if (player->entity_shape_->Collides(
                      win_platform->entity_shape_)) {
                // Platform stays fixed
                win_platform->entity_shape_->velocity_ = {0, 0};
                win_platform->entity_shape_->position_ = prev_platform_position;

                // Advance to next level!
                NextLevel();
              }
            }
          } // end of collision block
        }
      }

      // Check for coin collisions
      for (auto it = current_level_->level_entities_.begin();
           it != current_level_->level_entities_.end();) {
        auto coin = std::dynamic_pointer_cast<Coin>(*it);
        if (coin) {
          if (player_->entity_shape_->Collides(coin->entity_shape_)) {
            coin_count_++;
            it = current_level_->level_entities_.erase(it); // Remove coin
            continue;
          }
        }
        ++it;
      }

      // handle if player falls:
      if (player_->entity_shape_->position_.y >
          current_level_->camera_->target.y + GetScreenHeight() / 2) {
        current_game_state_ = kDead;
      }

      //-------------------------------------------------------------------------------------------------
      // input handling
      //-------------------------------------------------------------------------------------------------
      if (IsKeyDown(KEY_LEFT)) {
        player_->entity_shape_->velocity_.x = -100;
      }
      if (IsKeyDown(KEY_RIGHT)) {

        player_->entity_shape_->velocity_.x = 100;
      }
      if (IsKeyDown(KEY_UP)) {
        player_->Jump();
        player_->can_jump_ = false;
      }

      // is to avoid any movement when we are not pressing any key
      if (IsKeyUp(KEY_UP) && IsKeyUp(KEY_LEFT) && IsKeyUp(KEY_RIGHT)) {
        // set the velocity of player to zero
        player_->entity_shape_->velocity_.x = 0;
      }

      current_level_->camera_->target.x = player_->entity_shape_->position_.x;

      //-------------------------------------------------------------------------------------------------
      // core game loop
      //-------------------------------------------------------------------------------------------------
      ClearBackground(SKYBLUE);
      BeginMode2D(*current_level_->camera_);

      // Update all the entity states:
      for (auto entity : current_level_->level_entities_) {
        entity->Update();
      }

      player_->Update();

      EndMode2D();

      // Draw Timer
      std::string time_str = std::format("Time: {:.2f}", current_level_time_);
      DrawText(time_str.c_str(), 10, 10, 20, WHITE);
      std::string level_str =
          std::format("Level: {}", current_level_index_ + 1);
      DrawText(level_str.c_str(), 10, 35, 20, YELLOW);
      std::string total_time_str =
          std::format("Total: {:.2f}", total_game_time_);
      DrawText(total_time_str.c_str(), 10, 60, 20, WHITE);
      std::string coin_str = std::format("Coins: {}", coin_count_);
      DrawText(coin_str.c_str(), 10, 85, 20, GOLD);

    } break;
    case kDead: {
      ClearBackground(GRAY);
      Text death_message = {"You Died!", 100, WHITE};
      death_message.DrawTextOnScreen(GetScreenWidth() / 2.f,
                                     GetScreenHeight() / 3.f);

      Text restart_instruction = {"Press space to restart", 50, WHITE};
      restart_instruction.DrawTextOnScreen(GetScreenWidth() / 2.f,
                                           2.f * GetScreenHeight() / 3.f);

      if (IsKeyPressed(KEY_SPACE)) {
        StartGame(); // Restart current level
      }
    } break;
    case kWin: {
      ClearBackground(GOLD);
      Text death_message = {"You Won the Game!", 80, WHITE};
      death_message.DrawTextOnScreen(GetScreenWidth() / 2.f,
                                     GetScreenHeight() / 3.f);

      std::string total_time_str =
          std::format("Final Time: {:.2f}", total_game_time_);

      Text time_msg = {total_time_str.c_str(), 40, WHITE};
      time_msg.DrawTextOnScreen(GetScreenWidth() / 2.f,
                                GetScreenHeight() / 2.f);

      std::string total_coins_str =
          std::format("Coins Collected: {}", coin_count_);
      Text coin_msg = {total_coins_str.c_str(), 40, GOLD};
      coin_msg.DrawTextOnScreen(GetScreenWidth() / 2.f,
                                GetScreenHeight() / 2.f + 50);

      Text restart_instruction = {"Press space to play again", 50, WHITE};
      restart_instruction.DrawTextOnScreen(GetScreenWidth() / 2.f,
                                           2.f * GetScreenHeight() / 3.f);

      if (IsKeyPressed(KEY_SPACE)) {
        current_level_index_ = 0;
        total_game_time_ = 0.0f;
        coin_count_ = 0;
        current_level_ = levels_[0];

        current_level_->level_entities_.push_back(player_);
        StartGame();
      }
    } break;
    }
  }

private:
  std::shared_ptr<Level> current_level_;
  std::vector<std::shared_ptr<Level>> levels_;
  int current_level_index_;
  std::shared_ptr<Player> player_;
  GameState current_game_state_;
  float current_level_time_;
  float total_game_time_;
  int coin_count_;
};

#endif // GAME_H