//
// Created by vanya on 2/2/26.
//

#include "Slingshoter.h"

#include "Collider.h"
#include "Player.h"
#include "Run.h"
#include "Weapon.h"

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
    float r = deg * DEG2RAD;
    float c = cosf(r);
    float s = sinf(r);
    return { v.x*c - v.y*s, v.x*s + v.y*c };
}

static Vector2 CenterFromRect(Rectangle r)
{
    return { r.x + r.width/2.0f, r.y + r.height/2.0f };
}

Slingshoter::Slingshoter(float x, float y, int id)
{
    tag = Tag::Enemy;
    this->id = id;

    hp = 25.0f;
    maxHP = 25.0f;

    speed = 160.0f;
    damage = 4.0f;
    size = 3.0f;
    scale = size/10.0f;

    position = {x,y};

    Asset::LoadEnemySprites("Assets/Sprites/enemies/shot", enemysprites);
    if (enemysprites) sprites = *enemysprites;

    frame = 8;
    direction = IDLE;

    if (!sprites.empty())
    {
        Rectangle r = { position.x, position.y, 100, 100 };
        collider = Collider(r, this);
        collider.Register();
    }

    weapon = make_unique<HandPistol>(*this);
    weapon->ChangeDamage(damage, Oper::Set);
    weapon->ChangeAttackSpeed(1.3f, Oper::Mul);

    moveDir = {1,0};
    moveTimer = 0.0f;
    rnd = 0;

    state = Moving;
    stateTimer = (float)Run::GenerateinRange(130, 230, tag, id) / 100.0f;
}

Slingshoter::~Slingshoter()
{
    collider.Unregister();
}

void Slingshoter::FindPlayer()
{
    if (player) return;

    auto& colliders = Collider::GetAllColliders();
    for (auto& c : colliders)
    {
        if (!c) continue;
        Asset* parent = c->GetParent();
        if (!parent) continue;
        if (parent->GetTag()!=Tag::Player) continue;
        player = dynamic_cast<Player*>(parent);
        if (player) break;
    }
}

bool Slingshoter::SolidHit()
{
    auto& colliders = Collider::GetAllColliders();
    for (auto& c : colliders)
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

bool Slingshoter::CanMoveTo(Vector2 pos)
{
    if (sprites.empty()) return false;

    Rectangle old = collider.GetRectangle();
    Rectangle r = { pos.x, pos.y, 100, 100 };
    collider.SetRectangle(r);

    bool hit = SolidHit();

    collider.SetRectangle(old);

    return !hit;
}

void Slingshoter::SetDir(Vector2 d)
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

void Slingshoter::Animate(float dt, bool moving)
{
    if (sprites.empty()) return;

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
        animationTimer = 0.0f;

        if (frame == (int)direction) frame = (int)direction + 1;
        else frame = (int)direction;
    }

    if (frame < 0 || frame >= (int)sprites.size()) frame = 8;

    Rectangle r = { position.x, position.y, 100, 100 };
    collider.SetRectangle(r);
}

void Slingshoter::Move(float dt, bool& moving)
{
    moveTimer -= dt;
    if (moveTimer <= 0.0f)
    {
        int a = Run::GenerateinRange(-180, 180, tag, id + rnd*19 + 3);
        moveDir = Rot({1,0}, (float)a);
        moveDir = Norm(moveDir);

        moveTimer = (float)Run::GenerateinRange(40, 90, tag, id + rnd*11 + 7) / 100.0f;
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

void Slingshoter::UpdateState(float dt)
{
    stateTimer -= dt;

    if (stateTimer > 0.0f) return;

    if (state == Moving)
    {
        state = Shooting;
        stateTimer = (float)Run::GenerateinRange(80, 160, tag, id + rnd*23 + 5) / 100.0f;
    }
    else
    {
        state = Moving;
        stateTimer = (float)Run::GenerateinRange(120, 220, tag, id + rnd*29 + 9) / 100.0f;
        moveTimer = 0.0f;
    }
}

void Slingshoter::Update()
{
    float dt = GetFrameTime();

    FindPlayer();
    UpdateState(dt);


    bool moving = false;
    if (state == Moving)
    {
        Move(dt, moving);
    }
    else if (weapon) weapon->Update();


    Animate(dt, moving);
}

void Slingshoter::Draw()
{
    if (sprites.empty()) return;
    if (frame < 0 || frame >= (int)sprites.size()) frame = 8;

    Rectangle r = collider.GetRectangle();
    Rectangle src = {0,0,(float)sprites[frame].width,(float)sprites[frame].height};
    DrawTexturePro(sprites[frame], src, r, {0,0}, 0.0f, WHITE);

    if (weapon) weapon->Draw();
}
