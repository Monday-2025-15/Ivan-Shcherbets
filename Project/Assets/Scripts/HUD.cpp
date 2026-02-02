//
// Created by vanya on 1/28/26.
//

#include "HUD.h"

#include "Collider.h"
#include "Player.h"
#include "Inventory.h"
#include "Item.h"
#include "GadgetItem.h"
#include "OneUseItem.h"

#include <cmath>

HUD::HUD(Player* p)
{
    player = p;
    if (player) inventory = &player->GetInventory();
}

void HUD::TryFindPlayer()
{
    if (player) return;

    auto& colliders = Collider::GetAllColliders();
    for (auto& c : colliders)
    {
        if (!c) continue;

        Asset* parent = c->GetParent();
        if (!parent) continue;
        if (parent->GetTag()!=Tag::Player) continue;

        player = dynamic_cast<Player*>(parent);
        if (player)
        {
            inventory = &player->GetInventory();
            break;
        }
    }
}

static float Clamp01(float v)
{
    if (v < 0) return 0;
    if (v > 1) return 1;
    return v;
}

static void DrawSlot(float x, float y, float s, Texture2D* tex)
{
    Rectangle r = { x, y, s, s };
    DrawRectangleRec(r, (Color){160,160,160,220});
    DrawRectangleLinesEx(r, 2, (Color){0,0,0,255});

    if (!tex) return;

    Rectangle src = {0,0,(float)tex->width,(float)tex->height};
    Rectangle dst = { x + s/2.0f, y + s/2.0f, s, s };
    DrawTexturePro(*tex, src, dst, {s/2.0f, s/2.0f}, 0, WHITE);
}

static string CooldownText(GadgetItem* g, Player* p)
{
    if (!g) return "";

    int base = g->GetBaseCooldown();
    if (base == 0) return "";

    if (base > 0)
    {
        float t = g->GetTimeLeft();
        if (t <= 0.01f) return "";
        int sec = (int)ceilf(t);
        return to_string(sec) + "s";
    }

    if (p && p->HasClock())
    {
        float t = g->GetTimeLeft();
        if (t <= 0.01f) return "";
        int sec = (int)ceilf(t);
        return to_string(sec) + "s";
    }
    else
    {
        int r = 0;
        if (p) r = g->GetRoomLeft(p->GetClearedRooms());
        if (r <= 0) return "";
        return to_string(r) + "r";
    }
}

void HUD::Update()
{
    map.Update();
}

void HUD::Draw()
{
    map.Draw();

    TryFindPlayer();
    if (!player) return;
    if (!inventory) inventory = &player->GetInventory();

    float zoom = 1.0f;
    Vector2 target = {0,0};
    Vector2 offset = {0,0};

    float screenW = 1920.0f;
    float screenH = 1080.0f;

    if (camera)
    {
        zoom = camera->zoom;
        if (zoom < 0.001f) zoom = 1.0f;

        target = camera->target;
        offset = camera->offset;

        if (offset.x > 0.01f && offset.y > 0.01f)
        {
            screenW = offset.x * 2.0f;
            screenH = offset.y * 2.0f;
        }
    }

    float left = target.x - offset.x/zoom;
    float top  = target.y - offset.y/zoom;
    float right = left + screenW/zoom;

    float s = 1.0f/zoom;

    float pad = 18*s;

    float barW = 500*s;
    float barH = 50*s;

    float x = left + pad;
    float y = top + pad;

    float hp = player->GetHP();
    float maxhp = player->GetMaxHP();
    if (maxhp < 0.01f) maxhp = 1.0f;

    Rectangle back = { x, y, barW, barH };
    DrawRectangleRec(back, (Color){150,150,150,220});

    Rectangle fill = back;
    fill.width = barW * Clamp01(hp/maxhp);
    DrawRectangleRec(fill, (Color){0,200,0,230});

    DrawRectangleLinesEx(back, 2, (Color){0,0,0,255});

    int f1 = (int)(40/zoom);
    if (f1 < 12) f1 = 12;

    int f2 = (int)(40/zoom);
    if (f2 < 10) f2 = 10;

    string hpText = "HP: " + to_string((int)hp) + "/" + to_string((int)maxhp);
    DrawText(hpText.c_str(), (int)(x + 6*s), (int)(y + 1*s), f2, BLACK);

    y += barH + 10*s;

    if (player->HasShield())
    {
        float sh = player->GetShieldHP();
        float msh = player->GetShieldMax();
        if (msh < 0.01f) msh = 1.0f;

        Rectangle sb = { x, y, barW, 14*s };
        DrawRectangleRec(sb, (Color){150,150,150,220});

        Rectangle sf = sb;
        sf.width = barW * Clamp01(sh/msh);
        DrawRectangleRec(sf, (Color){0,120,255,230});

        DrawRectangleLinesEx(sb, 2, (Color){0,0,0,255});

        string shText = "SH: " + to_string((int)sh) + "/" + to_string((int)msh);
        DrawText(shText.c_str(), (int)(x + 6*s), (int)(y - 2*s), f2, BLACK);

        y += 14*s + 10*s;
    }

    string label = "Points: ";
    string num = to_string(player->GetPoints()) + "p.";

    DrawText(label.c_str(), (int)x, (int)y, f1, BLACK);

    int lw = MeasureText(label.c_str(), f1);
    DrawText(num.c_str(), (int)(x + lw), (int)y, f1, RED);


    float slot = 84*s;
    float gap = 10*s;

    float sx = right - pad - slot;
    float sy = top + pad;

    OneUseItem* one = inventory ? inventory->GetOneUseItem() : nullptr;
    GadgetItem* gad = inventory ? inventory->GetGadgetItem() : nullptr;

    DrawSlot(sx, sy, slot, one ? one->GetTexture() : nullptr);

    sy += slot + gap;

    DrawSlot(sx, sy, slot, gad ? gad->GetTexture() : nullptr);

    if (gad)
    {
        string cd = CooldownText(gad, player);
        if (cd.size() > 0)
        {
            int fcd = (int)(46/zoom);
            if (fcd < 12) fcd = 12;

            int tw = MeasureText(cd.c_str(), fcd);
            DrawText(cd.c_str(), (int)(sx + slot/2.0f - (tw/2.0f)*s), (int)(sy + slot - (fcd+6)*s)+50, fcd, BLACK);
        }
    }
}
