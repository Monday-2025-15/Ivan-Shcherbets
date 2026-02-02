//
// Created by vanya on 1/30/26.
//

#ifndef ROGUELIKE_INVENTORY_H
#define ROGUELIKE_INVENTORY_H
#include <memory>
#include "Asset.h"



class Item;
#include "PassiveItems.h"
#include "OneUseItem.h"
#include "GadgetItem.h"
#include "WeaponItem.h"

class Inventory : public Asset
{
    vector<unique_ptr<PassiveItems>> passiveItems;
    unique_ptr<OneUseItem> oneUseItem;
    unique_ptr<GadgetItem> gadgetItem;
    unique_ptr<WeaponItem> weaponItem = make_unique<WeaponItem>(51);
public:
    Inventory();
    void Add(Item* item);
    vector<unique_ptr<PassiveItems>>& GetPassiveItems(){return passiveItems;}
    OneUseItem* GetOneUseItem(){return oneUseItem.get();}
    GadgetItem* GetGadgetItem(){return gadgetItem.get();}
    WeaponItem* GetWeaponItem(){return weaponItem.get();}
    bool HasItemId(int id);
    void Update() override;
    void Draw() override;
    bool UseOneUse();
    bool RemoveItemById(int id);
};


#endif //ROGUELIKE_INVENTORY_H