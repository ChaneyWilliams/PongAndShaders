#pragma once
#include <math.h>
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"
#include "cpup/window.h"
#include <SDL3/SDL.h>

enum CollisionSide
{
    NONE,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};
enum ScoreBoard
{
    BASIC,
    START,
    REDSCORE,
    BLUESCORE,
    COUNT,
    FIREWORK,
    DISCO,
    TEST
};

typedef struct
{
    int leftScore;
    int rightScore;
    int collisionCount;
    int pulseFinished;
    Vector4 color;
    enum ScoreBoard scoreBoard;
    int gameOver;
    float animTime;
    float pulseTime;
    float inversePulseTime;
    Vector3 fireworkPos[5];
    float fireworkTime[5];
    float fireworkRadius[5];
    float fireworkPulse[5];
    float fireworkLife[5];
    float fireworkSpawnTimer;
    Vector3 trails[10];
    int index;
} Ball;

Entity *SpawnBall(AppContext *_app, Entity *_entity);
Entity *Find(Scene **_scene, const char *_name);
Vector4 PositionColor(Vector3 position);
Entity *Find(Scene **_scene, const char *_name);
enum CollisionSide Collision(Entity *_ball, Entity *_paddle);
void BallRoomBlitz(AppContext *_app, Entity *_entity, Vector4 color);

void BallStart(AppContext *_app, Entity *_entity)
{
    _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);

    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);
    Ball *ball = (Ball *)_entity->data;
    ball->collisionCount = 0; // its this: sqrtf(_app->windowWidth * _app->windowWidth + _app->windowHeight * _app->windowHeight
    ball->pulseTime = 0.0f;
    ball->inversePulseTime = 848.5f;
    ball->scoreBoard = START;
    ball->gameOver = 0;
    for (int i = 0; i < 10; i++)
        ball->trails[i] = _entity->transform.position;
    ball->index = 0;
}

void BallUpdate(AppContext *_app, Entity *_entity)
{
    Ball *ball = (Ball *)_entity->data;
    Vector3 lastPos = ball->trails[(ball->index - 1 + 10) % 10];

    if (Vec2Distance(InitVector2(lastPos.x, lastPos.y), InitVector2(_entity->transform.position.x, _entity->transform.position.y)) > 10.0f)
    {
        ball->trails[ball->index] = _entity->transform.position;
        ball->index = (ball->index + 1) % 10;
    }

    char title[32];
    sprintf(title, "Blue: %i | Red %i", ball->leftScore, ball->rightScore);
    SetWindowTitle(_app, title);

    if (_entity->transform.position.x <= 0.0f || _entity->transform.position.x >= _app->windowWidth)
    {
        if (_entity->transform.position.x <= 0.0f)
        {
            ball->rightScore++;
            ball->scoreBoard = REDSCORE;
        }
        else if (_entity->transform.position.x >= _app->windowWidth)
        {
            ball->leftScore++;
            ball->scoreBoard = BLUESCORE;
        }
        _entity->transform.position = InitVector3(_app->windowHeight * 0.5f, _app->windowHeight * 0.5f, 0.0f);
        _entity->velocity = InitVector2(0.0f, 0.0f);
        ball->collisionCount = 0;
        ball->pulseFinished = 0;
        ball->pulseTime = 0.0f;
        ball->inversePulseTime = 848.5f;
    }

    if (GetKeyDown(_app, SDL_SCANCODE_P))
    {
        SpawnBall(_app, _entity);
    }
    if (Vec2EqualsZero(_entity->velocity) && GetKey(_app, SDL_SCANCODE_SPACE))
    {
        i32 startingDirection = rand() % 4;

        static Vector2 directions[4] = {
            (Vector2){0.72f, 0.72f},
            (Vector2){0.72f, -0.72f},
            (Vector2){-0.72f, 0.72f},
            (Vector2){-0.72f, -0.72f},
        };

        _entity->velocity = Vec2Mul(directions[startingDirection], 150.0f);
    }

    // check if ball is heading below the screen
    if (_entity->transform.position.y - _entity->transform.scale.y * 0.5f <= 0.0f && _entity->velocity.y < 0.0f)
    {
        _entity->velocity.y *= -1.0f;
    }

    // check if ball is heading above the screen
    if (_entity->transform.position.y + _entity->transform.scale.y * 0.5f >= _app->windowHeight && _entity->velocity.y > 0.0f)
    {
        _entity->velocity.y *= -1.0f;
    }

    Entity *leftPaddle = Find(&_app->scene, "LeftPaddle");
    Entity *rightPaddle = Find(&_app->scene, "RightPaddle");
    // if(leftPaddle){printf("%s\n",leftPaddle->name);} im leaving this comment as a memento mori for all the suffering the above caused

    enum CollisionSide lPaddle = Collision(_entity, leftPaddle);
    enum CollisionSide rPaddle = Collision(_entity, rightPaddle);

    if ((lPaddle == LEFT || lPaddle == RIGHT) ||
        (rPaddle == LEFT || rPaddle == RIGHT))
    {
        ball->collisionCount++;
        _entity->velocity.x *= -1.0f;
        _entity->velocity = Vec2Mul(_entity->velocity, 1.15f);
    }
    else if ((lPaddle == TOP || lPaddle == BOTTOM) ||
             (rPaddle == TOP || rPaddle == BOTTOM))
    {
        ball->collisionCount++;
        _entity->velocity = Vec2Mul(_entity->velocity, -1.0f);
    }

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);
    ball->fireworkSpawnTimer -= _app->deltaTime;

    if (ball->fireworkSpawnTimer <= 0)
    {
        int i = rand() % 5;

        ball->fireworkPos[i] = InitVector3(
            rand() % _app->windowWidth,
            rand() % _app->windowHeight,
            0);

        ball->fireworkRadius[i] = 0.0f;
        ball->fireworkTime[i] = 1.0f;

        ball->fireworkPulse[i] = 0.0f;
        ball->fireworkLife[i] = 1.5f;

        ball->fireworkSpawnTimer = (rand() % 200) / 100.0f + 0.5f;
    }

    if ((ball->leftScore >= 5 || ball->rightScore >= 5) && !ball->gameOver)
    {
        ball->gameOver = 1;
        ball->scoreBoard = DISCO;
        Vector4 color = (ball->leftScore > ball->rightScore) ? leftPaddle->color : rightPaddle->color;
        BallRoomBlitz(_app, _entity, color);
    }
}

