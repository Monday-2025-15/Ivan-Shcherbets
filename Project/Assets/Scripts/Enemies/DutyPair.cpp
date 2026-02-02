#include "DutyPair.h"

#include "Collider.h"
#include "Run.h"
#include "Player.h"

#include <cmath>

using namespace std;

static Texture2D g_tex;
static bool g_loaded = false;

static void LoadTexOnce()
{
    if (g_loaded) return;

    g_tex = LoadTexture("Assets/Sprites/enemies/twins.png");
    if (g_tex.id == 0)
        g_tex = LoadTexture("Assets/Sprites/enemies/mouse.png");

    g_loaded = true;
}

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

DutyPair::DutyPair(float x, float y, int id)
{
    tag = Tag::Enemy;

    this->id = id;

    hp = 75.0f;
    maxHP = 75.0f;
    damage = 7.0f;

    speed = 220.0f;
    size = 4.0f;
    scale = size/10.0f;

    position = {x, y};

    LoadTexOnce();

    Rectangle r = { position.x, position.y, 512.0f*scale, 512.0f*scale };
    collider = Collider(r, this);
    collider.Register();

    player = nullptr;
    lastPlayerPos = {0,0};

    moveDir = {1,0};
    moveTimer = 0.0f;
    rnd = 0;
    weapon = make_unique<FireExtinguisher>(*this);
    weapon->SetDefault();
    weapon->ChangeDamage(1.5,Oper::Mul);
    weapon->ChangeSize(1.5,Oper::Mul);
}

DutyPair::~DutyPair()
{
    collider.Unregister();
}

void DutyPair::Damage(float dmg)
{
    hp -= dmg;
}

void DutyPair::FindPlayer()
{
    if (player) return;

    auto& colliders = Collider::GetAllColliders();
    for (auto& c : colliders)
    {
        if (!c) continue;
        Asset* p = c->GetParent();
        if (!p) continue;
        if (p->GetTag() != Tag::Player) continue;

        player = dynamic_cast<Player*>(p);
        if (player)
        {
            lastPlayerPos = player->GetPosition();
            break;
        }
    }
}

bool DutyPair::PlayerMoving() const
{
    if (!player) return true;
    Vector2 now = player->GetPosition();
    Vector2 d = { now.x - lastPlayerPos.x, now.y - lastPlayerPos.y };
    return (fabsf(d.x) > 0.05f || fabsf(d.y) > 0.05f);
}

Vector2 DutyPair::Center() const
{
    Rectangle r = collider.GetRectangle();
    return { r.x + r.width/2.0f, r.y + r.height/2.0f };
}

bool DutyPair::SolidHit() const
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

bool DutyPair::CanMoveTo(Vector2 pos)
{
    Rectangle old = collider.GetRectangle();
    Rectangle r = { pos.x, pos.y, old.width, old.height };
    collider.SetRectangle(r);

    bool hit = SolidHit();

    collider.SetRectangle(old);
    return !hit;
}

void DutyPair::MoveAI(float dt)
{
    if (!player) return;

    Vector2 from = Center();
    Vector2 to = player->GetPosition();
    Vector2 wish = Norm({ to.x - from.x, to.y - from.y });

    moveTimer -= dt;
    if (moveTimer <= 0.0f)
    {
        int a = Run::GenerateinRange(-25, 25, tag, id + rnd*19 + 3);
        rnd++;
        moveDir = Norm(Rot(wish, (float)a));

        moveTimer = (float)Run::GenerateinRange(20, 70, tag, id + rnd*11 + 7) / 100.0f;
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
        position = np;

    Rectangle rr = collider.GetRectangle();
    rr.x = position.x;
    rr.y = position.y;
    collider.SetRectangle(rr);
}

void DutyPair::Regen(float dt)
{
    hp += 10.0f * dt;
    if (hp > maxHP) hp = maxHP;
}

void DutyPair::Update()
{
    FindPlayer();

    float dt = GetFrameTime();

    if (player)
    {
        Vector2 now = player->GetPosition();
        Vector2 d = { now.x - lastPlayerPos.x, now.y - lastPlayerPos.y };

        if (fabsf(d.x) > 0.05f || fabsf(d.y) > 0.05f)
            stillTimer = 0.0f;
        else
            stillTimer += dt;

        lastPlayerPos = now;
    }
    else
    {
        stillTimer = 0.0f;
    }

    if (stillTimer >= 0.5f)
        Regen(dt);
    else
    {
        MoveAI(dt);
        weapon->Update();
    }
}

void DutyPair::Draw()
{
    if (!g_loaded || g_tex.id == 0) return;

    Rectangle dest = collider.GetRectangle();
    Rectangle src = { 0, 0, (float)g_tex.width, (float)g_tex.height };
    if (stillTimer < 0.5f) weapon->Draw();
    DrawTexturePro(g_tex, src, dest, {0,0}, 0.0f, WHITE);
}
