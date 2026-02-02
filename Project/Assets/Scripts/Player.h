//
// Created by user on 10.11.2025.
//

#ifndef ROGUELIKE_PLAYER_H
#define ROGUELIKE_PLAYER_H

#include <algorithm>

#include "Entity.h"
#include <raylib.h>
#include <string>
#include <vector>

#include "HUD.h"
#include "Inventory.h"

#include "MainCamera.h"
#include "Weapon.h"
class Stairs;
class Key;
class PassiveItems;

class Player : public Entity
{
    private:
    MainCamera camera;
    unique_ptr<Weapon> weapons[3];
    Weapon* currentweapon;
    bool invincible =false;
    float invincibleTimer = 0.0f;
    MODE mode = MODE::HandPistol;
    enum Direction {
        DOWN = 0,
        RIGHT = 2,
        LEFT = 4,
        UP = 6,
        IDLE = 8
    };
    Direction direction = IDLE;
    void Move(float &dt, bool &moving);
    void Animate(float &dt, bool &moving);
    void CollisionCheck(Vector2 &oldpos, bool &moving);
    void InvincibleTimer(float &dt);
    Inventory inventory;
    HUD hud;
    float critChance;
    float critDamage;
    Item* touchedItem = nullptr;
    Key* touchedkey =nullptr;
    Stairs* touchedstairs=nullptr;
    bool shield = false;
    float shieldhp = 20;
    bool activenote = false;
    bool clock = false;
    bool regen = false;
    int points=0;
    int currentRoomId = -1;
    int clearedRooms = 0;
    int lastRoomId = -1;
    int keycount = 0;
    int roomGadgetId = -1;
    int roomGadgetRoom = -1;
    struct HistState
    {
        Vector2 pos;
        float hp;
        float t;
    };
    vector<HistState> history;
    float histTimer = 0.0f;
    bool fightActive = false;
    float fightTime = 0.0f;
    int fightRoomId = -1;
    int activeRoomEnemies = 0;
    struct ChemFx
    {
        Vector2 pos;
        float r;
        float a;
    };
    vector<ChemFx> chem;
    struct TempEffect
    {
        Stats stat;
        Oper op;
        float value;
        float timer;
    };
    vector<TempEffect> tempEffects;
    vector<unique_ptr<Room>>* roomsPtr = nullptr;
    int GetEnemiesInRoomById(int rid);
    public:
    Player(const string &folderpath, Vector2 position);
    ~Player();
    void Draw() override;
    void Update() override;
    Camera2D& GetCamera(){return camera.GetCamera();}
    float GetCritDamage(){return critDamage;}
    float GetCritChance(){return critChance*100;}
    void Damage(float damage) override;
    void ApplyGhost(bool apply);
    void ApplyRicochet(bool apply);
    void SetWeapon(MODE m);
    void InitMap(vector<unique_ptr<Room>>& rooms);
    void SetHP(float value, Oper op);
    void SetMaxHP(float percent, Oper op);
    void SetDamage(float value, Oper op);
    void SetWeaponSize(float value, Oper op);
    void SetCritDamage(float value, Oper op);
    void SetCritChance(float value, Oper op);
    void SetAttackSpeed(float value, Oper op);
    void SetSize(float value, Oper op);
    void AddPoints(int amount){points+=amount;}
    void SetShield(bool value){shield=value;}
    void SetRicochet(bool value);
    void SetGhost(bool value);
    void SetExplosive(bool value);
    void SetVampire(bool value);
    void SetNote(bool value){activenote=value;}
    void SetClock(bool value){clock = value;}
    void SetRegen(bool value){regen = value;}
    void SetSpeed(float value, Oper op);
    void Recalculate();
    void UpdateRoomInfo();
    Inventory& GetInventory() { return inventory; }
    int GetPoints() const { return points; }
    void SetRoomGadget(int gadgetId, int roomId);
    void Rewind3s();
    void Blink();
    void GiveInvincible(float sec);
    void RecordHistory(float dt);
    void ChemExplosion();
    void UpdateChem(float dt);
    int GetCurrentRoomId(){ return currentRoomId; }
    int GetClearedRooms(){ return clearedRooms; }
    bool HasClock(){ return clock; }
    void UpdateTempEffects(float dt);
    void AddTempEffect(Stats stat, Oper op, float value, float time);
    bool HasShield(){ return shield; }
    float GetShieldHP(){ return shieldhp; }
    float GetShieldMax()
    {
        return 20;
    }
    bool TryRewind3s();
    bool TryBlink();
    void SetPosition(Vector2 pos){position=pos;}
    int GetKeyCount(){ return keycount; }
    void OnFloorChange();
};


#endif //ROGUELIKE_PLAYER_H