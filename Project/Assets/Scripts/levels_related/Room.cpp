//
// Created by vanya on 12/8/25.
//

#include "Room.h"
#include "Door.h"
#include "Block.h"
#include "Bully.h"
#include "DutyPair.h"
#include "Ghost.h"
#include "Healer.h"
#include "HELICOPTER.h"
#include "Key.h"
#include "Nerd.h"
#include "OldStudent.h"
#include "PETeacher.h"
#include "PhoneGuy.h"
#include "PhysicTeacher.h"
#include "Enemies/Mouse.h"
#include "Enemies/Spider.h"
#include "Player.h"
#include "Security.h"
#include "Slingshoter.h"
#include "Solider.h"
#include "Stairs.h"
#include "YoungStudent.h"

Room::Room(room_type type, int depth, int x, int y, int id): type(type), depth(depth), cords{x,y}, id(id){
    activeroom = false;
    auto& colliders = Collider::GetAllColliders();
    for (auto& othercollider : colliders)
    {
        if (othercollider->GetParent()->GetTag()==Tag::Player)
        {
            player = dynamic_cast<Player*>(othercollider->GetParent());
            break;
        }
    }
}
void Room::Draw(Texture2D roomTex, int lvl)
{
    DrawTexture(roomTex, (getX()-10)*1920, (getY()-10)*1080, WHITE);
    for (auto& door : doors)
    {
        door->Draw();
    }
    for (int i = 0; i < GRID_W; i++)
    {
        for (int j = 0; j < GRID_H; j++)
        {
            if (!grid[i][j]) continue;
            else grid[i][j]->Draw();
        }
    }
    if (type==start && lvl==0)
    {
        const float ox = (getX() - 10) * 1920.0f;
        const float oy = (getY() - 10) * 1080.0f;

        const int fontSize = 36;
        const int lineGap  = 10;

        const char* lines[] = {
            "WASD - movement",
            "Leftclick - shoot",
            "Mouse - Aim",
            "Ctrl - Gadget",
            "Shift - One time use item",
            "E - pickup/interact"
        };

        const int n = (int)(sizeof(lines) / sizeof(lines[0]));

        const float startX = ox + 520.0f;
        const float centerY = oy + 540.0f;
        const float blockH = n * fontSize + (n - 1) * lineGap;
        float y = centerY - blockH * 0.5f;

        for (int i = 0; i < n; ++i)
        {
            DrawText(lines[i], (int)startX + 2, (int)y + 2, fontSize, BLACK);
            y += (float)fontSize + (float)lineGap;
        }
    }

}
void Room::SetDoors(vector<unique_ptr<Room>>& allRooms)
{
    doors.clear();
    doors.reserve(neighbors.size());

    float ox = (getX() - 10) * 1920.0f;
    float oy = (getY() - 10) * 1080.0f;
    for (int id : neighbors)
    {
        Room* other = allRooms[id].get();
        int dx = other->getX() - getX();
        int dy = other->getY() - getY();
        Vector2 pos;
        if (dx == 1 && dy == 0) pos = Vector2(ox+1690+75, 75+450+oy-50);
        else if (dx == -1 && dy == 0) pos = Vector2(ox+160+75, 75+450+oy-50);
        else if (dx == 0 && dy == 1) pos = Vector2(ox+225+730-50, oy+910);
        else if (dx == 0 && dy == -1) pos = Vector2(ox+225+730-50, oy+75-25);
        bool rotate = false;
        if (dx!=0) rotate = true;
        doors.push_back(make_unique<Door>(pos, this, other, nullptr, rotate));
        Door* myDoor = doors.back().get();
        Door* backDoor = nullptr;
        for (auto& d : other->GetDoors())
        {
            if (d->GetOtherRoom() == this) { backDoor = d.get(); break; }
        }
        if (backDoor) myDoor->ConnectDoors(backDoor);
    }
}
void Room::FillRoom(Tag tag, int lvl)
{
    int EnemyAmount    = Run::GenerateinRange(2+lvl*1.5, 5+lvl*1.5, tag, lvl);
    //int EnemyAmount=15;
    enemiescount = EnemyAmount;
    int ObstacleAmount = Run::GenerateinRange(0, 5, tag, lvl);
    //int ObstacleAmount = 27;

    int minI = 2;
    int maxI = GRID_W - 2;
    int minJ = 2;
    int maxJ = GRID_H - 2;

    int amountofcells = 0;
    for (int i = minI; i < maxI; ++i)
        for (int j = minJ; j < maxJ; ++j)
            if (!grid[i][j])
                ++amountofcells;
    nextEnemyId=0;
    for (int k = 0; k < EnemyAmount + ObstacleAmount; ++k)
    {
        if (amountofcells <= 0) break;

        int cell = 0;
        int cellD = Run::GenerateinRange(1, amountofcells, tag, lvl);
        bool placed = false;

        for (int i = minI; i < maxI && !placed; ++i)
        {
            for (int j = minJ; j < maxJ && !placed; ++j)
            {
                if (grid[i][j]) continue;

                if (++cell == cellD)
                {
                    float x = (getX()-10)*1920 + i * CELL + gridOrigin.x;
                    float y = (getY()-10)*1080 + j * CELL + gridOrigin.y;

                    if (k < EnemyAmount)
                    {
                        int res = Run::GenerateinRange(0,100,tag,id);
                        switch (lvl)
                        {
                            case 0:
                            //grid[i][j] = make_unique<HELICOPTER>(x,y,nextEnemyId++);
                            if (res<=50) grid[i][j] = make_unique<Mouse>(x,y,nextEnemyId++);
                            else grid[i][j] = make_unique<Spider>(x,y,nextEnemyId++, *this);
                            break;
                        case 1:
                            if (res<=50)
                            {
                                if(Run::GenerateDecision(50, tag,id))grid[i][j] =  make_unique<Bully>(x,y,nextEnemyId++);
                                else grid[i][j]=make_unique<Nerd>(x,y,nextEnemyId++);
                            }
                            else
                            {
                                int res2 = Run::GenerateinRange(0,100,tag,id);
                                if (res2<=50) grid[i][j] = make_unique<Mouse>(x,y,nextEnemyId++);
                                else grid[i][j] = make_unique<Spider>(x,y,nextEnemyId++, *this);
                            }
                            break;
                        case 2:
                            if (res<=50)
                            {
                                if(Run::GenerateDecision(50, tag,id))grid[i][j] =  make_unique<OldStudent>(x,y,nextEnemyId++);
                                else grid[i][j]=make_unique<Healer>(x,y,nextEnemyId++);
                            }
                            else
                            {
                                int res2 = Run::GenerateinRange(0,100,tag,id);
                                if (res2<=25) grid[i][j] = make_unique<Mouse>(x,y,nextEnemyId++);
                                else if (res2<=50)grid[i][j] = make_unique<Spider>(x,y,nextEnemyId++, *this);
                                else if (res2<=75)grid[i][j] = make_unique<Bully>(x,y,nextEnemyId++);
                                else grid[i][j]=make_unique<Nerd>(x,y,nextEnemyId++);
                            }
                            break;
                        case 3:
                            if (res<=50)
                            {
                                if(Run::GenerateDecision(50, tag,id))grid[i][j] =  make_unique<YoungStudent>(x,y,nextEnemyId++);
                                else grid[i][j]=make_unique<Slingshoter>(x,y,nextEnemyId++);
                            }
                            else
                            {
                                int res2 = Run::GenerateinRange(0,150,tag,id);
                                if (res2<=25) grid[i][j] = make_unique<Mouse>(x,y,nextEnemyId++);
                                else if (res2<=50)grid[i][j] = make_unique<Spider>(x,y,nextEnemyId++, *this);
                                else if (res2<=75)grid[i][j] = make_unique<Bully>(x,y,nextEnemyId++);
                                else if (res2<=100) grid[i][j]=make_unique<Nerd>(x,y,nextEnemyId++);
                                else if (res2<=125) grid[i][j]=make_unique<OldStudent>(x,y,nextEnemyId++);
                                else  grid[i][j]=make_unique<Healer>(x,y,nextEnemyId++);
                            }
                            break;
                        case 4:
                            if (res<=50)
                            {
                                if(Run::GenerateDecision(50, tag,id))grid[i][j] =  make_unique<Ghost>(x,y,nextEnemyId++);
                                else grid[i][j]=make_unique<PhoneGuy>(x,y,nextEnemyId++);
                            }
                            else
                            {
                                int res2 = Run::GenerateinRange(0,200,tag,id);
                                if (res2<=25) grid[i][j] = make_unique<Mouse>(x,y,nextEnemyId++);
                                else if (res2<=50)grid[i][j] = make_unique<Spider>(x,y,nextEnemyId++, *this);
                                else if (res2<=75)grid[i][j] = make_unique<Bully>(x,y,nextEnemyId++);
                                else if (res2<=100) grid[i][j]=make_unique<Nerd>(x,y,nextEnemyId++);
                                else if (res2<=125) grid[i][j]=make_unique<OldStudent>(x,y,nextEnemyId++);
                                else if (res2<=150) grid[i][j]=make_unique<Healer>(x,y,nextEnemyId++);
                                else if (res2<=175) grid[i][j]=make_unique<YoungStudent>(x,y,nextEnemyId++);
                                else  grid[i][j]=make_unique<Slingshoter>(x,y,nextEnemyId++);
                            }
                            break;
                        case 5:
                            if (res<=25)
                            {
                                grid[i][j] =  make_unique<Solider>(x,y,nextEnemyId++);
                            }
                            else
                            {
                                int res2 = Run::GenerateinRange(0,250,tag,id);
                                if (res2<=25) grid[i][j] = make_unique<Mouse>(x,y,nextEnemyId++);
                                else if (res2<=50)grid[i][j] = make_unique<Spider>(x,y,nextEnemyId++, *this);
                                else if (res2<=75)grid[i][j] = make_unique<Bully>(x,y,nextEnemyId++);
                                else if (res2<=100) grid[i][j]=make_unique<Nerd>(x,y,nextEnemyId++);
                                else if (res2<=125) grid[i][j]=make_unique<OldStudent>(x,y,nextEnemyId++);
                                else if (res2<=150) grid[i][j]=make_unique<Healer>(x,y,nextEnemyId++);
                                else if (res2<=175) grid[i][j]=make_unique<YoungStudent>(x,y,nextEnemyId++);
                                else if (res2<=200) grid[i][j]=make_unique<Slingshoter>(x,y,nextEnemyId++);
                                else if (res2<=250) grid[i][j]=make_unique<Ghost>(x,y,nextEnemyId++);
                                else  grid[i][j]=make_unique<PhoneGuy>(x,y,nextEnemyId++);
                            }
                            break;
                        }
                    }
                    else
                    {
                        int tp = Run::GenerateinRange(1, 3, tag, lvl);
                        float rot = (float)Run::GenerateinRange(0, 360, tag, lvl);
                        auto block = make_unique<Block>(x, y, (ObstacleType)tp);
                        block->SetRotation(rot);
                        grid[i][j] = move(block);
                    }
                    --amountofcells;
                    placed = true;
                }
            }
        }
    }
}

