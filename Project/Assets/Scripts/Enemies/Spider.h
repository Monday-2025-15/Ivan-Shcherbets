//
// Created by vanya on 1/25/26.
//

#ifndef ROGUELIKE_SPIDER_H
#define ROGUELIKE_SPIDER_H
#include "Entity.h"
#include "Player.h"
#include "Room.h"


class Spider : public Entity
{
    int id;
    Texture2D* texture[2] = {nullptr,nullptr};
    int currentTex;
    Circle circle;
    float rotation = 0.0f;
    Player* player;
    bool moving;
    Room& room;

    Vector2 startPos;
    Vector2 targetPos;
    Vector2 controlPos;
    float jumpTimer;
    float jumpDuration;
    float waitTimer;
    float hitCooldown;

public:
    Spider(float x, float y, int id, Room& room);
    ~Spider();
    void Draw() override;
    void Update() override;
    void JumpRandom();
    float JumpToPlayer();
    void Damage(float damage) override;
};


#endif //ROGUELIKE_SPIDER_H
