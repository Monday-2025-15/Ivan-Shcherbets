#include "Healer.h"

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
    return { v.x*c - v.y*s, v.x*s + v.y*c };
}

struct HealFx
{
    Vector2 pos;
    float r;
    float maxR;
    float t;
    float life;
};

static vector<HealFx> fx;

static void HealEnemies(Vector2 pos, float radius, float heal, Asset* skip)
{
    auto &colliders = Collider::GetAllColliders();
    Collider temp(Circle{pos, radius}, nullptr);

    for (auto &c : colliders)
    {
        if (!c) continue;
        Asset* parent = c->GetParent();
        if (!parent) continue;
        if (parent == skip) continue;
        if (parent->GetTag()!=Tag::Enemy) continue;
        if (!temp.IsColliding(*c)) continue;

        Entity* e = dynamic_cast<Entity*>(parent);
        if (!e) continue;
        e->AddHealth(heal);
    }
}


static void SpawnHeal(Vector2 pos, float radius, float heal, Asset* skip)
{
    HealEnemies(pos, radius, heal, skip);

    HealFx h;
    h.pos = pos;
    h.r = 0.0f;
    h.maxR = radius;
    h.life = 1.2f;
    h.t = h.life;
    fx.push_back(h);
}

static void UpdateFxOnce()
{
    static double last = -1.0;
    double now = GetTime();
    if (last >= 0.0 && now - last < 0.00001) return;
    last = now;

    float dt = GetFrameTime();
    for (auto &e : fx) e.t -= dt;

    for (int i = (int)fx.size()-1; i >= 0; i--)
        if (fx[i].t <= 0.0f)
            fx.erase(fx.begin()+i);
}

static void DrawFxOnce()
{
    static double last = -1.0;
    double now = GetTime();
    if (last >= 0.0 && now - last < 0.00001) return;
    last = now;

    for (auto &e : fx)
    {
        float k = 1.0f - (e.t / e.life);
        if (k < 0) k = 0;
        if (k > 1) k = 1;

        float r = e.maxR * k;
        unsigned char a = (unsigned char)(200.0f * (1.0f - k));

        Color fill = {0,255,0,a};
        Color line = {0,160,0,a};

        DrawCircleV(e.pos, r, fill);
        DrawCircleLines((int)e.pos.x, (int)e.pos.y, r, line);
    }
}

Healer::Healer(float x, float y, int id)
{
    tag = Tag::Enemy;

    this->id = id;

    hp = 20;
    maxHP = 20;
    speed = 200;
    size = 2.5f;

    position = {x,y};

    scale = size/10.0f;

    Asset::LoadEnemySprites("Assets/Sprites/enemies/healer", enemysprites);
    if (enemysprites) sprites = *enemysprites;

    frame = 8;
    animationTimer = 0.0f;
    direction = IDLE;

    moveDir = {1,0};
    moveTimer = 0.0f;

    healTick = 0.0f;
    rnd = 0;

    if (sprites.size() > 0)
    {
        Rectangle r = { position.x, position.y, 100, 100 };
        collider = Collider(r, this);
        collider.Register();
    }
}

Healer::~Healer()
{
    collider.Unregister();
}

bool Healer::SolidHit()
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

bool Healer::CanMoveTo(Vector2 pos)
{
    if (sprites.size() <= 0) return false;

    Rectangle old = collider.GetRectangle();
    Rectangle r = { pos.x, pos.y, 100, 100 };
    collider.SetRectangle(r);

    bool hit = SolidHit();
    collider.SetRectangle(old);

    return !hit;
}

void Healer::SetDir(Vector2 d)
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

void Healer::Animate(float dt, bool moving)
{
    if (!moving)
    {
        frame = 8;
        Rectangle r = { position.x, position.y, 100, 100 };
        collider.SetRectangle(r);
        return;
    }

    animationTimer += dt;
    if (animationTimer >= 0.15f)
    {
        animationTimer = 0.0f;
        frame++;
        if (frame > (int)direction + 1) frame = (int)direction;
    }

    Rectangle r = { position.x, position.y, sprites[frame].width*scale, sprites[frame].height*scale };
    collider.SetRectangle(r);
}

void Healer::Move(float dt, bool &moving)
{
    moveTimer -= dt;

    if (moveTimer <= 0.0f)
    {
        int a = Run::GenerateinRange(-180, 180, tag, id + rnd*19 + 3);
        moveDir = Rot({1,0}, (float)a);
        moveDir = Norm(moveDir);
        moveTimer = (float)Run::GenerateinRange(40, 120, tag, id + rnd*11 + 7) / 100.0f;
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

void Healer::TryHeal(float dt)
{
    healTick += dt;
    if (healTick < 1.0f) return;
    healTick -= 1.0f;

    int idx = id + rnd*101 + 55;
    rnd++;

    if (!Run::GenerateDecision(25, tag, idx)) return;

    float heal = (float)Run::GenerateinRange(5, 15, tag, idx + 7);
    float radius = 120.0f * size;

    SpawnHeal(position, radius, heal, this);
}

void Healer::Update()
{
    UpdateFxOnce();

    float dt = GetFrameTime();

    bool moving = false;

    Move(dt, moving);
    Animate(dt, moving);

    TryHeal(dt);
}

void Healer::Draw()
{
    DrawFxOnce();

    if (sprites.size() <= 0) return;

    Rectangle r = collider.GetRectangle();
    Rectangle src = {0,0,(float)sprites[frame].width,(float)sprites[frame].height};

    DrawTexturePro(sprites[frame], src, r, {0,0}, 0, WHITE);
    collider.DebugDraw();
}
