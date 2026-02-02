#ifndef ROGUELIKE_SECURITY_H
#define ROGUELIKE_SECURITY_H

#include "Entity.h"
#include <memory>

class Weapon;
class Player;

class Security : public Entity
{
protected:
    int id = 0;

    Player* player = nullptr;

    float scale = 1.0f;

    std::unique_ptr<Weapon> weapon;

    Vector2 moveDir = {1,0};
    float moveTimer = 0.0f;
    int rnd = 0;

    int direction = 8;
    int frame = 8;

    float animationTimer = 0.0f;
    float animationSpeed = 0.15f;

    void FindPlayer();
    bool SolidHit();
    bool CanMoveTo(Vector2 pos);
    void SetDir(Vector2 d);
    void Animate(float dt, bool moving);
    void Move(float dt, bool& moving);
    std::vector<Texture2D>* enemysprites = nullptr;
public:
    Security(float x, float y, int id);
    virtual ~Security();

    void Update() override;
    void Draw() override;
    void Damage(float damage) override
    {
        hp-=damage;
    };
};

#endif
