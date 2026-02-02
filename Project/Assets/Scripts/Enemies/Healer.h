#ifndef ROGUELIKE_HEALER_H
#define ROGUELIKE_HEALER_H

#include "Entity.h"

class Healer : public Entity
{
    int id;
    float scale;

    vector<Texture2D> sprites;
    int frame;
    float animationTimer;

    enum Direction {
        DOWN = 0,
        RIGHT = 2,
        LEFT = 4,
        UP = 6,
        IDLE = 8
    };
    Direction direction;

    Vector2 moveDir;
    float moveTimer;

    float healTick;
    int rnd;

    bool SolidHit();
    bool CanMoveTo(Vector2 pos);
    std::vector<Texture2D>* enemysprites = nullptr;
    void SetDir(Vector2 d);
    void Animate(float dt, bool moving);
    void Move(float dt, bool &moving);
    void TryHeal(float dt);

public:
    Healer(float x, float y, int id);
    ~Healer();

    void Update() override;
    void Draw() override;
    void Damage(float damage) override
    {
        hp-=damage;
    }
};

#endif
