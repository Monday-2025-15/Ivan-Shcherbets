#include "PhoneGuy.h"

#include "Collider.h"
#include "Player.h"

#include <cmath>

using namespace std;

static Texture2D g_phoneTex;
static bool g_phoneTexLoaded = false;

static void LoadPhoneTex()
{
    if (g_phoneTexLoaded) return;
    g_phoneTex = LoadTexture("Assets/Sprites/enemies/phoneguy.png");
    g_phoneTexLoaded = true;
}

static float Len(Vector2 v)
{
    return sqrtf(v.x*v.x + v.y*v.y);
}

static Vector2 CenterOf(const Collider* c)
{
    if (!c) return {0,0};

    if (c->GetType() == ColliderType::CIRCLE)
    {
        Circle circle = c->GetCircle();
        return circle.center;
    }

    Rectangle rect = c->GetRectangle();
    return { rect.x + rect.width/2.0f, rect.y + rect.height/2.0f };
}


PhoneGuy::PhoneGuy(float x, float y, int id)
{
    tag = Tag::Enemy;

    this->id = id;

    hp = 40.0f;
    maxHP = 40.0f;
    damage = 5.0f;

    speed = 0.0f;
    size = 1.0f;

    position = {x, y};

    LoadPhoneTex();

    Rectangle r = { position.x, position.y, 100.0f, 100.0f };
    collider = Collider(r, this);
    collider.Register();

    auraRadius = 150.0f;
    auraTick = 0.0f;
}

PhoneGuy::~PhoneGuy()
{
    collider.Unregister();
}

void PhoneGuy::Damage(float dmg)
{
    hp -= dmg;
}

void PhoneGuy::FindPlayer()
{
    if (player && playerCollider) return;

    player = nullptr;
    playerCollider = nullptr;

    auto& colliders = Collider::GetAllColliders();
    for (auto& c : colliders)
    {
        if (!c) continue;
        Asset* parent = c->GetParent();
        if (!parent) continue;

        if (parent->GetTag() == Tag::Player)
        {
            player = dynamic_cast<Player*>(parent);
            playerCollider = c;
            return;
        }
    }
}

Vector2 PhoneGuy::GetCenter() const
{
    return CenterOf(&collider);
}

Vector2 PhoneGuy::GetPlayerCenter() const
{
    return CenterOf(playerCollider);
}

void PhoneGuy::UpdateAura(float dt)
{
    if (!player) return;

    Vector2 c = GetCenter();
    Vector2 p = GetPlayerCenter();

    Vector2 d = { p.x - c.x, p.y - c.y };
    float dist = Len(d);

    if (dist > auraRadius)
    {
        auraTick = 0.0f;
        return;
    }

    auraTick += dt;
    if (auraTick >= 1.0f)
    {
        auraTick = 0.0f;
        player->Damage(5.0f);
    }
}

void PhoneGuy::DrawAura() const
{
    Vector2 c = GetCenter();

    int steps = 26;
    float R = auraRadius;

    for (int i = steps; i >= 1; i--)
    {
        float k = (float)i / (float)steps;
        float r = R * k;

        float a = 140.0f * (1.0f - k);
        if (a < 0) a = 0;
        if (a > 255) a = 255;

        Color col = { 0, 110, 255, (unsigned char)a };
        DrawCircleV(c, r, col);
    }
}

void PhoneGuy::Update()
{
    float dt = GetFrameTime();

    FindPlayer();

    Rectangle r = { position.x, position.y, 100.0f*size, 100.0f*size };
    collider.SetRectangle(r);

    UpdateAura(dt);
}

void PhoneGuy::Draw()
{
    DrawAura();

    Rectangle r = collider.GetRectangle();

    if (!g_phoneTexLoaded)
    {
        DrawRectangleRec(r, BLUE);
        return;
    }

    Rectangle src = { 0.0f, 0.0f, (float)g_phoneTex.width, (float)g_phoneTex.height };

    Rectangle dest = {
        r.x + r.width/2.0f,
        r.y + r.height/2.0f,
        r.width,
        r.height
    };

    Vector2 origin = { r.width/2.0f, r.height/2.0f };

    DrawTexturePro(g_phoneTex, src, dest, origin, 0.0f, WHITE);
}
