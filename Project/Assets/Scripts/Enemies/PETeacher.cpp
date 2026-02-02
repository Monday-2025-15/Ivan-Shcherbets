// PETeacher.cpp

#include "PETeacher.h"

#include "Collider.h"
#include "Run.h"
#include "Player.h"

#include <cmath>

using namespace std;

static float Len(Vector2 v)
{
    return sqrtf(v.x*v.x + v.y*v.y);
}

static Vector2 Norm(Vector2 v)
{
    float l = Len(v);
    if (l <= 0.0001f) return {1, 0};
    return { v.x/l, v.y/l };
}

static Vector2 Rot(Vector2 v, float deg)
{
    float r = deg*DEG2RAD;
    float c = cosf(r);
    float s = sinf(r);
    return { v.x*c - v.y*s, v.x*s + v.y*c };
}

PETeacher::PETeacher(float x, float y, int id)
{
    tag = Tag::Enemy;

    this->id = id;

    hp = 75.0f;
    maxHP = 75.0f;
    damage = 5.0f;

    speed = 260.0f;
    size = 1.0f;

    position = {x, y};

    Asset::LoadEnemySprites("Assets/Sprites/enemoies/PE", enemySprites);

    frame = 8;
    direction = IDLE;
    animationTimer = 0.0f;

    moveDir = {1.0f, 0.0f};
    moveTimer = 0.0f;
    rnd = 0;

    ballTick = 0.0f;
    balls.clear();

    Rectangle r = { position.x, position.y, 150.0f, 150.0f };
    collider = Collider(r, this);
    collider.Register();
}

PETeacher::~PETeacher()
{
    balls.clear();
    collider.Unregister();
}

void PETeacher::Damage(float dmg)
{
    hp -= dmg;
    if (hp <= 0.0f)
    {
        hp = 0.0f;
        balls.clear();
    }
}

void PETeacher::FindPlayerPtr()
{
    if (player && playerCollider) return;

    auto& cols = Collider::GetAllColliders();
    for (auto& c : cols)
    {
        if (!c) continue;
        Asset* parent = c->GetParent();
        if (!parent) continue;

        if (parent->GetTag() == Tag::Player)
        {
            player = dynamic_cast<Player*>(parent);
            playerCollider = c;
            break;
        }
    }
}

bool PETeacher::SolidCircleHit(Vector2 c, float r) const
{
    auto& cols = Collider::GetAllColliders();
    for (auto& col : cols)
    {
        if (!col) continue;
        Asset* p = col->GetParent();
        if (!p) continue;
        if (p->GetTag() != Tag::Solid) continue;

        ColliderType t = col->GetType();
        if (t == ColliderType::CIRCLE)
        {
            Circle cc = col->GetCircle();
            if (CheckCollisionCircles(c, r, cc.center, cc.radius)) return true;
        }
        else
        {
            Rectangle rr = col->GetRectangle();
            if (CheckCollisionCircleRec(c, r, rr)) return true;
        }
    }
    return false;
}

bool PETeacher::PlayerCircleHit(Vector2 c, float r) const
{
    if (!playerCollider) return false;

    ColliderType t = playerCollider->GetType();
    if (t == ColliderType::CIRCLE)
    {
        Circle pc = playerCollider->GetCircle();
        return CheckCollisionCircles(c, r, pc.center, pc.radius);
    }

    Rectangle pr = playerCollider->GetRectangle();
    return CheckCollisionCircleRec(c, r, pr);
}

