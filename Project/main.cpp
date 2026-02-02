#include <iostream>
#include "raylib.h"
#include "Player.h"
#include "Block.h"

#include "Floor.h"
#include <Run.h>

#include "Enemies/Mouse.h"

int main()
{
    Run::GenerateRandomSeed();

    const int VIRTUAL_W = 1920;
    const int VIRTUAL_H = 1080;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Game");
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_W, VIRTUAL_H);

    Player player("Assets/Sprites/Player", { 800, 750 });

    int lvl=0;
    unique_ptr<Floor> f = make_unique<Floor>(10+(lvl*3), lvl);
    bool fullscreen = false;
    int winW = GetScreenWidth();
    int winH = GetScreenHeight();
    while (!WindowShouldClose())
    {
        Camera2D camera_2d;
        camera_2d.offset = {1920/2, 1080/2};
        camera_2d.rotation = 0;
        camera_2d.zoom = 0.7;
        camera_2d.target = {player.GetPosition().x, player.GetPosition().y};
        if (IsKeyPressed(KEY_F11))
        {
            if (!fullscreen)
            {
                winW = GetScreenWidth();
                winH = GetScreenHeight();
            }

            ToggleFullscreen();
            fullscreen = !fullscreen;

            if (!fullscreen)
                SetWindowSize(winW, winH);
        }

        player.Update();
        if (player.GetHP()<=0) return 0;;
        f->Update();
        if (f->IsComplete())
        {
            lvl++;
            Collider::ClearRegistry();
            player.OnFloorChange();
            f.reset();
            f = make_unique<Floor>(10+(lvl*3), lvl);
            player.SetPosition({800,700});
            continue;
        }
        BeginTextureMode(target);
        ClearBackground(RAYWHITE);

        BeginMode2D(player.GetCamera());

        f->Draw();
        player.Draw();
        EndMode2D();

        EndTextureMode();
        int sw = GetRenderWidth();
        int sh = GetRenderHeight();
        float scale = std::min((float)sw / VIRTUAL_W, (float)sh / VIRTUAL_H);
        int rw = (int)(VIRTUAL_W * scale);
        int rh = (int)(VIRTUAL_H * scale);
        Rectangle src = { 0, 0, (float)VIRTUAL_W, -(float)VIRTUAL_H };
        Rectangle dst = { (sw - rw) * 0.5f, (sh - rh) * 0.5f, (float)rw, (float)rh };
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }
    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}
