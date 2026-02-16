#pragma once
namespace nyaa {

struct Point
{
    double x, y;

    Point operator+(const Point& b) const { return { x + b.x, y + b.y }; }
    Point operator-(const Point& b) const { return { x - b.x, y - b.y }; }
    Point operator-() const { return { -x, -y }; }
    Point operator*(double b) const { return { x * b, y * b }; }
    Point operator/(double b) const { return { x / b, y / b }; }
};

}