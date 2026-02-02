#include "Weapon.h"
#include "Collider.h"
#include "Block.h"
#include "Player.h"

#include <cmath>
#include <unordered_map>

#include "Run.h"

using namespace std;

static float Length(Vector2 v)
{
    return sqrtf(v.x*v.x + v.y*v.y);
}

static Vector2 Normalize(Vector2 v)
{
    float l = Length(v);
    if (l <= 0.0001f) return {1.0f, 0.0f};
    return { v.x/l, v.y/l };
}

static Vector2 NormalizeOr(Vector2 v, Vector2 fallback)
{
    float l = Length(v);
    if (l <= 0.0001f) return fallback;
    return { v.x/l, v.y/l };
}

static Vector2 RotateVector(Vector2 v, float deg)
{
    float r = deg * DEG2RAD;
    float c = cosf(r);
    float s = sinf(r);
    return { v.x*c - v.y*s, v.x*s + v.y*c };
}

static Vector2 GetColliderCenter(Collider* c)
{
    if (!c) return {0,0};
    if (c->GetType()==ColliderType::CIRCLE)
    {
        Circle circle = c->GetCircle();
        return circle.center;
    }
    Rectangle rect = c->GetRectangle();
    return { rect.x + rect.width/2.0f, rect.y + rect.height/2.0f };
}

static Vector2 GetOwnerCenter(Entity &owner)
{
    auto &colliders = Collider::GetAllColliders();
    for (auto &c : colliders)
    {
        if (c && c->GetParent()==&owner)
            return GetColliderCenter(c);
    }
    return owner.GetPosition();
}

static Vector2 GetMouseVirtual()
{
    Vector2 m = GetMousePosition();

    int sw = GetRenderWidth();
    int sh = GetRenderHeight();

    float s1 = (float)sw / 1920.0f;
    float s2 = (float)sh / 1080.0f;
    float scale = s1;
    if (s2 < scale) scale = s2;
    if (scale < 0.0001f) scale = 1.0f;

    float rw = 1920.0f * scale;
    float rh = 1080.0f * scale;

    float offx = ((float)sw - rw) * 0.5f;
    float offy = ((float)sh - rh) * 0.5f;

    m.x = (m.x - offx) / scale;
    m.y = (m.y - offy) / scale;

    return m;
}

static Vector2 MouseDir(Entity &owner, Vector2 from, Vector2 fallback)
{
    if (owner.GetTag()!=Tag::Player) return fallback;

    Player* p = dynamic_cast<Player*>(&owner);
    if (!p) return fallback;

    Camera2D &cam = p->GetCamera();
    Vector2 m = GetMouseVirtual();
    Vector2 w = GetScreenToWorld2D(m, cam);

    return NormalizeOr({ w.x - from.x, w.y - from.y }, fallback);
}

static bool IsWall(Asset* parent)
{
    if (!parent) return false;
    if (parent->GetTag()!=Tag::Solid) return false;

    Block* block = dynamic_cast<Block*>(parent);
    if (block!=nullptr) return false;

    return true;
}

static bool CanDamage(Tag ownerTag, Tag targetTag)
{
    if (targetTag==Tag::Enemy && ownerTag!=Tag::Enemy) return true; // player/item/etc -> enemy
    if (targetTag==Tag::Player && ownerTag==Tag::Enemy) return true; // enemy -> player
    return false;
}


static Tag GetTargetTag(Tag ownerTag)
{
    if (ownerTag==Tag::Enemy) return Tag::Player;
    return Tag::Enemy;
}


static bool PointInCollider(Vector2 p, Collider* c)
{
    if (!c) return false;

    if (c->GetType()==ColliderType::CIRCLE)
    {
        Circle circle = c->GetCircle();
        return CheckCollisionPointCircle(p, circle.center, circle.radius);
    }

    Rectangle rect = c->GetRectangle();
    return CheckCollisionPointRec(p, rect);
}

