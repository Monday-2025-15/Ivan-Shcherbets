//
// Created by vanya on 2/2/26.
//

#ifndef ROGUELIKE_PHONEGUY_H
#define ROGUELIKE_PHONEGUY_H

#include "Entity.h"

class Player;
class Collider;

class PhoneGuy : public Entity
{
private:
    int id = 0;

    Player* player = nullptr;
    Collider* playerCollider = nullptr;

    float auraRadius = 150.0f;
    float auraTick = 0.0f;

    void FindPlayer();
    Vector2 GetPlayerCenter() const;
    Vector2 GetCenter() const;

    void UpdateAura(float dt);
    void DrawAura() const;

public:
    PhoneGuy(float x, float y, int id);
    ~PhoneGuy();

    void Update() override;
    void Draw() override;
    void Damage(float damage) override;
};

#endif //ROGUELIKE_PHONEGUY_H
