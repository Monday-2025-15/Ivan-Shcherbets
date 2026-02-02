//
// Created by vanya on 12/8/25.
//

#ifndef ROGUELIKE_FLOOR_H
#define ROGUELIKE_FLOOR_H
#include <vector>

#include "Room.h"
#include "Asset.h"
#include <memory>

#include "LevelMap.h"
#include "Stairs.h"

class Floor : public Asset
{
    private:
    int lvl;
    room_type grid[20][20];
    vector<unique_ptr<Room>> rooms;
    Texture2D roomTex;
    void AutoConnect();
    void PlaceSpecial();
    Stairs* stairs;
    public:
    Floor(int roomscount, int level);
    void Draw() override;
    void Update() override;
    ~Floor();
    vector<unique_ptr<Room>>& GetRooms(){return rooms;}
    bool IsComplete(){if (stairs) return stairs->IsUsed(); else return false;}
};


#endif //ROGUELIKE_FLOOR_H