void BallDraw(AppContext *_app, Entity *_entity)
{
    Ball *ballStr = (Ball *)_entity->data;
    // ---- Base transform (no scale) ----
    Matrix4 base = IdentityMatrix4();
    Mat4Translate(&base, _entity->transform.position);
    Mat4Rotate(&base, _entity->transform.rotation * DEG2RAD, InitVector3(0, 0, 1));

    BindShader(_entity->shaderId);
    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);

    // ---- HALO PASS ----
    glDepthMask(GL_FALSE);
    for (int i = 0; i < 10; i++)
    {
        int index = (ballStr->index - 1 - i + 10) % 10; // newest first
        Vector3 pos = ballStr->trails[index];

        Matrix4 trailMat = IdentityMatrix4();
        Mat4Translate(&trailMat, pos); // translate to trail position
        Mat4Scale(&trailMat, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, 1.0f));

        float alpha = 0.2f + 0.8f * (1.0f - (float)i / 10.0f); // fading
        ShaderSetVector4(_entity->shaderId, "COLOR", (Vector4){1.0f, 1.0f, 1.0f, alpha});
        ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", trailMat);
        DrawModel(*_entity->model);
    }
    glDepthMask(GL_TRUE);

    // ---- BALL PASS ----
    Matrix4 ball = base;
    Mat4Scale(&ball, _entity->transform.scale);
    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", ball);
    DrawModel(*_entity->model);

    UnBindShader();
}

void BallOnDestroy(AppContext *_app, Entity *_entity)
{
}

Vector4 PositionColor(Vector3 position)
{
    Vector4 color;
    float frequency = 0.05f;

    color.x = 0.5f + 0.5f * sinf(position.x);
    color.y = 0.5f + 0.5f * sinf(position.y);
    color.z = 0.5f + 0.5f * sinf(position.x + position.y);
    color.w = 1.0f;
    return color;
}

Entity *SpawnBall(AppContext *_app, Entity *_entity)
{

    Entity *ball = Spawn(&(_app->scene));
    ball->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
    ball->data = calloc(1, sizeof(Ball));
    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->Start = BallStart;
    ball->Update = BallUpdate;
    ball->Draw = BallDraw;
    ball->OnDestroy = BallOnDestroy;
    return ball;
}

enum CollisionSide Collision(Entity *_ball, Entity *_paddle)
{
    Ball *ball = (Ball *)_ball->data;
    float distanceX = _ball->transform.position.x - _paddle->transform.position.x;
    float distanceY = _ball->transform.position.y - _paddle->transform.position.y;

    float intersectX = fabs(distanceX) - (_ball->transform.scale.x * 0.5f + _paddle->transform.scale.x * 0.5f);
    float intersectY = fabs(distanceY) - (_ball->transform.scale.y * 0.5f + _paddle->transform.scale.y * 0.5f);

    if (intersectX < 0.0f && intersectY < 0.0f)
    {
        if (fabs(intersectX) < fabs(intersectY))
        {
            ball->color = _paddle->color;
            _ball->color = _paddle->color;
            // Collision on X axis
            if (distanceX > 0)
            {
                return RIGHT;
            } // Ball hit paddle's right side
            else
            {

                return LEFT;
            } // Ball hit paddle's left side
        }
        else
        {
            // Collision on Y axis
            if (distanceY > 0)
            {
                return TOP;
            } // Ball hit paddle's top
            else
            {
                return BOTTOM;
            } // Ball hit paddle's bottom
        }
    }

    return NONE;
}
void BallRoomBlitz(AppContext *_app, Entity *_entity, Vector4 color)
{
    // i dont think it should be spawning as many as it does but eh
    for (int i = 0; i < _app->windowWidth; i += 10)
    {
        printf("Color RGBA: %f, %f, %f, %f\n", _entity->color.x, _entity->color.y, _entity->color.z, _entity->color.w);
        Entity *ball = Spawn(&(_app->scene));
        ball->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);
        ball->transform.position = InitVector3(i, _app->windowHeight - 50, 0.0f);
        ball->data = calloc(1, sizeof(Ball));
        ball->image = _entity->image;
        ball->model = _entity->model;
        ball->color = _entity->color;
        ball->velocity = InitVector2(0.0f, -10.0f * ((rand() % 10) + 1));
        ball->shaderId = _entity->shaderId;
        ball->Update = BallUpdate;
        ball->Draw = BallDraw;
        ball->OnDestroy = BallOnDestroy;
    }
}