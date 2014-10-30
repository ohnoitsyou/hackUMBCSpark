#ifndef __COLOR__
#define __COLOR__

class Color {
    public:
    Color(int r, int g, int b):
    r(r), g(g), b(b){};
    Color():
    r(0),g(0),b(0){};
    int getR() { return r; }
    int getG() { return g; }
    int getB() { return b; }
    private:
    int r, g, b;
};

#endif
