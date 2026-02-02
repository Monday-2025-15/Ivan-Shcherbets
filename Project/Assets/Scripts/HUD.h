//
// Created by vanya on 1/28/26.
//

#ifndef ROGUELIKE_HUD_H
#define ROGUELIKE_HUD_H

#include "Asset.h"
#include "LevelMap.h"

class Player;
class Inventory;

class HUD : public Asset
{
private:
    LevelMap map;
    bool ready = false;

    Player* player = nullptr;
    Inventory* inventory = nullptr;
    Camera2D* camera = nullptr;

    void TryFindPlayer();

public:
    HUD(Player* p = nullptr);

    void InitMap(vector<unique_ptr<Room>>& rooms, Camera2D* camera=nullptr)
    {
        map.Init(rooms, camera);
        ready = true;
        this->camera = camera;
    }

    void Update() override;
    void Draw() override;
    bool IsReady(){return ready;}
};

#endif //ROGUELIKE_HUD_H
