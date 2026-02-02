#ifndef ROGUELIKE_LEVELMAP_H
#define ROGUELIKE_LEVELMAP_H
#include "Asset.h"
#include "Room.h"

#include <vector>
#include <memory>

class LevelMap: public Asset
{
private:
    vector<unique_ptr<Room>>* rooms=nullptr;
    Camera2D* camera;
    float cellSize;

    void TryFindCamera();

public:
    LevelMap();
    LevelMap(vector<unique_ptr<Room>>& rooms, Camera2D* camera=nullptr);
    void Init(vector<unique_ptr<Room>>& rooms, Camera2D* camera=nullptr);
    void SetCamera(Camera2D* cam){camera = cam;}
    void Update() override;
    void Draw() override;
};

#endif //ROGUELIKE_LEVELMAP_H
