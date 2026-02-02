//
// Created by vanya on 12/8/25.
//

#include "Floor.h"
#include <numeric>
#include <Run.h>
#include <algorithm>

#include <iostream>

#include "Player.h"

bool InBounds(int x, int y)
{
    return x >= 0 && x < 20 && y >= 0 && y < 20;
}
Floor::Floor(int roomscount, int level)
{
    tag = Tag::Floor;
    lvl=level;
    if (lvl == 0) roomTex = LoadTexture("Assets/Sprites/Rooms/basement.png");
    else if (lvl == 5) roomTex = LoadTexture("Assets/Sprites/Rooms/roof.png");
    else roomTex = LoadTexture("Assets/Sprites/Rooms/room.png");
    for (int x = 0; x < 20; x++)
        for (int y = 0; y < 20; y++)
            grid[x][y] = room_type::null;
    rooms.push_back(make_unique<Room>(room_type::start, 0, 10, 10,0));
    grid[10][10] = room_type::start;
    int dx[4] = { 1, -1,  0,  0 };
    int dy[4] = { 0,  0,  1, -1 };
    while ((int)rooms.size() < roomscount)
    {
        int currentsize = rooms.size();
        vector<int> weights(currentsize,0);
        for (int i=0;i<currentsize;i++)
        {
            if (rooms[i]->getDegree() == 4)continue;
            double wDegree = 1.0/(1.0+rooms[i]->getDegree());
            double wDepth = 0.15/(1.0+rooms[i]->getDepth());
            double weightD= wDegree+wDepth;
            weights[i] = (int)(weightD*10000);
            if (weights[i] < 1) weights[i] = 1;
        }
        int total = accumulate(weights.begin(),weights.end(),0);
        if (total <= 0) break;
        int result = Run::GenerateinRange(1, total, tag, lvl);
        int parent=-1;
        int sum=0;
        for (int i=0;i<currentsize;i++)
        {
            sum += weights[i];
            if (result <= sum)
            {
                parent=i;
                break;
            }
        }
        int px = rooms[parent]->getX();
        int py = rooms[parent]->getY();
        int order[4] = {0,1,2,3};
        for (int k = 3; k > 0; k--) {
            int j = Run::GenerateinRange(0, k, tag, lvl);
            std::swap(order[k], order[j]);
        }
        bool placed = false;
        int nx=0, ny=0;
        for (int t = 0; t < 4; t++) {
            int dir = order[t];
            nx = px + dx[dir];
            ny = py + dy[dir];
            if (!InBounds(nx, ny)) continue;
            if (grid[nx][ny] != room_type::null) continue;
            placed = true;
            break;
        }
        if (!placed) continue;
        int newId = (int)rooms.size();
        int newDepth = rooms[parent]->getDepth() + 1;
        rooms.push_back(make_unique<Room>(room_type::common, newDepth, nx, ny,newId));
        grid[nx][ny] = room_type::common;
        rooms[parent]->addNeighbor(newId);
        rooms[newId]->addNeighbor(parent);
    }
    PlaceSpecial();
    AutoConnect();
    for (auto& r : rooms)
    {
        r->SetDoors(rooms);
        if (r->getType()==room_type::common) r->FillRoom(tag, level);
        else if (r->getType()==room_type::boss) r->PlaceBoss(tag, level);
        else if (r->getType()==room_type::treasure) r->FillTreasures(tag, level);
        else if (r->getType()==room_type::shop) r->FillShop(tag, level);
        else
        {
            r->FillStart(tag, level);
            stairs = dynamic_cast<Stairs*>(r->GetCell(6,3));
        }
    }
    auto& colliders = Collider::GetAllColliders();
    for (auto& othercollider : colliders)
    {
        if (othercollider->GetParent()->GetTag()==Tag::Player)
        {
            dynamic_cast<Player*>(othercollider->GetParent())->InitMap(rooms);
        }
    }
}
void Floor::PlaceSpecial()
{
    std::vector<int> leaves;
    leaves.reserve(rooms.size());

    for (int i = 0; i < (int)rooms.size(); i++)
    {
        if (rooms[i]->getType() == room_type::start) continue;
        if (rooms[i]->getType() == room_type::boss) continue;
        if (rooms[i]->getType() == room_type::treasure) continue;
        if (rooms[i]->getType() == room_type::shop) continue;

        if (rooms[i]->getDegree() == 1) leaves.push_back(i);
    }

    std::sort(leaves.begin(), leaves.end(), [&](int a, int b) {
        return rooms[a]->getDepth() > rooms[b]->getDepth();
    });

    int bossId = -1;
    int treasureId = -1;
    int shopId = -1;

    int size = (int)leaves.size();

    if (size >= 3)
    {
        bossId = leaves[0];

        int mid = size / 2;
        if (mid < 1) mid = 1;

        int shopid = Run::GenerateinRange(1, mid, tag, lvl);
        int treasureid = Run::GenerateinRange(mid, size - 1, tag, lvl);

        if (shopid == treasureid)
        {
            if (shopid < size - 1) shopid++;
            else shopid = mid;
            if (shopid == treasureid) shopid = size - 1;
        }

        treasureId = leaves[treasureid];
        shopId = leaves[shopid];
    }
    else
    {
        int best = -100000;
        for (int i = 0; i < (int)rooms.size(); i++)
        {
            if (rooms[i]->getType()==room_type::start) continue;
            if (rooms[i]->getDepth() > best)
            {
                best = rooms[i]->getDepth();
                bossId = i;
            }
        }
        if (bossId < 0) return;

        std::vector<int> pool;
        pool.reserve(rooms.size());
        for (int i = 0; i < (int)rooms.size(); i++)
        {
            if (i == bossId) continue;
            if (rooms[i]->getType()==room_type::start) continue;
            pool.push_back(i);
        }

        if ((int)pool.size() >= 1)
        {
            int ti = Run::GenerateinRange(0, (int)pool.size()-1, tag, lvl);
            treasureId = pool[ti];
            pool.erase(pool.begin() + ti);
        }

        if ((int)pool.size() >= 1)
        {
            int si = Run::GenerateinRange(0, (int)pool.size()-1, tag, lvl);
            shopId = pool[si];
        }
    }

    if (bossId >= 0)
    {
        rooms[bossId]->setType(room_type::boss);
        grid[rooms[bossId]->getX()][rooms[bossId]->getY()] = room_type::boss;
    }

    if (treasureId >= 0)
    {
        rooms[treasureId]->setType(room_type::treasure);
        grid[rooms[treasureId]->getX()][rooms[treasureId]->getY()] = room_type::treasure;
    }

    if (shopId >= 0)
    {
        rooms[shopId]->setType(room_type::shop);
        grid[rooms[shopId]->getX()][rooms[shopId]->getY()] = room_type::shop;
    }
}

