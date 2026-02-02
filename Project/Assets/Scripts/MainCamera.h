//
// Created by vanya on 1/5/26.
//

#ifndef ROGUELIKE_MAINCAMERA_H
#define ROGUELIKE_MAINCAMERA_H
#include <Asset.h>
#include <Collider.h>

class MainCamera: public Asset
{
    private:
    Camera2D camera;
    Collider colliders[4];
    Vector2 currentPosition;
    public:
    MainCamera();
    Camera2D& GetCamera(){return camera;}
    ~MainCamera() {for (int i = 0; i < 4; i++) colliders[i].Unregister();}
    void Move(int dx, int dy);
};


#endif //ROGUELIKE_MAINCAMERA_H