//
// Created by vanya on 1/26/26.
//

#ifndef ROGUELIKE_ITEM_H
#define ROGUELIKE_ITEM_H
#include <memory>

#include "Asset.h"
#include "Collider.h"
#include "Room.h"
#include <nlohmann/json.hpp>

#include "Entity.h"

using json = nlohmann::json;
class Player;
class Inventory;
enum class ItemType
{
    Passive,
    Weapon,
    OneUse,
    Gadget
};
enum class Stats
{
    //float
    Random,
    Hp,
    MaxHp,
    Damage,
    Size,
    WeaponSize,
    Speed,
    CritChance,
    CritDamage,
    AttackSpeed,
    //bool
    Ghost,
    Ricochet,
    Shield,
    Explosive,
    Vampire,
    Regen,
    Clock,
    Note,
};
enum class Oper
{
    Add,
    Mul,
    Set
};
class Item : public Entity
{
    protected:
    int id;
    Texture2D* texture = nullptr;
    string name;
    string description;
    ItemType itemtype;
    Player* player = nullptr;
    unique_ptr<Asset>* worldSlot = nullptr;
    bool forSale = false;
    int price = 0;
public:
    Item(int id);
    void OnPickUp();
    virtual void OnPickUpExtra()=0;
    void OnDrop(Vector2 position);
    virtual void OnDropExtra()=0;
    void Get(Inventory& inv);
    static const json* GetJson(int id);
    void Draw() override;
    void DrawPopup();
    static unique_ptr<Item> CreateItem(int id);
    ItemType GetItemType() const { return itemtype; }
    void SetWorldSlot(unique_ptr<Asset>* slot) { worldSlot = slot; }
    unique_ptr<Asset>* GetWorldSlot() const { return worldSlot; }
    void SetShopPrice(int p){ forSale = true; price = p; }
    bool IsForSale(){ return forSale; }
    int GetPrice(){ return price; }
    int GetId(){ return id; }
    ItemType GetItemType(){ return itemtype; }
    Texture2D* GetTexture(){ return texture; }

};

#endif //ROGUELIKE_ITEM_H