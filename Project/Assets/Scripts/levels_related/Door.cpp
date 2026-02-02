//
// Created by vanya on 1/6/26.
//

#include "Door.h"
#include "Room.h"

Door::Door(Vector2 position, Room* thisroom, Room* otherroom, Door* otherdoor, bool rotate)
{
    this->rotate = rotate;
    this->thisRoom = thisroom;
    this->otherRoom = otherroom;
    otherSide = otherdoor;
    tag=Tag::Door;
    textures[0] = LoadTexture("Assets/Sprites/objects/dooropen.png");
    textures[1] = LoadTexture("Assets/Sprites/objects/doorclosed.png");
    this->position = position;
    if (rotate) position.x -= 100;
     rectangle = {position.x, position.y, (float)textures[0].width * 0.2f, (float)textures[0].height * 0.2f};
    collider = Collider(rectangle,this);
    collider.Register();
    opened=true;
}

Door::~Door()
{
    collider.Unregister();
    UnloadTexture(textures[0]);
    UnloadTexture(textures[1]);
}

void Door::ConnectDoors(Door* otherdoor)
{
    otherSide = otherdoor;
    if (otherdoor->GetOtherSide()==nullptr) otherdoor->ConnectDoors(this);
}

void Door::Draw()
{
    float rotation = 0.0f;
    if (rotate) rotation = 90.0f;
    DrawTextureEx(textures[!opened], position, rotation, 0.2, WHITE);
    //DrawRectangleRec( rectangle , (Color){ 0, 0, 0, 150 });
}

void Door::Use(Vector2* position, MainCamera* camera)
{
    if (opened)
    {
        int dx = otherRoom->getX()- thisRoom->getX();
    int dy = otherRoom->getY() - thisRoom->getY();
    if (dx == 1 && dy == 0) position->x += 600;
    else if (dx == -1 && dy == 0) position->x -= 650;
    else if (dx == 0 && dy == 1) position->y += 500;
    else if (dx == 0 && dy == -1) position->y -= 500;
    camera->Move(dx, dy);
        if (thisRoom) thisRoom->Freeze();
        if (otherRoom) otherRoom->Activate();
}
}

void Door::Update()
{
    opened = thisRoom->IsActive() && thisRoom->GetEnemiesCount()==0;
}
