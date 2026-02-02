//
// Created by vanya on 2/1/26.
//

#include "WeaponItem.h"
#include "Player.h"

void WeaponItem::OnPickUpExtra()
{
    if (!player)
    {
        auto& colliders = Collider::GetAllColliders();
        for (auto& othercollider : colliders)
        {
            if (othercollider->GetParent()->GetTag()==Tag::Player)
            {
                player = dynamic_cast<Player*>(othercollider->GetParent());
                break;
            }
        }
    }
    player->SetWeapon(mode);
}

