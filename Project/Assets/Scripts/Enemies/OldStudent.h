#ifndef ROGUELIKE_OLDSTUDENT_H
#define ROGUELIKE_OLDSTUDENT_H

#include "Bully.h"

class Player;

class OldStudent : public Bully
{
private:
    Player* player = nullptr;

    float hitTimer = 0.0f;
    float baseHitDamage = 2.0f;

    Vector2 lastDir = {1, 0};

    void FindPlayerPtr();
    Collider* FindPlayerCollider();
    bool PushPlayer(Vector2 dir, float dist);

public:
    OldStudent(float x, float y, int id);

    void Update() override;
};

#endif
