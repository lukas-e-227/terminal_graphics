#include <ncurses.h>
#include <stdlib.h>

#include "./draw.h"


void draw_flat_triangle_bottom(Vec2 a, Vec2 b, Vec2 c, char ch)
{   
    float slope_1 = (b.x - a.x) / (b.y - a.y);
    float slope_2 = (c.x - a.x) / (c.y - a.y);

    float current_x_1 = a.x;
    float current_x_2 = a.x;

    for (int line_y = a.y; line_y <= b.y; ++line_y)
    {
        //draw_line((int) current_x_1, line_y, (int) current_x_2, line_y);
        draw_horizontal_line(current_x_1, current_x_2, line_y, ch);
        current_x_1 += slope_1;
        current_x_2 += slope_2;
    }
}

void draw_flat_triangle_top(Vec2 a, Vec2 b, Vec2 c, char ch)
{
    float slope_1 = (c.x - a.x) / (c.y - a.y);
    float slope_2 = (c.x - b.x) / (c.y - b.y);

    float current_x_1 = c.x;
    float current_x_2 = c.x;

    for (int line_y = c.y; line_y > a.y; --line_y)
    {
        //draw_line((int) current_x_1, line_y, (int) current_x_2, line_y);
        draw_horizontal_line(current_x_1, current_x_2, line_y, ch);
        current_x_1 -= slope_1;
        current_x_2 -= slope_2;
    }
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
}

/**
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
}

/**
 * draw a line between two points (x1, y1) and (x2, y2) using bresenhams algorithm
 */
void draw_line(int x1, int y1, int x2, int y2, char c)
{
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);

    int error = dx + dy;
    int e2;

    // Determine the direction of the line based on the starting and ending points
    int xinc = x1 < x2 ? 1 : -1;
    int yinc = y1 < y2 ? 1 : -1;

    while (1)
    {
        move(y1, x1);
        addch(c);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * error;
        if (e2 > dy)
        {
            error += dy; 
            x1 += xinc;
        }
        if (e2 < dx) 
        {
            error += dx;
            y1 += yinc;
        }
    }
    return;
}

/**
 * simple horizontal line is enough to fill the triangles
*/
void draw_horizontal_line(int x1, int x2, int y, char c)
{
    for (int x = x1; x <= x2; ++x)
    {
        move(y, x);
        addch(c);
    }
}