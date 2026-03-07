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
    Ball *ball;
    int reactedToScore;
    void (*Animate)(AppContext *, Entity *);
} Cell;

void DrawLetter(AppContext *_app, Entity *_entity, char letter, int startGX, int startGY, Vector4 color);
void DrawWord(AppContext *app, Entity *entity, const char *str, int gx, int gy, Vector4 color);
void RandomChange(AppContext *_app, Entity *_entity);
void PulseChange(AppContext *_app, Entity *_entity);
void InvertedPulseChange(AppContext *_app, Entity *_entity);
void ScrollRight(AppContext *_app, Entity *_entity);
void ResetPulse(AppContext *app, Entity *entity);
void DrawLetterH(AppContext *app, Entity *entity, int startGX, int startGY);
Vector4 PositionColor(Vector3 position);

void CellStart(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;
    Entity *ball = Find(&_app->scene, "Ball");
    cell->ball = (Ball *)ball->data;

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

    if (cell->ball->scoreBoard == SCORE)
    {

        cell->Animate = ResetPulse;
    }
    if (GetKey(_app, SDL_SCANCODE_E))
    {
        cell->Animate = ScrollRight;
    }
    else if (GetKey(_app, SDL_SCANCODE_R))
    {
        cell->Animate = ResetPulse;
    }
    else if (GetKey(_app, SDL_SCANCODE_T))
    {
        cell->Animate = RandomChange;
    }
    else if (GetKey(_app, SDL_SCANCODE_H))
    {
        Vector4 color = PositionColor(_entity->transform.position);
        DrawWord(_app, _entity, "HEY", 3, 14, color);
        DrawWord(_app, _entity, "HIT", 3, 8, color);
        DrawWord(_app, _entity, "RKEY", 3, 2, color);
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
    Ball *ball = cell->ball;
    float dx = _entity->transform.position.x - _app->windowWidth / 2.0f;
    float dy = _entity->transform.position.y - _app->windowHeight / 2.0f;

    cell->distance = sqrtf(dx * dx + dy * dy);

    ball->pulseTime += 0.001f;

    if (cell->distance < ball->pulseTime && cell->distance > ball->pulseTime - 32.0f)
    {
        _entity->color = (Vector4){1.0f, 1.0f, 1.0f, 1.0f};
    }
}
void InvertedPulseChange(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;
    Ball *ball = cell->ball;
    float dx = _entity->transform.position.x - _app->windowWidth / 2.0f;
    float dy = _entity->transform.position.y - _app->windowHeight / 2.0f;

    cell->distance = sqrtf(dx * dx + dy * dy);

    ball->inversePulseTime -= 0.001f;

    if (cell->distance > ball->inversePulseTime && cell->distance < ball->inversePulseTime + 32.0f)
    {
        _entity->color = (Vector4){0.0f, 0.0f, 0.0f, 1.0f};
    }
}
void ResetPulse(AppContext *app, Entity *entity)
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
// Big plans 20 rows 21 columns
// 6 Chars per line
// 4 lines (maybe 3 to be safe)

// Valid gx 3, 7, 11, 15
// Valid gy 2, 8, 14
void DrawWord(AppContext *app, Entity *entity, const char *str, int gx, int gy, Vector4 color)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        DrawLetter(app, entity, str[i], gx + i * 4, gy, color);
    }
}
void DrawLetter(AppContext *_app, Entity *_entity, char letter, int startGX, int startGY, Vector4 color)
{
    Cell *cell = (Cell *)_entity->data;

    if (letter < 'A' || letter > 'Z')
        return;

    int glyphIndex = letter - 'A';
    const int (*glyph)[3] = font[glyphIndex];

    int localX = cell->gx - startGX;
    int localY = cell->gy - startGY;

    // Only consider cells inside the letter bounds
    if (localX >= 0 && localX < 3 && localY >= 0 && localY < 5)
    {
        if (glyph[localY][localX])
        {
            _entity->color = color; // ON
        }
        else
        {
            _entity->color = (Vector4){0.0f, 0.0f, 0.0f, 1.0f}; // OFF
        }
    }
}
void DrawLetterH(AppContext *app, Entity *entity, int startGX, int startGY)
{
    Cell *cell = (Cell *)entity->data;

    // grid coords

    const int glyph_H[5][3] = {
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
        {1, 0, 1},
        {1, 0, 1}};

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