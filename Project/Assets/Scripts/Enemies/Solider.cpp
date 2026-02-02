//
// Created by vanya on 2/2/26.
//

#include "Solider.h"

#include "Player.h"
#include "Collider.h"
#include "Run.h"

#include <cmath>

static float Dist2(Vector2 a, Vector2 b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx*dx + dy*dy;
}

static Vector2 CenterOfRect(Rectangle r)
{
    return { r.x + r.width/2.0f, r.y + r.height/2.0f };
}

Solider::Solider(float x, float y, int id)
    : Security(x, y, id)
{
    Asset::LoadEnemySprites("Assets/Sprites/enemies/solider", enemysprites);
    hp=45;
    maxHP=45;
    if (enemysprites) sprites = *enemysprites;
    Rectangle r = collider.GetRectangle();
    r.width = 100.0f;
    r.height = 100.0f;
    collider.SetRectangle(r);
    weapon->ChangeAttackSpeed(2,Oper::Mul);
    weapon->ChangeSize(0.8, Oper::Mul);
}

Solider::~Solider()
{
    grenades.clear();
}

void Solider::FindPlayerPtr()
{
    if (player && playerCollider) return;

    auto& cols = Collider::GetAllColliders();
    for (auto& c : cols)
    {
        if (!c) continue;
        Asset* p = c->GetParent();
        if (!p) continue;

        if (p->GetTag() == Tag::Player)
        {
            playerCollider = c;
            player = dynamic_cast<Player*>(p);
            return;
        }
    }
}

void Solider::Explode(Grenade& g)
{
    g.exploded = true;
    g.boomLife = 0.60f;
    g.boomT = g.boomLife;
    g.boomR = 150.0f;

    FindPlayerPtr();
    if (!player || !playerCollider) return;

    Rectangle pr = playerCollider->GetRectangle();
    Vector2 pc = CenterOfRect(pr);

    if (Dist2(pc, g.pos) <= g.boomR * g.boomR)
        player->Damage(GetDamage());
}

void Solider::TryThrowGrenade(float dt)
{
    grenadeTick += dt;
    if (grenadeTick < 5.0f) return;
    grenadeTick = 0.0f;

    FindPlayerPtr();
    if (!playerCollider) return;

    if (!Run::GenerateDecision(50, Tag::Enemy, id))
        return;

    Vector2 from = CenterOfRect(collider.GetRectangle());
    Vector2 to   = CenterOfRect(playerCollider->GetRectangle());

    float dx = to.x - from.x;
    float dy = to.y - from.y;
    float dist = sqrtf(dx*dx + dy*dy);

    float speed = 700.0f;
    float dur = dist / speed;
    if (dur < 0.25f) dur = 0.25f;
    if (dur > 0.90f) dur = 0.90f;

    Grenade g;
    g.start = from;
    g.target = to;
    g.pos = from;

    g.flyT = 0.0f;
    g.flyDur = dur;
    g.arcH = 160.0f;

    g.landed = false;
    g.fuse = 2.0f;

    g.exploded = false;
    g.boomT = 0.0f;
    g.boomLife = 0.0f;
    g.boomR = 250.0f;

    grenades.push_back(g);
}


void Solider::UpdateGrenades(float dt)
{
    for (int i = (int)grenades.size()-1; i >= 0; i--)
    {
        Grenade& g = grenades[i];

        if (!g.landed && !g.exploded)
        {
            g.flyT += dt;
            float k = g.flyT / g.flyDur;
            if (k >= 1.0f)
            {
                g.pos = g.target;
                g.landed = true;
            }
            else
            {
                float x = g.start.x + (g.target.x - g.start.x) * k;
                float y = g.start.y + (g.target.y - g.start.y) * k;

                float arc = sinf(k * PI) * g.arcH;
                y -= arc;

                g.pos = { x, y };
            }
            continue;
        }

        if (g.landed && !g.exploded)
        {
            g.fuse -= dt;
            if (g.fuse <= 0.0f)
                Explode(g);
            continue;
        }

        g.boomT -= dt;
        if (g.boomT <= 0.0f)
            grenades.erase(grenades.begin() + i);
    }
}


void Solider::Update()
{
    Security::Update();

    Rectangle r = collider.GetRectangle();
    r.x = position.x;
    r.y = position.y;
    r.width = 100.0f;
    r.height = 100.0f;
    collider.SetRectangle(r);

    float dt = GetFrameTime();

    TryThrowGrenade(dt);
    UpdateGrenades(dt);
}

void Solider::Draw()
{
    Security::Draw();

    for (auto& g : grenades)
    {
        if (!g.exploded)
        {
            Color c = { 120, 0, 0, 220 };
            DrawCircleV(g.pos, 18.0f, c);
            continue;
        }

        float k = 1.0f - (g.boomT / g.boomLife);
        if (k < 0) k = 0;
        if (k > 1) k = 1;

        float rr = g.boomR * k;
        unsigned char a = (unsigned char)(200.0f * (1.0f - k));

        Color fill = { 255, 0, 0, a };
        Color line = { 160, 0, 0, a };

        DrawCircleV(g.pos, rr, fill);
        DrawCircleLines((int)g.pos.x, (int)g.pos.y, rr, line);
    }
}

void Solider::Damage(float dmg)
{
    hp -= dmg;
}
