#include "math.h"

#include <math.h>

Vector2 InitVector2(f32 _x, f32 _y)
{
    Vector2 v;
    v.x = _x;
    v.y = _y;
    return v;
}

Vector2 RotatePoint(Vector2 _vector, float _radian)
{
    float c = (float)cos((double)_radian);
    float s = (float)sin((double)_radian);
    
    Vector2 v;
    v.x = _vector.x * c - _vector.y * s;
    v.y =  _vector.x * s + _vector.y * c;
    return v;
}

Vector2 RotatePointAroundPivot(Vector2 _point, Vector2 _pivot, float _radian)
{
    float s = (float)sin((double)-_radian);
    float c = (float)cos((double)-_radian);

    Vector2 holder = Vec2Sub(_point, _pivot);

    Vector2 v;
    v.x = holder.x * c - holder.y * s;
    v.y = holder.x * s + holder.y * c;

    return Vec2Add(v, _pivot);
}

Vector2 Vec2Add(Vector2 _a, Vector2 _b)
{
    Vector2 v;
    v.x = _a.x + _b.x;
    v.y = _a.y + _b.y;
    return v;
}

Vector2 Vec2Sub(Vector2 _a, Vector2 _b)
{
    Vector2 v;
    v.x = _a.x - _b.x;
    v.y = _a.y - _b.y;
    return v;
}

Vector2 Vec2Mul(Vector2 _a, f32 _scalar)
{
    Vector2 v;
    v.x = _a.x * _scalar;
    v.y = _a.y * _scalar;
    return v;
}

f32 Vec2Distance(Vector2 _a, Vector2 _b)
{
    return sqrtf(powf(_b.x - _a.x, 2.0f) + powf(_b.y - _a.y, 2.0f));
}

f32 Vec2Magnitude(Vector2 _v)
{
    return sqrtf(_v.x * _v.x + _v.y * _v.y);
}

Vector2 Vec2Normalize(Vector2 _v)
{
    float mag = Vec2Magnitude(_v);

    if (mag == 0.0f)
        return (Vector2){0.0f, 0.0f};

    return (Vector2){
        _v.x / mag,
        _v.y / mag
    };
}

bool Vec2Equals(Vector2 _a, Vector2 _b)
{
    return (_a.x == _b.x && _a.y == _b.y);
}

bool Vec2EqualsZero(Vector2 _v)
{
    return (_v.x == 0.0f && _v.y == 0.0f);
}

Vector3 Vec2ToVec3(Vector2 _v)
{
    return (Vector3){_v.x, _v.y, 0.0f};
}

Vector3 InitVector3(f32 _x, f32 _y, f32 _z)
{
    Vector3 v;
    v.x = _x;
    v.y = _y;
    v.z = _z;
    return v;
}

Vector3 Vec3Add(Vector3 _a, Vector3 _b)
{
    Vector3 v;
    v.x = _a.x + _b.x;
    v.y = _a.y + _b.y;
    v.z = _a.z + _b.z;
    return v;
}

Vector3 Vec3Sub(Vector3 _a, Vector3 _b)
{
    Vector3 v;
    v.x = _a.x - _b.x;
    v.y = _a.y - _b.y;
    v.z = _a.z - _b.z;
    return v;
}

Vector3 Vec3Mul(Vector3 _a, f32 _scalar)
{
    Vector3 v;
    v.x = _a.x * _scalar;
    v.y = _a.y * _scalar;
    v.z = _a.z * _scalar;
    return v;
}
bool Vec3EqualsZero(Vector3 _a){
    return (fabs(_a.x) <= 0.00001f && fabs(_a.y) <= 0.00001f && fabs(_a.z) <= 0.00001f);
}


Vector4 InitVector4(f32 _x, f32 _y, f32 _z, f32 _w)
{
    Vector4 v;
    v.x = _x;
    v.y = _y;
    v.z = _z;
    v.w = _w;
    return v;
}

bool CampareMatrix4(Matrix4* _a, Matrix4* _b)
{
    return memcmp(_a, _b, sizeof(Matrix4)) == 0;
}

Matrix4 IdentityMatrix4()
{
    Matrix4 matrix = {};

    matrix.m[0] = 1.0f;
    matrix.m[4] = 0.0f;
    matrix.m[8] = 0.0f;
    matrix.m[12] = 0.0f;
    matrix.m[1] = 0.0f;
    matrix.m[5] = 1.0f;
    matrix.m[9] = 0.0f;
    matrix.m[13] = 0.0f;
    matrix.m[2] = 0.0f;
    matrix.m[6] = 0.0f;
    matrix.m[10] = 1.0f;
    matrix.m[14] = 0.0f;
    matrix.m[3] = 0.0f;
    matrix.m[7] = 0.0f;
    matrix.m[11] = 0.0f;
    matrix.m[15] = 1.0f;

    return matrix;
}

