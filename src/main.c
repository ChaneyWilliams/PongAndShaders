#ifdef _WIN32
#include <windows.h>
#endif

#include <math.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "cpup/io.h"
#include "cpup/vec.h"
#include "cpup/math.h"
#include "cpup/types.h"
#include "cpup/model.h"
#include "cpup/shader.h"
#include "cpup/window.h"
#include "cpup/inputmanager.h"

#include "cpup/scene.h"

#include "ball.h"
#include "paddle.h"
#include "GridCell.h"

//Look at GridCell.h and Ball.h to find pretty much everything
//or to look at my hyper fixation for a weekend
//ALSO i had task manager open for a couple of runs and allow to proudly say my big numbers
//CPU utilization ~25%
//Memory ~70%
//
AppContext app;

int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (InitCanis() > 0)
        return 1;

    app.windowWidth = 600;
    app.windowHeight = 600;

    if (InitWindow(&app) > 0)
        return 1;

    Scene *scene = SceneInit();
    app.scene = scene;

    Image iconImage = IOLoadImage("assets/textures/canis_engine_icon.tga");
    Image containerImage = IOLoadImage("assets/textures/container.tga");
    Image circleImage = IOLoadImage("assets/textures/circle.tga");
    Image squareImage = IOLoadImage("assets/textures/square.tga");

    // build and compile our shader program
    u32 shaderProgram = GenerateShaderFromFiles("assets/shaders/logo.vs", "assets/shaders/logo.fs");
    printf("shaderID: %i\n", shaderProgram);

    float ve[] = {
        // positions            // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top left
    };
    unsigned int in[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    f32 *vertices = vec_init(32, sizeof(f32));
    vec_append(&vertices, ve, 32);

    u32 *indices = vec_init(6, sizeof(u32));
    vec_append(&indices, in, 6);

    Model model = BuildModel(&vertices, &indices, STATIC_DRAW);

    float cellSize = 32.0f;
    float border = 2.0f;
    int gx = 0;
    for (float x = -cellSize; x <= app.windowHeight + 2.0f * cellSize; x += cellSize + border)
    {
        int gy = 0;
        for (float y = -cellSize; y <= app.windowWidth + 2.0f * cellSize; y += cellSize + border)
        {
            Entity *cell = Spawn(&scene);
            cell->transform.position = InitVector3(x, y, -0.001f);

            Cell *c = calloc(1, sizeof(Cell));
            c->gx = gx; // integer index
            c->gy = gy; // integer index
            cell->data = c;

            cell->image = &squareImage;
            cell->model = &model;
            cell->shaderId = shaderProgram;

            cell->Start = CellStart;
            cell->Update = CellUpdate;
            cell->Draw = CellDraw;
            cell->OnDestroy = CellDestroy;

            gy++;
        }
        gx++;
    }

    Entity *ball = Spawn(&scene);
    ball->transform.position = InitVector3(app.windowWidth * 0.5f, app.windowHeight * 0.5f, 0.0f);
    ball->id = 0;
    ball->name = "Ball";
    ball->data = calloc(1, sizeof(Ball));
    ball->image = &circleImage;
    ball->model = &model;
    ball->shaderId = shaderProgram;
    ball->Start = BallStart;
    ball->Update = BallUpdate;
    ball->Draw = BallDraw;
    ball->OnDestroy = BallOnDestroy;

    Entity *leftPaddle = Spawn(&scene);
    leftPaddle->id = 1;
    leftPaddle->color = (Vector4){0.0f, 0.0f, 1.0f, 1.0f};
    leftPaddle->name = (char *)"LeftPaddle";
    leftPaddle->transform.position = InitVector3(16.0f, app.windowHeight * 0.5f, 0.0f);
    leftPaddle->data = calloc(1, sizeof(Paddle));
    leftPaddle->image = &squareImage;
    leftPaddle->model = &model;
    leftPaddle->shaderId = shaderProgram;
    leftPaddle->Start = PaddleStart;
    leftPaddle->Update = PaddleUpdateTheOriginal;
    leftPaddle->Draw = PaddleDraw;
    leftPaddle->OnDestroy = PaddleOnDestroy;

    Entity *rightPaddle = Spawn(&scene);
    rightPaddle->id = 2;
    rightPaddle->color = (Vector4){1.0f, 0.0f, 0.0f, 1.0f};
    rightPaddle->name = (char *)"RightPaddle";
    rightPaddle->transform.position = InitVector3(app.windowWidth - 16.0f, app.windowHeight * 0.5f, 0.0f);
    rightPaddle->data = calloc(1, sizeof(Paddle));
    rightPaddle->image = &squareImage;
    rightPaddle->model = &model;
    rightPaddle->shaderId = shaderProgram;
    rightPaddle->Start = PaddleStart;
    rightPaddle->Update = PaddleUpdateTheSequal;
    rightPaddle->Draw = PaddleDraw;
    rightPaddle->OnDestroy = PaddleOnDestroy;

    bool running = true;
    f32 time = 0.0f;
    while (running)
    {
        // imput
        InputManagerNewFrame(&app);
        // printf("FPS: %f Entity Count: %i\n", 1.0f/app.deltaTime, vec_count(&scene->entities));

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // render
        ClearWindow();

        if (app.time != 0.0f)
            app.deltaTime = (SDL_GetTicksNS() * 1e-9) - app.time;

        app.time = SDL_GetTicksNS() * 1e-9;

        SceneStart(&app, &scene);

        app.projection = Mat4Orthographic(0.0f, (float)app.windowWidth, 0.0f, (float)app.windowHeight, 0.001f, 100.0f);
        app.view = IdentityMatrix4();
        Mat4Translate(&app.view, InitVector3(0.0f, 0.0f, -0.5f));

        SceneUpdate(&app, &scene);

        SceneDraw(&app, &scene);
        SwapWindow(&app);
    }

    FreeModel(model);

    free(iconImage.data);
    free(containerImage.data);
    free(circleImage.data);
    free(squareImage.data);

    SceneFree(&scene);

    FreeWindow(&app);

    DeleteShader(shaderProgram);
    return 0;
}
