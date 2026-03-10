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
    Vector3 worldPos;
    void (*Animate)(AppContext *, Entity *);
} Cell;
// "Give the background a grid pattern." doesnt specify HOW the grid should be made
// only that it has to be made so...
// I may have gone overboard
// BEHOLD YEE EFFICIENCY AND DESPAIR AT MY NEARLY 500 LINE ANIMATION STATE MACHINE
void DrawLetter(AppContext *_app, Entity *_entity, char letter, int startGX, int startGY, Vector4 color);
void DrawWord(AppContext *app, Entity *entity, const char *str, int gx, int gy, Vector4 color);
void RandomChange(AppContext *_app, Entity *_entity);
void PulseChange(AppContext *_app, Entity *_entity);
void InvertedPulseChange(AppContext *_app, Entity *_entity);
void ScrollRight(AppContext *_app, Entity *_entity);
void ResetPulse(AppContext *app, Entity *entity);
void ScoreRed(AppContext *_app, Entity *_entity);
void ScoreBlue(AppContext *_app, Entity *_entity);
Vector4 PositionColor(Vector3 position);
void DrawNumber(AppContext *app, Entity *entity, int number, int startGX, int startGY, Vector4 color);
void CountCollisions(AppContext *_app, Entity *_entity);
void StartGame(AppContext *_app, Entity *_entity);
void ResetDisco(AppContext *_app, Entity *_entity);
void FireWorks(AppContext *_app, Entity *_entity);

void CellStart(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;
    Entity *ball = Find(&_app->scene, "Ball");
    cell->ball = (Ball *)ball->data;
    cell->worldPos = _entity->transform.position;

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
    // For myself because I will forget how this spaghetti was cooked
    // When someone scores, Ball.h resets:
    // the ball pos, the ball velocity, the collision count,
    // pulseFinished, pulseTime, and inversePulseTime to their default vaules
    // The ball then tells who scored, resets the background, displays the score
    // then it goes to BASIC mode where it randomly chooses a background animation
    // the new animation Resets the background again and then plays whatever the coin flip was



    //both REDSCORE and BLUESCORE track time so the text stays on long enough to read
    //then it resets
    if (cell->ball->scoreBoard == REDSCORE)
    {
        cell->Animate = ResetPulse;
        if (cell->ball->pulseFinished)
        {
            cell->Animate = ScoreRed;
            if (cell->ball->animTime >= 2.0f)
            {
                cell->ball->scoreBoard = BASIC;
                cell->ball->pulseFinished = 0;
            }
        }
    }
    else if (cell->ball->scoreBoard == BLUESCORE)
    {
        cell->Animate = ResetPulse;
        if (cell->ball->pulseFinished)
        {
            cell->Animate = ScoreBlue;
            if (cell->ball->animTime >= 2.0f)
            {
                cell->ball->scoreBoard = BASIC;
                cell->ball->pulseFinished = 0;
            }
        }
    }
    // ALL CHECKS BELOW MUST SET animTime = 0
    // these are the animations that play after a score
    // and animTime isnt set back to zero elsewhere
    else if (cell->ball->scoreBoard == BASIC)
    {
        int pickAnim = rand() % 10;
        printf("%i", pickAnim);
        if (pickAnim < 5)
        {
            cell->ball->scoreBoard = COUNT;
        }
        else if (pickAnim >= 5)
        {
            cell->ball->scoreBoard = FIREWORK;
        }
    }
    else if (cell->ball->scoreBoard == COUNT)
    {
        cell->Animate = ResetPulse;
        if (cell->ball->pulseFinished)
        {
            cell->ball->animTime = 0.0;
            cell->Animate = CountCollisions;
        }
    }
    else if (cell->ball->scoreBoard == FIREWORK)
    {
        cell->Animate = ResetPulse;
        if (cell->ball->pulseFinished)
        {
            cell->ball->animTime = 0.0f;
            cell->Animate = FireWorks;
        }
    }
    else if (cell->ball->scoreBoard == DISCO)
    {
        cell->Animate = ResetDisco;
        if (cell->ball->pulseFinished)
        {
            cell->ball->animTime = 0.0f;
            if (_entity->color.x == 0.0f && _entity->color.y == 0.0f && _entity->color.z == 0.0f)
            {
                _entity->color = PositionColor(_entity->transform.position);
            }
            cell->Animate = RandomChange;
        }
    }
    else if (cell->ball->scoreBoard == START)
    {
        cell->Animate = StartGame;
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
    Cell *cell = (Cell *)_entity->data;

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

    cell->ball->pulseTime += 0.001f;

    if (cell->distance < cell->ball->pulseTime && cell->distance > cell->ball->pulseTime - 32.0f)
    {
        _entity->color = (Vector4){1.0f, 1.0f, 1.0f, 1.0f};
    }
}
void InvertedPulseChange(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;

    cell->ball->inversePulseTime -= 0.001f;

    if (cell->distance > cell->ball->inversePulseTime && cell->distance < cell->ball->inversePulseTime + 32.0f)
    {
        _entity->color = (Vector4){0.0f, 0.0f, 0.0f, 1.0f};
        _entity->transform.position = cell->worldPos;
    }
}
void ImplodeColor(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;

    cell->ball->inversePulseTime -= 0.001f;

    if (cell->distance > cell->ball->inversePulseTime && cell->distance < cell->ball->inversePulseTime + 32.0f)
    {
        _entity->color = PositionColor(_entity->transform.position);
    }
}
void ResetPulse(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;

    PulseChange(_app, _entity);
    InvertedPulseChange(_app, _entity);

    if (cell->ball->pulseTime >= 848.5f && cell->ball->inversePulseTime <= 0)
    {
        cell->ball->pulseFinished = 1;
        cell->ball->pulseTime = 0.0f;
        cell->ball->inversePulseTime = 848.5;
    }
}
void ResetDisco(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;
    PulseChange(_app, _entity);
    ImplodeColor(_app, _entity);
    if (cell->ball->pulseTime >= 848.5f && cell->ball->inversePulseTime <= 0)
    {
        cell->ball->pulseFinished = 1;
        cell->ball->pulseTime = 0.0f;
        cell->ball->inversePulseTime = 848.5;
    }
}
void RandomChange(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;

    cell->var -= _app->deltaTime;

    if (cell->var <= 0)
    {
        if (rand() % 2 == 0)
        {
            _entity->color = (Vector4){fabs(1.0f - _entity->color.x), fabs(1.0f - _entity->color.y), fabs(1.0f - _entity->color.z), 1.0f};
        }

        cell->var = rand() % 500 + 500;
    }
}
void FireWorks(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;

    for (int i = 0; i < 5; i++)
    {
        if (cell->ball->fireworkLife[i] <= 0)
            continue;

        cell->ball->fireworkPulse[i] += 300.0f * 0.001f;
        cell->ball->fireworkLife[i] -= _app->deltaTime;

        Vector3 pos = cell->ball->fireworkPos[i];

        float dx = _entity->transform.position.x - pos.x;
        float dy = _entity->transform.position.y - pos.y;

        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < cell->ball->fireworkPulse[i] &&
            dist > cell->ball->fireworkPulse[i] - 32.0f)
        {
            _entity->color = PositionColor(pos);
        }
    }
    ScrollRight(_app, _entity);
}
// Big plans 20 rows 21 columns
// 6 Chars per line
// 4 lines (maybe 3 to be safe)

