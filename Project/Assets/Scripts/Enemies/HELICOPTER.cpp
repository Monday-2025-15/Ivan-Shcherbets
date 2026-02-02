#include "HELICOPTER.h"

#include "Player.h"
#include "Collider.h"
#include "Run.h"
#include "Weapon.h"

#include <cmath>

static Texture2D heliTex;
static bool g_heliLoaded = false;

static void LoadHeliTexture()
{
    if (g_heliLoaded) return;
    heliTex = LoadTexture("Assets/Sprites/enemies/HELICOPTER.png");
    g_heliLoaded = true;
}

static float Dist2(Vector2 a, Vector2 b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx*dx + dy*dy;
}

static Vector2 Norm(Vector2 v)
{
    float l = sqrtf(v.x*v.x + v.y*v.y);
    if (l < 0.0001f) return {1,0};
    return { v.x/l, v.y/l };
}

HELICOPTER::HELICOPTER(float x, float y, int id)
{
    tag = Tag::Enemy;

    this->id = id;

    maxHP = 200.0f;
    hp = 200.0f;

    size = 3.0f;
    speed = 300.0f;

    damage = 0.0f;

    LoadHeliTexture();

    circle = Circle({x+60, y+60}, 45.0f * size);
    collider = Collider(circle, this);
    collider.Register();

    rotation = RandomAngle();
    newAngle = rotation;

    weapon = new HandPistol(*this);
    weapon->ChangeDamage(2,Oper::Mul);
    weapon->ChangeAttackSpeed(1.75, Oper::Mul);
    weapon->ChangeSize(0.75, Oper::Mul);
}

HELICOPTER::~HELICOPTER()
{
    collider.Unregister();
    delete weapon;
}

void HELICOPTER::FindPlayerPtr()
{
    if (player) return;

    auto& cols = Collider::GetAllColliders();
    for (auto& c : cols)
    {
        if (!c) continue;
        Asset* p = c->GetParent();
        if (p && p->GetTag() == Tag::Player)
        {
            player = dynamic_cast<Player*>(p);
            return;
        }
    }
}

float HELICOPTER::RandomAngle()
{
    return (float)Run::GenerateinRange(10, 360, tag, id);
}

void HELICOPTER::Move(float dt)
{
    Vector2 old = circle.center;

    float r = rotation * DEG2RAD;
    circle.center.x -= cosf(r) * speed * dt;
    circle.center.y -= sinf(r) * speed * dt;

    collider.SetCircle(circle);
    CollisionCheck(old);
}

void HELICOPTER::CollisionCheck(Vector2 oldpos)
{
    auto& cols = Collider::GetAllColliders();
    float dt = GetFrameTime();

    for (auto& c : cols)
    {
        if (!c || c == &collider) continue;
        Asset* p = c->GetParent();
        if (!p) continue;

        if (p->GetTag() == Tag::Solid && collider.IsColliding(*c))
        {
            float r = rotation * DEG2RAD;
            circle.center.x += cosf(r) * (speed*dt + 2);
            circle.center.y += sinf(r) * (speed*dt + 2);
            collider.SetCircle(circle);
            moving = false;
        }
    }
}

void HELICOPTER::TryStartStrike(float dt)
{
    if (warning || rocketFlying || exploding) return;

    strikeTick += dt;
    if (strikeTick < 5.0f) return;
    strikeTick -= 5.0f;

    if (!Run::GenerateDecision(50, tag, id + strikeRnd++)) return;

    FindPlayerPtr();
    if (!player) return;

    strikePos = player->GetPosition();
    warning = true;
    warningTimer = 3.0f;
}

void HELICOPTER::UpdateStrike(float dt)
{
    if (warning)
    {
        warningTimer -= dt;
        if (warningTimer <= 0)
        {
            warning = false;
            rocketFlying = true;

            rocketPos = circle.center;
            rocketTarget = strikePos;

            Vector2 dir = Norm({rocketTarget.x - rocketPos.x,
                                rocketTarget.y - rocketPos.y});

            rocketVel = {dir.x * 800.0f, dir.y * 800.0f};
        }
        return;
    }

    if (rocketFlying)
    {
        rocketPos.x += rocketVel.x * dt;
        rocketPos.y += rocketVel.y * dt;

        if (Dist2(rocketPos, rocketTarget) < 25*25)
        {
            rocketFlying = false;
            exploding = true;
            boomLife = 0.7f;
            boomT = boomLife;
            boomR = 325.0f;

            FindPlayerPtr();
            if (player && Dist2(player->GetPosition(), rocketTarget) < boomR*boomR)
                player->Damage(4.0f);
        }
        return;
    }

    if (exploding)
    {
        boomT -= dt;
        if (boomT <= 0) exploding = false;
    }
}

void HELICOPTER::Update()
{
    float dt = GetFrameTime();

    TryStartStrike(dt);
    UpdateStrike(dt);

    if (!moving)
    {
        rotation = RandomAngle();
        moving = true;
    }
    else
    {
        Move(dt);
    }

    if (weapon) weapon->Update();
}

void HELICOPTER::Draw()
{
    Rectangle src = {0,0,(float)heliTex.width,(float)heliTex.height};

    float w = circle.radius*2;
    Rectangle dst = {circle.center.x, circle.center.y, w, w};
    Vector2 o = {w/2,w/2};

    DrawTexturePro(heliTex, src, dst, o, rotation, WHITE);

    if (weapon) weapon->Draw();

    if (warning)
    {
        float warnR = 250.0f;

        int blink = ((int)(GetTime() * 10.0)) % 2;
        unsigned char a = blink ? 150 : 125;
        Color y = { 255, 255, 0, a };
        DrawCircleV(strikePos, warnR, y);
    }

    if (rocketFlying)
        DrawCircleV(rocketPos, 14, RED);

    if (exploding)
    {
        float k = 1.0f - boomT/boomLife;
        DrawCircleV(rocketTarget, boomR*k, {255,0,0,(unsigned char)(200*(1-k))});
    }
}

void HELICOPTER::Damage(float dmg)
{
    hp -= dmg;
}
