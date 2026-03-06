#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "ball.h"
#include "Fonts.h"

typedef struct
{
    int var;
    int gx;
    int gy;
    float distance;
    void (*Animate)(AppContext*, Entity*);
} Cell;

void RandomChange(AppContext *_app, Entity *_entity);
void PulseChange(AppContext *_app, Entity *_entity);
void InvertedPulseChange(AppContext *_app, Entity *_entity);
void ScrollRight(AppContext *_app, Entity *_entity);
void PulseBoth(AppContext *app, Entity *entity);
void DrawLetterH(AppContext *app, Entity *entity);

void CellStart(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;

    _entity->transform.rotation = 0.0f;
    _entity->color = (Vector4){0.0f, 0.0f, 0.0f, 1.0f}; //(PositionColor(_entity->transform.position));
    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);

    float centerX = _app->windowWidth / 2.0f;
    float centerY = _app->windowHeight / 2.0f;

    float dx = _entity->transform.position.x - centerX;
    float dy = _entity->transform.position.y - centerY;

    cell->distance = sqrtf(dx * dx + dy * dy);
}

void CellUpdate(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;

    if (GetKey(_app, SDL_SCANCODE_E))
    {
        cell->Animate = ScrollRight;
    }
    else if (GetKey(_app, SDL_SCANCODE_R))
    {
        cell->Animate = PulseBoth;
    }
    else if (GetKey(_app, SDL_SCANCODE_T))
    {
        cell->Animate = RandomChange;
    }
    else if(GetKey(_app,SDL_SCANCODE_H)){
        DrawLetterH(_app,_entity);
    }
    if (cell->Animate)
    {
        cell->Animate(_app, _entity);
    }
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

void ScrollRight(AppContext *_app, Entity *_entity)
{
    float scrollSpeed = 0.1f;

    // Move toward top-right
    _entity->transform.position.x += scrollSpeed;
    _entity->transform.position.y += scrollSpeed;

    // Wrap horizontally
    if (_entity->transform.position.x > _app->windowWidth + _entity->transform.scale.x * 0.5f)
        _entity->transform.position.x = -32.0f;

    // Wrap vertically
    if (_entity->transform.position.y > _app->windowHeight + _entity->transform.scale.y * 0.5)
        _entity->transform.position.y = -32.0f;
}

void PulseChange(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;

    float dx = _entity->transform.position.x - _app->windowWidth / 2.0f;
    float dy = _entity->transform.position.y - _app->windowHeight / 2.0f;

    cell->distance = sqrtf(dx * dx + dy * dy);
    
    static float pulseTime = 0.0f;
    pulseTime += 0.001f;

    if (cell->distance < pulseTime && cell->distance > pulseTime - 32.0f)
    {
        _entity->color = (Vector4){
            fabs(1.0f - _entity->color.x),
            fabs(1.0f - _entity->color.y),
            fabs(1.0f - _entity->color.z),
            1.0f};
    }

    if (pulseTime > sqrtf(_app->windowWidth * _app->windowWidth +
                          _app->windowHeight * _app->windowHeight))
    {
        pulseTime = 0.0f;
    }
}
void InvertedPulseChange(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;
    float dx = _entity->transform.position.x - _app->windowWidth / 2.0f;
    float dy = _entity->transform.position.y - _app->windowHeight / 2.0f;

    cell->distance = sqrtf(dx * dx + dy * dy);

    static float inversePulse = 848.5f; // its this: sqrtf(_app->windowWidth * _app->windowWidth + _app->windowHeight * _app->windowHeight
    inversePulse -= 0.001f;

    if (cell->distance > inversePulse && cell->distance < inversePulse + 32.0f)
    {
        _entity->color = (Vector4){fabs(1.0f - _entity->color.x), fabs(1.0f - _entity->color.y), fabs(1.0f - _entity->color.z), 1.0f};
    }
    if (inversePulse <= 0)
    {
        inversePulse = 848.5f;
    }
}
void PulseBoth(AppContext *app, Entity *entity)
{
    PulseChange(app, entity);
    InvertedPulseChange(app, entity);
}
void RandomChange(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;

    cell->var--;

    if (cell->var <= 0)
    {
        if (rand() % 2 == 0)
        {
            _entity->color = (Vector4){
                fabs(1.0f - _entity->color.x),
                fabs(1.0f - _entity->color.y),
                fabs(1.0f - _entity->color.z),
                1.0f};
        }

        cell->var = rand() % 500 + 500;
    }
}
//Big plans
void DrawLetterH(AppContext *app, Entity *entity)
{
    Cell *cell = (Cell*)entity->data;

    //grid coords
    int startGX = 10; 
    int startGY = 10; 

    const int glyph_H[5][3] = {
        {1,0,1},
        {1,0,1},
        {1,1,1},
        {1,0,1},
        {1,0,1}
    };

    int localX = cell->gx - startGX;
    int localY = cell->gy - startGY; 

    // Only consider cells inside the letter bounds
    if (localX >= 0 && localX < 3 && localY >= 0 && localY < 5)
    {
        if (glyph_H[localY][localX])
        {
            entity->color = (Vector4){1.0f, 1.0f, 1.0f, 1.0f}; // ON
        }
        else
        {
            entity->color = (Vector4){0.0f, 0.0f, 0.0f, 1.0f}; // OFF
        }
    }
}