void Room::PlaceBoss(Tag tag, int lvl)
{
    enemiescount=1;
    Vector2 xy = GetCellPosition(GRID_W/2, GRID_H/2);
    switch (lvl)
    {
    case 0:
        grid[GRID_W/2][GRID_H/2] = make_unique<Mouse>(xy.x,xy.y,nextEnemyId++,true);
        break;
    case 1:
        grid[GRID_W/2][GRID_H/2] = make_unique<Security>(xy.x,xy.y,nextEnemyId++);
        break;
    case 2:
        grid[GRID_W/2][GRID_H/2] = make_unique<PhysicTeacher>(xy.x,xy.y,nextEnemyId++);
        break;
    case 3:
        grid[GRID_W/2][GRID_H/2] = make_unique<PETeacher>(xy.x,xy.y,nextEnemyId++);
        break;
    case 4:
        grid[GRID_W/2][GRID_H/2] = make_unique<DutyPair>(xy.x,xy.y,nextEnemyId++);
        break;
    case 5:
        grid[GRID_W/2][GRID_H/2] = make_unique<HELICOPTER>(xy.x,xy.y,nextEnemyId++);
        break;

    }

}

void Room::FillTreasures(Tag tag, int lvl)
{
    enemiescount = 0;

    int cx = GRID_W/2;
    int cy = GRID_H/2;

    Vector2 xy = GetCellPosition(cx, cy);

    Player* player = nullptr;
    auto &colliders = Collider::GetAllColliders();
    for (auto &c : colliders)
    {
        if (!c) continue;
        Asset* p = c->GetParent();
        if (!p) continue;
        if (p->GetTag()==Tag::Player)
        {
            player = dynamic_cast<Player*>(p);
            break;
        }
    }

    int tries = 80;
    int itemid = 0;

    while (tries-- > 0)
    {
        itemid = Run::GenerateinRange(0, 50, tag, lvl);

        if (!player) break;

        Inventory& inv = player->GetInventory();
        if (!inv.HasItemId(itemid)) break;
    }

    grid[cx][cy] = Item::CreateItem(itemid);
    if (!grid[cx][cy]) return;

    Item* it = dynamic_cast<Item*>(grid[cx][cy].get());
    if (!it) return;

    it->SetWorldSlot(&grid[cx][cy]);
    it->OnDrop(xy);
}

