#ifndef ROGUELIKE_GADGETITEM_H
#define ROGUELIKE_GADGETITEM_H

#include "Item.h"

class Player;

class GadgetItem : public Item
{
    int baseCooldown = 0;
    int readyAtCleared = 0;
    float cdTimer = 0.0f;

public:
    GadgetItem(int id);

    void OnPickUpExtra() override {}
    void OnDropExtra() override {}

    void UpdateCooldown(float dt, int clearedRooms, bool clock);
    bool TryUse(Player* player, int roomId, int clearedRooms, bool clock);

    int GetBaseCooldown() const { return baseCooldown; }
    float GetTimeLeft() const { return cdTimer; }
    int GetRoomLeft(int clearedRooms) const;
    void ResetCooldown();
};

#endif