void Floor::AutoConnect()
{
    int dx[4] = { 1, -1,  0,  0 };
    int dy[4] = { 0,  0,  1, -1 };

    int idAt[20][20];
    for (int x = 0; x < 20; x++)
        for (int y = 0; y < 20; y++)
            idAt[x][y] = -1;

    for (int rid = 0; rid < (int)rooms.size(); rid++)
        idAt[rooms[rid]->getX()][rooms[rid]->getY()] = rid;

    for (int a = 0; a < (int)rooms.size(); a++)
    {
        if (rooms[a]->getType() != room_type::common && rooms[a]->getType() != room_type::start) continue;

        int x = rooms[a]->getX();
        int y = rooms[a]->getY();

        for (int d = 0; d < 4; d++)
        {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (!InBounds(nx, ny)) continue;

            int b = idAt[nx][ny];
            if (b < 0) continue;
            if (rooms[b]->getType() != room_type::common && rooms[b]->getType() != room_type::start) continue;

            if (b > a) {
                rooms[a]->addNeighbor(b);
                rooms[b]->addNeighbor(a);
            }
        }
    }
}
void Floor::Draw()
{
    for (auto& room: rooms)
    {
        room->Draw(roomTex, lvl);
    }
}

Floor::~Floor()
{
    UnloadTexture(roomTex);
}

void Floor::Update()
{
    for (auto& room: rooms)
    {
        room->Update();
    }
}