// Valid gx LEFT->RIGTH 3, 7, 11, 15
// Valid gy DOWN->UP 2, 8, 14
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
// Valid gx LEFT->RIGTH 3, 7, 11, 15
// Valid gy DOWN->UP 2, 8, 14
void DrawNumber(AppContext *app, Entity *entity, int number, int startGX, int startGY, Vector4 color)
{
    if (number < 0 || number > 9)
        return;

    const int (*glyph)[3] = fontNumbers[number];

    Cell *cell = (Cell *)entity->data;
    int localX = cell->gx - startGX;
    int localY = cell->gy - startGY;

    if (localX >= 0 && localX < 3 && localY >= 0 && localY < 5)
    {
        if (glyph[localY][localX])
            entity->color = color;
        else
            entity->color = (Vector4){0, 0, 0, 1};
    }
}

void ScoreRed(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;
    DrawWord(_app, _entity, "GOAL", 3, 14, InitVector4(1.0f, 1.0f, 1.0f, 1.0f));
    DrawWord(_app, _entity, "RED", 5, 8, InitVector4(1.0f, 0.0f, 0.0f, 1.0f));
    DrawNumber(_app, _entity, cell->ball->rightScore, 9, 2, InitVector4(1.0f, 0.0f, 0.0f, 1.0f));
    if (cell->gx == 0 && cell->gy == 0) // only once cell increments the timer
    {
        cell->ball->animTime += _app->deltaTime;
    }
}
void ScoreBlue(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;
    DrawWord(_app, _entity, "GOAL", 3, 14, InitVector4(1.0f, 1.0f, 1.0f, 1.0f));
    DrawWord(_app, _entity, "BLUE", 3, 8, InitVector4(0.0f, 0.0f, 1.0f, 1.0f));
    DrawNumber(_app, _entity, cell->ball->leftScore, 9, 2, InitVector4(0.0f, 0.0f, 1.0f, 1.0f));
    if (cell->gx == 0 && cell->gy == 0) // only once cell increments the timer
    {
        cell->ball->animTime += _app->deltaTime;
    }
}
void CountCollisions(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;
    Vector4 color = cell->ball->color;
    if (cell->ball->collisionCount > 5)
    {
        color = PositionColor(_entity->transform.position);
    }
    int ones = cell->ball->collisionCount % 10;
    int tens = (cell->ball->collisionCount / 10) % 10;
    DrawNumber(_app, _entity, tens, 7, 8, color);
    DrawNumber(_app, _entity, ones, 11, 8, color);
}
void StartGame(AppContext *_app, Entity *_entity)
{
    Cell *cell = (Cell *)_entity->data;
    DrawWord(_app, _entity, "HEY", 5, 14, InitVector4(0.7f, 0.7f, 0.7f, 1.0f));
    DrawWord(_app, _entity, "HIT", 5, 8, InitVector4(0.7f, 0.7f, 0.7f, 1.0f));
    DrawWord(_app, _entity, "SPACE", 1, 2, InitVector4(0.7f, 0.7f, 0.7f, 1.0f));
}