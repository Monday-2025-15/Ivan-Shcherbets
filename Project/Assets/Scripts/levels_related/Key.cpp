#include "Key.h"

#include <iostream>

static Texture2D keyTex;
static bool g_keyLoaded = false;

static void LoadKeyTexture()
{
    if (g_keyLoaded) return;
    keyTex = LoadTexture("Assets/Sprites/objects/key.png");
    g_keyLoaded = true;
}

Key::Key(Vector2 position)
{
    tag = Tag::Key;
    LoadKeyTexture();

    this->position = position;

    Rectangle r = { position.x, position.y, 100, 100 };
    collider = Collider(r, this);
    collider.Register();
}

Key::~Key()
{
    collider.Unregister();
}

void Key::Draw()
{
    if (pickedup) return;

    Rectangle src = { 0.0f, 0.0f, (float)keyTex.width, (float)keyTex.height };
    Rectangle dest = {
        position.x,
        position.y,
        100,
        100
    };
    Vector2 origin = { 0, 0 };


    DrawTexturePro(keyTex, src, dest, origin, 0, WHITE);
}

void Key::Update()
{
    cout << pickedup << endl;
}

void Key::PickUp(int &keycount)
{
    if (pickedup) return;
    pickedup = true;
    visible = false;
    collider.Unregister();
    keycount++;
}
