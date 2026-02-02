// YoungStudent.h

#ifndef ROGUELIKE_YOUNGSTUDENT_H
#define ROGUELIKE_YOUNGSTUDENT_H

#include "Entity.h"

#include <vector>

class YoungStudent : public Entity
{
private:
    int id = 0;

    enum Direction
    {
        DOWN = 0,
        RIGHT = 2,
        LEFT = 4,
        UP = 6,
        IDLE = 8
    };

    Direction direction = IDLE;
    int frame = 8;
    float animationTimer = 0.0f;

    Vector2 moveDir{1.0f, 0.0f};
    float moveTimer = 0.0f;
    int rnd = 0;

    std::vector<Texture2D>* enemySprites = nullptr;

    bool SolidHit();
    bool CanMoveTo(Vector2 pos);
    void SetDir(Vector2 d);
    void Animate(float dt, bool moving);
    void Move(float dt, bool& moving);

public:
    YoungStudent(float x, float y, int id);
    ~YoungStudent();

    void Update() override;
    void Draw() override;
    void Damage(float damage) override;
};

#endif
