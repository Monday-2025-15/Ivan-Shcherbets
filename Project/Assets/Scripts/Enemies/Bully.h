//
// Created by vanya on 2/1/26.
//

#ifndef ROGUELIKE_BULLY_H
#define ROGUELIKE_BULLY_H
#include "Entity.h"

class Player;

class Bully : public Entity
{
protected:
    int id;
    Player* player = nullptr;
    float scale;

    enum Direction {
        DOWN = 0,
        RIGHT = 2,
        LEFT = 4,
        UP = 6,
        IDLE = 8
    };
    Direction direction = IDLE;

    int avoidSign = 1;
    float avoidTimer = 0.0f;
    std::vector<Texture2D>* enemysprites = nullptr;
    void FindPlayer();
    bool SolidHit();
    bool CanMoveTo(Vector2 pos);
    void SetDir(Vector2 d);
    void Animate(float dt, bool moving);
    void Move(float dt, bool &moving);

public:
    Bully(float x, float y, int id);
    ~Bully();
    void Draw() override;
    void Update() override;
    void Damage(float damage) override;
};

#endif //ROGUELIKE_BULLY_H
