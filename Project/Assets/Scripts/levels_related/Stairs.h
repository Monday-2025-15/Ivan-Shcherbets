//
// Created by vanya on 2/2/26.
//

#ifndef ROGUELIKE_STAIRS_H
#define ROGUELIKE_STAIRS_H
#include "Asset.h"
#include "Collider.h"


class Stairs : public Asset
{
    Collider collider;
    bool used = false;
    bool unlocked = false;
    int lvl;
public:
    Stairs(Vector2 position, int lvl);
    ~Stairs();
    void Draw() override;
    void Update() override;
    void TryUnlock(int keycount) { if (keycount==(lvl+1)) unlocked = true; }
    void Use();
    bool IsUsed(){return used;}
    bool IsUnlocked(){return unlocked;}

};


#endif //ROGUELIKE_STAIRS_H