static Vector2 NearestPointOnRect(Vector2 p, Rectangle r)
{
    float x = p.x;
    float y = p.y;
    if (x < r.x) x = r.x;
    if (x > r.x + r.width) x = r.x + r.width;
    if (y < r.y) y = r.y;
    if (y > r.y + r.height) y = r.y + r.height;
    return {x,y};
}

static Vector2 CollisionNormal(Vector2 prevPos, Vector2 newPos, Collider* other)
{
    if (!other) return Normalize({prevPos.x - newPos.x, prevPos.y - newPos.y});

    if (other->GetType()==ColliderType::CIRCLE)
    {
        Circle c = other->GetCircle();
        Vector2 n = { newPos.x - c.center.x, newPos.y - c.center.y };
        if (Length(n) <= 0.001f) n = { prevPos.x - c.center.x, prevPos.y - c.center.y };
        return Normalize(n);
    }

    Rectangle r = other->GetRectangle();
    Vector2 nearest = NearestPointOnRect(newPos, r);
    Vector2 n = { newPos.x - nearest.x, newPos.y - nearest.y };

    if (Length(n) <= 0.001f)
    {
        Vector2 nearest2 = NearestPointOnRect(prevPos, r);
        n = { prevPos.x - nearest2.x, prevPos.y - nearest2.y };
    }

    if (Length(n) <= 0.001f)
    {
        float left = fabsf(newPos.x - r.x);
        float right = fabsf(newPos.x - (r.x + r.width));
        float top = fabsf(newPos.y - r.y);
        float bottom = fabsf(newPos.y - (r.y + r.height));
        float m = left;
        n = {-1,0};
        if (right < m){ m = right; n = {1,0}; }
        if (top < m){ m = top; n = {0,-1}; }
        if (bottom < m){ m = bottom; n = {0,1}; }
    }

    return Normalize(n);
}

static bool BulletCollide(Vector2 pos, float radius, Entity* owner, bool ghost, Collider* &hit, Asset* &hitParent)
{
    auto &colliders = Collider::GetAllColliders();
    Collider temp(Circle{pos, radius}, nullptr);

    for (auto &other : colliders)
    {
        if (!other) continue;
        Asset* parent = other->GetParent();
        if (!parent) continue;
        if (owner && parent==owner) continue;
        if (owner)
        {
            if (owner->GetTag()!=Tag::Enemy && parent->GetTag()==Tag::Player) continue;
            if (owner->GetTag()==Tag::Enemy && parent->GetTag()==Tag::Enemy) continue;
        }

        if (ghost)
        {
            if (parent->GetTag()==Tag::Solid && IsWall(parent) && temp.IsColliding(*other))
            {
                hit = other;
                hitParent = parent;
                return true;
            }
            continue;
        }

        if ((parent->GetTag()==Tag::Solid || parent->GetTag()==Tag::Enemy || parent->GetTag()==Tag::Player) && temp.IsColliding(*other))
        {
            hit = other;
            hitParent = parent;
            return true;
        }
    }
    return false;
}

static bool CircleHitsSolid(Vector2 pos, float radius, Entity* owner, bool ghost)
{
    auto &colliders = Collider::GetAllColliders();
    Collider temp(Circle{pos, radius}, nullptr);

    for (auto &c : colliders)
    {
        if (!c) continue;
        Asset* parent = c->GetParent();
        if (!parent) continue;
        if (owner && parent==owner) continue;
        if (parent->GetTag()!=Tag::Solid) continue;

        if (ghost)
        {
            if (!IsWall(parent)) continue;
        }

        if (temp.IsColliding(*c))
            return true;
    }
    return false;
}

static Asset* HitOpponent(Vector2 pos, float radius, Entity* owner)
{
    if (!owner) return nullptr;

    Tag targetTag = GetTargetTag(owner->GetTag());
    if (targetTag==Tag::None) return nullptr;

    auto &colliders = Collider::GetAllColliders();
    Collider temp(Circle{pos, radius}, nullptr);

    for (auto &c : colliders)
    {
        if (!c) continue;
        Asset* parent = c->GetParent();
        if (!parent) continue;
        if (parent==owner) continue;
        if (parent->GetTag()!=targetTag) continue;

        if (temp.IsColliding(*c))
            return parent;
    }

    return nullptr;
}

