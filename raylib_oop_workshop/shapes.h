#ifndef SHAPES_H
#define SHAPES_H
#include "raylib.h"
#include "raymath.h"
#include <memory>
#include <set>
#include <algorithm>
#include <iostream>
#include <format>

class Shape
{
public:

    // virtual destructor
    ~Shape(){}
    virtual void Update() = 0; 
    virtual bool Collides(std::shared_ptr<Shape> other) = 0;
    virtual void OnNewFrame() = 0;

    // attributes
Vector2 position_,velocity_,acceleration_ = {0,0};
Color shape_color_;
bool enable_screen_edge_collision_ = false;


};

class Rect ; // forward declaration

class Circle : public Shape
{
public:
    Circle(
        float initial_pos_x,float initial_pos_y, // where the circle will first appear
        float initial_velocity_x,float initial_velocity_y, // how the circle will move from frame to the next
        float radius, // how big is our circle
        Color shape_color, bool enable_screen_edge_collision=true
    ) // constructor
    {
        position_ = Vector2{initial_pos_x,initial_pos_y},
        velocity_ = Vector2{initial_velocity_x,initial_velocity_y},
        circle_radius_ = radius;
        enable_screen_edge_collision_ = enable_screen_edge_collision;
        shape_color_ = shape_color;
    }

    void Update()
    {
        // update logic
        // update the current position using the velocity
        float dt = GetFrameTime();
        if(dt > 0.05) dt = 0;

        // update velocity
        velocity_.x += acceleration_.x * dt;
        velocity_.y += acceleration_.y * dt;
        
        // update position
        position_.x += velocity_.x * dt;
        position_.y += velocity_.y * dt;       
        // std::cout << std::format("postion: {},{}",position_.x,position_.y);

        if(enable_screen_edge_collision_)
            HandleEdgeCollision();

        DrawCircle(position_.x,position_.y,circle_radius_,shape_color_);
    }    

    bool Collides(std::shared_ptr<Shape> other);

    void OnNewFrame()
    {
        
    }

float circle_radius_;
private:
    void HandleEdgeCollision()
    {
        std::cout << "Trigger collision\n";
        if(position_.x + circle_radius_ > GetScreenWidth())
        {
            velocity_.x = -velocity_.x;
            position_.x = GetScreenWidth() - circle_radius_;
        }
        if(position_.x - circle_radius_ < 0)
        {
            position_.x = circle_radius_;
            velocity_.x = -velocity_.x;
        }

        if(position_.y + circle_radius_ > GetScreenHeight() )
        {
            position_.y = GetScreenHeight() - circle_radius_;
            velocity_.y = 0;
        }
        if(position_.y - circle_radius_ <0)
        {
            position_.y =  circle_radius_;
            velocity_.y = -velocity_.y;
        }
    }

    
};

class Rect : public Shape
{
public:
    Rect(        
        float initial_pos_x,float initial_pos_y, // where the rect will first appear
        float initial_velocity_x,float initial_velocity_y, // how the circle will move from frame to the next
        float width,float height, // size of rect
        Color shape_color, bool enable_screen_edge_collision=true
    )
    {
        position_ = Vector2{initial_pos_x,initial_pos_y};
        velocity_ = Vector2{initial_velocity_x,initial_velocity_y};
        width_ = width;
        height_ = height;
        shape_color_ = shape_color;
        enable_screen_edge_collision_ = enable_screen_edge_collision;

    }

    void Update()
    {
        // update logic
        // update the current position using the velocity
        float dt = GetFrameTime();
        if(dt > 0.05) dt = 0;

        // update velocity
        velocity_.x += acceleration_.x * dt;
        velocity_.y += acceleration_.y * dt;
        
        // update position
        position_.x += velocity_.x * dt;
        position_.y += velocity_.y * dt;       

        if(enable_screen_edge_collision_)
            HandleEdgeCollision();
            
        DrawRectangle(position_.x,position_.y,width_,height_,shape_color_);

    }


    bool Collides(std::shared_ptr<Shape> other);

    void OnNewFrame()
    {
    }


float width_,height_;

private:

    void HandleEdgeCollision()
    {
        if(position_.x + width_ > GetScreenWidth())
        {
            position_.x = GetScreenWidth() - width_;
            velocity_.x = -velocity_.x;
        }
        if(position_.x - width_ < 0)
        {
            position_.x = width_;
            velocity_.x = -velocity_.x;

        }
        if(position_.y + height_ > GetScreenHeight())
        {
            position_.y = GetScreenHeight() - height_;
            velocity_.y = -velocity_.y;
        }
        if(position_.y - height_ <0)
        {
            position_.y = height_;
            velocity_.y = -velocity_.y;
        }

    }

};

