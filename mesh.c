#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "./draw.h"
#include "./geometry.h"

#define LINE_LENGTH 50

#define HEIGHT 30
#define WIDTH 40

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


void draw_triangle_from_mesh(Triangle t, float delta_time)
{
    Matrix4 m_z = z_rotate(delta_time * 0.5);
    Matrix4 m_x = x_rotate(delta_time * 0.25);
    Matrix4 m_z_x = multiply_matrix_matrix(&m_z, &m_x);
    // translate and project maybe dont need to be instantiated every frame?
    Matrix4 translate = translation_matrix(0.f, 0.f, 3.f);

    Matrix4 w_m = multiply_matrix_matrix(&translate,  &m_z_x);

    multiply_triangle_matrix(&w_m, &t);
    
    Matrix4 project = projection_matrix(90.f, (float) HEIGHT / (float) WIDTH, 0.1f, 1000.f);
    multiply_triangle_matrix(&project, &t);

    t.points[0].x += 5.0f; t.points[0].y += 5.0f;
	t.points[1].x += 5.0f; t.points[1].y += 5.0f;
	t.points[2].x += 5.0f; t.points[2].y += 5.0f;
	t.points[0].x *= 0.5f * (float)WIDTH;
	t.points[0].y *= 0.5f * (float)HEIGHT;
	t.points[1].x *= 0.5f * (float)WIDTH;
	t.points[1].y *= 0.5f * (float)HEIGHT;
	t.points[2].x *= 0.5f * (float)WIDTH;
	t.points[2].y *= 0.5f * (float)HEIGHT;

    //Vec2 a = {t.points[0].x, t.points[0].y};
    //Vec2 b = {t.points[1].x, t.points[1].y};
    //Vec2 c = {t.points[2].x, t.points[2].y};
    
    draw_line(t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, '*');
    draw_line(t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y, '*');
    draw_line(t.points[2].x, t.points[2].y, t.points[0].x, t.points[0].y, '*');

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
        m = load_mesh("./assets/cube.obj");
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
    // make pointer to start of mesh to reset after iterating over it
    Mesh *mesh_first_element = m;
    while (1)
    {
        clear();
        while (m->next != NULL)
        {
            end = clock();
            delta_time = ((double) (end - start)) /CLOCKS_PER_SEC;
            draw_triangle_from_mesh(m->t, delta_time);
            m = m->next;
        }
        refresh();
        m = mesh_first_element;
        usleep(500);
    }
    getch();

    endwin();
    return 0;
}
