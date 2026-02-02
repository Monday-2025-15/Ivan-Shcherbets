//
// Created by vanya on 1/30/26.
//

#ifndef ROGUELIKE_PASSIVEITEMS_H
#define ROGUELIKE_PASSIVEITEMS_H
#include "Entity.h"
#include "Item.h"


class Weapon;

struct Effect
{
    Stats stat;
    Oper operation;
    float value;
};
class PassiveItems : public Item
{
private:
    vector<Effect> effects;
    public:
    PassiveItems(int id);
    vector<Effect>& GetEffects(){return effects;}
    void OnPickUpExtra() override;
    void OnDropExtra() override;
};
class EyeItem : public PassiveItems
{
private:
    std::unique_ptr<Weapon> weapon;
    float timer = 0;
public:
    EyeItem(int id);
    ~EyeItem();
    void Update() override;
    void Draw() override;
    void OnPickUpExtra() override;
    void OnDropExtra() override;
};

class ThirdHandItem : public PassiveItems
{
private:
    std::unique_ptr<Weapon> weapon;
    float timer = 0;
public:
    ThirdHandItem(int id);
    ~ThirdHandItem();
    void Update() override;
    void Draw() override;
    void OnPickUpExtra() override;
    void OnDropExtra() override;
};


#endif //ROGUELIKE_PASSIVEITEMS_H