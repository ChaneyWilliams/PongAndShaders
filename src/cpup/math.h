#pragma once
#include "types.h"

static const f32 PI = 3.14159265f;
static const f32 RAD2DEG = 180.0f / PI;
static const f32 DEG2RAD = PI / 180.0f;

typedef struct {
    f32 x;
    f32 y;
} Vector2;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vector3;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

typedef struct {
    f32 m[9];
} Matrix3;

typedef struct {
    f32 m[16];
} Matrix4;

Vector2 InitVector2(f32 _x, f32 _y);

Vector2 RotatePoint(Vector2 _point, float _radian);

Vector2 RotatePointAroundPivot(Vector2 _point, Vector2 _pivot, float _radian);

Vector2 Vec2Add(Vector2 _a, Vector2 _b);

Vector2 Vec2Sub(Vector2 _a, Vector2 _b);

Vector2 Vec2Mul(Vector2 _a, f32 _scalar);

f32 Vec2Distance(Vector2 _a, Vector2 _b);

f32 Vec2Magnitude(Vector2 _v);

Vector2 Vec2Normalized(Vector2 _v);

bool Vec2Equals(Vector2 _a, Vector2 _b);

bool Vec2EqualsZero(Vector2 _v);

Vector3 Vec2ToVec3(Vector2 _v);

Vector3 InitVector3(f32 _x, f32 _y, f32 _z);

Vector3 Vec3Add(Vector3 _a, Vector3 _b);

Vector3 Vec3Sub(Vector3 _a, Vector3 _b);

Vector3 Vec3Mul(Vector3 _a, f32 _scalar);

bool Vec3EqualsZero(Vector3 _a);

Vector4 InitVector4(f32 _x, f32 _y, f32 _z, f32 _w);

bool CampareMatrix4(Matrix4* _a, Matrix4* _b);

Matrix4 IdentityMatrix4();

void Mat4Add(Matrix4* _mat, f32 _scalar);

void Mat4Sub(Matrix4* _mat, f32 _scalar);

void Mat4Mul(Matrix4* _mat, f32 _scalar);

void Mat4Div(Matrix4* _mat, f32 _scalar);

Matrix4 Mat4MulMat4(Matrix4 a, Matrix4 b);

void Mat4Translate(Matrix4* _mat, Vector3 _translation);

void Mat4Scale(Matrix4* _mat, Vector3 _scale);

void Mat4Rotate(Matrix4* _mat, float _radians, Vector3 _axis);

Matrix4 Mat4Orthographic(f32 _left, f32 _right, f32 _bottom, f32 _top, f32 _near, f32 _far);