void Room::FillStart(Tag tag, int lvl)
{
    grid[GRID_W/2][GRID_H/2] = make_unique<Stairs>(GetCellPosition(GRID_W/2, GRID_H/2), lvl);
}

void Room::FillShop(Tag tag, int lvl)
{
    enemiescount = 0;

    Player* player = nullptr;
    auto &colliders = Collider::GetAllColliders();
    for (auto &c : colliders)
    {
        if (!c) continue;
        Asset* p = c->GetParent();
        if (!p) continue;
        if (p->GetTag()==Tag::Player)
        {
            player = dynamic_cast<Player*>(p);
            break;
        }
    }

    Inventory* inv = nullptr;
    if (player) inv = &player->GetInventory();

    int spots[4][2] =
    {
        { GRID_W/2 - 2, GRID_H/2 },
        { GRID_W/2 - 1, GRID_H/2 },
        { GRID_W/2 + 1, GRID_H/2 },
        { GRID_W/2 + 2, GRID_H/2 },
    };

    for (int k = 0; k < 4; k++)
    {
        int price = Run::GenerateinRange((2+lvl*1.5)*(6+lvl*3), (5+lvl*1.5)*(6+lvl*3), tag,  lvl);
        int i = spots[k][0];
        int j = spots[k][1];

        Vector2 xy = GetCellPosition(i, j);

        int itemid = 0;

        if (k==0)
        {
            int tries = 80;
            while (tries-- > 0)
            {
                itemid = Run::GenerateinRange(35, 41, tag, lvl);
                price= price /2;
                if (!inv) break;
                if (!inv->HasItemId(itemid)) break;
            }
        }
        else
        {
            int tries = 80;
            while (tries-- > 0)
            {
                itemid = Run::GenerateinRange(0, 50, tag, lvl);
                if (!inv) break;
                if (!inv->HasItemId(itemid)) break;
            }
        }

        grid[i][j] = Item::CreateItem(itemid);
        if (!grid[i][j]) continue;

        Item* it = dynamic_cast<Item*>(grid[i][j].get());
        if (!it) continue;

        it->SetWorldSlot(&grid[i][j]);

        it->SetShopPrice(price);

        it->OnDrop(xy);
    }
}


