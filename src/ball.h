#pragma once
#include <math.h>
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"

#include <SDL3/SDL.h>

typedef struct {
    int leftScore;
    int rightScore;
} Ball;

Entity* SpawnBall(AppContext* _app, Entity* _entity);
Entity* Find(Scene** _scene, const char* _name);
Vector4 PositionColor(Vector3 position);
Entity* GetEntity(Scene* scene, int id);


void BallStart(AppContext* _app, Entity* _entity) {
    _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);

    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);
}

void BallUpdate(AppContext* _app, Entity* _entity) {

    if (GetKeyDown(_app, SDL_SCANCODE_P))
    {
        SpawnBall(_app, _entity);
    }
    if (Vec2EqualsZero(_entity->velocity) && GetKey(_app, SDL_SCANCODE_SPACE) )
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
    if (_entity->transform.position.y - _entity->transform.scale.y * 0.5f <= 0.0f && _entity->velocity.y < 0.0f){
        SpawnBall(_app, _entity);
        _entity->velocity.y *= -1.0f;
        _entity->velocity = Vec2Mul(_entity->velocity, 2.0f);
    }
    
    // check if ball is heading above the screen
    if (_entity->transform.position.y + _entity->transform.scale.y * 0.5f >= _app->windowHeight && _entity->velocity.y > 0.0f){
        SpawnBall(_app, _entity);
        _entity->velocity.y *= -1.0f;
        _entity->velocity = Vec2Mul(_entity->velocity, 0.5f);
    }
        
    Entity* leftPaddle = GetEntity((Scene*)_app->scene, 1);
    Entity* rightPaddle = GetEntity((Scene*)_app->scene, 2);
    //if(leftPaddle){printf("%s\n",leftPaddle->name);} im leaving this comment as a memento mori for all the suffering the above caused


    if(_entity->transform.position.x + _entity->transform.scale.x * 0.5f >= _app->windowWidth && _entity->velocity.x > 0.0f)
    {
        _entity->velocity.x *= -1.0f;
    }
    if(_entity->transform.position.x - _entity->transform.scale.x * 0.5f <= 0.0f && _entity->velocity.x < 0.0f)
    {
        _entity->velocity.x *= -1.0f;
    }

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);
}

void BallDraw(AppContext* _app, Entity* _entity) {
    Matrix4 transform = IdentityMatrix4(); // the order is important
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    ShaderSetVector4(_entity->shaderId, "COLOR", PositionColor(_entity->transform.position));
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void BallOnDestroy(AppContext* _app, Entity* _entity) {

}


Vector4 PositionColor(Vector3 position) {
    Vector4 color;
    float frequency = 0.05f; // controls how many waves fit across the space

    color.x = 0.5f + 0.5f * sinf(position.x * frequency + 0.0f); // R depends on X
    color.y = 0.5f + 0.5f * sinf(position.y * frequency + 2.0f); // G depends on Y
    color.z = 0.5f + 0.5f * sinf((position.x + position.y) * frequency + 4.0f); // B depends on XY
    color.w = 1.0f; // full alpha

    return color;
}

Entity* SpawnBall(AppContext* _app, Entity* _entity) {
    void** scene = &(_app->scene);
    Entity* ball = Spawn((Scene**)scene);
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