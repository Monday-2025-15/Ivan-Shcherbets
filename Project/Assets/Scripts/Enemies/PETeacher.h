// PETeacher.h
//
// Created by vanya on 2/2/26.
//

#ifndef ROGUELIKE_PETEACHER_H
#define ROGUELIKE_PETEACHER_H

#include "Entity.h"
#include <vector>

class Player;
class Collider;

class PETeacher : public Entity
{
private:
    struct Ball
    {
        Vector2 pos;
        Vector2 vel;
        float radius = 40.0f;
    };

    int id = 0;

    Player* player = nullptr;
    Collider* playerCollider = nullptr;

    std::vector<Texture2D>* enemySprites = nullptr;

    enum Direction
    {
        DOWN  = 0,
        RIGHT = 2,
        LEFT  = 4,
        UP    = 6,
        IDLE  = 8
    };

    Direction direction = IDLE;

    int frame = 8;
    float animationTimer = 0.0f;

    Vector2 moveDir{1.0f, 0.0f};
    float moveTimer = 0.0f;
    int rnd = 0;

    float ballTick = 0.0f;
    std::vector<Ball> balls;

    void FindPlayerPtr();

    bool SolidCircleHit(Vector2 c, float r) const;
    bool PlayerCircleHit(Vector2 c, float r) const;

    bool SolidHit() const;
    bool CanMoveTo(Vector2 pos);

    void SetDir(Vector2 d);
    void Animate(float dt, bool moving);
    void Move(float dt, bool& moving);

    void SpawnBall();
    void UpdateBalls(float dt);
    void DrawBalls() const;

public:
    PETeacher(float x, float y, int id);
    ~PETeacher();

    void Update() override;
    void Draw() override;
    void Damage(float damage) override;
};

#endif //ROGUELIKE_PETEACHER_H
