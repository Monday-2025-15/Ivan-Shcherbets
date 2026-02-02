#include "LevelMap.h"
#include "Collider.h"
#include "Player.h"

#include <algorithm>
#include <cmath>

static Texture2D MapTex[6];
static bool loaded = false;

static void LoadMapTextures()
{
    if (loaded) return;
    MapTex[0] = LoadTexture("Assets/Sprites/Map/room.png");
    MapTex[1] = LoadTexture("Assets/Sprites/Map/roomcleared.png");
    MapTex[2] = LoadTexture("Assets/Sprites/Map/shop.png");
    MapTex[3] = LoadTexture("Assets/Sprites/Map/treasure.png");
    MapTex[4] = LoadTexture("Assets/Sprites/Map/boss.png");
    MapTex[5] = LoadTexture("Assets/Sprites/Map/start.png");
    loaded = true;
}

static bool PlayerMoving()
{
    return IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT);
}

static bool Discovered(Room* r)
{
    if (!r) return false;
    if (r->getType()==room_type::start) return true;
    if (r->IsDiscovered()) return true;
    if (r->IsActive()) return true;
    return false;
}

static bool BossRevealed(Room* boss, std::vector<Room*>& byId)
{
    if (!boss) return false;
    for (int id : boss->getNeighbors())
    {
        if (id < 0 || id >= (int)byId.size()) continue;
        if (Discovered(byId[id])) return true;
    }
    return false;
}

LevelMap::LevelMap(std::vector<std::unique_ptr<Room>>& rooms, Camera2D* camera)
{
    this->camera = camera;
    cellSize = 70.0f * 4.0f;
    tag = Tag::None;
    LoadMapTextures();
}

LevelMap::LevelMap()
{
    cellSize = 70.0f * 4.0f;
    tag = Tag::None;
    LoadMapTextures();
}

void LevelMap::Init(vector<unique_ptr<Room>>& rooms, Camera2D* camera)
{
    this->rooms=&rooms;
    this->camera=camera;
}

void LevelMap::TryFindCamera()
{
    if (camera) return;
    auto& colliders = Collider::GetAllColliders();
    for (auto& c : colliders)
    {
        if (!c) continue;
        Asset* p = c->GetParent();
        if (!p) continue;
        if (p->GetTag()!=Tag::Player) continue;

        Player* player = dynamic_cast<Player*>(p);
        if (!player) continue;

        camera = &player->GetCamera();
        break;
    }
}

void LevelMap::Update()
{
    LoadMapTextures();
    TryFindCamera();
}

