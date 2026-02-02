//
// Created by vanya on 2/1/26.
//

#ifndef ROGUELIKE_ONEUSEITEM_H
#define ROGUELIKE_ONEUSEITEM_H
#include "Item.h"


class OneUseItem : public Item
{
public:
    OneUseItem(int id) : Item(id) { itemtype = ItemType::OneUse; }
    void Use();
    void OnPickUpExtra() override{}
    void OnDropExtra() override {}
    static void KillAllEnemies(Player* player);
};


#endif //ROGUELIKE_ONEUSEITEM_H