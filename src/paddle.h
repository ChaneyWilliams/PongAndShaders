#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"

typedef struct
{
    int var;
} Paddle;

void PaddleStart(AppContext *_app, Entity *_entity)
{
    _entity->transform.rotation = 0.0f;
    _entity->transform.scale = InitVector3(32.0f, 128.0f, 1.0f);
}

void PaddleUpdateTheOriginal(AppContext *_app, Entity *_entity)
{
    Vector2 direction = {0.0f, 0.0f};

    if (GetKey(_app, SDL_SCANCODE_W))
    {
        direction = (Vector2){0.0f, 1.0f};
    }
    else if (GetKey(_app, SDL_SCANCODE_S))
    {
        direction = (Vector2){0.0f, -1.0f};
    }
    _entity->velocity = Vec2Mul(direction, 150.0f);

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);
}
void PaddleUpdateTheSequal(AppContext *_app, Entity *_entity)
{
    Vector2 direction = {0.0f, 0.0f};

    if (GetKey(_app, SDL_SCANCODE_I))
    {
        direction = (Vector2){0.0f, 1.0f};
    }
    else if (GetKey(_app, SDL_SCANCODE_K))
    {
        direction = (Vector2){0.0f, -1.0f};
    }
    _entity->velocity = Vec2Mul(direction, 150.0f);

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);
}

void PaddleDraw(AppContext *_app, Entity *_entity)
{

    Matrix4 transform = IdentityMatrix4(); // the order is important
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);

    DrawModel(*_entity->model);

    UnBindShader();
}
void PaddleOnDestroy(AppContext *_app, Entity *_entity)
{
}