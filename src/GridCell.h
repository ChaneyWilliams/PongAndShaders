#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "ball.h"

typedef struct
{
    int var;
} Cell;

void RandomChange(Entity *_entity);
void PulseChange(AppContext *_app, Entity *_entity);
void InvertedPulseChange(AppContext *_app, Entity *_entity);

void CellStart(AppContext *_app, Entity *_entity)
{
    _entity->transform.rotation = 0.0f;
    _entity->color = PositionColor(_entity->transform.position);
    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);

    // Compute center of screen
    float centerX = _app->windowWidth / 2.0f;
    float centerY = _app->windowHeight / 2.0f;

    // Compute distance from center and store it in _entity->id temporarily
    float dx = _entity->transform.position.x - centerX;
    float dy = _entity->transform.position.y - centerY;
    _entity->id = sqrtf(dx * dx + dy * dy); // distance from center
}

void CellUpdate(AppContext *_app, Entity *_entity)
{
    PulseChange(_app, _entity);
    InvertedPulseChange(_app, _entity);
}
void CellDraw(AppContext *_app, Entity *_entity)
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
void CellDestroy(AppContext *_app, Entity *_entity)
{
}
void PulseChange(AppContext *_app, Entity *_entity)
{
    static float pulseTime = 0.0;
    pulseTime += 0.001f;

    if (_entity->id < pulseTime && _entity->id > pulseTime - 32.0f)
    {
        _entity->color = (Vector4){fabs(1.0f - _entity->color.x), fabs(1.0f - _entity->color.y), fabs(1.0f - _entity->color.z), 1.0f};
    }
    if (pulseTime > sqrtf(_app->windowWidth * _app->windowWidth + _app->windowHeight * _app->windowHeight))
    {
        pulseTime = 0.0f;
    }
}
void InvertedPulseChange(AppContext *_app, Entity *_entity)
{

    static float inversePulse = 848.5f; // its this: sqrtf(_app->windowWidth * _app->windowWidth + _app->windowHeight * _app->windowHeight
    inversePulse -= 0.001f;

    if (_entity->id > inversePulse && _entity->id < inversePulse + 32.0f)
    {
        _entity->color = (Vector4){fabs(1.0f - _entity->color.x), fabs(1.0f - _entity->color.y), fabs(1.0f - _entity->color.z), 1.0f};
    }
    if (inversePulse <= 0)
    {
        inversePulse = 848.5f;
    }
}
void RandomChange(Entity *_entity)
{
    _entity->id--;

    if (_entity->id <= 0)
    {
        if (rand() % 2 == 0)
        {
            _entity->color = (Vector4){fabs(1.0f - _entity->color.x), fabs(1.0f - _entity->color.y), fabs(1.0f - _entity->color.z), 1.0f};
        }
        _entity->id = rand() % 500 + 500;
    }
}