static float ApplyCrit(Entity* owner, float dmg)
{
    if (!owner) return dmg;
    if (owner->GetTag()!=Tag::Player) return dmg;

    Player* p = dynamic_cast<Player*>(owner);
    if (!p) return dmg;

    int chance = (int)p->GetCritChance();
    if (chance <= 0) return dmg;
    if (chance > 100) chance = 100;

    static int idx = 0;
    if (!Run::GenerateDecision(chance, Tag::Weapon, idx++)) return dmg;

    float mult = p->GetCritDamage();
    if (mult <= 0.01f) mult = 1.0f;
    return dmg * mult;
}

static float DealDamage(Asset* a, float dmg, Entity* owner, bool vampire)
{
    if (!a) return 0.0f;
    Entity* e = dynamic_cast<Entity*>(a);
    if (!e) return 0.0f;

    float dealt = ApplyCrit(owner, dmg);
    e->Damage(dealt);

    if (vampire && owner)
        owner->AddHealth(1.0f);

    return dealt;
}

static float DealDamageNoCrit(Asset* a, float dmg, Entity* owner, bool vampire)
{
    if (!a) return 0.0f;
    Entity* e = dynamic_cast<Entity*>(a);
    if (!e) return 0.0f;

    e->Damage(dmg);

    if (vampire && owner)
        owner->AddHealth(1.0f);

    return dmg;
}

// ---------- Explosions ----------

struct Explosion
{
    Vector2 pos;
    float radius;
    float maxRadius;
    float timer;
    float maxTimer;
};

static vector<Explosion> explosions;

static void ExplosionDamage(Vector2 pos, float radius, float dmg, Entity* owner, Asset* ignore, bool vampire)
{
    if (!owner) return;

    Tag targetTag = GetTargetTag(owner->GetTag());
    if (targetTag==Tag::None) return;

    auto &colliders = Collider::GetAllColliders();
    Collider temp(Circle{pos, radius}, nullptr);

    for (auto &c : colliders)
    {
        if (!c) continue;
        Asset* parent = c->GetParent();
        if (!parent) continue;
        if (parent==owner) continue;
        if (parent==ignore) continue;
        if (parent->GetTag()!=targetTag) continue;

        if (temp.IsColliding(*c))
        {
            DealDamageNoCrit(parent, dmg, owner, vampire);
        }
    }
}

static void SpawnExplosion(Vector2 pos, float dmg, float maxR, Entity* owner, Asset* ignore, bool vampire)
{
    ExplosionDamage(pos, maxR, dmg, owner, ignore, vampire);

    Explosion e;
    e.pos = pos;
    e.radius = 2.0f;
    e.maxRadius = maxR;
    e.timer = 0.40f;
    e.maxTimer = e.timer;
    explosions.push_back(e);
}

static void UpdateExplosionsOnce(float dt)
{
    static double last = -1.0;
    double now = GetTime();
    if (now - last < 0.00001) return;
    last = now;

    for (int i = (int)explosions.size()-1; i >= 0; i--)
    {
        explosions[i].timer -= dt;
        if (explosions[i].timer <= 0)
        {
            explosions.erase(explosions.begin()+i);
            continue;
        }

        float f = 1.0f - (explosions[i].timer / explosions[i].maxTimer);
        if (f < 0) f = 0;
        if (f > 1) f = 1;
        explosions[i].radius = explosions[i].maxRadius * f;
    }
}

static void DrawExplosionsOnce()
{
    static double last = -1.0;
    double now = GetTime();
    if (now - last < 0.00001) return;
    last = now;

    for (auto &e : explosions)
    {
        float a = e.timer / e.maxTimer;
        if (a < 0) a = 0;
        if (a > 1) a = 1;

        unsigned char a1 = (unsigned char)(150 * a);
        unsigned char a2 = (unsigned char)(200 * a);

        Color fill = {255, 165, 0, a1};
        Color line = {255, 165, 0, a2};

        DrawCircleV(e.pos, e.radius, fill);
        DrawCircleLines((int)e.pos.x, (int)e.pos.y, (int)e.radius, line);
    }
}

