//
// Created by vanya on 2/1/26.
//

#include "Bully.h"

#include "Player.h"
#include "Collider.h"

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

static float Dist2(Vector2 a, Vector2 b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx*dx + dy*dy;
}

static Vector2 Rot(Vector2 v, float deg)
{
    float r = deg * DEG2RAD;
    float c = cosf(r);
    float s = sinf(r);
    return { v.x*c - v.y*s, v.x*s + v.y*c };
}

Bully::Bully(float x, float y, int id)
{
    tag = Tag::Enemy;
    this->id = id;
    speed = 175;
    size = 3.0f;
    maxHP=20;
    hp = 20;
    damage = 3;
    position = {x,y};

    scale = size/10;

    Asset::LoadEnemySprites("Assets/Sprites/enemies/bully", enemysprites);
    if (enemysprites) sprites = *enemysprites;
    frame = 8;
    if (sprites.size() > 0)
    {
        Rectangle r = { position.x, position.y, 100, 100 };
        collider = Collider(r, this);
        collider.Register();
    }
}

Bully::~Bully()
{
    collider.Unregister();
}

void Bully::FindPlayer()
{
    if (player) return;
    auto& colliders = Collider::GetAllColliders();
    for (auto &c : colliders)
    {
        if (!c) continue;
        Asset* p = c->GetParent();
        if (!p) continue;
        if (p->GetTag()!=Tag::Player) continue;
        player = dynamic_cast<Player*>(p);
        if (player) break;
    }
}

bool Bully::SolidHit()
{
    auto& colliders = Collider::GetAllColliders();
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

bool Bully::CanMoveTo(Vector2 pos)
{
    if (sprites.size() <= 0) return false;

    Rectangle old = collider.GetRectangle();

    Rectangle r = { pos.x, pos.y, 100, 100 };
    collider.SetRectangle(r);

    bool hit = SolidHit();

    collider.SetRectangle(old);

    return !hit;
}

void Bully::SetDir(Vector2 d)
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

void Bully::Animate(float dt, bool moving)
{
    if (!moving)
    {
        frame = 8;
        Rectangle r = { position.x, position.y, 100, 100 };
        collider.SetRectangle(r);
        return;
    }

    animationTimer += dt;
    if (animationTimer >= animationSpeed)
    {
        animationTimer = 0;
        if (frame == (int)direction) frame = (int)direction + 1;
        else frame = (int)direction;
    }
}

void Bully::Move(float dt, bool &moving)
{
    if (!player) return;
    if (sprites.size() <= 0) return;

    if (avoidTimer > 0) avoidTimer -= dt;

    Vector2 tp = player->GetPosition();
    Vector2 d = { tp.x - position.x, tp.y - position.y };
    if (Len(d) < 5.0f) return;

    Vector2 dir = Norm(d);

    float step = speed*dt;

    Vector2 forwardPos = { position.x + dir.x*step, position.y + dir.y*step };
    bool forwardFree = CanMoveTo(forwardPos);

    if (!forwardFree && avoidTimer <= 0.0f)
    {
        Vector2 rdir = Rot(dir, 90);
        Vector2 ldir = Rot(dir, -90);

        bool rfree = CanMoveTo({ position.x + rdir.x*step, position.y + rdir.y*step });
        bool lfree = CanMoveTo({ position.x + ldir.x*step, position.y + ldir.y*step });

        if (rfree && !lfree) avoidSign = 1;
        else if (!rfree && lfree) avoidSign = -1;
        else if (rfree && lfree)
        {
            float rd = Dist2({ position.x + rdir.x*step, position.y + rdir.y*step }, tp);
            float ld = Dist2({ position.x + ldir.x*step, position.y + ldir.y*step }, tp);
            avoidSign = (rd <= ld ? 1 : -1);
        }

        avoidTimer = 0.45f;
    }

    float base[] = { 0, 25, 50, 90, 135, 180 };

    bool found = false;
    Vector2 bestPos = position;
    float bestDist = Dist2(position, tp);

    for (int i = 0; i < (int)(sizeof(base)/sizeof(base[0])); i++)
    {
        float a = base[i];
        if (a != 0) a *= (float)avoidSign;

        Vector2 nd = Rot(dir, a);
        Vector2 np = { position.x + nd.x*step, position.y + nd.y*step };

        if (!CanMoveTo(np)) continue;

        float dd = Dist2(np, tp);
        if (!found || dd < bestDist)
        {
            found = true;
            bestDist = dd;
            bestPos = np;
        }
    }

    if (!found)
    {
        avoidSign = -avoidSign;
        for (int i = 0; i < (int)(sizeof(base)/sizeof(base[0])); i++)
        {
            float a = base[i];
            if (a != 0) a *= (float)avoidSign;

            Vector2 nd = Rot(dir, a);
            Vector2 np = { position.x + nd.x*step, position.y + nd.y*step };

            if (!CanMoveTo(np)) continue;

            float dd = Dist2(np, tp);
            if (!found || dd < bestDist)
            {
                found = true;
                bestDist = dd;
                bestPos = np;
            }
        }
        avoidTimer = 0.45f;
    }

    if (!found) return;

    Vector2 old = position;
    position = bestPos;

    Rectangle r = { position.x, position.y, 100, 100 };
    collider.SetRectangle(r);

    Vector2 mv = { position.x - old.x, position.y - old.y };
    if (fabsf(mv.x) > 0.01f || fabsf(mv.y) > 0.01f)
    {
        moving = true;
        SetDir(mv);
    }
}

void Bully::Update()
{
    FindPlayer();
    float dt = GetFrameTime();

    bool moving = false;
    Move(dt, moving);
    Animate(dt, moving);
}

void Bully::Draw()
{
    if (sprites.size() <= 0) return;
    if (frame < 0 || frame >= (int)sprites.size()) frame = 8;
    Texture2D &tex = sprites[frame];
    DrawTextureEx(tex, position, 0.0f, scale, WHITE);
}

void Bully::Damage(float damage)
{
    hp -= damage;
}


