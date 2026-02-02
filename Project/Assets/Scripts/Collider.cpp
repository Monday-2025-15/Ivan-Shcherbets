//
// Created by user on 10.11.2025.
//
#include "Collider.h"
#include <raylib.h>
#include <filesystem>

#include "MainCamera.h"

bool CheckPixelPerfectCollision(const Image& imageA, const Rectangle& rectangleA, float scaleA,
                                const Image& imageB, const Rectangle& rectangleB, float scaleB)
{
    Rectangle overlap = GetCollisionRec(rectangleA, rectangleB);
    if (overlap.width <= 0 || overlap.height <= 0) return false;

    int w = (int)overlap.width;
    int h = (int)overlap.height;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            float worldX = overlap.x + (float)x;
            float worldY = overlap.y + (float)y;

            int ax = (int)((worldX - rectangleA.x) / scaleA);
            int ay = (int)((worldY - rectangleA.y) / scaleA);
            int bx = (int)((worldX - rectangleB.x) / scaleB);
            int by = (int)((worldY - rectangleB.y) / scaleB);

            if (ax < 0 || ay < 0 || ax >= imageA.width || ay >= imageA.height) continue;
            if (bx < 0 || by < 0 || bx >= imageB.width || by >= imageB.height) continue;

            Color colorA = GetImageColor(imageA, ax, ay);
            Color colorB = GetImageColor(imageB, bx, by);

            if (colorA.a > 0 && colorB.a > 0) return true;
        }
    }

    return false;
}

bool CheckPixelPerfectCollision(const Image& imageA, const Rectangle& rectangleA, float scaleA,
                                const Rectangle& rectangleB)
{
    Rectangle overlap = GetCollisionRec(rectangleA, rectangleB);
    if (overlap.width <= 0 || overlap.height <= 0) return false;

    int w = (int)overlap.width;
    int h = (int)overlap.height;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            float worldX = overlap.x + (float)x;
            float worldY = overlap.y + (float)y;

            int imgX = (int)((worldX - rectangleA.x) / scaleA);
            int imgY = (int)((worldY - rectangleA.y) / scaleA);

            if (imgX < 0 || imgX >= imageA.width || imgY < 0 || imgY >= imageA.height) continue;

            Color color = GetImageColor(imageA, imgX, imgY);
            if (color.a > 0) return true;
        }
    }

    return false;
}

bool CheckPixelPerfectCollision(const Image& image, const Rectangle& rectangle, float scale,
                                const Circle& circle)
{
    Rectangle bounds;
    bounds.x = circle.center.x - circle.radius;
    bounds.y = circle.center.y - circle.radius;
    bounds.width = circle.radius * 2.0f;
    bounds.height = circle.radius * 2.0f;

    Rectangle overlap = GetCollisionRec(rectangle, bounds);
    if (overlap.width <= 0 || overlap.height <= 0) return false;

    int w = (int)overlap.width;
    int h = (int)overlap.height;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            float worldX = overlap.x + (float)x;
            float worldY = overlap.y + (float)y;

            int imgX = (int)((worldX - rectangle.x) / scale);
            int imgY = (int)((worldY - rectangle.y) / scale);

            if (imgX < 0 || imgX >= image.width || imgY < 0 || imgY >= image.height) continue;

            Color color = GetImageColor(image, imgX, imgY);
            if (color.a == 0) continue;

            float dx = worldX - circle.center.x;
            float dy = worldY - circle.center.y;

            if (dx*dx + dy*dy <= circle.radius * circle.radius) return true;
        }
    }

    return false;
}


bool Collider::IsColliding(const Collider& otherCollider) const {
    switch (type) {
        case ColliderType::RECTANGLE: {
            Rectangle thisrect = this->GetRectangle();
            switch (otherCollider.type) {
                case ColliderType::RECTANGLE: {
                    return CheckCollisionRecs(thisrect, otherCollider.GetRectangle());
                }
                case ColliderType::CIRCLE: {
                    Circle circle = otherCollider.GetCircle();
                    return CheckCollisionCircleRec(circle.center, circle.radius, thisrect);
                }
                case ColliderType::MIXED: {
                    Rectangle otherrect = otherCollider.GetRectangle();
                    if (CheckCollisionRecs(thisrect, otherrect)) {
                        return CheckPixelPerfectCollision(otherCollider.GetImage(), otherrect, otherCollider.GetScale(), thisrect);
                    }
                    break;
                }
            }
            break;
        }
        case ColliderType::CIRCLE: {
            Circle circle = GetCircle();
            switch (otherCollider.type) {
                case ColliderType::RECTANGLE: {
                    return CheckCollisionCircleRec(circle.center, circle.radius, otherCollider.GetRectangle());
                }
                case ColliderType::CIRCLE: {
                    Circle othercircle = otherCollider.GetCircle();
                    return CheckCollisionCircles(circle.center, circle.radius, othercircle.center, othercircle.radius);
                }
                case ColliderType::MIXED: {
                    Rectangle otherrect = otherCollider.GetRectangle();
                    if (CheckCollisionCircleRec(circle.center, circle.radius, otherrect)) {
                        return CheckPixelPerfectCollision(otherCollider.GetImage(), otherrect, otherCollider.GetScale(), circle);
                    }
                    break;
                }
            }
            break;
        }
        case ColliderType::MIXED: {
            Rectangle thisrect = GetRectangle();
            switch (otherCollider.type) {
                case ColliderType::RECTANGLE: {
                    Rectangle otherrect = otherCollider.GetRectangle();
                    if (CheckCollisionRecs(thisrect, otherrect)) {
                        return CheckPixelPerfectCollision(GetImage(), thisrect, scale, otherrect);
                    }
                    break;
                }
                case ColliderType::CIRCLE: {
                    Circle circle = otherCollider.GetCircle();
                    if (CheckCollisionCircleRec(circle.center, circle.radius, thisrect))
                    {
                        return CheckPixelPerfectCollision(this->GetImage(), this->GetRectangle(), scale, circle);
                    }
                    break;
                }
                case ColliderType::MIXED: {
                    Rectangle otherrect = otherCollider.GetRectangle();
                    if (CheckCollisionRecs(thisrect, otherrect)) {
                        return CheckPixelPerfectCollision(GetImage(), thisrect, scale, otherCollider.GetImage(), otherrect, otherCollider.GetScale());
                    }
                    break;
                }
            }
            break;
        }
    }
    return false;
}
void Collider::SetFrame(int id) {
    if (id < 0 || id >= sprites.size()) return;
    if (image.data) UnloadImage(image);
    image = ImageCopy(sprites[id]);
}
Collider::~Collider() {
    if (image.data) UnloadImage(image);
    for (auto &img : sprites)
        if (img.data) UnloadImage(img);
}
void Collider::Init(Rectangle rectangle, std::vector<std::filesystem::path>& files, Asset* parent) {
    this->rectangle = rectangle;
    this->parent = parent;
    type = ColliderType::MIXED;
    for (auto path : files) {
        Image img = LoadImage(path.string().c_str());
        sprites.push_back(img);
    }
    if (!sprites.empty())
        image = ImageCopy(sprites[8]);
}

void Collider::ClearRegistry()
{
    auto copy = registry;

    for (Collider* c : copy)
    {
        if (!c) continue;

        auto* parent = c->GetParent();
        if (!parent) continue;

        if (parent->GetTag() == Tag::Player) continue;
        if (dynamic_cast<MainCamera*>(parent) != nullptr) continue;

        c->Unregister();
    }
}