// ---------- Target search / laser ----------

static bool FindNearestTarget(Vector2 from, Tag tag, float radius, Vector2 &outpos)
{
    auto &colliders = Collider::GetAllColliders();
    float best = radius*radius;
    bool found = false;

    for (auto &c : colliders)
    {
        if (!c) continue;
        Asset* parent = c->GetParent();
        if (!parent) continue;
        if (parent->GetTag()!=tag) continue;

        Vector2 p = GetColliderCenter(c);
        float dx = p.x - from.x;
        float dy = p.y - from.y;
        float d = dx*dx + dy*dy;
        if (d < best)
        {
            best = d;
            outpos = p;
            found = true;
        }
    }
    return found;
}

static bool RaycastLaser(Vector2 start, Vector2 dir, float maxDist, Entity* owner, bool ghost,
                         Vector2 &outPos, Collider* &outCol, Asset* &outParent,
                         float dmg, std::vector<Asset*> &hitlist,
                         bool explosive, bool vampire, float exRadius)
{
    float step = 4.0f;
    Tag targetTag = Tag::None;
    bool doDamage = false;

    if (owner)
    {
        targetTag = GetTargetTag(owner->GetTag());
        doDamage = (targetTag!=Tag::None);
    }


    auto &colliders = Collider::GetAllColliders();

    for (float d = 0.0f; d <= maxDist; d += step)
    {
        Vector2 p = { start.x + dir.x*d, start.y + dir.y*d };

        if (doDamage)
        {
            Collider probe(Circle{p, 2.0f}, nullptr);
            for (auto &c : colliders)
            {
                if (!c) continue;
                Asset* parent = c->GetParent();
                if (!parent) continue;
                if (parent==owner) continue;
                if (parent->GetTag()!=targetTag) continue;

                if (probe.IsColliding(*c))
                {
                    bool already = false;
                    for (auto &h : hitlist)
                        if (h==parent) already = true;

                    if (!already)
                    {
                        float dealt = DealDamage(parent, dmg, owner, vampire);
                        hitlist.push_back(parent);

                        if (explosive && dealt > 0.0f)
                            SpawnExplosion(p, dealt*0.5f, exRadius, owner, parent, vampire);
                    }
                }
            }
        }

        Collider* hit = nullptr;
        Asset* parent = nullptr;

        if (BulletCollide(p, 2.0f, owner, ghost, hit, parent))
        {
            outPos = p;
            outCol = hit;
            outParent = parent;
            return true;
        }
    }

    outPos = { start.x + dir.x*maxDist, start.y + dir.y*maxDist };
    outCol = nullptr;
    outParent = nullptr;
    return false;
}

Weapon::Weapon(MODE mode, Entity &owner) : owner(owner)
{
    this->mode = mode;
    damage = 1.0f;
    attackSpeed = 1.5f;
    size = 1.5f;
    ghost = false;
    ricochet = false;

    tag = Tag::Weapon;
    position = owner.GetPosition();
}

// ---------------- HandPistol ----------------

HandPistol::HandPistol(Entity &owner) : Weapon(MODE::HandPistol, owner)
{
    damage = 6.0f;
}

