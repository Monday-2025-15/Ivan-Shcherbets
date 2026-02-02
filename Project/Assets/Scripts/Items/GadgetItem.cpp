//
// Created by vanya on 2/1/26.
//

#include "GadgetItem.h"
#include "GadgetItem.h"
#include "Player.h"

GadgetItem::GadgetItem(int id) : Item(id)
{
    itemtype = ItemType::Gadget;

    auto data = GetJson(id);
    if (!data) return;

    baseCooldown = data->value("cooldown", 0);
}

void GadgetItem::UpdateCooldown(float dt, int clearedRooms, bool clock)
{
    if (baseCooldown > 0)
    {
        if (cdTimer > 0)
        {
            cdTimer -= dt;
            if (cdTimer < 0) cdTimer = 0;
        }
    }
}

int GadgetItem::GetRoomLeft(int clearedRooms) const
{
    if (baseCooldown >= 0) return 0;
    return std::max(0, readyAtCleared - clearedRooms);
}

bool GadgetItem::TryUse(Player* p, int roomId, int clearedRooms, bool clock)
{
    if (!p) return false;
    if (baseCooldown > 0)
    {
        if (cdTimer > 0.0f) return false;
    }
    else if (baseCooldown < 0)
    {
        if (clock)
        {
            if (cdTimer > 0.0f) return false;
        }
        else
        {
            if (readyAtCleared > 0 && clearedRooms < readyAtCleared)
                return false;
        }
    }

    bool used = false;

    switch (id)
    {
    case 42:
    case 43:
    case 46:
        p->SetRoomGadget(id, roomId);
        used = true;
        break;

    case 44:
        used = p->TryRewind3s();
        break;

    case 45:
        used = p->TryBlink();
        break;

    case 47:
        p->GiveInvincible(10.0f);
        used = true;
        break;

    case 48:
        p->ChemExplosion();
        used = true;
        break;

    default:
        return false;
    }

    if (!used) return false;

    if (baseCooldown > 0)
    {
        cdTimer = (float)baseCooldown;
    }
    else if (baseCooldown < 0)
    {
        if (clock) cdTimer = 90.0f;
        else readyAtCleared = clearedRooms + (-baseCooldown);
    }

    return true;
}


void GadgetItem::ResetCooldown()
{
    cdTimer = 0.0f;
    readyAtCleared = 0;
}

