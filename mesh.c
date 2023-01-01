#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "./draw.h"
#include "./geometry.h"

#define LINE_LENGTH 50

/**
 * parses an obj file to return a mesh of triangles
*/
Mesh * load_mesh(char * file_name)
{
    FILE *fp;
    fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Was not able to open file\n");
        exit(1);
    }
    char line[LINE_LENGTH];
    
    // count vertices
    int vertices_count = 0;
    while (feof(fp) != true)
    {
        fgets(line, LINE_LENGTH, fp);
        if(line[0] == 'v')
            ++vertices_count;
    }

    rewind(fp);
    Mesh *m = (Mesh *) malloc(sizeof(Mesh));
    size_t length = 0;
    if (m == NULL)
    {
        fprintf(stderr, "Memory allocation for head failed");
        exit(1);
    }
    char *token;
    int i = 0;
    Vec4 vertices[vertices_count];
    while (feof(fp) != true)
    {
        fgets(line, LINE_LENGTH, fp);
        if (line[0] == 'v')
        {   
            float arr_v[3];
            int j = 0;
            token = strtok(line, " ");
            while (token != NULL)
            {
                if (token[0] != 'v') // ignore first token
                {
                    arr_v[j] = atof(token);
                    ++j;
                }
                token = strtok(NULL, " ");
            }
            Vec4 v = {arr_v[0], arr_v[1], arr_v[2], 1.f};
            vertices[i] = v;
            ++i;
        }
        else if (line[0] == 'f')
        {
            Triangle t;
            // make triangle
            int j = 0;
            token = strtok(line, " ");
            while (token != NULL)
            {
                if (token[0] != 'f')
                {
                    int index = atoi(token);
                    // subtract 1 because obj files indexes first vertex with 1
                    t.points[j] = vertices[index - 1];
                    j++;
                }
                token = strtok(NULL, " ");
            }
            // add triangle to mesh
            add_to_mesh(m, t, length);
            ++length;
        }
    }
    return m;
}

void add_to_mesh(Mesh *m, Triangle triangle, size_t length)
{
    Mesh *current = m;
    
    if (length == 0)
    {
        m->t = triangle;
    }
    else
    {
        while (current->next != NULL)
        {
            current = current->next;
        }
        Mesh *new_triangle = (Mesh *) malloc(sizeof(Mesh));
        if (new_triangle == NULL)
        {
            fprintf(stderr, "Memory allocation for new triangle failed");
            exit(1);
        }
        new_triangle->t = triangle;
        current->next = new_triangle;
    }
    return;
}

void free_mesh(Mesh *m)
{
    Mesh *current;
    while (m != NULL)
    {
        current = m;
        m = m->next;
        free(current);
    }
    return;
}

void draw_triangle_from_mesh(Triangle *t, float delta_time, int width, int height, Vec3 *light_dir_n, FILE *fp_test)
{
    Matrix4 m_x = x_rotate(M_PI);
    Matrix4 m_y = y_rotate(delta_time * 8);
    //Matrix4 m_z = z_rotate(90.f);
    
    Matrix4 m_x_y = multiply_matrix_matrix(&m_x, &m_y);
    // translate and project maybe dont need to be instantiated every frame?
    
    Matrix4 translate = translation_matrix(0.f, 0.f, 4.f);
    
    Matrix4 w_m = multiply_matrix_matrix(&translate,  &m_x_y);

    multiply_triangle_matrix(&w_m, t); 
    
    Vec3 normal = get_normal(t);

    if(normal.z < 0.f)
    {

        float light_value = dot_product(&normal, light_dir_n);
        char color = map_float_char(light_value);
        //char color = map_float_char_2(light_value);
        //fprintf(fp_test, "%f \n", light_value);
        Matrix4 project = projection_matrix(90.f, 4.f / 3.f, 0.1f, 1000.f);
        multiply_triangle_matrix(&project, t);

        Matrix4 scale = scale_matrix(16.f);
        multiply_triangle_matrix(&scale, t);
        
        // center the model 
        t->points[0].x += (float) width / 2.f; t->points[0].y += (float) height / 2.f;
        t->points[1].x += (float) width / 2.f; t->points[1].y += (float) height / 2.f;
        t->points[2].x += (float) width / 2.f; t->points[2].y += (float) height / 2.f;
        

        Vec2 _a = {round(t->points[0].x), round(t->points[0].y)};
        Vec2 _b = {round(t->points[1].x), round(t->points[1].y)};
        Vec2 _c = {round(t->points[2].x), round(t->points[2].y)};

        draw_triangle(_a, _b, _c, color);

        /*
        draw_line(t->points[0].x, t->points[0].y, t->points[1].x, t->points[1].y, '*');
        draw_line(t->points[1].x, t->points[1].y, t->points[2].x, t->points[2].y, '*');
        draw_line(t->points[2].x, t->points[2].y, t->points[0].x, t->points[0].y, '*');
        */
        //draw_line(_a.x, _a.y, _b.x, _b.y, '*');
        //draw_line(_b.x, _b.y, _c.x, _c.y, '*');
        //draw_line(_c.x, _c.y, _a.x, _a.y, '*');   
    }
    return;
}

int main(int argc, char *argv[])
{
    clock_t start, end;
    float delta_time;

    start = clock();
    
    Mesh *m;
    if (argc > 2)
    {
        fprintf(stderr, "Provide one obj file as a command line argument\n");
        exit(1);
    }
    else if (argc < 2)
    {
        m = load_mesh("./assets/snowman_2.obj");
        if (m == NULL)
        {
            fprintf(stderr, "failure while loading file");
            exit(1);
        }
    }
    else 
    {
        m = load_mesh(argv[1]);
        if (m == NULL)
        {
            fprintf(stderr, "failure while loading file");
            exit(1);
        }
    }

    initscr();
    cbreak();
    timeout(1);
    Vec3 light_dir = {0.f, 0.f, -1.f};
    Vec3 light_dir_n = normalize(&light_dir);
    int width = 0, height = 0;
    int c = 0;
    // make pointer to start of mesh to reset after iterating over it
    Mesh *mesh_first_element = m;

    FILE *fp_test;
    
    fp_test = fopen("lighting_values", "w");

    while (c != ' ')
    {
        clear();
        getmaxyx(stdscr, height, width);
        
        while (m->next != NULL)
        {
            end = clock();
            delta_time = ((double) (end - start)) /CLOCKS_PER_SEC;
            Triangle t = m->t;
            draw_triangle_from_mesh(&t, delta_time, width, height, &light_dir_n, fp_test);
            m = m->next;
        }
        refresh();
        m = mesh_first_element;
        c = getch();
        usleep(50000);
    }

    free_mesh(m);
    endwin();
    exit(0);
    return 0;
}
