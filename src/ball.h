#pragma once
#include <math.h>
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"

#include <SDL3/SDL.h>

typedef struct
{
    int leftScore;
    int rightScore;
} Ball;

int count = 0;
Entity *SpawnBall(AppContext *_app, Entity *_entity);
Entity *Find(Scene **_scene, const char *_name);
Vector4 PositionColor(Vector3 position);
Entity *GetEntity(Scene *scene, int id);
bool Collision(Entity *_ball, Entity *_paddle);

void BallStart(AppContext *_app, Entity *_entity)
{
    _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);

    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);
}

void BallUpdate(AppContext *_app, Entity *_entity)
{

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

    Entity *leftPaddle = GetEntity((Scene *)_app->scene, 1);
    Entity *rightPaddle = GetEntity((Scene *)_app->scene, 2);
    // if(leftPaddle){printf("%s\n",leftPaddle->name);} im leaving this comment as a memento mori for all the suffering the above caused

    if (Collision(_entity, leftPaddle) || Collision(_entity, rightPaddle))
    {
        count++;
        printf("collision: %i\n", count);
        _entity->velocity.x *= -1.0f;
        _entity->velocity = Vec2Mul(_entity->velocity, 1.15f);
    }

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);
}

void BallDraw(AppContext *_app, Entity *_entity)
{
    // ---- Base transform (no scale) ----
    Matrix4 base = IdentityMatrix4();
    Mat4Translate(&base, _entity->transform.position);
    Mat4Rotate(&base, _entity->transform.rotation * DEG2RAD, InitVector3(0,0,1));

    BindShader(_entity->shaderId);
    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    // ---- BALL PASS ----
    Matrix4 ball = base;
    Mat4Scale(&ball, _entity->transform.scale);
    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", ball);
    DrawModel(*_entity->model);

    // ---- HALO PASS ----
    Matrix4 halo = base;
    Mat4Scale(&halo, InitVector3(_entity->transform.scale.x * 1.4f, _entity->transform.scale.y * 1.4f, _entity->transform.scale.z));
    ShaderSetVector4(_entity->shaderId, "COLOR", (Vector4){_entity->color.x, _entity->color.y, _entity->color.z, 0.3f});
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", halo);
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

    color.x = 0.5f + 0.5f * sinf(position.x * frequency + 0.0f);
    color.y = 0.5f + 0.5f * sinf(position.y * frequency + 2.0f);
    color.z = 0.5f + 0.5f * sinf((position.x + position.y) * frequency + 4.0f);
    color.w = 1.0f;
    return color;
}

Entity *SpawnBall(AppContext *_app, Entity *_entity)
{
    void **scene = &(_app->scene);
    Entity *ball = Spawn((Scene **)scene);
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

bool Collision(Entity *_ball, Entity *_paddle)
{
    bool touchingX = false;
    bool touchingY = false;
    if (_ball->transform.position.x + _ball->transform.scale.x * 0.5f > _paddle->transform.position.x - _paddle->transform.scale.x * 0.5f &&
        _ball->transform.position.x - _ball->transform.scale.x * 0.5f < _paddle->transform.position.x + _paddle->transform.scale.x * 0.5f)
    {
        touchingX = true;
    }
    if (_ball->transform.position.y + _ball->transform.scale.y * 0.5f > _paddle->transform.position.y - _paddle->transform.scale.y * 0.5f &&
        _ball->transform.position.y - _ball->transform.scale.y * 0.5f < _paddle->transform.position.y + _paddle->transform.scale.y * 0.5f)
    {
        touchingY = true;
    }
    if (touchingX && touchingY)
    {
        _ball->color = _paddle->color;
        return true;
    }
    return false;
}