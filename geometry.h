struct matrix4 {
    float m[4][4];
};

typedef struct matrix4 Matrix4;

struct vec3 {
    float x;
    float y;
    float z;
};

typedef struct vec3 Vec3;

struct vec4 {
    float x;
    float y;
    float z;
    float w;
};

typedef struct vec4 Vec4;

struct triangle {
    Vec4 points[3];
};

typedef struct triangle Triangle;

struct mesh {
    Triangle t;
    struct mesh *next;
};

typedef struct mesh Mesh;

Matrix4 x_rotate(float a);
Matrix4 y_rotate(float a);
Matrix4 z_rotate(float a);

Matrix4 translation_matrix(float x, float y, float z);

Matrix4 projection_matrix(float fov_deg, float aspect_ratio, float near, float far);

Matrix4 scale_matrix(float scale_factor);

Matrix4 multiply_matrix_matrix(Matrix4 *m0, Matrix4 *m1);

Vec4 multiply_matrix_vector(Matrix4 *m, Vec4 v);

void multiply_triangle_matrix(Matrix4 *m, Triangle *t);

Vec4 to_vec4(Vec3 v);

Vec3 to_vec3(Vec4 v);

float dot_product(Vec3 *v0, Vec3 *v1);

Vec3 sub(Vec3 *v0, Vec3 *v1);

Vec3 get_normal(Triangle *t);

void add_to_mesh(Mesh *m, Triangle t, size_t length);

Mesh * load_mesh(char * file_name);