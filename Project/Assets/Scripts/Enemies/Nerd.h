#ifndef ROGUELIKE_NERD_H
#define ROGUELIKE_NERD_H

#include "Entity.h"

class Player;

class Nerd : public Entity
{
private:
    int id = 0;
    float cooldown = 0.0f;
    float attackTimer = 0.0f;

    Player* player = nullptr;

    void FindPlayerPtr();
    void TryStartTask();
    void UpdateTask(float dt);

public:
    Nerd(float x, float y, int id);
    ~Nerd();

    void Update() override;
    void Draw() override;
    void Damage(float damage) override
    {
        hp-=damage;
    }
};

#endif
