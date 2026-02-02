//
// Created by vanya on 1/24/26.
//

#ifndef ROGUELIKE_WEAPON_H
#define ROGUELIKE_WEAPON_H

#include "Entity.h"
#include <vector>

#include "PassiveItems.h"

enum class MODE
{
    HandPistol,
    Laser,
    FireExtinguisher
};

class Weapon : public Asset
{
    protected:
    MODE mode;
    float damage;
    float attackSpeed;
    float size;
    Entity &owner;
    bool ghost=false;
    bool ricochet=false;
    bool explosive=false;
    bool vampire=false;
    public:
    Weapon(MODE mode, Entity &owner);

    MODE GetMode() {return mode;}
    float GetDamage() {return damage;}
    float GetAttackSpeed() {return attackSpeed;}
    float GetSize() {return size;}
    void SetDefault()
    {
        damage = 2.0f;
        attackSpeed = 1.5f;
        size = 1.5f;
        ricochet = false;
        explosive = false;
        ghost = false;
        vampire = false;
    }
    Entity& GetOwner() {return owner;}

    bool IsGhost() {return ghost;}
    bool IsRicochet() {return ricochet;}
    void SetGhost(bool value) {ghost = value;}
    void SetRicochet(bool value) {ricochet = value;}
    void SetExplosive(bool value) {explosive = value;}
    void SetVampire(bool value) {vampire = value;}

    void ChangeDamage(float value, Oper op);
    void ChangeSize(float value, Oper op);
    void ChangeAttackSpeed(float value, Oper op);

    virtual void Shoot(Vector2 &start, Vector2 &dir, bool fire) = 0;
};

class HandPistol : public Weapon
{
    private:
    struct Bullet
    {
        Vector2 position;
        Vector2 velocity;
        float radius;
        float timer;
        int bounces;
        bool active;
        Asset* lastEntity;
    };

    std::vector<Bullet> bullets;
    float cooldown = 0.0f;

    Vector2 lastOwnerPos = {0,0};
    Vector2 lastMoveDir = {1,0};
    bool first = true;

    public:
    HandPistol(Entity &owner);
    void Update() override;
    void Draw() override;
    void Shoot(Vector2 &start, Vector2 &dir, bool fire) override;
};

class Laser : public Weapon
{
    private:
    float cooldown = 0.0f;
    float lazerTimer = 0.0f;
    std::vector<Vector2> points;

    Vector2 lastOwnerPos = {0,0};
    Vector2 lastMoveDir = {1,0};
    bool first = true;

    public:
    Laser(Entity &owner);
    void Update() override;
    void Draw() override;
    void Shoot(Vector2 &start, Vector2 &dir, bool fire) override;
};

class FireExtinguisher : public Weapon
{
    private:
    bool spraying = false;
    std::vector<Vector2> dots;

    Vector2 lastOwnerPos = {0,0};
    Vector2 lastMoveDir = {1,0};
    bool first = true;

    public:
    FireExtinguisher(Entity &owner);
    void Update() override;
    void Draw() override;
    void Shoot(Vector2 &start, Vector2 &dir, bool fire) override;
};

#endif //ROGUELIKE_WEAPON_H
