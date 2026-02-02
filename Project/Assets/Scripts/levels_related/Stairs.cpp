#include "Stairs.h"

#include <iostream>

static Texture2D stairsTex;
static Texture2D stairsLockedTex;
static bool g_stairsLoaded = false;

static void LoadStairsTextures()
{
    if (g_stairsLoaded) return;
    stairsTex = LoadTexture("Assets/Sprites/objects/stairs.png");
    stairsLockedTex = LoadTexture("Assets/Sprites/objects/stairslocked.png");
    g_stairsLoaded = true;
}

Stairs::Stairs(Vector2 position, int lvl)
{
    tag = Tag::Stairs;
    LoadStairsTextures();

    this->position = position;
    this->lvl = lvl;

    used = false;
    unlocked = false;

    Rectangle r = { position.x, position.y, 100, 100 };
    collider = Collider(r, this);
    collider.Register();
}

Stairs::~Stairs()
{
    collider.Unregister();
}

void Stairs::Use()
{
    if (!unlocked) return;
    used = true;
}

void Stairs::Draw()
{
    Rectangle src = { 0.0f, 0.0f, (float)stairsTex.width, (float)stairsTex.height };
    Rectangle dest = {
        position.x,
        position.y,
        100,
        100
    };
    Vector2 origin = { 0, 0 };



    if (unlocked) DrawTexturePro(stairsTex, src, dest, origin, 0, WHITE);
    else          DrawTexturePro(stairsLockedTex, src, dest, origin, 0, WHITE);
    //collider.DebugDraw();
}

void Stairs::Update()
{
    cout << unlocked << used << endl;
}
