//
// Created by user on 14.11.2025.
//

#include "Block.h"

static Texture2D g_chairTex;
static Texture2D g_deskTex;
static Texture2D g_shelfTex;
static bool g_obstacleTexturesLoaded = false;

static void LoadObstacleTextures()
{
    if (g_obstacleTexturesLoaded) return;
    g_chairTex = LoadTexture("Assets/Sprites/objects/chair.png");
    g_deskTex  = LoadTexture("Assets/Sprites/objects/desk.png");
    g_shelfTex = LoadTexture("Assets/Sprites/objects/shelf.png");
    g_obstacleTexturesLoaded = true;
}

Block::Block(float x, float y, ObstacleType type)
    : rectangle{ x, y, 114.0f, 114.0f }
{
    collider = Collider({x,y,100,100}, this);
    tag = Tag::Solid;
    LoadObstacleTextures();

    switch (type)
    {
    case ObstacleType::Chair:
        texture = &g_chairTex;
        break;
    case ObstacleType::Desk:
        texture = &g_deskTex;
        break;
    case ObstacleType::Shelf:
        texture = &g_shelfTex;
        break;
    default:
        texture = nullptr;
        break;
    }

    collider.Register();
}

Block::~Block()
{
    collider.Unregister();
}

void Block::Draw()
{
    if (!texture)
    {
        DrawRectangleRec(rectangle, GRAY);
        return;
    }

    Rectangle src = { 0.0f, 0.0f, (float)texture->width, (float)texture->height };
    Rectangle dest = {
        rectangle.x + rectangle.width / 2.0f,
        rectangle.y + rectangle.height / 2.0f,
        rectangle.width,
        rectangle.height
    };
    Vector2 origin = { rectangle.width / 2.0f, rectangle.height / 2.0f };

    DrawTexturePro(*texture, src, dest, origin, rotation, WHITE);
    //collider.DebugDraw();
}

void Block::SetRotation(float angle)
{
    rotation = angle;
}

void Block::Rotate(float deltaAngle)
{
    rotation += deltaAngle;
}

void Block::SetPosition(float x, float y)
{
    rectangle.x = x;
    rectangle.y = y;
}

const Rectangle& Block::GetRect() const
{
    return rectangle;
}

float Block::GetRotation() const
{
    return rotation;
}

Texture2D* Block::GetTexture() const
{
    return texture;
}

void Block::SetTexture(Texture2D* tex)
{
    texture = tex;
}
