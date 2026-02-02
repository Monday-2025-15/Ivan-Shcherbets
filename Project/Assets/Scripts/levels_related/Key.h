#ifndef ROGUELIKE_KEY_H
#define ROGUELIKE_KEY_H

#include "Asset.h"
#include "Collider.h"

class Key : public Asset
{
    Collider collider;
    bool pickedup = false;
public:
    Key(Vector2 position);
    ~Key();
    void Draw() override;
    void Update() override;
    void PickUp(int &keycount);
};

#endif //ROGUELIKE_KEY_H