void HandPistol::Update()
{
    float dt = GetFrameTime();
    UpdateExplosionsOnce(dt);

    position = GetOwnerCenter(owner);

    if (first)
    {
        lastOwnerPos = position;
        lastMoveDir = {1.0f, 0.0f};
        first = false;
    }

    Vector2 delta = { position.x - lastOwnerPos.x, position.y - lastOwnerPos.y };
    lastOwnerPos = position;
    if (Length(delta) > 0.5f)
        lastMoveDir = Normalize(delta);

    if (cooldown > 0) cooldown -= dt;

    for (auto &b : bullets)
    {
        if (!b.active) continue;

        Vector2 prev = b.position;
        b.position.x += b.velocity.x * dt;
        b.position.y += b.velocity.y * dt;
        b.timer -= dt;

        if (b.timer <= 0) b.active = false;
        if (!b.active) continue;

        if (ghost)
        {
            Asset* hit = HitOpponent(b.position, b.radius, &owner);
            if (hit && hit!=b.lastEntity)
            {
                if (CanDamage(owner.GetTag(), hit->GetTag()))
                {
                    float dealt = DealDamage(hit, damage, &owner, vampire);
                    if (explosive && dealt > 0.0f)
                        SpawnExplosion(b.position, dealt*0.5f, 120.0f*size, &owner, hit, vampire);
                }
                b.lastEntity = hit;
            }
        }

        Collider* hit = nullptr;
        Asset* parent = nullptr;

        if (BulletCollide(b.position, b.radius, &owner, ghost, hit, parent))
        {
            bool solidHit = parent && parent->GetTag()==Tag::Solid;
            bool enemyHit = parent && (parent->GetTag()==Tag::Enemy || parent->GetTag()==Tag::Player);

            if (!ghost && enemyHit)
            {
                if (CanDamage(owner.GetTag(), parent->GetTag()))
                {
                    float dealt = DealDamage(parent, damage, &owner, vampire);
                    if (explosive && dealt > 0.0f)
                        SpawnExplosion(b.position, dealt*0.5f, 120.0f*size, &owner, parent, vampire);
                }
                b.active = false;
            }
            else
            {
                if (ricochet && solidHit)
                {
                    if (b.bounces > 0)
                    {
                        Vector2 n = CollisionNormal(prev, b.position, hit);
                        float dot = b.velocity.x*n.x + b.velocity.y*n.y;
                        b.velocity.x = b.velocity.x - 2.0f*dot*n.x;
                        b.velocity.y = b.velocity.y - 2.0f*dot*n.y;
                        b.position = prev;
                        b.bounces--;
                    }
                    else b.active = false;
                }
                else
                {
                    b.active = false;
                }
            }
        }
    }

    for (int i = (int)bullets.size()-1; i >= 0; i--)
        if (!bullets[i].active) bullets.erase(bullets.begin()+i);

    Vector2 dir = {1,0};
    bool fire = false;

    if (owner.GetTag()==Tag::Player)
    {
        fire = IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        dir = MouseDir(owner, position, lastMoveDir);
    }
    else
    {
        Tag targetTag = Tag::Enemy;
        if (owner.GetTag()==Tag::Enemy) targetTag = Tag::Player;

        Vector2 targetPos;
        if (!FindNearestTarget(position, targetTag, 800.0f, targetPos))
            return;

        dir = Normalize({ targetPos.x - position.x, targetPos.y - position.y });
        fire = true;
    }

    Shoot(position, dir, fire);
}

void HandPistol::Draw()
{
    Color c = DARKGRAY;
    if (owner.GetTag()==Tag::Player) c = ORANGE;
    for (auto &b : bullets)
        DrawCircleV(b.position, b.radius, c);

    DrawExplosionsOnce();
}

void HandPistol::Shoot(Vector2 &start, Vector2 &dir, bool fire)
{
    if (fire && cooldown <= 0)
    {
        Bullet b;
        b.position = start;
        b.velocity = { dir.x * 1200.0f, dir.y * 1200.0f };
        b.radius = 6.0f * size;
        b.timer = 2.5f;
        b.active = true;
        b.bounces = ricochet ? 2 : 0;
        b.lastEntity = nullptr;
        bullets.push_back(b);

        cooldown = 1/attackSpeed;
    }
}

// ---------------- Laser ----------------

Laser::Laser(Entity &owner) : Weapon(MODE::Laser, owner)
{
    damage = 2.0f;
}

