#ifndef KDTREEWIDGET_H
#define KDTREEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <vector>
#include "kdtree.h"

class KDTreeWidget : public QWidget {
    Q_OBJECT

public:
    explicit KDTreeWidget(QWidget *parent = nullptr);

    void setTree(const KDTree* tree);
    void addPoint(const Point& point);
    void clearPoints();
    void setQueryPoint(const Point& point);
    void setQueryRadius(double radius);
    void performRangeQuery();
    void performNearestNeighbor();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    void pointAdded(const Point& point);
    void rangeQueryPerformed(const std::vector<Point>& points);
    void nearestNeighborFound(const Point& point);

private:
    void drawGrid(QPainter& painter);
    void drawPoints(QPainter& painter);
    void drawSplittingLines(QPainter& painter);
    void drawQuery(QPainter& painter);
    Point widgetToPoint(const QPoint& pos) const;
    QPoint pointToWidget(const Point& p) const;

    const KDTree* kdtree;
    std::vector<Point> allPoints;
    Point queryPoint;
    double queryRadius;
    std::vector<Point> queryResult;
    Point nearestPoint;
    bool showRangeQuery;
    bool showNearestNeighbor;

    QRectF worldBounds;
    QRect widgetBounds;
};

#endif // KDTREEWIDGET_H
