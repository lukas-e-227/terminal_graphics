typedef struct vec2 {
    float x;
    float y;
} Vec2;

char map_float_char(float light_value);

char map_float_char_2(float light_value);

void draw_line(int x1, int y1, int x2, int y2, char c);

void draw_horizontal_line(int x1, int x2, int y, char c);

void draw_triangle(Vec2 a, Vec2 b, Vec2 c, char ch);

void draw_triangle_b(Vec2 *a, Vec2 *b, Vec2 *c, int width, int height, char color);