void Laser::Update()
{
    float dt = GetFrameTime();
    UpdateExplosionsOnce(dt);

    position = GetOwnerCenter(owner);

    if (first)
    {
        lastOwnerPos = position;
        lastMoveDir = {1.0f, 0.0f};
        first = false;
    }

    Vector2 delta = { position.x - lastOwnerPos.x, position.y - lastOwnerPos.y };
    lastOwnerPos = position;
    if (Length(delta) > 0.5f)
        lastMoveDir = Normalize(delta);

    if (cooldown > 0) cooldown -= dt;
    if (lazerTimer > 0) lazerTimer -= dt;

    Vector2 dir = {1,0};
    bool fire = false;

    if (owner.GetTag()==Tag::Player)
    {
        fire = IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        dir = MouseDir(owner, position, lastMoveDir);
    }
    else
    {
        Tag targetTag = Tag::Enemy;
        if (owner.GetTag()==Tag::Enemy) targetTag = Tag::Player;

        Vector2 targetPos;
        if (!FindNearestTarget(position, targetTag, 800.0f, targetPos))
            return;

        dir = Normalize({ targetPos.x - position.x, targetPos.y - position.y });
        fire = true;
    }

    Shoot(position, dir, fire);
}

void Laser::Draw()
{
    if (lazerTimer > 0)
    {
        for (int i = 0; i < (int)points.size()-1; i++)
            DrawLineEx(points[i], points[i+1], 6.0f*size, GREEN);
    }

    DrawExplosionsOnce();
}

void Laser::Shoot(Vector2 &start, Vector2 &dir, bool fire)
{
    if (fire && cooldown <= 0)
    {
        points.clear();
        points.push_back(start);

        vector<Asset*> hitlist;

        float remain = 2500.0f;
        int b = ricochet ? 2 : 0;

        Vector2 curStart = start;
        Vector2 curDir = dir;

        while (remain > 0)
        {
            Vector2 hitPos;
            Collider* hitCol = nullptr;
            Asset* hitParent = nullptr;

            Vector2 oldStart = curStart;
            bool hit = RaycastLaser(curStart, curDir, remain, &owner, ghost, hitPos, hitCol, hitParent,
                                    damage, hitlist, explosive, vampire, 120.0f*size);

            points.push_back(hitPos);

            float segLen = Length({ hitPos.x - oldStart.x, hitPos.y - oldStart.y });
            remain -= segLen;
            if (remain <= 0) break;

            if (!hit) break;

            bool solidHit = hitParent && hitParent->GetTag()==Tag::Solid;
            bool enemyHit = hitParent && (hitParent->GetTag()==Tag::Enemy || hitParent->GetTag()==Tag::Player);

            if (!ghost && enemyHit) break;

            if (solidHit && b > 0)
            {
                Vector2 prev = { hitPos.x - curDir.x*4.0f, hitPos.y - curDir.y*4.0f };
                Vector2 n = CollisionNormal(prev, hitPos, hitCol);
                float dot = curDir.x*n.x + curDir.y*n.y;
                Vector2 refl = { curDir.x - 2.0f*dot*n.x, curDir.y - 2.0f*dot*n.y };
                int idx = (int)(uintptr_t)(&owner);
                float spread = (float)Run::GenerateinRange(-120, 120, tag, idx) / 100.0f;
                curDir = Normalize(RotateVector(refl, spread));
                curStart = { hitPos.x + curDir.x*5.0f, hitPos.y + curDir.y*5.0f };

                b--;
                continue;
            }

            break;
        }

        lazerTimer = 0.08f;
        cooldown = 1/attackSpeed;
    }
}

// ---------------- FireExtinguisher ----------------

FireExtinguisher::FireExtinguisher(Entity &owner) : Weapon(MODE::FireExtinguisher, owner)
{
    damage = 2.0f;
}