void Mat4Add(Matrix4* _mat, f32 _scalar)
{
    for (int i = 0; i < 16; ++i)
        (*_mat).m[i] = (*_mat).m[i] + _scalar;
}

void Mat4Sub(Matrix4* _mat, f32 _scalar)
{
    for (int i = 0; i < 16; ++i)
        (*_mat).m[i] = (*_mat).m[i] - _scalar;
}

void Mat4Mul(Matrix4* _mat, f32 _scalar)
{
    for (int i = 0; i < 16; ++i)
        (*_mat).m[i] = (*_mat).m[i] * _scalar;
}

void Mat4Div(Matrix4* _mat, f32 _scalar)
{
    for (int i = 0; i < 16; ++i)
        (*_mat).m[i] = (*_mat).m[i] / _scalar;
}

Matrix4 Mat4MulMat4(Matrix4 _a, Matrix4 _b)
{
    Matrix4 r;
    for (int col = 0; col < 4; ++col)
    {
        // Grab B's column 'col'
        const float b0 = _b.m[col * 4 + 0];
        const float b1 = _b.m[col * 4 + 1];
        const float b2 = _b.m[col * 4 + 2];
        const float b3 = _b.m[col * 4 + 3];
        // r(:,col) = A * [b0 b1 b2 b3]
        r.m[col * 4 + 0] = _a.m[0 * 4 + 0] * b0 + _a.m[1 * 4 + 0] * b1 + _a.m[2 * 4 + 0] * b2 + _a.m[3 * 4 + 0] * b3;
        r.m[col * 4 + 1] = _a.m[0 * 4 + 1] * b0 + _a.m[1 * 4 + 1] * b1 + _a.m[2 * 4 + 1] * b2 + _a.m[3 * 4 + 1] * b3;
        r.m[col * 4 + 2] = _a.m[0 * 4 + 2] * b0 + _a.m[1 * 4 + 2] * b1 + _a.m[2 * 4 + 2] * b2 + _a.m[3 * 4 + 2] * b3;
        r.m[col * 4 + 3] = _a.m[0 * 4 + 3] * b0 + _a.m[1 * 4 + 3] * b1 + _a.m[2 * 4 + 3] * b2 + _a.m[3 * 4 + 3] * b3;
    }
    return r;
}

void Mat4Translate(Matrix4* _mat, Vector3 _translation)
{
    (*_mat).m[12] = _translation.x; // (0,3)
    (*_mat).m[13] = _translation.y; // (1,3)
    (*_mat).m[14] = _translation.z; // (2,3)
}

void Mat4Scale(Matrix4* _mat, Vector3 _scale)
{
    Matrix4 temp;
    temp = IdentityMatrix4();
    temp.m[0] *= _scale.x;   // (0,0)
    temp.m[5] *= _scale.y;   // (1,1)
    temp.m[10] *= _scale.z;  // (2,2)
    temp.m[15] = 1.0f;       // (3,3)
    *_mat = Mat4MulMat4(*_mat, temp);
}

void Mat4Rotate(Matrix4* mat, float radians, Vector3 axis)
{
    Matrix4 r = IdentityMatrix4();
    float len2 = axis.x*axis.x + axis.y*axis.y + axis.z*axis.z;
    if (len2 < 1e-20f) return;

    float invLen = 1.0f / sqrtf(len2);
    float x = axis.x*invLen, y = axis.y*invLen, z = axis.z*invLen;
    float c = cosf(radians), s = sinf(radians), t = 1.0f - c;

    r.m[0]  = t*x*x + c;     r.m[4]  = t*x*y - s*z; r.m[8]  = t*x*z + s*y;
    r.m[1]  = t*y*x + s*z;   r.m[5]  = t*y*y + c;   r.m[9]  = t*y*z - s*x;
    r.m[2]  = t*z*x - s*y;   r.m[6]  = t*z*y + s*x; r.m[10] = t*z*z + c;

    *mat = Mat4MulMat4(*mat, r); // local-space rotate (column vectors)
}

Matrix4 Mat4Orthographic(f32 _left, f32 _right, f32 _bottom, f32 _top, f32 _near, f32 _far)
{
    Matrix4 matrix;

    f32 rl = (_right - _left);
    f32 tb = (_top - _bottom);
    f32 fn = (_far - _near);

    for (int i = 0; i < 16; ++i)
        matrix.m[i] = 0.0f;

    matrix.m[0] = 2.0f / rl;   // (0,0)
    matrix.m[5] = 2.0f / tb;   // (1,1)
    matrix.m[10] = -2.0f / fn; // (2,2)
    matrix.m[15] = 1.0f;       // (3,3)

    // Translation (last column)
    matrix.m[12] = -(_right + _left) / rl; // (0,3)
    matrix.m[13] = -(_top + _bottom) / tb; // (1,3)
    matrix.m[14] = -(_far + _near) / fn;   // (2,3)

    return matrix;
}