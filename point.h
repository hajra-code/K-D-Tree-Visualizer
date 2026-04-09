#ifndef POINT_H
#define POINT_H

#include <QPointF>
#include <vector>
#include <cmath>

struct Point {
    double x, y;
    int id;
    bool isHighlighted;

    Point(double x = 0, double y = 0, int id = -1)
        : x(x), y(y), id(id), isHighlighted(false) {}

    double distance(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx*dx + dy*dy);
    }

    bool operator==(const Point& other) const {
        return std::abs(x - other.x) < 1e-9 &&
               std::abs(y - other.y) < 1e-9;
    }

    QPointF toQPointF() const {
        return QPointF(x, y);
    }
};

#endif // POINT_H
