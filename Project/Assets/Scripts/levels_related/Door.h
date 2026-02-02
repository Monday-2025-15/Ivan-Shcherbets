//
// Created by vanya on 1/6/26.
//

#ifndef ROGUELIKE_DOOR_H
#define ROGUELIKE_DOOR_H
#include <raylib.h>
#include <Collider.h>

#include "MainCamera.h"
class Room;

class Door : public Asset
{
    private:
    Texture2D textures[2];
    Collider collider;
    bool opened;
    Room* thisRoom = nullptr;
    Room* otherRoom = nullptr;
    Door* otherSide = nullptr;
    bool rotate = false;
    Rectangle rectangle ;
    public:
    Door(Vector2 position, Room* thisroom, Room* otherroom, Door* otherdoor, bool rotate);
    ~Door();
    void ConnectDoors(Door* otherdoor);
    Door* GetOtherSide(){return otherSide;}
    Room* GetThisRoom() const { return thisRoom; }
    Room* GetOtherRoom() const { return otherRoom; }
    void Draw() override;
    void Use(Vector2* position, MainCamera* camera);
    void Activate(){opened=true;}
    void Update() override;
};


#endif //ROGUELIKE_DOOR_H