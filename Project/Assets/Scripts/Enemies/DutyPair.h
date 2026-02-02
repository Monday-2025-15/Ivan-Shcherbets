//
// Created by vanya on 2/2/26.
//

#ifndef ROGUELIKE_DUTYPAIR_H
#define ROGUELIKE_DUTYPAIR_H

#include "Entity.h"

class Weapon;
class Player;

class DutyPair : public Entity
{
private:
    int id = 0;

    Player* player = nullptr;
    Vector2 lastPlayerPos{0,0};

    Vector2 moveDir{1,0};
    float moveTimer = 0.0f;
    int rnd = 0;
    float stillTimer = 0.0f;
    float scale = 1.0f;
    unique_ptr<Weapon> weapon;
    void FindPlayer();
    bool PlayerMoving() const;

    bool SolidHit() const;
    bool CanMoveTo(Vector2 pos);
    void MoveAI(float dt);
    void Regen(float dt);

    Vector2 Center() const;

public:
    DutyPair(float x, float y, int id);
    ~DutyPair();

    void Update() override;
    void Draw() override;
    void Damage(float damage) override;
};

#endif //ROGUELIKE_DUTYPAIR_H
