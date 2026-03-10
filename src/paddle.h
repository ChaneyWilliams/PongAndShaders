#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "ball.h"

typedef struct
{
    int var;
    float recoil;
} Paddle;
enum CollisionSide PaddleCollision(Entity *_entity1, Entity *_paddle);
void PaddleStart(AppContext *_app, Entity *_entity)
{
    _entity->transform.rotation = 0.0f;
    _entity->transform.scale = InitVector3(32.0f, 128.0f, 1.0f);
    Paddle *paddle = (Paddle *)_entity->data;
    Entity* ball = Find(&_app->scene, "Ball");
    paddle->recoil = 6.0f;
}

void PaddleUpdateTheOriginal(AppContext *_app, Entity *_entity)
{
    Paddle *paddle = (Paddle *)_entity->data;
    Entity* ball = Find(&_app->scene, "Ball");
    Vector2 direction = {0.0f, 0.0f};
    if (_entity->transform.position.x < 16.0f)
    {
        _entity->transform.position.x = 16.0f;
    }

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
    if (PaddleCollision(_entity, ball))
    {
        _entity->transform.position.x -= paddle->recoil;
    }
}
void PaddleUpdateTheSequal(AppContext *_app, Entity *_entity)
{
    Paddle *paddle = (Paddle *)_entity->data;
    Entity* ball = Find(&_app->scene, "Ball");
    Vector2 direction = {0.0f, 0.0f};

    if (_entity->transform.position.x > _app->windowWidth - 16.0f)
    {
        _entity->transform.position.x = _app->windowWidth - 16.0f;
    }
    if (GetKey(_app, SDL_SCANCODE_UP))
    {
        direction = (Vector2){0.0f, 1.0f};
    }
    else if (GetKey(_app, SDL_SCANCODE_DOWN))
    {
        direction = (Vector2){0.0f, -1.0f};
    }
    _entity->velocity = Vec2Mul(direction, 150.0f);

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);

    if (PaddleCollision(_entity, ball))
    {
        _entity->transform.position.x += paddle->recoil;
    }
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
enum CollisionSide PaddleCollision(Entity *_entity1, Entity *_paddle)
{
    float distanceX = _entity1->transform.position.x - _paddle->transform.position.x;
    float distanceY = _entity1->transform.position.y - _paddle->transform.position.y;

    float intersectX = fabs(distanceX) - (_entity1->transform.scale.x * 0.5f + _paddle->transform.scale.x * 0.5f);
    float intersectY = fabs(distanceY) - (_entity1->transform.scale.y * 0.5f + _paddle->transform.scale.y * 0.5f);

    if (intersectX < 0.0f && intersectY < 0.0f)
    {
        if (fabs(intersectX) < fabs(intersectY))
        {
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