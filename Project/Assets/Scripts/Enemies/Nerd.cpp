#include "Nerd.h"

#include "Player.h"
#include "Collider.h"
#include "Run.h"

#include <string>
#include <unordered_set>
#include <cmath>

using namespace std;

static bool g_taskActive = false;
static float g_taskTimer = 0.0f;
static int g_taskAnswer = 0;
static string g_taskText;

static Nerd* g_taskOwner = nullptr;
static unordered_set<Nerd*> g_liveNerds;

static constexpr float V_W = 1920.0f;
static constexpr float V_H = 1080.0f;
static Texture2D nerdTex;
static bool g_obstacleTexturesLoaded = false;


static void LoadObstacleTextures()
{
    if (g_obstacleTexturesLoaded) return;
    nerdTex = LoadTexture("Assets/Sprites/enemies/nerd.png");
    g_obstacleTexturesLoaded = true;
}

static int ReadDigitKey()
{
    if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_KP_0)) return 0;
    if (IsKeyPressed(KEY_ONE)  || IsKeyPressed(KEY_KP_1)) return 1;
    if (IsKeyPressed(KEY_TWO)  || IsKeyPressed(KEY_KP_2)) return 2;
    if (IsKeyPressed(KEY_THREE)|| IsKeyPressed(KEY_KP_3)) return 3;
    if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) return 4;
    if (IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_KP_5)) return 5;
    if (IsKeyPressed(KEY_SIX)  || IsKeyPressed(KEY_KP_6)) return 6;
    if (IsKeyPressed(KEY_SEVEN)|| IsKeyPressed(KEY_KP_7)) return 7;
    if (IsKeyPressed(KEY_EIGHT)|| IsKeyPressed(KEY_KP_8)) return 8;
    if (IsKeyPressed(KEY_NINE) || IsKeyPressed(KEY_KP_9)) return 9;
    return -1;
}

static Vector2 VirtualWorldPoint(const Camera2D& cam, float vx, float vy)
{
    return GetScreenToWorld2D({vx, vy}, cam);
}

Nerd::Nerd(float x, float y, int id)
{
    tag = Tag::Enemy;

    this->id = id;

    hp = 15.0f;
    maxHP = 15.0f;
    speed = 0.0f;
    size = 1.0f;

    position = { x, y };

    collider = Collider({ position.x, position.y, 90, 90 }, this);
    collider.Register();

    g_liveNerds.insert(this);
    LoadObstacleTextures();
}

Nerd::~Nerd()
{
    if (g_taskActive && g_taskOwner == this)
    {
        g_taskActive = false;
        g_taskOwner = nullptr;
        g_taskText.clear();
        g_taskTimer = 0.0f;
        g_taskAnswer = 0;
    }

    collider.Unregister();
    g_liveNerds.erase(this);
}

void Nerd::FindPlayerPtr()
{
    if (player) return;

    auto& colliders = Collider::GetAllColliders();
    for (auto& c : colliders)
    {
        if (!c) continue;
        Asset* parent = c->GetParent();
        if (!parent) continue;

        if (parent->GetTag() == Tag::Player)
        {
            player = dynamic_cast<Player*>(parent);
            break;
        }
    }
}

void Nerd::TryStartTask()
{
    if (!player) return;
    if (cooldown > 0.0f) return;
    if (g_taskActive) return;

    int ans = Run::GenerateinRange(0, 9, tag, id);
    int op  = Run::GenerateinRange(0, 1, tag, id);

    int a = 0;
    int b = 0;

    if (op == 0)
    {
        a = Run::GenerateinRange(0, ans, tag, id);
        b = ans - a;
        g_taskText = to_string(a) + " + " + to_string(b) + " = ?";
    }
    else
    {
        a = Run::GenerateinRange(ans, 9, tag, id);
        b = a - ans;
        g_taskText = to_string(a) + " - " + to_string(b) + " = ?";
    }

    g_taskAnswer = ans;
    g_taskTimer = 3.0f;
    g_taskActive = true;
    g_taskOwner = this;
}

void Nerd::UpdateTask(float dt)
{
    if (!g_taskActive) return;
    if (g_taskOwner != this) return;

    if (g_liveNerds.find(g_taskOwner) == g_liveNerds.end())
    {
        g_taskActive = false;
        g_taskOwner = nullptr;
        return;
    }

    g_taskTimer -= dt;

    int digit = ReadDigitKey();
    if (digit != -1)
    {
        if (player)
        {
            if (digit != g_taskAnswer)
                player->Damage(2.0f);
        }

        g_taskActive = false;
        g_taskOwner = nullptr;
        cooldown = 5.0f;
        return;
    }

    if (g_taskTimer <= 0.0f)
    {
        if (player) player->Damage(2.0f);

        g_taskActive = false;
        g_taskOwner = nullptr;
        cooldown = 5.0f;
        return;
    }
}

void Nerd::Update()
{
    float dt = GetFrameTime();

    FindPlayerPtr();

    if (cooldown > 0.0f) cooldown -= dt;
    if (cooldown < 0.0f) cooldown = 0.0f;

    if (g_taskActive)
    {
        UpdateTask(dt);
        return;
    }

    attackTimer += dt;
    if (attackTimer >= 1.0f)
    {
        attackTimer -= 1.0f;

        if (cooldown <= 0.0f)
        {
            if (Run::GenerateDecision(30, tag, id))
                TryStartTask();
        }
    }
}

void Nerd::Draw()
{
    Rectangle r = collider.GetRectangle();


    Rectangle src = { 0.0f, 0.0f, (float)nerdTex.width, (float)nerdTex.height };

    Rectangle dest = {
        r.x + r.width/2.0f,
        r.y + r.height/2.0f,
        r.width,
        r.height
    };

    Vector2 origin = { r.width/2.0f, r.height/2.0f };

    DrawTexturePro(nerdTex, src, dest, origin, 0.0f, WHITE);
    if (!g_taskActive) return;
    if (g_taskOwner != this) return;
    if (!player) return;

    Camera2D& cam = player->GetCamera();

    float zoom = cam.zoom;
    if (zoom < 0.01f) zoom = 1.0f;

    Color red = {255, 0, 0, 255};

    Vector2 mid = VirtualWorldPoint(cam, V_W * 0.5f, V_H * 0.42f);

    int big = (int)(110.0f / zoom);
    if (big < 32) big = 32;

    int w = MeasureText(g_taskText.c_str(), big);
    float halfW = (float)w / (2.0f * zoom);

    DrawText(g_taskText.c_str(), (int)(mid.x - halfW), (int)mid.y, big, red);

    int sec = (int)ceilf(g_taskTimer);
    if (sec < 0) sec = 0;

    string hint = "Press 0-9   Time: " + to_string(sec);

    int small = (int)(48.0f / zoom);
    if (small < 22) small = 22;

    Vector2 mid2 = VirtualWorldPoint(cam, V_W * 0.5f, V_H * 0.55f);
    int w2 = MeasureText(hint.c_str(), small);
    float halfW2 = (float)w2 / (2.0f * zoom);

    DrawText(hint.c_str(), (int)(mid2.x - halfW2), (int)mid2.y, small, red);
}
