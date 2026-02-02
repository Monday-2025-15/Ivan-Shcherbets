//
// Created by user on 10.11.2025.
//

#include "Player.h"
#include <filesystem>
#include <iostream>

#include "Block.h"

#include "Door.h"
#include "Floor.h"
#include "Inventory.h"
#include "WeaponItem.h"
#include "Item.h"
#include "Key.h"
#include "Stairs.h"
using namespace std;

Player::Player(const string &folderpath, Vector2 position) : hud(this){
    this->tag = Tag::Player;
    speed = 500;
    size = 0.25;
    hp = 20;
    this->position = position;
    namespace fs = filesystem;
    vector<fs::path> files;
    for (const auto &entry: fs::directory_iterator(folderpath)) {
        if (entry.is_regular_file()) {
            const string extension = entry.path().extension().string();
            if (extension == ".png") {
                files.push_back(entry.path());
            }
        }
    }
    sort(files.begin(), files.end());
    for (auto path: files) {
        Texture2D texture = LoadTexture(path.string().c_str());
        sprites.push_back(texture);
    }
    frame = 0;
    Rectangle rectangle = {position.x, position.y, sprites[8].width*size, sprites[8].height * size};
    collider.Init(rectangle, files, this);
    collider.Register();
    weapons[0] = make_unique<HandPistol>(*this);
    weapons[1] = make_unique<Laser>(*this);
    weapons[2] = make_unique<FireExtinguisher>(*this);
    mode = MODE::HandPistol;
    currentweapon = weapons[int(mode)].get();
    maxHP=hp;
}
Player::~Player(){
    for (auto &tex : sprites)
        UnloadTexture(tex);
    collider.Unregister();
}
void Player::Draw() {
    Texture2D &texture = sprites[frame];
    DrawTextureEx(texture, position, 0.0f, size, WHITE);
    currentweapon->Draw();
    if (touchedItem) touchedItem->DrawPopup();
    if (hud.IsReady()) hud.Draw();
    inventory.Draw();
    for (auto& fx : chem)
    {
        Color col = {255,140,0,(unsigned char)(fx.a*255)};
        DrawCircleV(fx.pos, fx.r, col);
    }
}
void Player::Move(float &dt, bool &moving) {
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        position.y -= dt * speed;
        direction = UP;
        moving = true;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        position.y += dt * speed;
        direction = DOWN;
        moving = true;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        position.x -= dt * speed;
        direction = LEFT;
        moving = true;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        position.x += dt * speed;
        direction = RIGHT;
        moving = true;
    }
}

void Player::Animate(float &dt, bool &moving) {
    if (!moving) {
        frame = 8;
    }
    else {
        animationTimer+=dt;
        if (animationTimer >= animationSpeed) {
            animationTimer = 0;
            if (frame%2 == 0)  frame=direction+1;
            else frame = direction;
        }
    }
    if (frame < 0 || frame >= (int)sprites.size())
        frame = 0;
}
void Player::CollisionCheck(Vector2 &oldpos, bool &moving){
    auto& colliders = Collider::GetAllColliders();
    touchedItem = nullptr;
    for (auto &othercollider: colliders) {
        if (othercollider == &collider)
            continue;
        Asset* parent = othercollider->GetParent();
        if (parent->GetTag()==Tag::Solid && collider.IsColliding(*othercollider)) {

                position = oldpos;
        }
        if (parent->GetTag()==Tag::Door && collider.IsColliding(*othercollider))
        {
            Door* door = static_cast<Door*>(parent);
            door->Use(&position, &camera);
        }
        if (parent->GetTag()==Tag::Enemy && collider.IsColliding(*othercollider))
        {
            Damage(dynamic_cast<Entity*>(othercollider->GetParent())->GetDamage());
        }
        if (parent->GetTag()==Tag::Item )
        {
            if (collider.IsColliding(*othercollider))
            {
                Item* it = dynamic_cast<Item*>(parent);
                if (it) touchedItem = it;
            }
        }
        if (parent->GetTag()==Tag::Key && collider.IsColliding(*othercollider))
        {
            if (IsKeyDown(KEY_E))
            {
                Key* k = dynamic_cast<Key*>(othercollider->GetParent());
                k->PickUp(keycount);
            }
        }
        if (parent->GetTag()==Tag::Stairs && collider.IsColliding(*othercollider))
        {
            if (IsKeyPressed(KEY_E))
            {
                auto st = dynamic_cast<Stairs*>(othercollider->GetParent());
                if (st->IsUnlocked()==false) st->TryUnlock(keycount);
                else st->Use();
            }
        }
    }
    if (IsKeyPressed(KEY_E))
    {
        if (touchedItem )
        {
            touchedItem->Get(inventory);
            touchedItem = nullptr;
        }
    }

}

