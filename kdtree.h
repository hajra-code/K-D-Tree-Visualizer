#ifndef KDTREE_H
#define KDTREE_H

#include "point.h"
#include <vector>
#include <memory>
#include <queue>
#include <algorithm>

class KDNode {
public:
    Point point;
    int depth;
    std::unique_ptr<KDNode> left;
    std::unique_ptr<KDNode> right;

    KDNode(Point p, int d) : point(p), depth(d) {}
};

class KDTree {
private:
    std::unique_ptr<KDNode> root;

    std::unique_ptr<KDNode> buildTree(std::vector<Point>& points, int depth);
    void rangeSearch(KDNode* node, const Point& center, double radius,
                     std::vector<Point>& result) const;
    void nearestNeighbor(KDNode* node, const Point& target,
                         KDNode*& best, double& bestDist) const;
    void inorderTraversal(KDNode* node, std::vector<Point>& points) const;

public:
    KDTree();

    void build(const std::vector<Point>& points);
    std::vector<Point> rangeQuery(const Point& center, double radius) const;
    Point nearestNeighbor(const Point& target) const;
    std::vector<Point> getAllPoints() const;
    void clear();
    bool isEmpty() const { return !root; }

    // For visualization
    std::vector<std::pair<Point, Point>> getSplittingLines() const;

private:
    void collectSplittingLines(KDNode* node,
                               std::vector<std::pair<Point, Point>>& lines) const;
};

#endif // KDTREE_H