void LevelMap::Draw()
{
    static bool opened = false;
    if (IsKeyPressed(KEY_TAB)) opened = !opened;
    if (!opened) return;

    Update();

    unsigned char a = 255;
    if (PlayerMoving()) a = 120;

    float screenW = 1920.0f;
    float screenH = 1080.0f;
    if (!camera)
    {
        screenW = (float)GetScreenWidth();
        screenH = (float)GetScreenHeight();
    }
    else if (camera->offset.x > 0.01f && camera->offset.y > 0.01f)
    {
        screenW = camera->offset.x * 2.0f;
        screenH = camera->offset.y * 2.0f;
    }

    int baseX = 0;
    int baseY = 0;
    bool hasBase = false;
    for (auto& r : *rooms)
    {
        if (!r) continue;
        if (r->getType()!=room_type::start) continue;
        baseX = r->getX();
        baseY = r->getY();
        hasBase = true;
        break;
    }
    if (!hasBase)
    {
        for (auto &r : *rooms)
        {
            if (!r) continue;
            baseX = r->getX();
            baseY = r->getY();
            hasBase = true;
            break;
        }
    }
    if (!hasBase) return;


    float zoom = 1.0f;
    Vector2 target = {0,0};
    Vector2 offset = {0,0};
    if (camera)
    {
        zoom = camera->zoom;
        if (zoom < 0.001f) zoom = 1.0f;
        target = camera->target;
        offset = camera->offset;
    }

    bool hasCur = false;
    int curX = baseX;
    int curY = baseY;
    if (camera)
    {
        Vector2 tl = {target.x - offset.x/zoom, target.y - offset.y/zoom};
        curX = (int)roundf(tl.x / 1920.0f) + baseX;
        curY = (int)roundf(tl.y / 1080.0f) + baseY;
        hasCur = true;
    }
    else hasCur = true;

    int minX = 100000, minY = 100000, maxX = -100000, maxY = -100000;
    int maxId = -1;
    for (auto& r : *rooms)
    {
        if (!r) continue;
        minX = std::min(minX, r->getX());
        maxX = std::max(maxX, r->getX());
        minY = std::min(minY, r->getY());
        maxY = std::max(maxY, r->getY());
        maxId = std::max(maxId, r->getId());
    }
    if (maxId < 0) return;

    int w = maxX - minX + 1;
    int h = maxY - minY + 1;

    float tileScreen = cellSize;
    float mapWscreen = w * tileScreen;
    float mapHscreen = h * tileScreen;

    float maxW = screenW * 0.85f;
    float maxH = screenH * 0.85f;

    float s1 = 1.0f; if (mapWscreen > maxW) s1 = maxW / mapWscreen;
    float s2 = 1.0f; if (mapHscreen > maxH) s2 = maxH / mapHscreen;
    float s = std::min(s1, s2);
    if (s < 0.25f) s = 0.25f;

    tileScreen *= s;
    mapWscreen = w * tileScreen;
    mapHscreen = h * tileScreen;

    float sx = screenW/2.0f - mapWscreen/2.0f;
    float sy = screenH/2.0f - mapHscreen/2.0f;

    float tile = tileScreen / zoom;
    float mapW = mapWscreen / zoom;
    float mapH = mapHscreen / zoom;

    float x0 = sx;
    float y0 = sy;
    if (camera)
    {
        x0 = target.x + (sx - offset.x)/zoom;
        y0 = target.y + (sy - offset.y)/zoom;
    }

    float pad = (28.0f * s) / zoom;

    Color back = {255,255,255,(unsigned char)(a*0.90f)};
    Color line = {0,0,0,a};
    Color outline = {0,0,0,a};

    DrawRectangle((int)(x0-pad), (int)(y0-pad), (int)(mapW+pad*2), (int)(mapH+pad*2), back);
    DrawRectangleLinesEx((Rectangle){x0-pad, y0-pad, mapW+pad*2, mapH+pad*2}, 6.0f*s/zoom, outline);

    float roomFrac = 0.75f;
    float roomSize = tile * roomFrac;
    float roomOff = (tile - roomSize) / 2.0f;

    std::vector<Vector2> centers;
    std::vector<bool> has;
    std::vector<Room*> byId;
    centers.resize(maxId+1);
    has.resize(maxId+1, 0);
    byId.resize(maxId+1, nullptr);

    for (auto& r : *rooms)
    {
        if (!r) continue;
        int id = r->getId();
        if (id < 0 || id > maxId) continue;

        float bx = x0 + (r->getX() - minX) * tile;
        float by = y0 + (r->getY() - minY) * tile;

        centers[id] = { bx + tile/2.0f, by + tile/2.0f };
        has[id] = 1;
        byId[id] = r.get();
    }

    float lineTh = 10.0f*s/zoom;
    Color curFill = {0,255,0,(unsigned char)(a*0.20f)};
    Color curLine = {0,255,0,a};
    float curTh = 8.0f*s/zoom;

    for (auto& r : *rooms)
    {
        if (!r) continue;
        int aId = r->getId();
        if (aId < 0 || aId > maxId) continue;
        if (!has[aId]) continue;

        Vector2 aC = centers[aId];

        for (int bId : r->getNeighbors())
        {
            if (bId <= aId) continue;
            if (bId < 0 || bId > maxId) continue;
            if (!has[bId]) continue;

            Vector2 bC = centers[bId];
            Vector2 d = { bC.x - aC.x, bC.y - aC.y };
            float l = sqrtf(d.x*d.x + d.y*d.y);
            if (l < 0.001f) continue;
            d.x /= l; d.y /= l;

            float cut = roomSize/2.0f + 3.0f*s/zoom;
            Vector2 p1 = { aC.x + d.x*cut, aC.y + d.y*cut };
            Vector2 p2 = { bC.x - d.x*cut, bC.y - d.y*cut };

            DrawLineEx(p1, p2, lineTh, line);
        }
    }

    for (auto& r : *rooms)
    {
        if (!r) continue;

        int id = r->getId();
        if (id < 0 || id > maxId) continue;

        float bx = x0 + (r->getX() - minX) * tile;
        float by = y0 + (r->getY() - minY) * tile;

        Rectangle dest = { bx + roomOff, by + roomOff, roomSize, roomSize };

        bool disc = Discovered(r.get());
        bool beat = disc && r->GetEnemiesCount() <= 0;

        int texId = 0;

        if (r->getType() == room_type::start) texId = 5;
        else if (!disc)
        {
            if (r->getType()==room_type::boss && BossRevealed(r.get(), byId)) texId = 4;
            else texId = 0;
        }
        else
        {
            if (r->getType()==room_type::shop) texId = 2;
            else if (r->getType()==room_type::treasure) texId = 3;
            else if (r->getType()==room_type::boss) texId = 4;
            else
            {
                if (beat) texId = 1;
                else texId = 0;
            }
        }

        if (hasCur && r->getX()==curX && r->getY()==curY)
            DrawRectangleRec(dest, curFill);

        Texture2D& tex = MapTex[texId];
        Rectangle src = {0,0,(float)tex.width,(float)tex.height};
        DrawTexturePro(tex, src, dest, (Vector2){0,0}, 0.0f, (Color){255,255,255,a});

        if (hasCur && r->getX()==curX && r->getY()==curY)
            DrawRectangleLinesEx(dest, curTh, curLine);
    }
}