void Player::Damage(float damage)
{
    if (invincible) return;
    if (shield)
    {
        shieldhp-=damage;
        if (shieldhp<0) shieldhp = 0;
    }
    else
    {
        float res = hp - damage;
        if (res<0 && activenote)
        {
            hp = maxHP;
            activenote = false;
            inventory.RemoveItemById(0);
            return;
        }
        hp = res;
    }
    invincible=true;
    invincibleTimer = 2.0f;
}

void Player::Update() {
    bool moving = false;
    float dt = GetFrameTime();

    UpdateRoomInfo();
    RecordHistory(dt);
    UpdateChem(dt);
    UpdateTempEffects(dt);

    if (inventory.GetGadgetItem())
    {
        inventory.GetGadgetItem()->UpdateCooldown(dt, clearedRooms, clock);

        if (IsKeyPressed(KEY_LEFT_CONTROL) || IsKeyPressed(KEY_RIGHT_CONTROL))
        {
            inventory.GetGadgetItem()->TryUse(this, currentRoomId, clearedRooms, clock);
        }
    }

    if (inventory.GetOneUseItem() && (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)))
    {
        inventory.UseOneUse();
    }

    Vector2 oldposition = position;

    if (shield && shieldhp < 20)
    {
        shieldhp += 2.0f * dt;
        if (shieldhp > 20) shieldhp = 20;
    }

    if (regen)
    {
        hp += 0.5f * dt;
        if (shieldhp > maxHP) shieldhp = maxHP;
    }

    Move(dt, moving);
    InvincibleTimer(dt);

    Rectangle rectangle{position.x, position.y, sprites[frame].width*size, sprites[frame].height * size};
    collider.SetRectangle(rectangle);
    collider.SetFrame(frame);
    collider.SetScale(size);

    CollisionCheck(oldposition, moving);

    Animate(dt, moving);
    currentweapon->Update();

    if (hud.IsReady()) hud.Update();
    if (hud.IsReady()) hud.Update();
    inventory.Update();
    //Debug
     if (IsKeyDown(KEY_ONE)) SetWeapon(MODE::HandPistol);
     if (IsKeyDown(KEY_TWO)) SetWeapon(MODE::Laser);
     if (IsKeyDown(KEY_THREE)) SetWeapon(MODE::FireExtinguisher);
     if (IsKeyDown(KEY_FOUR)) ApplyGhost(true);
     if (IsKeyDown(KEY_FIVE)) ApplyGhost(false);
     if (IsKeyDown(KEY_SIX)) ApplyRicochet(true);
     if (IsKeyDown(KEY_SEVEN)) ApplyRicochet(false);
     if (IsKeyDown(KEY_EIGHT)) SetExplosive(true);
     if (IsKeyDown(KEY_NINE)) SetExplosive(false);
    if (IsKeyDown(KEY_O)) OneUseItem::KillAllEnemies(this);
    if (IsKeyDown(KEY_M)) AddPoints(999);
}
void Player::InvincibleTimer(float &dt) {
    if (invincible) {
        invincibleTimer -= dt;
        if (invincibleTimer <= 0) {
            invincible = false;
        }
    }
}

void Player::ApplyGhost(bool apply)
{
    for (auto &weapon : weapons)
    {
        weapon->SetGhost(apply);
    }
}
void Player::OnFloorChange()
{
    touchedItem = nullptr;
    touchedkey = nullptr;
    touchedstairs = nullptr;
    roomsPtr = nullptr;

    currentRoomId = -1;
    lastRoomId = -1;
    clearedRooms = 0;
    activeRoomEnemies = 0;
}
void Player::ApplyRicochet(bool apply)
{
    for (auto &weapon : weapons)
    {
        weapon->SetRicochet(apply);
    }
}

void Player::SetWeapon(MODE m)
{
    mode = m;
    currentweapon = weapons[(int)mode].get();
}



