#include "kdtreewidget.h"
#include <QMouseEvent>
#include <cmath>

KDTreeWidget::KDTreeWidget(QWidget *parent)
    : QWidget(parent), kdtree(nullptr), queryRadius(50),
    showRangeQuery(false), showNearestNeighbor(false) {
    setMinimumSize(800, 600);
    setMouseTracking(true);

    // Initialize world bounds (logical coordinates)
    worldBounds = QRectF(0, 0, 1000, 800);
    widgetBounds = QRect(50, 50, 700, 500);
}

void KDTreeWidget::setTree(const KDTree* tree) {
    kdtree = tree;
    if (kdtree) {
        allPoints = kdtree->getAllPoints();
    }
    update();
}

void KDTreeWidget::addPoint(const Point& point) {
    allPoints.push_back(point);
    update();
}

void KDTreeWidget::clearPoints() {
    allPoints.clear();
    queryResult.clear();
    nearestPoint = Point();
    showRangeQuery = false;
    showNearestNeighbor = false;
    update();
}

void KDTreeWidget::setQueryPoint(const Point& point) {
    queryPoint = point;
    update();
}

void KDTreeWidget::setQueryRadius(double radius) {
    queryRadius = radius;
    update();
}

void KDTreeWidget::performRangeQuery() {
    if (!kdtree) return;

    queryResult = kdtree->rangeQuery(queryPoint, queryRadius);
    showRangeQuery = true;
    showNearestNeighbor = false;
    emit rangeQueryPerformed(queryResult);
    update();
}

void KDTreeWidget::performNearestNeighbor() {
    if (!kdtree) return;

    nearestPoint = kdtree->nearestNeighbor(queryPoint);
    showNearestNeighbor = true;
    showRangeQuery = false;
    emit nearestNeighborFound(nearestPoint);
    update();
}

void KDTreeWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw background
    painter.fillRect(rect(), QColor(240, 240, 240));

    // Draw grid
    drawGrid(painter);

    // Draw query circle if active
    if (showRangeQuery) {
        drawQuery(painter);
    }

    // Draw splitting lines
    if (kdtree && !kdtree->isEmpty()) {
        drawSplittingLines(painter);
    }

    // Draw points
    drawPoints(painter);

    // Draw nearest neighbor if active
    if (showNearestNeighbor && nearestPoint.id != -1) {
        QPoint widgetPos = pointToWidget(nearestPoint);
        painter.setPen(QPen(Qt::green, 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(widgetPos, 12, 12);
    }

    // Draw border
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(widgetBounds);
}

void KDTreeWidget::drawGrid(QPainter& painter) {
    painter.setPen(QPen(QColor(200, 200, 200), 1));

    // Draw vertical lines
    for (int x = widgetBounds.left(); x <= widgetBounds.right(); x += 50) {
        painter.drawLine(x, widgetBounds.top(), x, widgetBounds.bottom());
    }

    // Draw horizontal lines
    for (int y = widgetBounds.top(); y <= widgetBounds.bottom(); y += 50) {
        painter.drawLine(widgetBounds.left(), y, widgetBounds.right(), y);
    }
}

void KDTreeWidget::drawPoints(QPainter& painter) {
    // Draw all points
    for (const auto& point : allPoints) {
        QPoint pos = pointToWidget(point);

        // Set color based on whether point is in query result
        bool inResult = std::find(queryResult.begin(), queryResult.end(), point)
                        != queryResult.end();

        if (inResult) {
            painter.setBrush(QBrush(QColor(255, 100, 100, 200)));
            painter.setPen(QPen(Qt::red, 2));
        } else if (point == nearestPoint) {
            painter.setBrush(QBrush(QColor(100, 255, 100, 200)));
            painter.setPen(QPen(Qt::green, 2));
        } else if (point.isHighlighted) {
            painter.setBrush(QBrush(QColor(100, 100, 255, 200)));
            painter.setPen(QPen(Qt::blue, 2));
        } else {
            painter.setBrush(QBrush(QColor(100, 100, 100, 200)));
            painter.setPen(QPen(Qt::black, 2));
        }

        painter.drawEllipse(pos, 8, 8);

        // Draw point ID
        painter.setPen(Qt::black);
        painter.drawText(pos + QPoint(10, -10), QString::number(point.id));
    }

    // Draw query point
    if (showRangeQuery || showNearestNeighbor) {
        QPoint pos = pointToWidget(queryPoint);
        painter.setBrush(QBrush(QColor(255, 165, 0, 200))); // Orange
        painter.setPen(QPen(Qt::darkRed, 2));
        painter.drawEllipse(pos, 10, 10);
        painter.drawText(pos + QPoint(15, -15), "Q");
    }
}

void KDTreeWidget::drawSplittingLines(QPainter& painter) {
    if (!kdtree) return;

    // For simplicity, we'll draw splitting lines at median positions
    // In a complete implementation, you would track the actual splitting coordinates

    std::vector<Point> points = kdtree->getAllPoints();
    if (points.empty()) return;

    painter.setPen(QPen(QColor(100, 100, 255, 150), 2, Qt::DashLine));

    // Draw vertical and horizontal lines through each point
    for (size_t i = 0; i < points.size(); i += 2) { // Sample some points
        QPoint pos = pointToWidget(points[i]);

        // Draw vertical line
        painter.drawLine(pos.x(), widgetBounds.top(),
                         pos.x(), widgetBounds.bottom());

        // Draw horizontal line
        painter.drawLine(widgetBounds.left(), pos.y(),
                         widgetBounds.right(), pos.y());
    }
}

void KDTreeWidget::drawQuery(QPainter& painter) {
    QPoint center = pointToWidget(queryPoint);
    int radius = queryRadius * (widgetBounds.width() / worldBounds.width());

    painter.setPen(QPen(Qt::blue, 2, Qt::DashLine));
    painter.setBrush(QBrush(QColor(100, 100, 255, 50)));
    painter.drawEllipse(center, radius, radius);
}

Point KDTreeWidget::widgetToPoint(const QPoint& pos) const {
    if (!widgetBounds.contains(pos)) {
        return Point();
    }

    double x = worldBounds.left() +
               (pos.x() - widgetBounds.left()) *
                   worldBounds.width() / widgetBounds.width();

    double y = worldBounds.top() +
               (pos.y() - widgetBounds.top()) *
                   worldBounds.height() / widgetBounds.height();

    return Point(x, y);
}

QPoint KDTreeWidget::pointToWidget(const Point& p) const {
    int x = widgetBounds.left() +
            (p.x - worldBounds.left()) *
                widgetBounds.width() / worldBounds.width();

    int y = widgetBounds.top() +
            (p.y - worldBounds.top()) *
                widgetBounds.height() / worldBounds.height();

    return QPoint(x, y);
}

void KDTreeWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && widgetBounds.contains(event->pos())) {
        Point newPoint = widgetToPoint(event->pos());
        newPoint.id = allPoints.size() + 1;

        emit pointAdded(newPoint);
    }
}

void KDTreeWidget::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);

    // Update widget bounds to maintain aspect ratio
    int margin = 50;
    widgetBounds = QRect(margin, margin,
                         width() - 2*margin,
                         height() - 2*margin);
}
