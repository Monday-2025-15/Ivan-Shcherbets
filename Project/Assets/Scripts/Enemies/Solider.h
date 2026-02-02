//
// Created by vanya on 2/2/26.
//

#ifndef ROGUELIKE_SOLIDER_H
#define ROGUELIKE_SOLIDER_H

#include "Security.h"
#include <vector>

class Player;

class Solider : public Security
{
private:
    struct Grenade
    {
        Vector2 start;
        Vector2 target;
        Vector2 pos;

        float flyT;
        float flyDur;
        float arcH;

        bool landed;
        float fuse;

        bool exploded;
        float boomT;
        float boomLife;
        float boomR;
    };


    float grenadeTick = 0.0f;

    std::vector<Grenade> grenades;

    Collider* playerCollider = nullptr;
    Player* player = nullptr;

    void FindPlayerPtr();
    void TryThrowGrenade(float dt);
    void UpdateGrenades(float dt);
    void Explode(Grenade& g);

public:
    Solider(float x, float y, int id);
    ~Solider();

    void Update() override;
    void Draw() override;
    void Damage(float dmg) override;
};

#endif //ROGUELIKE_SOLIDER_H