void Player::SetHP(float value, Oper op)
{
    switch (op)
    {
    case Oper::Add:
        hp += value;
        break;
    case Oper::Mul:
        hp *= value;
        break;
    case Oper::Set:
        hp = value/100.0f*maxHP;
        break;
    }
}

void Player::SetMaxHP(float value, Oper op)
{
    switch (op)
    {
    case Oper::Add:
        maxHP += value;
        break;
    case Oper::Mul:
        maxHP *= value;
        break;
    case Oper::Set:
        maxHP = value;
        break;
    }
}

void Player::SetDamage(float value, Oper op)
{
    for (int i = 0; i<3;i++)
    {
        weapons[i]->ChangeDamage(value, op);
    }
}

void Player::SetAttackSpeed(float value, Oper op)
{
    for (int i = 0; i<3;i++)
    {
        weapons[i]->ChangeAttackSpeed(value, op);
    }
}

void Player::SetWeaponSize(float value, Oper op)
{
    for (int i = 0; i<3;i++)
    {
        weapons[i]->ChangeSize(value, op);
    }
}

void Player::SetCritChance(float value, Oper op)
{
    switch (op)
    {
    case Oper::Add:
        critChance += value;
        break;
    case Oper::Mul:
        critChance *= value;
        break;
    case Oper::Set:
        critChance = value;
        break;
    }
}

void Player::SetCritDamage(float value, Oper op)
{
    switch (op)
    {
    case Oper::Add:
        critDamage += value;
        break;
    case Oper::Mul:
        critDamage *= value;
        break;
    case Oper::Set:
        critDamage = value;
        break;
    }
}

void Player::SetSize(float value, Oper op)
{
    switch (op)
    {
    case Oper::Add:
        return;
        break;
    case Oper::Mul:
        size *= value;
        break;
    case Oper::Set:
        size = value;
        break;
    }
    if (size > 0.4) size = 0.4;
}

void Player::SetSpeed(float value, Oper op)
{
    switch (op)
    {
    case Oper::Add:
        speed+=value;
        break;
    case Oper::Mul:
        speed *= value;
        break;
    case Oper::Set:
        speed = value;
        break;
    }
}

void Player::SetRicochet(bool value)
{
    for (int i = 0; i<3;i++)
    {
        weapons[i]->SetRicochet(value);
    }
}
void Player::SetGhost(bool value)
{
    for (int i = 0; i<3;i++)
    {
        weapons[i]->SetGhost(value);
    }
}
void Player::SetExplosive(bool value)
{
    for (int i = 0; i<3;i++)
    {
        weapons[i]->SetExplosive(value);
    }
}
void Player::SetVampire(bool value)
{
    for (int i = 0; i<3;i++)
    {
        weapons[i]->SetVampire(value);
    }
}

