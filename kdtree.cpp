#include "kdtree.h"
#include <algorithm>
#include <cmath>

KDTree::KDTree() : root(nullptr) {}

void KDTree::build(const std::vector<Point>& points) {
    if (points.empty()) {
        root = nullptr;
        return;
    }

    std::vector<Point> tempPoints = points;
    root = buildTree(tempPoints, 0);
}

std::unique_ptr<KDNode> KDTree::buildTree(std::vector<Point>& points, int depth) {
    if (points.empty()) {
        return nullptr;
    }

    // Alternate between x and y dimensions (0 for x, 1 for y)
    int axis = depth % 2;

    // Sort points based on current axis
    if (axis == 0) { // x-axis
        std::sort(points.begin(), points.end(),
                  [](const Point& a, const Point& b) { return a.x < b.x; });
    } else { // y-axis
        std::sort(points.begin(), points.end(),
                  [](const Point& a, const Point& b) { return a.y < b.y; });
    }

    // Median index
    size_t medianIndex = points.size() / 2;

    // Create node
    auto node = std::make_unique<KDNode>(points[medianIndex], depth);

    // Split points
    std::vector<Point> leftPoints(points.begin(), points.begin() + medianIndex);
    std::vector<Point> rightPoints(points.begin() + medianIndex + 1, points.end());

    // Recursively build subtrees
    node->left = buildTree(leftPoints, depth + 1);
    node->right = buildTree(rightPoints, depth + 1);

    return node;
}

std::vector<Point> KDTree::rangeQuery(const Point& center, double radius) const {
    std::vector<Point> result;
    if (root) {
        rangeSearch(root.get(), center, radius, result);
    }
    return result;
}

void KDTree::rangeSearch(KDNode* node, const Point& center, double radius,
                         std::vector<Point>& result) const {
    if (!node) return;

    // Check if current point is within range
    if (node->point.distance(center) <= radius) {
        result.push_back(node->point);
    }

    int axis = node->depth % 2;
    double nodeValue = (axis == 0) ? node->point.x : node->point.y;
    double targetValue = (axis == 0) ? center.x : center.y;

    // Decide which side(s) to search
    if (targetValue - radius <= nodeValue) {
        rangeSearch(node->left.get(), center, radius, result);
    }
    if (targetValue + radius >= nodeValue) {
        rangeSearch(node->right.get(), center, radius, result);
    }
}

Point KDTree::nearestNeighbor(const Point& target) const {
    if (!root) {
        return Point(); // Return empty point if tree is empty
    }

    KDNode* best = nullptr;
    double bestDist = std::numeric_limits<double>::max();
    nearestNeighbor(root.get(), target, best, bestDist);

    return best ? best->point : Point();
}

void KDTree::nearestNeighbor(KDNode* node, const Point& target,
                             KDNode*& best, double& bestDist) const {
    if (!node) return;

    // Calculate distance to current node
    double dist = node->point.distance(target);
    if (dist < bestDist) {
        bestDist = dist;
        best = node;
    }

    int axis = node->depth % 2;
    double nodeValue = (axis == 0) ? node->point.x : node->point.y;
    double targetValue = (axis == 0) ? target.x : target.y;

    // Decide which side to search first
    KDNode* first = nullptr;
    KDNode* second = nullptr;

    if (targetValue < nodeValue) {
        first = node->left.get();
        second = node->right.get();
    } else {
        first = node->right.get();
        second = node->left.get();
    }

    // Search first side
    nearestNeighbor(first, target, best, bestDist);

    // Check if we need to search second side
    if (std::abs(targetValue - nodeValue) < bestDist) {
        nearestNeighbor(second, target, best, bestDist);
    }
}

std::vector<Point> KDTree::getAllPoints() const {
    std::vector<Point> points;
    inorderTraversal(root.get(), points);
    return points;
}

void KDTree::inorderTraversal(KDNode* node, std::vector<Point>& points) const {
    if (!node) return;

    inorderTraversal(node->left.get(), points);
    points.push_back(node->point);
    inorderTraversal(node->right.get(), points);
}

void KDTree::clear() {
    root = nullptr;
}

std::vector<std::pair<Point, Point>> KDTree::getSplittingLines() const {
    std::vector<std::pair<Point, Point>> lines;
    if (root) {
        collectSplittingLines(root.get(), lines);
    }
    return lines;
}

void KDTree::collectSplittingLines(KDNode* node,
                                   std::vector<std::pair<Point, Point>>& lines) const {
    if (!node) return;

    int axis = node->depth % 2;
    double coord = (axis == 0) ? node->point.x : node->point.y;

    // For visualization, we'll draw splitting lines through the node
    // The exact coordinates would depend on the bounding box
    // We'll update this in the widget based on actual coordinates

    // Collect from children
    if (node->left) {
        collectSplittingLines(node->left.get(), lines);
    }
    if (node->right) {
        collectSplittingLines(node->right.get(), lines);
    }
}
