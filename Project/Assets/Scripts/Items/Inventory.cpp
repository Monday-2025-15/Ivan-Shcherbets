//
// Created by vanya on 1/30/26.
//

#include "Inventory.h"
#include "Item.h"
Inventory::Inventory() { }
void Inventory::Add(Item* item)
{
    if (!item) return;

    std::unique_ptr<Asset>* slot = item->GetWorldSlot();
    if (!slot)
    {
        item->OnPickUp();
        return;
    }

    Vector2 dropPos = item->GetPosition();

    auto takeFromWorld = [&]() -> std::unique_ptr<Asset>
    {
        std::unique_ptr<Asset> taken = std::move(*slot);
        slot->reset();
        return taken;
    };

    auto placeToWorld = [&](std::unique_ptr<Asset>&& a)
    {
        if (!a) return;
        *slot = std::move(a);

        Item* dropped = dynamic_cast<Item*>(slot->get());
        if (dropped)
        {
            dropped->SetWorldSlot(slot);
            dropped->OnDrop(dropPos);
        }
    };


    if (item->GetItemType() == ItemType::Passive)
    {
        std::unique_ptr<Asset> taken = takeFromWorld();
        PassiveItems* p = dynamic_cast<PassiveItems*>(taken.release());
        if (!p) return;

        p->SetWorldSlot(nullptr);
        passiveItems.push_back(std::unique_ptr<PassiveItems>(p));
        p->OnPickUp();
        return;
    }


    if (item->GetItemType() == ItemType::OneUse)
    {
        std::unique_ptr<Asset> taken = takeFromWorld();
        OneUseItem* n = dynamic_cast<OneUseItem*>(taken.release());
        if (!n) return;

        if (oneUseItem) placeToWorld(std::move(oneUseItem));

        n->SetWorldSlot(nullptr);
        oneUseItem.reset(n);
        oneUseItem->OnPickUp();
        return;
    }


    if (item->GetItemType() == ItemType::Gadget)
    {
        std::unique_ptr<Asset> taken = takeFromWorld();
        GadgetItem* n = dynamic_cast<GadgetItem*>(taken.release());
        if (!n) return;

        if (gadgetItem) placeToWorld(std::move(gadgetItem));

        n->SetWorldSlot(nullptr);
        gadgetItem.reset(n);
        gadgetItem->OnPickUp();
        return;
    }


    if (item->GetItemType() == ItemType::Weapon)
    {
        std::unique_ptr<Asset> taken = takeFromWorld();
        WeaponItem* n = dynamic_cast<WeaponItem*>(taken.release());
        if (!n) return;

        if (weaponItem) placeToWorld(std::move(weaponItem));

        n->SetWorldSlot(nullptr);
        weaponItem.reset(n);
        weaponItem->OnPickUp();
        return;

    }
}
bool Inventory::HasItemId(int id)
{
    for (auto &p : passiveItems)
        if (p && p->GetId()==id) return true;

    if (oneUseItem && oneUseItem->GetId()==id) return true;
    if (gadgetItem && gadgetItem->GetId()==id) return true;
    if (weaponItem && weaponItem->GetId()==id) return true;

    return false;
}

void Inventory::Update()
{
    for (auto &p : passiveItems)
    {
        if (dynamic_cast<EyeItem*>(p.get())|| dynamic_cast<ThirdHandItem*>(p.get()))
            p->Update();
    }
}

void Inventory::Draw()
{
    for (auto &p : passiveItems)
    {
        if (dynamic_cast<EyeItem*>(p.get())|| dynamic_cast<ThirdHandItem*>(p.get()))
            p->Draw();
    }
}
bool Inventory::UseOneUse()
{
    if (!oneUseItem) return false;
    if (oneUseItem->GetId()==40) return false;
    oneUseItem->Use();
    oneUseItem.reset();
    return true;
}
bool Inventory::RemoveItemById(int id)
{
    for (int i = (int)passiveItems.size() - 1; i >= 0; i--)
    {
        if (passiveItems[i] && passiveItems[i]->GetId() == id)
        {
            passiveItems.erase(passiveItems.begin() + i);
            return true;
        }
    }

    if (oneUseItem && oneUseItem->GetId() == id)
    {
        oneUseItem.reset();
        return true;
    }

    if (gadgetItem && gadgetItem->GetId() == id)
    {
        gadgetItem.reset();
        return true;
    }

    if (weaponItem && weaponItem->GetId() == id)
    {
        weaponItem.reset();
        return true;
    }

    return false;
}