bool PETeacher::SolidHit() const
{
    auto& cols = Collider::GetAllColliders();
    for (auto& c : cols)
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

bool PETeacher::CanMoveTo(Vector2 pos)
{
    Rectangle old = collider.GetRectangle();

    Rectangle r = { pos.x, pos.y, 150.0f, 150.0f };
    collider.SetRectangle(r);

    bool hit = SolidHit();

    collider.SetRectangle(old);
    return !hit;
}

void PETeacher::SetDir(Vector2 d)
{
    if (fabsf(d.x) > fabsf(d.y))
    {
        direction = (d.x > 0 ? RIGHT : LEFT);
    }
    else
    {
        direction = (d.y > 0 ? DOWN : UP);
    }
}

void PETeacher::Animate(float dt, bool moving)
{
    if (!moving)
    {
        frame = 8;
        direction = IDLE;

        Rectangle r = { position.x, position.y, 150.0f, 150.0f };
        collider.SetRectangle(r);
        return;
    }

    animationTimer += dt;
    if (animationTimer >= 0.15f)
    {
        animationTimer = 0.0f;

        if (frame == (int)direction) frame = (int)direction + 1;
        else frame = (int)direction;
    }

    Rectangle r = { position.x, position.y, 150.0f, 150.0f };
    collider.SetRectangle(r);
}

void PETeacher::Move(float dt, bool& moving)
{
    moveTimer -= dt;

    if (moveTimer <= 0.0f)
    {
        int a = Run::GenerateinRange(-180, 180, tag, id + rnd*19 + 3);
        moveDir = Norm(Rot({1,0}, (float)a));
        moveTimer = (float)Run::GenerateinRange(30, 90, tag, id + rnd*11 + 7) / 100.0f;
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

void PETeacher::SpawnBall()
{
    Ball b;
    b.radius = 40.0f;

    b.pos = { position.x + 75.0f, position.y + 75.0f };

    int a = Run::GenerateinRange(0, 359, tag, id + rnd*97 + 200);
    rnd++;

    Vector2 d = Norm(Rot({1,0}, (float)a));
    float sp = 520.0f;

    b.vel = { d.x*sp, d.y*sp };

    balls.push_back(b);
}

void PETeacher::UpdateBalls(float dt)
{
    for (auto& b : balls)
    {
        Vector2 p = b.pos;
        Vector2 v = b.vel;
        float r = b.radius;

        Vector2 px = { p.x + v.x*dt, p.y };
        if (SolidCircleHit(px, r)) v.x = -v.x;
        else p.x = px.x;

        Vector2 py = { p.x, p.y + v.y*dt };
        if (SolidCircleHit(py, r)) v.y = -v.y;
        else p.y = py.y;

        b.pos = p;
        b.vel = v;

        if (player && playerCollider && PlayerCircleHit(b.pos, b.radius))
        {
            player->Damage(1.0f);

            Rectangle pr = playerCollider->GetRectangle();
            Vector2 pc = { pr.x + pr.width/2.0f, pr.y + pr.height/2.0f };

            float dx = b.pos.x - pc.x;
            float dy = b.pos.y - pc.y;

            if (fabsf(dx) > fabsf(dy)) b.vel.x = -b.vel.x;
            else b.vel.y = -b.vel.y;
        }
    }
}

void PETeacher::DrawBalls() const
{
    for (auto& b : balls)
    {
        DrawCircleV(b.pos, b.radius, BLUE);
        DrawCircleLines((int)b.pos.x, (int)b.pos.y, b.radius, DARKBLUE);
    }
}

void PETeacher::Update()
{
    if (hp <= 0.0f)
    {
        balls.clear();
        return;
    }

    FindPlayerPtr();

    float dt = GetFrameTime();

    bool moving = false;
    Move(dt, moving);
    Animate(dt, moving);

    ballTick += dt;
    while (ballTick >= 2.0f)
    {
        ballTick -= 2.0f;

        int idx = id + rnd*101 + 500;
        rnd++;

        if (Run::GenerateDecision(33, tag, idx))
            SpawnBall();
    }

    UpdateBalls(dt);
}

void PETeacher::Draw()
{
    if (hp <= 0.0f) return;

    Rectangle dest = { position.x, position.y, 150.0f, 150.0f };

    if (enemySprites && !enemySprites->empty())
    {
        int f = frame;
        if (f < 0) f = 0;
        if (f >= (int)enemySprites->size()) f = 0;

        Texture2D& tex = (*enemySprites)[f];
        Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };

        DrawTexturePro(tex, src, dest, {0,0}, 0, WHITE);
    }
    else
    {
        DrawRectangleRec(dest, GRAY);
        DrawRectangleLinesEx(dest, 2, BLACK);
    }

    DrawBalls();
}
