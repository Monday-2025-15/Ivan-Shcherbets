//
// Created by vanya on 1/12/26.
//

#include "Mouse.h"
#include "Run.h"

static Texture2D mouseTex;
static bool g_obstacleTexturesLoaded = false;


static void LoadObstacleTextures()
{
    if (g_obstacleTexturesLoaded) return;
    mouseTex = LoadTexture("Assets/Sprites/enemies/mouse.png");
    g_obstacleTexturesLoaded = true;
}

Mouse::Mouse(float x, float y, int id, bool boss)
{
    this->boss=boss;
    this->id = id;
    position = {x+60,y+60};
    if (boss)
    {
        maxHP = 25;
        hp=25;
        size=2.5;
        speed=800;
        damage=5;
    }
    else
    {
        maxHP = 8;
        hp = 8;
        size = 1;
        speed = 200;
        damage = 2;
    }
    tag = Tag::Enemy;
    circle = Circle(position,50*size);
    collider = Collider(circle, this);
    collider.Register();

    player=nullptr;
    moving=false;
    rotating=false;
    LoadObstacleTextures();
    texture = &mouseTex;

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

Mouse::~Mouse()
{
    collider.Unregister();
}


void Mouse::Draw()
{
    Rectangle src = { 0.0f, 0.0f, (float)texture->width, (float)texture->height };

    float w = (circle.radius*2 + 14);
    float h = (circle.radius*2 + 14);

    Rectangle dest = { circle.center.x, circle.center.y, w, h };
    Vector2 origin = { w/2.0f, h/2.0f };

    DrawTexturePro(*texture, src, dest, origin, rotation, WHITE);
    //collider.DebugDraw();
}

float Mouse::RandomAngle()
{
    return Run::GenerateinRange(10, 360, tag,id);
}
float Mouse::AngleToPlayer()
{
    Vector2 m = position;
    Vector2 p = player->GetPosition();
    float a = atan2f(p.y - m.y, p.x - m.x) * RAD2DEG;
    if (a < 0) a += 360.0f;
    a += 180.0f;
    if (a >= 360.0f) a -= 360.0f;
    return a;
}
void Mouse::Move(float dt)
{
    Vector2 old = position;
    float r = rotation * DEG2RAD;
    position.x -= cosf(r) * speed * dt;
    position.y -= sinf(r) * speed * dt;

    circle.center = position;
    collider.SetCircle(circle);

    CollisionCheck(old);
}

void Mouse::CollisionCheck(Vector2 oldpos)
{
    auto& colliders = Collider::GetAllColliders();
    float dt = GetFrameTime();
    for (auto& othercollider : colliders)
    {
        if (othercollider == &collider)
            continue;
        Asset* parent = othercollider->GetParent();
        if (parent->GetTag()==Tag::Player && collider.IsColliding(*othercollider)) {
            moving = false;
        }
        else if (parent->GetTag()==Tag::Solid && collider.IsColliding(*othercollider))
        {
            float r = rotation * DEG2RAD;
            float back = speed * dt + 2.0f;
            position.x += cosf(r) * back;
            position.y += sinf(r) * back;

            circle.center = position;
            collider.SetCircle(circle);
            moving = false;
        }
    }
}

float deltaangle;

void Mouse::Update()
{
    float dt = GetFrameTime();
    if (!moving)
    {
        if (!rotating)
        {
            int k = Run::GenerateinRange(1,5,tag,id);
            if (k == 1||boss) newAngle =  AngleToPlayer();
            else newAngle = RandomAngle();
            float d = newAngle - rotation;
            deltaangle = (d > 0 ? 1 : -1) * speed * dt;
            if (deltaangle!=0)rotating = true;
        }
        else
        {
            float d = newAngle - rotation;
            if (fabsf(d) <= fabsf(deltaangle))
            {
                SetRotation(newAngle);
                rotating = false;
                moving = true;
            }
            else Rotate(deltaangle);
        }
    }
    else
    {
        Move(dt);
    }
}


void Mouse::SetRotation(float angle)
{
    rotation = angle;
}

void Mouse::Rotate(float deltaAngle)
{
    rotation += deltaAngle;
}

void Mouse::Damage(float damage)
{
    hp -= damage;
}
