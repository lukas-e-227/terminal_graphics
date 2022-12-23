struct vec2 {
    float x;
    float y;
};

typedef struct vec2 Vec2;

void draw_line(int x1, int y1, int x2, int y2, char c);

void draw_horizontal_line(int x1, int x2, int y, char c);

void draw_triangle(Vec2 a, Vec2 b, Vec2 c, char ch);

