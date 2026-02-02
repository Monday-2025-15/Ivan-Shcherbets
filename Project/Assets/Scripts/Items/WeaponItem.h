//
// Created by vanya on 2/1/26.
//

#ifndef ROGUELIKE_WEAPONITEM_H
#define ROGUELIKE_WEAPONITEM_H
#include "Item.h"
#include "Weapon.h"


class WeaponItem : public Item
{
    MODE mode;
    public:
    WeaponItem(int id) : Item(id)
    {
        itemtype=ItemType::Weapon;
        mode = (MODE)abs(id-51);
    }
    MODE GetMode(){return mode;}
    void OnPickUpExtra() override;
    void OnDropExtra() override {}
};


#endif //ROGUELIKE_WEAPONITEM_H