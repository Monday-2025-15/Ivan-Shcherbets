#include "PhysicTeacher.h"

#include "Weapon.h"

#include <filesystem>
#include <algorithm>

using namespace std;


PhysicTeacher::PhysicTeacher(float x, float y, int id)
    : Security(x, y, id)
{
    hp = 40.0f;
    maxHP = 40.0f;

    speed = 175.0f;

    size = 3.0f;
    scale = size/10.0f;


    Asset::LoadEnemySprites("Assets/Sprites/enemies/Phys", enemysprites);
    if (!enemysprites)
        Asset::LoadEnemySprites("Assets/Sprite/enemies/Phys", enemysprites);
    if (enemysprites) sprites = *enemysprites;


    frame = 8;
    direction = 8;
    animationTimer = 0.0f;

    if (!sprites.empty())
    {
        Rectangle r = { position.x, position.y, sprites[8].width*scale, sprites[8].height*scale };
        collider.SetRectangle(r);
    }

    weapon = make_unique<HandPistol>(*this);
    weapon->ChangeAttackSpeed(1.5, Oper::Mul);
    weapon->ChangeDamage(5, Oper::Set);
    weapon->ChangeSize(0.5, Oper::Mul);
    weapon->SetRicochet(true);
}
