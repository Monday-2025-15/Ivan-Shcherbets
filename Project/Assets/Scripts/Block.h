//
// Created by user on 14.11.2025.
//

#ifndef ROGUELIKE_BLOCK_H
#define ROGUELIKE_BLOCK_H
#include "Asset.h"
#include "Collider.h"
#include <raylib.h>


enum class ObstacleType
{
    None,
    Chair,
    Desk,
    Shelf
};

class Block : public Asset {
private:
    Collider collider;
    Rectangle rectangle;
    Texture2D* texture = nullptr;
    float rotation = 0.0f;

public:
    Block(float x, float y, ObstacleType type);
    ~Block();

    void Draw() override;

    void SetRotation(float angle);
    void Rotate(float deltaAngle);

    void SetPosition(float x, float y);
    const Rectangle& GetRect() const;
    float GetRotation() const;
    Texture2D* GetTexture() const;
    void SetTexture(Texture2D* tex);
};


#endif //ROGUELIKE_BLOCK_H