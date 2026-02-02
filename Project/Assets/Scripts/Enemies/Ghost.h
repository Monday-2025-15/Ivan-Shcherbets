#ifndef ROGUELIKE_GHOST_H
#define ROGUELIKE_GHOST_H

#include "Entity.h"

class Player;

class Ghost : public Entity
{
    int id;
    Player* player = nullptr;
    Texture2D* texture = nullptr;

    void FindPlayer();
    void Move(float dt);

public:
    Ghost(float x, float y, int id);
    ~Ghost();
    void Draw() override;
    void Update() override;
    void Damage(float damage) override;
};

#endif //ROGUELIKE_GHOST_H