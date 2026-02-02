//
// Created by user on 10.11.2025.
//

#ifndef ROGUELIKE_ASSET_H
#define ROGUELIKE_ASSET_H


#include <raylib.h>
#include <vector>
#include <string>

using namespace std;
enum class Tag {
    None,
    Player,
    Enemy,
    Door,
    Floor,
    Solid,
    Weapon,
    Item,
    Key,
    Stairs
};
class Asset {
public:
    bool IsActive() const {return active;}
    bool IsVisible() const {return visible;}
    Vector2 GetPosition() const {return position;}
    Tag GetTag() const {return tag;}
    virtual ~Asset() = default;
    virtual void Update() {}
    virtual void Draw() {}
    virtual void Initialize() {}
    static void LoadEnemySprites(const std::string& folder, std::vector<Texture2D>*& out);
protected:
    Tag tag=Tag::None;
    int frame=0;
    float animationSpeed = 0.15f;
    float animationTimer = 0.0f;
    std::vector<Texture2D> sprites;
    bool visible=true;
    bool active=true;
    Vector2 position={0,0};
};


#endif //ROGUELIKE_ASSET_H