bool Circle::Collides(std::shared_ptr<Shape> other)
{
    // don't proceed if it is with yourself
    if(this == other.get())
        return false;
    
    
    // cast to find what is the derived type
    auto circle = std::dynamic_pointer_cast<Circle>(other);
    auto rect = std::dynamic_pointer_cast<Rect>(other);

    if(circle)
    {

        
        float distance = Vector2Distance(circle->position_, position_);
    
        if(distance < circle_radius_ + circle->circle_radius_ && distance > 0.0001f)
        {
            /*
                v1' = v1 - (x1-x2) * (DOT((v1-v2),(x1-x2)) / d * d)
                v2' = v2 - (x2-x1) * (DOT((v2-v1),(x2-x1)) / d * d)
            */
            // Position difference vectors
            Vector2 pos_diff_this = Vector2Subtract(position_, circle->position_);
            Vector2 pos_diff_other = Vector2Subtract(circle->position_, position_);
            
            // Velocity difference vectors  
            Vector2 vel_diff_this = Vector2Subtract(velocity_, circle->velocity_);
            Vector2 vel_diff_other = Vector2Subtract(circle->velocity_, velocity_);
            
            // Calculate dot products
            float dot_this = Vector2DotProduct(vel_diff_this, pos_diff_this);
            float dot_other = Vector2DotProduct(vel_diff_other, pos_diff_other);
            
            // Squared distance
            float dist_squared = distance * distance;
            
            // Calculate new velocities (for equal mass)
            Vector2 this_new_vel = Vector2Subtract(velocity_, 
                Vector2Scale(pos_diff_this, dot_this / dist_squared));
            Vector2 other_new_vel = Vector2Subtract(circle->velocity_, 
                Vector2Scale(pos_diff_other, dot_other / dist_squared));
            
            // Check for NaN (should be && not ||)
            if(!std::isnan(this_new_vel.x) && !std::isnan(this_new_vel.y))
            {
                velocity_ = this_new_vel;

            }
            
            if(!std::isnan(other_new_vel.x) && !std::isnan(other_new_vel.y))
                circle->velocity_ = other_new_vel;
            
            // Separate overlapping circles
            float overlap = (circle_radius_ + circle->circle_radius_) - distance;
            if(overlap > 0)
            {
                Vector2 separation = Vector2Scale(pos_diff_this, overlap / (2.0f * distance));
                position_ = Vector2Add(position_, separation);
                circle->position_ = Vector2Subtract(circle->position_, separation);
            }
            return true;
        }
    
    }
    else if(rect)
    {
        // Find the closest point on the rectangle to the circle's center
        // Assuming rect->position_ is the center of the rectangle
        float closestX = std::clamp(position_.x, 
                                    rect->position_.x , 
                                    rect->position_.x + rect->width_);
        float closestY = std::clamp(position_.y, 
                                    rect->position_.y , 
                                    rect->position_.y + rect->height_);
        
        Vector2 closestPoint = {closestX, closestY};
        
        // Calculate distance from circle center to closest point
        Vector2 difference = Vector2Subtract(position_, closestPoint);
        float distance = Vector2Length(difference);
        
        // Check if collision occurs
        if(distance < circle_radius_ && distance > 0.0001f)
        {
            // Calculate collision normal (from closest point to circle center)
            Vector2 normal = Vector2Normalize(difference);
            
            // Calculate relative velocity
            Vector2 relativeVelocity = Vector2Subtract(velocity_, rect->velocity_);
            
            // Calculate relative velocity along the normal
            float velocityAlongNormal = Vector2DotProduct(relativeVelocity, normal);
            
            // Don't resolve if velocities are separating
            if(velocityAlongNormal < 0)
            {
                // Using similar approach to circle-circle collision
                // For elastic collision with equal masses
                float impulseScalar = -2.0f * velocityAlongNormal;
                
                // Apply impulse to both objects
                Vector2 impulse = Vector2Scale(normal, impulseScalar / 2.0f);
                
                Vector2 this_new_vel = Vector2Add(velocity_, impulse);
                Vector2 rect_new_vel = Vector2Subtract(rect->velocity_, impulse);
                
                // Check for NaN before applying
                if(!std::isnan(this_new_vel.x) && !std::isnan(this_new_vel.y))
                {
                    velocity_ = this_new_vel;

                }
                
                if(!std::isnan(rect_new_vel.x) && !std::isnan(rect_new_vel.y))
                    rect->velocity_ = rect_new_vel;
            }
            
            // Separate overlapping shapes
            float overlap = circle_radius_ - distance;
            if(overlap > 0)
            {
                Vector2 separation = Vector2Scale(normal, overlap / 2.0f);
                position_ = Vector2Add(position_, separation);
                rect->position_ = Vector2Subtract(rect->position_, separation);
                    
            }
            return true;
        }
    }
    
    return false;
}


