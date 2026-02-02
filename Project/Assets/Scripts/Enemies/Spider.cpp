//
// Created by vanya on 1/25/26.
//

#include "Spider.h"
#include "Run.h"
#include "Collider.h"

#include <cmath>

static Texture2D spiderTex[2];
static bool g_obstacleTexturesLoaded = false;

static void LoadObstacleTextures()
{
    if (g_obstacleTexturesLoaded) return;
    spiderTex[0] = LoadTexture("Assets/Sprites/enemies/spider/spider1.png"); // air
    spiderTex[1] = LoadTexture("Assets/Sprites/enemies/spider/spider2.png"); // ground
    g_obstacleTexturesLoaded = true;
}

static float Len(Vector2 v)
{
    return sqrtf(v.x*v.x + v.y*v.y);
}

static Vector2 Norm(Vector2 v)
{
    float l = Len(v);
    if (l < 0.0001f) return {1,0};
    return { v.x/l, v.y/l };
}

static bool IsFreePoint(Vector2 p, float radius, Asset* self)
{
    auto& colliders = Collider::GetAllColliders();
    Collider temp(Circle{p, radius}, nullptr);

    for (auto& c : colliders)
    {
        if (!c) continue;
        Asset* parent = c->GetParent();
        if (!parent) continue;
        if (parent==self) continue;
        if (parent->GetTag()!=Tag::Solid) continue;

        if (temp.IsColliding(*c)) return false;
    }
    return true;
}

Spider::Spider(float x, float y, int id, Room& room) : room(room)
{
    this->id = id;

    position = {x+57, y+57};
    circle = Circle(position,50);
    collider = Collider(circle,this);
    collider.Register();

    tag = Tag::Enemy;
    maxHP = 6;
    hp = 6;
    damage = 1;
    player=nullptr;

    moving=false;
    jumpTimer=0.0f;
    jumpDuration=0.35f;
    waitTimer=0.4f;

    startPos = circle.center;
    targetPos = circle.center;
    controlPos = circle.center;

    LoadObstacleTextures();
    texture[0] = &spiderTex[0];
    texture[1] = &spiderTex[1];
    currentTex=1;

    auto& colliders = Collider::GetAllColliders();
    for (auto& othercollider : colliders)
    {
        if (othercollider->GetParent()->GetTag()==Tag::Player)
        {
            player = dynamic_cast<Player*>(othercollider->GetParent());
            break;
        }
    }
}

Spider::~Spider()
{
    collider.Unregister();
}

void Spider::Draw()
{
    Rectangle src = { 0.0f, 0.0f, (float)texture[currentTex]->width, (float)texture[currentTex]->height };
    Rectangle dest = { circle.center.x , circle.center.y, circle.radius*2, circle.radius*2 };
    Vector2 origin = { circle.radius, circle.radius };

    DrawTexturePro(*texture[currentTex], src, dest, origin, rotation, WHITE);
    //collider.DebugDraw();
}

void Spider::JumpRandom()
{
    int minI = 1;
    int maxI = room.GetGridW() - 1;
    int minJ = 1;
    int maxJ = room.GetGridH() - 1;

    for (int k = 0; k < 25; k++)
    {
        int i = Run::GenerateinRange(minI, maxI-1, tag, id);
        int j = Run::GenerateinRange(minJ, maxJ-1, tag, id);

        if (!room.IsCellFree(i,j)) continue;

        Vector2 p = room.GetCellCenter(i,j);

        startPos = circle.center;
        targetPos = p;
        jumpTimer = 0.0f;

        Vector2 d = { targetPos.x - startPos.x, targetPos.y - startPos.y };
        float l = sqrtf(d.x*d.x + d.y*d.y);
        if (l < 0.001f) l = 1.0f;

        Vector2 perp = { -d.y/l, d.x/l };

        int s = Run::GenerateinRange(0, 1, tag, id);
        float side = (s==0 ? -1.0f : 1.0f);

        float mag = 200.0f;

        Vector2 mid = { (startPos.x + targetPos.x)/2.0f, (startPos.y + targetPos.y)/2.0f };
        controlPos = { mid.x + perp.x*mag*side, mid.y + perp.y*mag*side };

        float dist = sqrtf(d.x*d.x + d.y*d.y);
        jumpDuration = 0.50f + dist/1400.0f;
        if (jumpDuration < 0.20f) jumpDuration = 0.20f;
        if (jumpDuration > 0.70f) jumpDuration = 0.70f;

        moving = true;
        currentTex = 0;
        return;
    }
}


float Spider::JumpToPlayer()
{
    if (!player) return 0.0f;

    Vector2 p = player->GetPosition();

    startPos = circle.center;
    targetPos = p;
    jumpTimer = 0.0f;

    Vector2 d = { targetPos.x - startPos.x, targetPos.y - startPos.y };
    Vector2 nd = Norm(d);
    Vector2 perp = { -nd.y, nd.x };

    int s = Run::GenerateinRange(0, 1, tag, id);
    float side = (s==0 ? -1.0f : 1.0f);
    float mag = 200.0f;

    Vector2 mid = { (startPos.x + targetPos.x)/2.0f, (startPos.y + targetPos.y)/2.0f };
    controlPos = { mid.x + perp.x*mag*side, mid.y + perp.y*mag*side };

    float dist = Len(d);
    jumpDuration = 0.20f + dist/1400.0f;
    if (jumpDuration < 0.20f) jumpDuration = 0.20f;
    if (jumpDuration > 0.70f) jumpDuration = 0.70f;

    moving = true;
    currentTex = 0;
    return dist;
}

void Spider::Update()
{
    float dt = GetFrameTime();

    if (!moving)
    {
        currentTex = 1;

        circle.center = position;
        collider.SetCircle(circle);



        waitTimer -= dt;
        if (waitTimer <= 0)
        {
            int k = Run::GenerateinRange(1,5,tag,id);
            if (k==1 && player) JumpToPlayer();
            else JumpRandom();

            waitTimer = (float)Run::GenerateinRange(100, 200, tag, id)/100.0f;
        }

        return;
    }

    currentTex = 0;

    jumpTimer += dt;
    float t = jumpTimer / jumpDuration;
    if (t >= 1.0f) t = 1.0f;

    float u = 1.0f - t;

    circle.center.x =
            u*u*startPos.x +
            2*u*t*controlPos.x +
            t*t*targetPos.x;

    circle.center.y =
            u*u*startPos.y +
            2*u*t*controlPos.y +
            t*t*targetPos.y;

    position = circle.center;

    Circle air = circle;
    air.radius = 1;
    collider.SetCircle(air);

    if (t >= 1.0f)
    {
        circle.center = targetPos;
        position = circle.center;
        collider.SetCircle(circle);
        moving = false;
    }
}

void Spider::Damage(float damage)
{
    hp -= damage;
}
