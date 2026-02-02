//
// Created by vanya on 1/12/26.
//

#ifndef ROGUELIKE_MOUSE_H
#define ROGUELIKE_MOUSE_H
#include "Entity.h"
#include "Player.h"


class Mouse : public Entity
{
    int id;
    Texture2D* texture = nullptr;
    Collider collider;
    float rotation = 0.0f;
    Rectangle rect;
    Player* player;
    bool moving;
    bool rotating;
    float newAngle;
    Circle circle;
    bool boss = false;
    public:
    Mouse(float x, float y, int id, bool boss=false);
    ~Mouse();
    void Draw() override;
    void SetRotation(float angle);
    void Rotate(float deltaAngle);
    void Update() override;
    void Move(float dt);
    float AngleToPlayer();
    float RandomAngle();
    void CollisionCheck(Vector2 oldPos);
    void Damage(float damage) override;
};


#endif //ROGUELIKE_MOUSE_H