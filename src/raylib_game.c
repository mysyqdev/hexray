/*******************************************************************************************
*
*   raylib gamejam template
*
*   Code licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2022-2026 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>      // Emscripten library
#endif

#include <stdio.h>                          // Required for: printf()
#include <stdlib.h>                         // Required for: 
#include <string.h>                         // Required for:
#include <math.h>

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Simple log system to avoid printf() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { 
    SCREEN_LOGO = 0, 
    SCREEN_TITLE, 
    SCREEN_GAMEPLAY, 
    SCREEN_ENDING
} GameScreen;

// TODO: Define your custom data types here

typedef struct Button {
    Rectangle bounds;
    bool isPressed;
} Button;

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 720;
static const int screenHeight = 720;

static RenderTexture2D target = { 0 };  // Render texture to render our game
static int frameCounter = 0;

// TODO: Define global variables here, recommended to make them static
#define LIGHTGREEN  (Color){ 155, 188, 15, 255 }
#define REGULARGREEN (Color){ 139, 172, 15, 255 } 
#define MELONGREEN (Color){ 48, 98, 48, 255 }
#define BLACKGREEN (Color){ 15, 56, 15, 255 }

static int maxMelons = 100; 

static int mValue = 1;
static int clickerValue = 1;
static int multiplierCost = 15;
static int autoClickerCost = 35;
static int clickerMultiplierCost = 55;

bool isAutoClickerActive = false;
static float timer = 0.0f;

// Buttons
static Button melonButton = { 0 };
static Button melonMultiplierButton = { 0 };
static Button autoClickerButton = { 0 };
static Button clickerMultiplierButton = { 0 };
static Button upgradeBank = { 0 };

static int melonsCounter = 0;
static Vector2 mousePoint = { 0.0f, 0.0f };

static Texture2D melonTexture = { 0 };

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
#if !defined(_DEBUG)
    SetTraceLogLevel(LOG_NONE);         // Disable raylib trace log messages
#endif

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "CATOMELON");

    InitAudioDevice();
    
    // TODO: Load resources / Initialize variables at this point
    
    // Render texture to draw, enables screen scaling
    // NOTE: If screen is scaled, mouse input should be scaled proportionally
    target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    melonTexture = LoadTexture("resources/watermelon.png");

    melonButton.bounds = (Rectangle){ 440, 450, 220, 210 };
    melonMultiplierButton.bounds = (Rectangle){ 60, 170, 270, 70 };
    autoClickerButton.bounds = (Rectangle){ 60, 280, 270, 70 };
    clickerMultiplierButton.bounds = (Rectangle){ 60, 280, 270, 70 };

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);     // Set our game frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);
    
    // TODO: Unload all loaded resources at this point

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//--------------------------------------------------------------------------------------------
// Module Functions Definition
//--------------------------------------------------------------------------------------------
// Update and draw frame
void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update variables / Implement example logic at this point
    mousePoint = GetMousePosition();
    melonButton.isPressed = false;
    melonMultiplierButton.isPressed = false;
    autoClickerButton.isPressed = false;
    clickerMultiplierButton.isPressed = false;
    frameCounter++;
    timer += GetFrameTime();

    if (CheckCollisionPointRec(mousePoint, melonButton.bounds))
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) melonButton.isPressed = true;
    }
    else if (CheckCollisionPointRec(mousePoint, melonMultiplierButton.bounds))
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) melonMultiplierButton.isPressed = true;
    }
    else if (CheckCollisionPointRec(mousePoint, autoClickerButton.bounds) && isAutoClickerActive == false)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) autoClickerButton.isPressed = true;
    }
    else if (CheckCollisionPointRec(mousePoint, clickerMultiplierButton.bounds))
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) clickerMultiplierButton.isPressed = true;
    }

    if  (melonButton.isPressed) melonsCounter = (int)fmin(melonsCounter + mValue, maxMelons);

    if  (melonMultiplierButton.isPressed && melonsCounter >= multiplierCost) {
        melonsCounter -= multiplierCost;
        mValue *= 2;
        multiplierCost *= 5;
    } 
    if (autoClickerButton.isPressed && melonsCounter >= autoClickerCost) {
        melonsCounter -= autoClickerCost;
        autoClickerCost *= 5;
        isAutoClickerActive = true;
    }
    if (clickerMultiplierButton.isPressed && melonsCounter >= clickerMultiplierCost) {
        melonsCounter -= clickerMultiplierCost;
        clickerValue *= 2;
        clickerMultiplierCost *= 5;
    }

    if (timer >= 1.0f && isAutoClickerActive) {
        melonsCounter = (int)fmin(melonsCounter + clickerValue, maxMelons);
        timer -= 1.0f;
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    // Render game screen to a texture, 
    // it could be useful for scaling or further shader postprocessing
    BeginTextureMode(target);
        ClearBackground(BLACKGREEN);
        
        // TODO: Draw your game screen here
        DrawRectangleLinesEx((Rectangle){ 60, 60, 270, 70 }, 10, MELONGREEN);
        DrawText(TextFormat("%d", melonsCounter), 85, 80, 30, LIGHTGREEN);

        DrawRectangleLinesEx(melonMultiplierButton.bounds, 10, MELONGREEN);
        DrawText(TextFormat("MELON x2 - %d", multiplierCost), melonMultiplierButton.bounds.x + 25, melonMultiplierButton.bounds.y + 20, 30, LIGHTGREEN);

        if (isAutoClickerActive) {
            DrawRectangleLinesEx(clickerMultiplierButton.bounds, 10, MELONGREEN);
            DrawText(TextFormat("AUTO x2 - %d", clickerMultiplierCost), clickerMultiplierButton.bounds.x + 25, clickerMultiplierButton.bounds.y + 20, 30, LIGHTGREEN);
        } else {
            DrawRectangleLinesEx(autoClickerButton.bounds, 10, MELONGREEN);
            DrawText(TextFormat("AUTO - %d", autoClickerCost), autoClickerButton.bounds.x + 25, autoClickerButton.bounds.y + 20, 30, LIGHTGREEN);
        }
        // DrawRectangleLinesEx(autoClickerButton.bounds, 10, MELONGREEN);
        // DrawText(TextFormat("AUTO - %d", autoClickerCost), autoClickerButton.bounds.x + 25, autoClickerButton.bounds.y + 20, 30, LIGHTGREEN);

        DrawTextureRec(melonTexture, (Rectangle){ 0, 0, melonTexture.width, melonTexture.height }, (Vector2){ melonButton.bounds.x, melonButton.bounds.y }, WHITE);
        
    EndTextureMode();
    
    // Render to screen (main framebuffer)
    BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw render texture to screen, scaled if required
        DrawTexturePro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height }, 
            (Rectangle){ 0, 0, (float)target.texture.width, (float)target.texture.height }, (Vector2){ 0, 0 }, 0.0f, WHITE);

        // TODO: Draw everything that requires to be drawn at this point, maybe UI?

    EndDrawing();
    //----------------------------------------------------------------------------------  
}
