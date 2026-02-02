#ifndef ROGUELIKE_SLINGSHOTER_H
#define ROGUELIKE_SLINGSHOTER_H

#include "Entity.h"
#include <memory>

class Weapon;
class Player;

class Slingshoter : public Entity
{
    int id = 0;

    Player* player = nullptr;

    float scale = 1.0f;

    std::unique_ptr<Weapon> weapon;

    Vector2 moveDir = {1,0};
    float moveTimer = 0.0f;
    int rnd = 0;

    enum State
    {
        Moving,
        Shooting
    };

    State state = Moving;
    float stateTimer = 0.0f;

    enum Direction
    {
        DOWN = 0,
        RIGHT = 2,
        LEFT = 4,
        UP = 6,
        IDLE = 8
    };

    Direction direction = IDLE;

    std::vector<Texture2D>* enemysprites = nullptr;

    void FindPlayer();
    bool SolidHit();
    bool CanMoveTo(Vector2 pos);
    void SetDir(Vector2 d);
    void Animate(float dt, bool moving);
    void Move(float dt, bool& moving);
    void UpdateState(float dt);

public:
    Slingshoter(float x, float y, int id);
    ~Slingshoter();

    void Update() override;
    void Draw() override;
    void Damage(float damage) override
    {
        hp -= damage;
    }
};

#endif //ROGUELIKE_SLINGSHOTER_H