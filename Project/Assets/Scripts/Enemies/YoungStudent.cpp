// YoungStudent.cpp

#include "YoungStudent.h"

#include "Asset.h"
#include "Collider.h"
#include "Run.h"

#include <cmath>


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
    return { v.x*c - v.y*s, v.x*s + v.y*c };
}

YoungStudent::YoungStudent(float x, float y, int id)
{
    tag = Tag::Enemy;

    this->id = id;

    hp = 12.0f;
    maxHP = 12.0f;
    damage = 3.0f;

    speed = 750.0f;
    size = 1.0f;

    position = {x, y};

    Asset::LoadEnemySprites("Assets/Sprites/enemies/enemy1", enemySprites);

    frame = 8;
    direction = IDLE;

    moveDir = {1.0f, 0.0f};
    moveTimer = 0.0f;
    rnd = 0;

    Rectangle r = { position.x, position.y, 90.0f, 90.0f };
    collider = Collider(r, this);
    collider.Register();
}

YoungStudent::~YoungStudent()
{
    collider.Unregister();
}

void YoungStudent::Damage(float dmg)
{
    hp -= dmg;
}

bool YoungStudent::SolidHit()
{
    auto& colliders = Collider::GetAllColliders();
    for (auto& c : colliders)
    {
        if (!c) continue;
        if (c == &collider) continue;

        Asset* p = c->GetParent();
        if (!p) continue;
        if (p->GetTag() != Tag::Solid) continue;

        if (collider.IsColliding(*c)) return true;
    }
    return false;
}

bool YoungStudent::CanMoveTo(Vector2 pos)
{
    Rectangle old = collider.GetRectangle();

    Rectangle r = { pos.x, pos.y, 90.0f, 90.0f };
    collider.SetRectangle(r);

    bool hit = SolidHit();

    collider.SetRectangle(old);
    return !hit;
}

void YoungStudent::SetDir(Vector2 d)
{
    if (fabsf(d.x) > fabsf(d.y))
    {
        if (d.x > 0) direction = RIGHT;
        else direction = LEFT;
    }
    else
    {
        if (d.y > 0) direction = DOWN;
        else direction = UP;
    }
}

void YoungStudent::Animate(float dt, bool moving)
{
    if (!moving)
    {
        frame = 8;
        direction = IDLE;

        Rectangle r = { position.x, position.y, 90.0f, 90.0f };
        collider.SetRectangle(r);
        return;
    }

    animationTimer += dt;
    if (animationTimer >= 0.10f)
    {
        animationTimer = 0.0f;

        if (frame == (int)direction) frame = (int)direction + 1;
        else frame = (int)direction;
    }

    Rectangle r = { position.x, position.y, 90.0f, 90.0f };
    collider.SetRectangle(r);
}

void YoungStudent::Move(float dt, bool& moving)
{
    moveTimer -= dt;

    if (moveTimer <= 0.0f)
    {
        int a = Run::GenerateinRange(-180, 180, tag, id + rnd*19 + 3);
        moveDir = Norm(Rot({1,0}, (float)a));

        moveTimer = (float)Run::GenerateinRange(15, 45, tag, id + rnd*11 + 7) / 100.0f;
        rnd++;
    }

    float step = speed * dt;
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

void YoungStudent::Update()
{
    Asset::LoadEnemySprites("Assets/Sprites/Player", enemySprites);

    float dt = GetFrameTime();

    bool moving = false;

    Move(dt, moving);
    Animate(dt, moving);
}

void YoungStudent::Draw()
{
    if (!enemySprites) return;
    if ((int)enemySprites->size() <= 0) return;

    int f = frame;
    if (f < 0) f = 0;
    if (f >= (int)enemySprites->size()) f = 0;

    Rectangle dest = collider.GetRectangle();
    Rectangle src = { 0, 0, (float)(*enemySprites)[f].width, (float)(*enemySprites)[f].height };

    DrawTexturePro((*enemySprites)[f], src, dest, {0,0}, 0, WHITE);
}
