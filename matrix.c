#include <stdio.h>
#include <math.h>

#include "./geometry.h"



Matrix4 default_zero()
{
    Matrix4 m = {.m = {{0}}};
    return m;
}

Matrix4 x_rotate(float a)
{
    Matrix4 m = default_zero();
    m.m[0][0] = 1.;
    m.m[1][1] = cos(a);
    m.m[1][2] = sin(a);
    m.m[2][1] = -sin(a);
    m.m[2][2] = cos(a);
    m.m[3][3] = 1.;

    return m;
}

Matrix4 y_rotate(float a)
{
    Matrix4 m = default_zero();
    m.m[0][0] = cos(a);
    m.m[0][2] = sin(a);
	m.m[2][0] = -sin(a);
	m.m[1][1] = 1.;
	m.m[2][2] = cos(a);
	m.m[3][3] = 1.;
	
    return m;
}

Matrix4 z_rotate(float a)
{
    Matrix4 m = default_zero();
    m.m[0][0] = cos(a);
	m.m[0][1] = sin(a);
	m.m[1][0] = -sin(a);
	m.m[1][1] = cos(a);
	m.m[2][2] = 1.;
	m.m[3][3] = 1.;

	return m;
}

Matrix4 translation_matrix(float x, float y, float z)
{
    Matrix4 m = default_zero();
    m.m[0][0] = 1.;
    m.m[1][1] = 1.;
    m.m[2][2] = 1.;
    m.m[3][3] = 1.;
    m.m[0][3] = x;
    m.m[1][3] = y;
    m.m[2][3] = z;
    return m;
}

Matrix4 scale_matrix(float scale_factor)
{
    Matrix4 m = default_zero();
    m.m[0][0] = scale_factor;
    m.m[1][1] = scale_factor;
    m.m[2][2] = scale_factor;
    m.m[3][3] = 1.f;
    return m;
}

Matrix4 projection_matrix(float fov_deg, float aspect_ratio, float near, float far)
{
    Matrix4 m = default_zero();
    float fov_rad = 1.f/ tanf(fov_deg * 0.5 / 180. * M_PI);
    m.m[0][0] = aspect_ratio * fov_rad;
    m.m[1][1] = fov_rad;
    m.m[2][2] = far / (far - near);
    m.m[3][2] = (-far * near) / (far - near);
    m.m[2][3] = -1.f;
    m.m[3][3] = 0.f;
    return m;
}

Matrix4 multiply_matrix_matrix(Matrix4 *m0, Matrix4 *m1)
{
    Matrix4 m = default_zero();
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            m.m[j][i] = m0->m[j][0] * m1->m[0][i] + m0->m[j][1] * m1->m[1][i] + m0->m[j][2] * m1->m[2][i] + m0->m[j][3] * m1->m[3][i];
        }
    }
    return m;
}

Vec4 multiply_matrix_vector(Matrix4 *m, Vec4 v)
{
    Vec4 new_v = {
        v.x * m->m[0][0] + v.y * m->m[1][0] + v.z * m->m[2][0] + v.w * m->m[3][0],
        v.x * m->m[0][1] + v.y * m->m[1][1] + v.z * m->m[2][1] + v.w * m->m[3][1],
        v.x * m->m[0][2] + v.y * m->m[1][2] + v.z * m->m[2][2] + v.w * m->m[3][2],
        v.x * m->m[0][3] + v.y * m->m[1][3] + v.z * m->m[2][3] + v.w * m->m[3][3],
    };
    return new_v;
}

void multiply_triangle_matrix(Matrix4 *m, Triangle *t)
{
    t->points[0] = multiply_matrix_vector(m, t->points[0]);
    t->points[1] = multiply_matrix_vector(m, t->points[1]);
    t->points[2] = multiply_matrix_vector(m, t->points[2]);
    return;
}

Vec4 to_vec4(Vec3 v)
{
    Vec4 v_4 = {v.x, v.y, v.z, 1.f};
    return v_4;
}

Vec3 to_vec3(Vec4 v)
{
    Vec3 v_3 = {v.x, v.y, v.z};
    return v_3;
}

Vec3 cross_product(Vec3 *v0, Vec3 *v1)
{
    Vec3 c = {
        v0->y * v1->z - v0->z * v1->y,
        v0->z * v1->x - v0->x * v1->z,
        v0->x * v1->y - v0->y * v1->x,
    };
    return c;
}

float dot_product(Vec3 *v0, Vec3 *v1)
{
    float p = v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;
    return p;
}

Vec3 normalize(Vec3 *v)
{
    float length = sqrtf(dot_product(v, v));
    Vec3 n = {
        v->x / length,
        v->y / length,
        v->z / length
    };
    return n;
}

/**
 * v0 - v1
*/
Vec3 sub(Vec3 *v0, Vec3 *v1)
{
     Vec3 s = {
        v0->x - v1->x,
        v0->y - v1->y,
        v0->z - v1->z
    };
    return s;
}

Vec3 get_normal(Triangle *t)
{
    Vec3 a = to_vec3(t->points[0]);
    Vec3 b = to_vec3(t->points[1]);
    Vec3 c = to_vec3(t->points[2]);
    Vec3 ab = sub(&b, &a);
    Vec3 ac = sub(&c, &a);
    Vec3 cross = cross_product(&ab, &ac);
    Vec3 n = normalize(&cross);
    return n;
}