#include "Color.cpp"
class Color {
    public:
    Color(int r, int g, int b):
    r(r), g(g), b(b){};
    int getR() { return r; }
    int getG() { return g; }
    int getB() { return b; }
    private:
    int r, g, b;
};
