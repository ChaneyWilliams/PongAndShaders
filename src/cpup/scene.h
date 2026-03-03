#pragma once

#include "entity.h"

typedef struct {
    Entity* entities;
    Entity* startEntities;
    int nextId;
} Scene;

Scene*  SceneInit();
void    SceneStart(AppContext* _app, Scene** _scene);
void    SceneUpdate(AppContext* _app, Scene** _scene);
void    SceneDraw(AppContext* _app, Scene** _scene);
Scene*  SceneFree(Scene** _scene);

Entity* Spawn(Scene** _scene);
void    Destroy(AppContext* _app, Scene** _scene, int _id);

//look
//i tried everything
//id put * and & in every possible spot and every possible combo on Find()
//it would always give null
//i found GetEntity and changed it to this
//thanks to the power of adderall, AI that broke more than it fixed, and spite
//it doesnt return null and finds the entity
//whatever the consequences are
//i dont care
// the relief in the print statement going through
//was the biggest hit of dopamnie i've had since Christmas morning
Entity* GetEntity(Scene* scene, int id);
