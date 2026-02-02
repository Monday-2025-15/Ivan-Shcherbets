//
// Created by vanya on 1/5/26.
//

#include "MainCamera.h"

MainCamera::MainCamera()
    : colliders{
        Collider({160, 75, 60, 918}, this),
        Collider({220, 75, 1482, 60}, this),
        Collider({1702, 75, 60, 918}, this),
        Collider({220, 933, 1482, 60}, this)
      }
{
    tag = Tag::Solid;

    camera = Camera2D{};
    currentPosition = {0.0f, 0.0f};
    camera.target = currentPosition;
    camera.offset = {0.0f, 0.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    for (int i = 0; i < 4; i++) colliders[i].Register();
}

void MainCamera::Move(int dx, int dy)
{
    currentPosition = Vector2(currentPosition.x + dx* 1920, currentPosition.y + dy* 1080);
    camera.target = currentPosition;
    for (int i = 0; i < 4; i++)
    {
        Rectangle rect = colliders[i].GetRectangle();
        rect.x += dx*1920;
        rect.y += dy*1080;
        colliders[i].SetRectangle(rect);
    }
}
