#include "Security.h"

#include "Player.h"
#include "Weapon.h"
#include "Collider.h"
#include "Run.h"

#include <filesystem>
#include <algorithm>
#include <cmath>

using namespace std;

static float Len(Vector2 v)
{
    return sqrtf(v.x*v.x + v.y*v.y);
}

static Vector2 Norm(Vector2 v)
{
    float l = Len(v);
    if (l <= 0.0001f) return {1,0};
    return { v.x/l, v.y/l };
}

static Vector2 Rot(Vector2 v, float deg)
{
    float r = deg*DEG2RAD;
    float c = cosf(r);
    float s = sinf(r);
    return { v.x*c - v.y*s, v.y*c + v.x*s };
}

Security::Security(float x, float y, int id)
{
    tag = Tag::Enemy;

    this->id = id;

    hp = 35.0f;
    maxHP = 35.0f;

    speed = 160.0f;
    damage = 7;
    size = 3.0f;
    scale = size/10.0f;

    position = {x,y};

    Asset::LoadEnemySprites("Assets/Sprites/enemies/security", enemysprites);
    if (enemysprites) sprites = *enemysprites;

    frame = 8;
    direction = 8;
    animationTimer = 0.0f;

    moveDir = {1,0};
    moveTimer = 0.0f;
    rnd = 0;

    if (!sprites.empty())
    {
        Rectangle r = { position.x, position.y, sprites[8].width*scale, sprites[8].height*scale };
        collider = Collider(r, this);
        collider.Register();
    }

    weapon = make_unique<HandPistol>(*this);
    weapon->ChangeSize(1.5,Oper::Mul);
    weapon->ChangeAttackSpeed(0.75, Oper::Mul);
    weapon->ChangeDamage(damage, Oper::Set);
    FindPlayer();
}

Security::~Security()
{
    collider.Unregister();
}

void Security::FindPlayer()
{
    if (player) return;

    auto &colliders = Collider::GetAllColliders();
    for (auto &c : colliders)
    {
        if (!c) continue;
        Asset* parent = c->GetParent();
        if (!parent) continue;

        if (parent->GetTag()==Tag::Player)
        {
            player = dynamic_cast<Player*>(parent);
            break;
        }
    }
}

bool Security::SolidHit()
{
    auto &colliders = Collider::GetAllColliders();
    for (auto &c : colliders)
    {
        if (!c) continue;
        if (c==&collider) continue;
        Asset* p = c->GetParent();
        if (!p) continue;
        if (p->GetTag()!=Tag::Solid) continue;
        if (collider.IsColliding(*c)) return true;
    }
    return false;
}

bool Security::CanMoveTo(Vector2 pos)
{
    if (sprites.empty()) return false;

    Rectangle old = collider.GetRectangle();
    Rectangle r = { pos.x, pos.y, sprites[8].width*scale, sprites[8].height*scale };
    collider.SetRectangle(r);

    bool hit = SolidHit();
    collider.SetRectangle(old);

    return !hit;
}

void Security::SetDir(Vector2 d)
{
    if (fabsf(d.x) > fabsf(d.y))
    {
        if (d.x > 0) direction = 6;
        else direction = 4;
    }
    else
    {
        if (d.y > 0) direction = 2;
        else direction = 0;
    }
}

void Security::Animate(float dt, bool moving)
{
    if (sprites.empty()) return;

    if (!moving)
    {
        frame = 8;
        Rectangle r = { position.x, position.y, sprites[8].width*scale, sprites[8].height*scale };
        collider.SetRectangle(r);
        return;
    }

    animationTimer += dt;
    if (animationTimer >= animationSpeed)
    {
        animationTimer = 0.0f;
        frame++;
        if (frame > direction + 1) frame = direction;
    }

    Rectangle r = { position.x, position.y, sprites[frame].width*scale, sprites[frame].height*scale };
    collider.SetRectangle(r);
}

void Security::Move(float dt, bool& moving)
{
    moveTimer -= dt;

    if (moveTimer <= 0.0f)
    {
        int a = Run::GenerateinRange(-180, 180, tag, id + rnd*19 + 3);
        moveDir = Rot({1,0}, (float)a);
        moveDir = Norm(moveDir);
        moveTimer = (float)Run::GenerateinRange(60, 140, tag, id + rnd*11 + 7) / 100.0f;
        rnd++;
    }

    float step = speed*dt;
    Vector2 np = { position.x + moveDir.x*step, position.y + moveDir.y*step };

    if (!CanMoveTo(np))
    {
        Vector2 r = Rot(moveDir, 90);
        Vector2 l = Rot(moveDir, -90);

        Vector2 np1 = { position.x + r.x*step, position.y + r.y*step };
        Vector2 np2 = { position.x + l.x*step, position.y + l.y*step };

        if (CanMoveTo(np1)) moveDir = Norm(r);
        else if (CanMoveTo(np2)) moveDir = Norm(l);
        else moveDir = { -moveDir.x, -moveDir.y };

        np = { position.x + moveDir.x*step, position.y + moveDir.y*step };
        moveTimer = 0.0f;
    }

    if (CanMoveTo(np))
    {
        Vector2 old = position;
        position = np;

        Vector2 mv = { position.x - old.x, position.y - old.y };
        if (fabsf(mv.x) > 0.01f || fabsf(mv.y) > 0.01f)
        {
            moving = true;
            SetDir(mv);
        }
    }
}

void Security::Update()
{
    float dt = GetFrameTime();

    FindPlayer();

    bool moving = false;

    Move(dt, moving);
    Animate(dt, moving);

    if (weapon) weapon->Update();
}

void Security::Draw()
{
    if (sprites.empty()) return;

    Rectangle r = collider.GetRectangle();
    Rectangle src = {0,0,(float)sprites[frame].width,(float)sprites[frame].height};

    DrawTexturePro(sprites[frame], src, r, {0,0}, 0, WHITE);

    if (weapon) weapon->Draw();
}
