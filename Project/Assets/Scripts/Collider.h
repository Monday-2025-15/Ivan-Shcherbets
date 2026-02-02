

#ifndef ROGUELIKE_COLLIDER_H
#define ROGUELIKE_COLLIDER_H
#include <algorithm>
#include <raylib.h>
#include <vector>
#include "Asset.h"
#include <filesystem>
using namespace std;
struct Circle {
    Vector2 center;
    float radius;
};
enum class ColliderType {
    RECTANGLE,
    CIRCLE,
    MIXED
};

class Collider {
    private:
    Rectangle rectangle{};
    Circle circle{};
    Image image{};
    vector<Image> sprites{};
    ColliderType type;
    Asset* parent = nullptr;
    static inline vector<Collider*> registry;
    float scale = 1.0f;
public:
    Collider()=default;
    Collider(Rectangle rectangle, Asset* parent) : rectangle(rectangle),parent(parent) {type = ColliderType::RECTANGLE;}
    Collider(Circle circle, Asset* parent) : circle(circle), parent(parent) {type = ColliderType::CIRCLE;}
    void Init(Rectangle rectangle, std::vector<std::filesystem::path>& files, Asset* parent);
    bool IsColliding(const Collider& otherCollider) const;
    ColliderType GetType() const {return type;}
    Rectangle GetRectangle() const {return rectangle;}
    void SetRectangle(Rectangle &rectangle) {this->rectangle = rectangle;}
    Circle GetCircle() const {return circle;}
    void SetCircle(Circle &circle) {this->circle = circle;}
    Image GetImage() const {return image;}
    void SetFrame(int id);
    Asset* GetParent() const {return parent;}
    ~Collider();
    static std::vector<Collider*>& GetAllColliders() {
        return registry;
    }
    void Register() {
        GetAllColliders().push_back(this);
    }
    void Unregister() {
        registry.erase(remove(registry.begin(), registry.end(), this), registry.end());
    }
    void SetScale(float s) { scale = s; }
    float GetScale() const { return scale; }
    void DebugDraw(Color color = RED)
    {
        Color fill = { 0, 255, 0, 128 };

        switch (type)
        {
        case ColliderType::RECTANGLE:
            DrawRectangleRec(rectangle, fill);
            DrawRectangleLinesEx(rectangle, 2.0f, color);
            break;

        case ColliderType::CIRCLE:
            DrawCircleV(circle.center, circle.radius, fill);

            break;
        }
    }
    static void ClearRegistry();
};


#endif //ROGUELIKE_COLLIDER_H