void FireExtinguisher::Update()
{
    float dt = GetFrameTime();
    UpdateExplosionsOnce(dt);

    position = GetOwnerCenter(owner);

    if (first)
    {
        lastOwnerPos = position;
        lastMoveDir = {1.0f, 0.0f};
        first = false;
    }

    Vector2 delta = { position.x - lastOwnerPos.x, position.y - lastOwnerPos.y };
    lastOwnerPos = position;
    if (Length(delta) > 0.5f)
        lastMoveDir = Normalize(delta);

    Vector2 dir = {1,0};
    bool fire = false;

    if (owner.GetTag()==Tag::Player)
    {
        fire = IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        dir = MouseDir(owner, position, lastMoveDir);
    }
    else
    {
        Tag targetTag = Tag::Enemy;
        if (owner.GetTag()==Tag::Enemy) targetTag = Tag::Player;

        Vector2 targetPos;
        if (!FindNearestTarget(position, targetTag, 800.0f, targetPos))
        {
            spraying = false;
            dots.clear();
            return;
        }

        dir = Normalize({ targetPos.x - position.x, targetPos.y - position.y });
        fire = true;
    }

    Shoot(position, dir, fire);

    if (spraying)
    {
        Tag targetTag = GetTargetTag(owner.GetTag());
        if (targetTag!=Tag::None)
        {
            auto &colliders = Collider::GetAllColliders();

            for (auto &c : colliders)
            {
                if (!c) continue;
                Asset* parent = c->GetParent();
                if (!parent) continue;
                if (parent==&owner) continue;
                if (parent->GetTag()!=targetTag) continue;

                bool hit = false;
                for (auto &p : dots)
                {
                    if (PointInCollider(p, c))
                    {
                        hit = true;
                        break;
                    }
                }

                if (hit)
                {
                    if (CanDamage(owner.GetTag(), parent->GetTag()))
                    {
                        float dealt = DealDamage(parent, damage*dt*attackSpeed*7, &owner, vampire);
                        if (explosive && dealt > 0.0f)
                            SpawnExplosion(GetColliderCenter(c), dealt*0.5f, 120.0f*size, &owner, parent, vampire);
                    }
                }
            }
        }
    }
}

void FireExtinguisher::Draw()
{
    if (spraying)
    {
        for (auto &p : dots)
            DrawCircleV(p, 3.0f*size, SKYBLUE);
    }

    DrawExplosionsOnce();
}

void FireExtinguisher::Shoot(Vector2 &start, Vector2 &dir, bool fire)
{
    if (fire)
    {
        spraying = true;
        dots.clear();

        float range = 150.0f * size;
        float spread = 20.0f;
        int count = 18;

        for (int i = 0; i < count; i++)
        {
            int idx = (int)(uintptr_t)(&owner);
            float a = (float)Run::GenerateinRange(-(int)(spread*100), (int)(spread*100), tag, idx) / 100.0f;
            float d = (float)Run::GenerateinRange(25, (int)range, tag, idx +894367);
            Vector2 rd = RotateVector(dir, a);

            Vector2 p = start;
            float step = 4.0f;

            for (float dist = 0.0f; dist <= d; dist += step)
            {
                Vector2 pp = { start.x + rd.x*dist, start.y + rd.y*dist };

                if (ghost)
                {
                    if (CircleHitsSolid(pp, 2.0f, &owner, true)) { p = pp; break; }
                }
                else
                {
                    if (CircleHitsSolid(pp, 2.0f, &owner, false)) { p = pp; break; }
                }

                p = pp;
            }

            dots.push_back(p);
        }
    }
    else
    {
        spraying = false;
        dots.clear();
    }
}

// weapon
void Weapon::ChangeAttackSpeed(float value, Oper op)
{
    switch (op)
    {
        case Oper::Add:
        attackSpeed+=value;
        break;
        case Oper::Mul:
        attackSpeed*=value;
        break;
    case Oper::Set:
        attackSpeed=value;
        break;
    }
}

void Weapon::ChangeDamage(float value, Oper op)
{
    switch (op)
    {
    case Oper::Add:
        damage+=value;
        break;
    case Oper::Mul:
        damage*=value;
        break;
    case Oper::Set:
        damage=value;
        break;
    }
}

void Weapon::ChangeSize(float value, Oper op)
{
    switch (op)
    {
    case Oper::Add:
        size+=value;
        break;
    case Oper::Mul:
        size*=value;
        break;
    case Oper::Set:
        size=value;
        break;
    }
}
