#include "OldStudent.h"

#include "Player.h"
#include "Collider.h"

#include <cmath>

static float Len(Vector2 v)
{
    return sqrtf(v.x*v.x + v.y*v.y);
}

static Vector2 Norm(Vector2 v)
{
    float l = Len(v);
    if (l < 0.0001f) return {1, 0};
    return { v.x/l, v.y/l };
}

OldStudent::OldStudent(float x, float y, int id)
    : Bully(x, y, id)
{
    Asset::LoadEnemySprites("Assets/Sprites/enemies/oldstudent", enemysprites);
    maxHP=25;
    hp = maxHP;
    if (enemysprites) sprites = *enemysprites;
    baseHitDamage = 4;
    damage = 0.0f;
}

void OldStudent::FindPlayerPtr()
{
    if (player) return;

    auto& cols = Collider::GetAllColliders();
    for (auto& c : cols)
    {
        if (!c) continue;
        Asset* p = c->GetParent();
        if (!p) continue;
        if (p->GetTag() != Tag::Player) continue;

        player = dynamic_cast<Player*>(p);
        break;
    }
}

Collider* OldStudent::FindPlayerCollider()
{
    if (!player) return nullptr;

    auto& cols = Collider::GetAllColliders();
    for (auto& c : cols)
    {
        if (!c) continue;
        if (c->GetParent() == player) return c;
    }
    return nullptr;
}

bool OldStudent::PushPlayer(Vector2 dir, float dist)
{
    if (!player) return false;

    Collider* pcol = FindPlayerCollider();
    if (!pcol) return false;

    Rectangle pr = pcol->GetRectangle();

    dir = Norm(dir);

    Vector2 start = { pr.x, pr.y };
    Vector2 best = start;

    bool hitWall = false;

    float step = 10.0f;
    for (float d = step; d <= dist; d += step)
    {
        Vector2 np = { start.x + dir.x*d, start.y + dir.y*d };
        Rectangle test = { np.x, np.y, pr.width, pr.height };
        Collider probe(test, nullptr);

        bool blocked = false;

        auto& cols = Collider::GetAllColliders();
        for (auto& c : cols)
        {
            if (!c) continue;
            if (c == pcol) continue;

            Asset* par = c->GetParent();
            if (!par) continue;
            if (par->GetTag() != Tag::Solid) continue;

            if (probe.IsColliding(*c))
            {
                blocked = true;
                break;
            }
        }

        if (blocked)
        {
            hitWall = true;
            break;
        }

        best = np;
    }

    player->SetPosition(best);

    return hitWall;
}

void OldStudent::Update()
{
    float dt = GetFrameTime();

    if (hitTimer > 0.0f)
    {
        hitTimer -= dt;
        if (hitTimer < 0.0f) hitTimer = 0.0f;
    }

    Vector2 before = position;

    Bully::Update();

    Vector2 delta = { position.x - before.x, position.y - before.y };
    if (fabsf(delta.x) > 0.001f || fabsf(delta.y) > 0.001f)
        lastDir = Norm(delta);

    FindPlayerPtr();
    if (!player) return;

    Collider* pcol = FindPlayerCollider();
    if (!pcol) return;

    if (hitTimer > 0.0f) return;

    if (collider.IsColliding(*pcol))
    {
        bool wall = PushPlayer(lastDir, 260.0f);

        float dmg = baseHitDamage;
        if (wall) dmg *= 2.0f;

        player->Damage(dmg);

        hitTimer = 0.9f;
    }
}
