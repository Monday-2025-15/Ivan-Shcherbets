//
// Created by vanya on 2/1/26.
//

#include "OneUseItem.h"
#include "Player.h"
#include "Inventory.h"
#include "GadgetItem.h"
#include "Collider.h"
#include "Run.h"
#include <cmath>

void OneUseItem::KillAllEnemies(Player* player)
{
    if (!player) return;

    Vector2 pp = player->GetPosition();

    int prx = (int)floorf(pp.x / 1920.0f) + 10;
    int pry = (int)floorf(pp.y / 1080.0f) + 10;

    auto& colliders = Collider::GetAllColliders();
    for (auto& c : colliders)
    {
        if (!c) continue;

        Asset* parent = c->GetParent();
        if (!parent) continue;
        if (parent->GetTag()!=Tag::Enemy) continue;

        Entity* e = dynamic_cast<Entity*>(parent);
        if (!e) continue;

        Vector2 ep = e->GetPosition();

        int erx = (int)floorf(ep.x / 1920.0f) + 10;
        int ery = (int)floorf(ep.y / 1080.0f) + 10;

        if (erx!=prx || ery!=pry) continue;

        e->Damage(999999.0f);
    }
}


void OneUseItem::Use()
{
    if (!player) return;

    switch (id)
    {
    case 35:
        player->AddHealth(player->GetMaxHP() * 0.5f);
        break;

    case 36:
        player->AddHealth(player->GetMaxHP());
        break;

    case 37:
        {
            Stats pool[] = {
                Stats::Damage,
                Stats::Speed,
                Stats::AttackSpeed,
                Stats::CritChance,
                Stats::CritDamage,
                Stats::WeaponSize,
                Stats::Size,
                Stats::MaxHp
            };

            int n = (int)(sizeof(pool)/sizeof(pool[0]));

            for (int i = 0; i < 3 && n > 0; i++)
            {
                int pick = Run::GenerateinRange(0, n-1, tag, id + i*77);
                Stats st = pool[pick];
                pool[pick] = pool[n-1];
                n--;

                player->AddTempEffect(st, Oper::Mul, 2.0f, 10.0f);
            }
            break;
        }

    case 38:
        {
            GadgetItem* g = player->GetInventory().GetGadgetItem();
            if (g) g->ResetCooldown();
            break;
        }

    case 39:
        KillAllEnemies(player);
        break;

    case 40:
        CloseWindow();
        break;

    case 41:
        player->AddTempEffect(Stats::Damage, Oper::Mul, 2.0f, 15.0f);
        player->AddTempEffect(Stats::AttackSpeed, Oper::Mul, 1.5f, 15.0f);
        break;

    default:
        break;
    }
}

