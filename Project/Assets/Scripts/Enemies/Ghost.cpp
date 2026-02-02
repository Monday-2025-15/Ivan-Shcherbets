#include "Ghost.h"

#include "Player.h"
#include "Collider.h"

#include <cmath>

using namespace std;

static Texture2D ghostTex;
static bool g_ghostTextureLoaded = false;

static void LoadGhostTexture()
{
    if (g_ghostTextureLoaded) return;
    ghostTex = LoadTexture("Assets/Sprites/enemies/ghost.png");
    g_ghostTextureLoaded = true;
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

Ghost::Ghost(float x, float y, int id)
{
    tag = Tag::Enemy;
    this->id = id;
    speed = 185;
    size = 3.0f;
    maxHP = 30;
    hp = 30;
    damage = 5;
    position = {x,y};

    LoadGhostTexture();
    texture = &ghostTex;

    Rectangle r = { position.x, position.y, 100, 100 };
    collider = Collider(r, this);
    collider.Register();
}

Ghost::~Ghost()
{
    collider.Unregister();
}

void Ghost::FindPlayer()
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

void Ghost::Move(float dt)
{
    if (!player) return;

    Vector2 tp = player->GetPosition();
    Vector2 d = { tp.x - position.x, tp.y - position.y };
    if (Len(d) < 1.0f) return;

    Vector2 dir = Norm(d);

    position.x += dir.x * speed * dt;
    position.y += dir.y * speed * dt;

    Rectangle r = { position.x, position.y, 100, 100 };
    collider.SetRectangle(r);
}

void Ghost::Update()
{
    FindPlayer();
    float dt = GetFrameTime();
    Move(dt);
}

void Ghost::Draw()
{
    if (!texture) return;

    Rectangle src = { 0.0f, 0.0f, (float)texture->width, (float)texture->height };
    Rectangle dest = { position.x, position.y, 100, 100 };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(*texture, src, dest, origin, 0.0f, WHITE);
}

void Ghost::Damage(float damage)
{
    hp -= damage;
}