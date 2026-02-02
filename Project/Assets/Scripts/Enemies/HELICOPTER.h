#ifndef ROGUELIKE_HELICOPTER_H
#define ROGUELIKE_HELICOPTER_H

#include "Entity.h"

class Player;
class Weapon;

class HELICOPTER : public Entity
{
private:
    int id = 0;

    Player* player = nullptr;

    Circle circle;
    float rotation = 0.0f;

    bool moving = false;
    bool rotating = false;

    float newAngle = 0.0f;

    float strikeTick = 0.0f;
    int strikeRnd = 0;

    bool warning = false;
    float warningTimer = 0.0f;
    Vector2 strikePos{0,0};

    bool rocketFlying = false;
    Vector2 rocketPos{0,0};
    Vector2 rocketVel{0,0};
    Vector2 rocketTarget{0,0};

    bool exploding = false;
    float boomLife = 0.0f;
    float boomT = 0.0f;
    float boomR = 0.0f;

    Weapon* weapon = nullptr;

    void FindPlayerPtr();
    float RandomAngle();
    void Move(float dt);
    void CollisionCheck(Vector2 oldpos);

    void TryStartStrike(float dt);
    void UpdateStrike(float dt);

public:
    HELICOPTER(float x, float y, int id);
    ~HELICOPTER();

    void Update() override;
    void Draw() override;
    void Damage(float damage) override;
};

#endif
