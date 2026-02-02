//
// Created by vanya on 12/8/25.
//

#ifndef ROGUELIKE_ROOM_H
#define ROGUELIKE_ROOM_H
#include <vector>
#include <Run.h>
#include <algorithm>
#include <memory>
#include "Door.h"

class Player;
enum room_type
{
    null,
    start,
    common,
    treasure,
    shop,
    big,
    boss
};

class Room
{
    int id = -1;
    room_type type;
    vector<int> neighbors;
    int depth;
    pair<int,int> cords{0,0};
    vector<unique_ptr<Door>> doors;
    Player* player = nullptr;
    static const int GRID_W = 13;
    static const int GRID_H = 7;
    static const int CELL = 114;
    Vector2 gridOrigin = {220, 135};
    unique_ptr<Asset> grid[GRID_W][GRID_H];
    int nextEnemyId;
    bool activeroom;
    int enemiescount=0;
    bool discovered=false;
    bool keycreated=false;
public:
    Room(room_type type, int depth, int x, int y, int id);
    int getId(){return id;}
    room_type getType(){return type;}
    void setType(room_type type){this->type = type;}
    int getDegree() const { return (int)neighbors.size(); }
    int getDepth(){return depth;}
    void setDepth(int depth){this->depth = depth;}
    int getX() const { return cords.first; }
    int getY() const { return cords.second; }
    const vector<int>& getNeighbors() const { return neighbors; }
    bool hasNeighbor(int other) const {
        return find(neighbors.begin(), neighbors.end(), other) != neighbors.end();
    }
    void addNeighbor(int other) {
        if (other == id) return;
        if (hasNeighbor(other)) return;
        neighbors.push_back(other);
    }
    void Draw(Texture2D roomTex, int lvl);
    void SetDoors(vector<unique_ptr<Room>>& allRooms);
    vector<unique_ptr<Door>>& GetDoors() { return doors; }
    void FillRoom(Tag tag, int lvl);
    void PlaceBoss(Tag tag, int lvl);
    void FillTreasures(Tag tag, int lvl);
    void FillStart(Tag tag, int lvl);
    int GetGridW(){return GRID_W;}
    int GetGridH(){return GRID_H;}
    Vector2 GetCellPosition(int i, int j);
    Vector2 GetCellCenter(int i, int j);
    bool IsCellFree(int i, int j);
    void Update();
    void Freeze(){activeroom = false;}
    void Activate(){activeroom = true; discovered=true;}
    bool IsActive(){return activeroom;}
    int GetEnemiesCount(){return enemiescount;}
    bool IsDiscovered(){return discovered;}
    void FillShop(Tag tag, int lvl);
    Asset* GetCell(int x, int y){return grid[x][y].get();}
    };



#endif //ROGUELIKE_ROOM_H