void Player::Recalculate()
{
    speed = 500;
    size = 0.25;
    maxHP=20;
    critChance=0.2;
    critDamage=2;
    shield=false;
    regen=false;
    clock=false;
    activenote=false;
    for (int i = 0; i<3;i++)
    {
        weapons[i]->SetDefault();
    }
    auto& items = inventory.GetPassiveItems();
    int length = items.size();
    for (int i = 0; i<length;i++)
    {
        int efsize = items[i]->GetEffects().size();
        for (int j = 0; j<efsize; j++)
        {
            Effect& ef = items[i]->GetEffects()[j];
            if (ef.stat==Stats::Random)
            {
                int newstat = Run::GenerateinRange(2,9,tag,1);
                ef.stat = (Stats)newstat;
            }
            switch (ef.stat)
            {
                case Stats::Hp:
                SetHP(ef.value, ef.operation);
                break;
            case Stats::MaxHp:
                SetMaxHP(ef.value, ef.operation);
                break;
            case Stats::Damage:
                SetDamage(ef.value, ef.operation);
                break;
            case Stats::Size:
                SetSize(ef.value, ef.operation);
                break;
            case Stats::WeaponSize:
                SetWeaponSize(ef.value, ef.operation);
                break;
            case Stats::Speed:
                SetSpeed(ef.value, ef.operation);
                break;
            case Stats::CritChance:
                SetCritChance(ef.value, ef.operation);
                break;
            case Stats::CritDamage:
                SetCritDamage(ef.value, ef.operation);
                break;
            case Stats::AttackSpeed:
                SetAttackSpeed(ef.value, ef.operation);
                break;
            case Stats::Ghost:
                SetGhost(ef.value);
                break;
            case Stats::Ricochet:
                SetRicochet(ef.value);
                break;
            case Stats::Shield:
                SetShield(ef.value);
                break;
            case Stats::Explosive:
                SetExplosive(ef.value);
                break;
            case Stats::Vampire:
                SetVampire(ef.value);
                break;
            case Stats::Regen:
                SetRegen(ef.value);
                break;
            case Stats::Clock:
                SetClock(ef.value);
                break;
            case Stats::Note:
                SetNote(ef.value);
                break;
            }
        }
    }
    auto weaponitem = inventory.GetWeaponItem();
    if (weaponitem) SetWeapon(weaponitem->GetMode());
    if (roomGadgetId != -1 && roomGadgetRoom == currentRoomId)
    {
        const json* data = Item::GetJson(roomGadgetId);
        if (!data || !data->contains("effects")) return;

        for (auto& e : (*data)["effects"])
        {
            string stat = e["stat"];
            string opS = e["op"];
            float v = e["value"];

            Oper op = Oper::Add;
            if (opS=="Mul") op = Oper::Mul;
            if (opS=="Set") op = Oper::Set;

            if (stat=="Damage") SetDamage(v, op);
            else if (stat=="AttackSpeed") SetAttackSpeed(v, op);
            else if (stat=="CritChance") SetCritChance(v, op);
        }
    }
    for (auto &te : tempEffects)
    {
        switch (te.stat)
        {
        case Stats::Hp:
            SetHP(te.value, te.op);
            break;
        case Stats::MaxHp:
            SetMaxHP(te.value, te.op);
            break;
        case Stats::Damage:
            SetDamage(te.value, te.op);
            break;
        case Stats::Size:
            SetSize(te.value, te.op);
            break;
        case Stats::WeaponSize:
            SetWeaponSize(te.value, te.op);
            break;
        case Stats::Speed:
            SetSpeed(te.value, te.op);
            break;
        case Stats::CritChance:
            SetCritChance(te.value, te.op);
            break;
        case Stats::CritDamage:
            SetCritDamage(te.value, te.op);
            break;
        case Stats::AttackSpeed:
            SetAttackSpeed(te.value, te.op);
            break;
        default:
            break;
        }
    }
    if (hp > maxHP) hp = maxHP;

}

void Player::InitMap(vector<unique_ptr<Room>>& rooms)
{
    roomsPtr=&rooms;
    hud.InitMap(rooms, &GetCamera());
}
int Player::GetEnemiesInRoomById(int rid)
{
    if (!roomsPtr) return 0;
    for (auto& r : *roomsPtr)
    {
        if (!r) continue;
        if (r->getId() == rid) return r->GetEnemiesCount();
    }
    return 0;
}

void Player::UpdateRoomInfo()
{

    if (!roomsPtr) return;

    static double lastPrint = 0.0;
    double now = GetTime();


    int active = -1;
    int cleared = 0;
    int enemiesInActive = 0;

    for (auto& r : *roomsPtr)
    {
        if (!r) continue;

        if (r->IsActive())
        {
            active = r->getId();
            enemiesInActive = r->GetEnemiesCount();
        }

        if (r->IsDiscovered() && r->GetEnemiesCount() <= 0)
            cleared++;
    }

    currentRoomId = active;
    clearedRooms = cleared;
    activeRoomEnemies = enemiesInActive;

    if (currentRoomId != lastRoomId)
    {
        if (roomGadgetRoom != currentRoomId)
        {
            roomGadgetId = -1;
            roomGadgetRoom = -1;
            Recalculate();
        }
        lastRoomId = currentRoomId;
    }

    bool shouldFight = (currentRoomId != -1 && activeRoomEnemies > 0);

    if (shouldFight)
    {
        if (!fightActive || fightRoomId != currentRoomId)
        {
            fightActive = true;
            fightRoomId = currentRoomId;
            fightTime = 0.0f;
            history.clear();
        }
        return;
    }
    if (fightActive)
    {
        fightActive = false;
        fightRoomId = -1;
        fightTime = 0.0f;
        history.clear();
    }
}


void Player::GiveInvincible(float sec)
{
    invincible = true;
    if (invincibleTimer < sec) invincibleTimer = sec;
}
void Player::RecordHistory(float dt)
{
    if (!fightActive) return;

    fightTime += dt;

    history.push_back({ position, hp, fightTime });

    while (!history.empty() && fightTime - history.front().t > 3.2f)
        history.erase(history.begin());
}

