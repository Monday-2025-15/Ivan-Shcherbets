//
// Created by user on 10.11.2025.
//

#ifndef ROGUELIKE_ENTITY_H
#define ROGUELIKE_ENTITY_H

#include "Asset.h"
#include <raylib.h>
#include <Collider.h>

class Entity : public Asset
{
    protected:
    Collider collider;
    float maxHP;
    float hp;
    float speed;
    float size;
    float damage;
    public:
    float GetHP() { return hp; }
    virtual void Damage(float damage){};
    float GetDamage() { return damage; }
    bool IsDead() { return hp <= 0.0f; }
    void AddHealth(float health)
    {
        hp+=health;
        if (hp>maxHP) hp=maxHP;
    }
    float GetMaxHP() { return maxHP; }
};


#endif //ROGUELIKE_ENTITY_H