//
// Created by vanya on 1/30/26.
//

#include "PassiveItems.h"

#include "Weapon.h"
#include "Player.h"
#include "Collider.h"
#include <cmath>


static Stats ParseStat(const string& s)
{
    if (s=="Hp") return Stats::Hp;
    if (s=="MaxHp") return Stats::MaxHp;
    if (s=="Damage") return Stats::Damage;
    if (s=="Size") return Stats::Size;
    if (s=="WeaponSize") return Stats::WeaponSize;
    if (s=="Speed") return Stats::Speed;
    if (s=="CritChance") return Stats::CritChance;
    if (s=="CritDamage") return Stats::CritDamage;
    if (s=="AttackSpeed") return Stats::AttackSpeed;

    if (s=="Ghost") return Stats::Ghost;
    if (s=="Ricochet") return Stats::Ricochet;
    if (s=="Shield") return Stats::Shield;
    if (s=="Explosive") return Stats::Explosive;
    if (s=="Vampire") return Stats::Vampire;
    if (s=="Regen") return Stats::Regen;
    if (s=="Clock") return Stats::Clock;
    if (s=="Note") return Stats::Note;

    return Stats::Random;
}

static Oper ParseOper(const string& s)
{
    if (s=="Add") return Oper::Add;
    if (s=="Mul") return Oper::Mul;
    if (s=="Set") return Oper::Set;
    return Oper::Add;
}
PassiveItems::PassiveItems(int id) : Item(id)
{
    this->id = id;
    itemtype = ItemType::Passive;

    effects.clear();

    auto data = GetJson(id);
    if (!data) return;
    if (!data->contains("effects")) return;
    if (!(*data)["effects"].is_array()) return;

    for (auto &e : (*data)["effects"])
    {
        if (!e.is_object()) continue;
        string st = e.value("stat","");
        string op = e.value("op","");
        float val = e.value("value", 0.0f);
        Effect ef;
        ef.stat = ParseStat(st);
        ef.operation = ParseOper(op);
        ef.value = val;
        effects.push_back(ef);
    }
}

void PassiveItems::OnPickUpExtra()
{
    player->Recalculate();
}
void PassiveItems::OnDropExtra()
{
    player->Recalculate();
}
static Player* FindPlayerPtr()
{
    auto& colliders = Collider::GetAllColliders();
    for (auto& c : colliders)
    {
        if (!c) continue;
        Asset* p = c->GetParent();
        if (!p) continue;
        if (p->GetTag()!=Tag::Player) continue;
        return dynamic_cast<Player*>(p);
    }
    return nullptr;
}

static Rectangle GetPlayerRect(Player* p)
{
    if (!p) return {0,0,100,100};

    auto& colliders = Collider::GetAllColliders();
    for (auto& c : colliders)
    {
        if (!c) continue;
        if (c->GetParent()!=p) continue;

        if (c->GetType()==ColliderType::CIRCLE)
        {
            Circle cc = c->GetCircle();
            return { cc.center.x-cc.radius, cc.center.y-cc.radius, cc.radius*2, cc.radius*2 };
        }

        return c->GetRectangle();
    }

    Vector2 pos = p->GetPosition();
    return {pos.x, pos.y, 100, 100};
}

static void DrawIcon(Texture2D* tex, Vector2 center, float sz)
{
    Rectangle rect = { center.x - sz/2.0f, center.y - sz/2.0f, sz, sz };

    if (!tex)
    {
        DrawRectangleRec(rect, GRAY);
        return;
    }

    Rectangle src = {0,0,(float)tex->width,(float)tex->height};
    Rectangle dest = { rect.x + rect.width/2.0f, rect.y + rect.height/2.0f, rect.width, rect.height };
    Vector2 origin = { rect.width/2.0f, rect.height/2.0f };

    DrawTexturePro(*tex, src, dest, origin, 0, WHITE);
}

// ---------------- item 33: Eye ----------------

EyeItem::EyeItem(int id) : PassiveItems(id) { }
EyeItem::~EyeItem() {active=false; }

void EyeItem::OnPickUpExtra()
{
    PassiveItems::OnPickUpExtra();

    active = true;
    visible = true;

    if (!player) player = FindPlayerPtr();
    if (player && !weapon)
        weapon = std::make_unique<Laser>(*this);
}

void EyeItem::OnDropExtra()
{
    PassiveItems::OnDropExtra();

    active = false;
    weapon.reset();
}

void EyeItem::Update()
{
    if (!active) return;

    float dt = GetFrameTime();
    timer += dt;

    if (!player) player = FindPlayerPtr();
    if (!player) return;

    if (weapon) weapon->Update();

    Rectangle r = GetPlayerRect(player);
    Vector2 head = { r.x + r.width/2.0f, r.y + r.height*0.20f };

    float bob = sinf(timer * 3.5f) * 6.0f;
    position = { head.x - r.width*0.55f, head.y + bob };
}

void EyeItem::Draw()
{
    if (!active)
    {
        Item::Draw();
        return;
    }

    if (!player) player = FindPlayerPtr();

    Rectangle r = GetPlayerRect(player);
    float sz = r.width * 0.45f;
    if (sz < 28) sz = 28;
    if (sz > 70) sz = 70;

    DrawIcon(texture, position, sz);

    if (weapon) weapon->Draw();
}

// ---------------- item 34: Third hand ----------------

ThirdHandItem::ThirdHandItem(int id) : PassiveItems(id) { }
ThirdHandItem::~ThirdHandItem() {active=false; }

void ThirdHandItem::OnPickUpExtra()
{
    PassiveItems::OnPickUpExtra();

    active = true;
    visible = true;

    if (!player) player = FindPlayerPtr();
    if (player && !weapon)
        weapon = make_unique<HandPistol>(*this);
}

void ThirdHandItem::OnDropExtra()
{
    PassiveItems::OnDropExtra();

    active = false;
    weapon.reset();
}

void ThirdHandItem::Update()
{
    if (!active) return;

    float dt = GetFrameTime();
    timer += dt;

    if (!player) player = FindPlayerPtr();
    if (!player) return;

    if (weapon) weapon->Update();

    Rectangle r = GetPlayerRect(player);
    Vector2 head = { r.x + r.width/2.0f, r.y + r.height*0.20f };

    float bob = sinf(timer * 3.5f + 1.2f) * 6.0f;
    position = { head.x + r.width*0.55f, head.y + bob };
}

void ThirdHandItem::Draw()
{
    if (!active)
    {
        Item::Draw();
        return;
    }

    if (!player) player = FindPlayerPtr();

    Rectangle r = GetPlayerRect(player);
    float sz = r.width * 0.45f;
    if (sz < 28) sz = 28;
    if (sz > 70) sz = 70;

    DrawIcon(texture, position, sz);

    if (weapon) weapon->Draw();
}