void Player::Rewind3s()
{
    if (history.empty()) return;

    float target = fightTime - 3.0f;

    int best = 0;
    float bestDist = history[0].t - target;
    if (bestDist < 0) bestDist = -bestDist;

    int n = (int)history.size();
    for (int i = 1; i < n; i++)
    {
        float d = history[i].t - target;
        if (d < 0) d = -d;
        if (d < bestDist)
        {
            bestDist = d;
            best = i;
        }
    }

    position = history[best].pos;
    hp = history[best].hp;

    Rectangle rectangle = { position.x, position.y, sprites[frame].width*size, sprites[frame].height*size };
    collider.SetRectangle(rectangle);
    collider.SetFrame(frame);
    collider.SetScale(size);
}

void Player::Blink()
{
    Vector2 dir{0,0};
    if (IsKeyDown(KEY_W)) dir.y -= 1;
    if (IsKeyDown(KEY_S)) dir.y += 1;
    if (IsKeyDown(KEY_A)) dir.x -= 1;
    if (IsKeyDown(KEY_D)) dir.x += 1;

    if (dir.x==0 && dir.y==0) return;

    float l = sqrt(dir.x*dir.x + dir.y*dir.y);
    dir.x/=l; dir.y/=l;

    Vector2 start = position;
    Vector2 best = start;

    for (float d=10; d<=260; d+=10)
    {
        Vector2 p{start.x + dir.x*d, start.y + dir.y*d};
        Collider probe({p.x,p.y,100*size,100*size}, this);

        bool hit=false;
        for (auto& c:Collider::GetAllColliders())
            if (c && c->GetParent()->GetTag()==Tag::Solid && probe.IsColliding(*c))
            { hit=true; break; }

        if (hit) break;
        best=p;
    }

    position=best;
    Rectangle r={position.x, position.y, 100*size, 100*size};
    collider.SetRectangle(r);
}
void Player::ChemExplosion()
{
    Vector2 c = position;
    float r = 120 * size;

    chem.push_back({c, 10.0f, 1.0f});

    float dmg = currentweapon ? currentweapon->GetDamage() : damage;

    for (auto& col : Collider::GetAllColliders())
    {
        if (!col || col->GetParent()->GetTag()!=Tag::Enemy) continue;

        Vector2 p = col->GetRectangle().x < 0 ? c :
            Vector2{col->GetRectangle().x, col->GetRectangle().y};

        float dx=p.x-c.x, dy=p.y-c.y;
        if (dx*dx+dy*dy <= r*r)
            ((Entity*)col->GetParent())->Damage(dmg);
    }
}
void Player::UpdateChem(float dt)
{
    for (int i=chem.size()-1;i>=0;i--)
    {
        chem[i].r += 200*dt;
        chem[i].a -= 1.2f*dt;
        if (chem[i].a<=0) chem.erase(chem.begin()+i);
    }
}
void Player::SetRoomGadget(int gadgetId, int roomId)
{
    roomGadgetId = gadgetId;
    roomGadgetRoom = roomId;
    Recalculate();
}
void Player::UpdateTempEffects(float dt)
{
    bool changed = false;

    for (int i = (int)tempEffects.size()-1; i >= 0; i--)
    {
        tempEffects[i].timer -= dt;
        if (tempEffects[i].timer <= 0)
        {
            tempEffects.erase(tempEffects.begin()+i);
            changed = true;
        }
    }

    if (changed) Recalculate();
}

void Player::AddTempEffect(Stats stat, Oper op, float value, float time)
{
    TempEffect e;
    e.stat = stat;
    e.op = op;
    e.value = value;
    e.timer = time;
    tempEffects.push_back(e);
    Recalculate();
}
bool Player::TryRewind3s()
{
    if (!fightActive) return false;

    if (fightTime < 4.0f) return false;

    if (history.empty()) return false;

    Rewind3s();
    return true;
}


bool Player::TryBlink()
{
    Vector2 dir{0,0};
    if (IsKeyDown(KEY_W)) dir.y -= 1;
    if (IsKeyDown(KEY_S)) dir.y += 1;
    if (IsKeyDown(KEY_A)) dir.x -= 1;
    if (IsKeyDown(KEY_D)) dir.x += 1;

    if (dir.x==0 && dir.y==0) return false;

    Blink();
    return true;
}