bool Rect::Collides(std::shared_ptr<Shape> other)
{
        // don't proceed if it is with yourself
    if(this == other.get())
        return false;
    
    
    // cast to find what is the derived type
    auto circle = std::dynamic_pointer_cast<Circle>(other);
    auto rect = std::dynamic_pointer_cast<Rect>(other);

    if(circle)
    {
        // Convert rect corner position to center for calculations
        Vector2 rect_center = {position_.x + width_/2.0f, position_.y + height_/2.0f};
        
        // Find closest point on rectangle to circle center
        float closestX = std::clamp(circle->position_.x, 
                                    rect_center.x - width_/2.0f, 
                                    rect_center.x + width_/2.0f);
        float closestY = std::clamp(circle->position_.y, 
                                    rect_center.y - height_/2.0f, 
                                    rect_center.y + height_/2.0f);
        
        Vector2 closestPoint = {closestX, closestY};
        Vector2 difference = Vector2Subtract(circle->position_, closestPoint);
        float distance = Vector2Length(difference);
        
        if(distance < circle->circle_radius_ && distance > 0.0001f)
        {
            Vector2 normal = Vector2Normalize(difference);
            Vector2 relativeVelocity = Vector2Subtract(circle->velocity_, velocity_);
            float velocityAlongNormal = Vector2DotProduct(relativeVelocity, normal);
            
            if(velocityAlongNormal < 0)
            {
                float impulseScalar = -2.0f * velocityAlongNormal;
                Vector2 impulse = Vector2Scale(normal, impulseScalar / 2.0f);
                
                Vector2 this_new_vel = Vector2Add(circle->velocity_, impulse);
                Vector2 rect_new_vel = Vector2Subtract(velocity_, impulse);
                
                // Check for NaN before applying
                if(!std::isnan(this_new_vel.x) && !std::isnan(this_new_vel.y))
                {
                    velocity_ = this_new_vel;

                }
                
                if(!std::isnan(rect_new_vel.x) && !std::isnan(rect_new_vel.y))
                    circle->velocity_ = rect_new_vel;
            }
            
            float overlap = circle->circle_radius_ - distance;
            if(overlap > 0)
            {
                Vector2 separation = Vector2Scale(normal, overlap / 2.0f);
                circle->position_ = Vector2Add(circle->position_, separation);
                position_ = Vector2Subtract(position_, separation);
            }
            return true;
        }
    }
    else if(rect)
    {
        // Rectangle to Rectangle collision (AABB collision)
        // Check if rectangles overlap
        bool xOverlap = position_.x < rect->position_.x + rect->width_ &&
                       position_.x + width_ > rect->position_.x;
        bool yOverlap = position_.y < rect->position_.y + rect->height_ &&
                       position_.y + height_ > rect->position_.y;
        
        if(xOverlap && yOverlap)
        {
            // Calculate overlap amounts on each axis
            float overlapLeft = (position_.x + width_) - rect->position_.x;
            float overlapRight = (rect->position_.x + rect->width_) - position_.x;
            float overlapTop = (position_.y + height_) - rect->position_.y;
            float overlapBottom = (rect->position_.y + rect->height_) - position_.y;
            
            // Find the minimum overlap (this is the collision axis)
            float minOverlapX = std::min(overlapLeft, overlapRight);
            float minOverlapY = std::min(overlapTop, overlapBottom);
            
            Vector2 normal;
            float overlap;
            
            // Determine collision normal based on minimum overlap
            if(minOverlapX < minOverlapY)
            {
                // Collision on X axis
                overlap = minOverlapX;
                if(overlapLeft < overlapRight)
                    normal = {1, 0};  // Collision from left
                else
                    normal = {-1, 0}; // Collision from right
            }
            else
            {
                // Collision on Y axis
                overlap = minOverlapY;
                if(overlapTop < overlapBottom)
                    normal = {0, 1};  // Collision from top
                else
                    normal = {0, -1}; // Collision from bottom
            }
            
            // Calculate relative velocity
            Vector2 relativeVelocity = Vector2Subtract(velocity_, rect->velocity_);
            float velocityAlongNormal = Vector2DotProduct(relativeVelocity, normal);
            
            // Don't resolve if velocities are separating
            if(velocityAlongNormal > 0)
            {
                // Apply elastic collision response
                float impulseScalar = -2.0f * velocityAlongNormal;
                Vector2 impulse = Vector2Scale(normal, impulseScalar / 2.0f);
                
                Vector2 this_new_vel = Vector2Add(velocity_, impulse);
                Vector2 rect_new_vel = Vector2Subtract(rect->velocity_, impulse);
                
                // Check for NaN before applying
                if(!std::isnan(this_new_vel.x) && !std::isnan(this_new_vel.y))
                {
                    velocity_ = this_new_vel;

                }
                
                if(!std::isnan(rect_new_vel.x) && !std::isnan(rect_new_vel.y))
                    rect->velocity_ = rect_new_vel;
            }
            
            // Separate overlapping rectangles
            Vector2 separation = Vector2Scale(normal, overlap / 2.0f);
            rect->position_ = Vector2Add(rect->position_, separation);
            position_ = Vector2Subtract(position_, separation);

            return true;
        }

    }
    
    return false;
};


#endif