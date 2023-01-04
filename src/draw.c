#include <stdlib.h>
#include <math.h>

#include "./draw.h"
#define WIDTH 916
#define HEIGHT 148
               
static const char CHARS[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
static const char CHARS_2[] = {' ','.',':','-','=','+','*','#','%','@'};
char CANVAS[WIDTH * HEIGHT];

char map_float_char(float light_value)
{
    int len = sizeof(CHARS) / sizeof(CHARS[0]);
    int index = (int)(len - fabs(light_value) * len);
    char c = CHARS[index];
    return c;
}

char map_float_char_2(float light_value)
{
    int len = sizeof(CHARS_2) / sizeof(CHARS_2[0]);
    int index = (int) (fabs(light_value) * len);
    char c = CHARS_2[index];
    return c;
}

void draw_flat_triangle_bottom(Vec2 a, Vec2 b, Vec2 c, char ch)
{   
    float slope_1 = (b.x - a.x) / (b.y - a.y);
    float slope_2 = (c.x - a.x) / (c.y - a.y);

    float current_x_1 = a.x;
    float current_x_2 = a.x;

    for (int line_y = a.y; line_y <= b.y; ++line_y)
    { 
        draw_horizontal_line((int) current_x_1, (int) current_x_2, line_y, ch);
        current_x_1 += slope_1;
        current_x_2 += slope_2;
    }
    return;
}

void draw_flat_triangle_top(Vec2 a, Vec2 b, Vec2 c, char ch)
{
    float slope_1 = (c.x - a.x) / (c.y - a.y);
    float slope_2 = (c.x - b.x) / (c.y - b.y);

    float current_x_1 = c.x;
    float current_x_2 = c.x;

    for (int line_y = c.y; line_y > a.y; --line_y)
    {
        draw_horizontal_line((int) current_x_1, (int) current_x_2, line_y, ch);
        current_x_1 -= slope_1;
        current_x_2 -= slope_2;
    }
    return;
}

void sort_vertices_by_y(Vec2 *a, Vec2 *b, Vec2 *c)
{
    if (a->y > c->y)
    {
        Vec2 tmp = *a;
        *a = *c;
        *c = tmp;
    }
    if (a->y > b->y)
    {
        Vec2 tmp = *a;
        *a = *b;
        *b = tmp;
    }
    if (b->y > c->y)
    {
        Vec2 tmp = *b;
        *b = *c;
        *c = tmp;
    }
    return;
}

/**
 * source for the rasterization algorithm: http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
 * Vec2 a, b, c are the vertices of the triangle, which is filled with character ch
*/
void draw_triangle(Vec2 a, Vec2 b, Vec2 c, char ch)
{
    sort_vertices_by_y(&a, &b, &c);
    if (b.y == c.y)
    {
        draw_flat_triangle_bottom(a, b, c, ch);
    }
    else if (a.y == b.y)
    {
        draw_flat_triangle_top(a, b, c, ch);
    }
    else 
    {
        // split in two
        Vec2 d = {(int) (a.x + ((float) (b.y - a.y) / (float) (c.y - a.y)) * (c.x - a.x)), b.y};
        draw_flat_triangle_bottom(a, b, d, ch);
        draw_flat_triangle_top(b, d, c, ch);
    }
    return;
}

/**
 * simple horizontal line is enough to fill the triangles
*/
void draw_horizontal_line(int x1, int x2, int y, char c)
{
    int start = x1 < x2 ? x1 : x2;
    int end = x1 > x2 ? x1 : x2;

    for (int x = start; x <= end; ++x)
    {
        //move(y, x);
        //addch(c);
        CANVAS[x + WIDTH * y] = c;
    }
    return;
}

void clear_screen()
{
    memset(CANVAS, ' ', WIDTH * HEIGHT);
    printf("\033[2J");
}

void print_screen()
{
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            char c = CANVAS[x + WIDTH * y];
            printf("%c", c);
        }
        printf("\n");
    }
}