Vector2 Room::GetCellPosition(int i, int j)
{
    float x = (getX()-10)*1920 + i * CELL + gridOrigin.x;
    float y = (getY()-10)*1080 + j * CELL + gridOrigin.y;
    return {x,y};
}

Vector2 Room::GetCellCenter(int i, int j)
{
    Vector2 p = GetCellPosition(i,j);
    p.x += CELL/2.0f;
    p.y += CELL/2.0f;
    return p;
}

bool Room::IsCellFree(int i, int j)
{
    if (i < 0 || i >= GRID_W) return false;
    if (j < 0 || j >= GRID_H) return false;
    if (!grid[i][j]) return true;
    if (grid[i][j]->GetTag()==Tag::Solid) return false;
    return true;
}

void Room::Update()
{
    if (!activeroom) return;
    for (int i = 0; i < GRID_W; i++)
    {
        for (int j = 0; j < GRID_H; j++)
        {
            if (!grid[i][j]) continue;
            if (grid[i][j]->GetTag()==Tag::Enemy)
            {
                Entity* e = dynamic_cast<Entity*>(grid[i][j].get());
                if (e && e->IsDead())
                {
                    if (player) player->AddPoints(1);
                    grid[i][j].reset();
                    enemiescount--;
                    continue;
                }
                else grid[i][j]->Update();
            }
        }
    }
    if (type==boss && enemiescount==0 && !keycreated)
    {
        grid[GRID_W/2][GRID_H/2] = make_unique<Key>(GetCellPosition(GRID_W/2, GRID_H/2));
        keycreated=true;
    }
    for (auto& door : doors)
        door->Update();
}

