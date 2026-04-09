#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "kdtreewidget.h"
#include "kdtree.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddRandomPoints();
    void onClearAll();
    void onBuildTree();
    void onRangeSearch();
    void onNearestNeighbor();
    void onPointAdded(const Point& point);
    void onRangeQueryPerformed(const std::vector<Point>& points);
    void onNearestNeighborFound(const Point& point);

private:
    void setupUI();
    void updateTreeInfo();

    KDTreeWidget* treeWidget;
    KDTree kdtree;

    // Widgets
    QPushButton* btnAddRandom;
    QPushButton* btnClear;
    QPushButton* btnBuild;
    QPushButton* btnRangeSearch;
    QPushButton* btnNearestNeighbor;

    QSpinBox* spinNumPoints;
    QDoubleSpinBox* spinRadius;

    QTextEdit* textInfo;

    std::vector<Point> allPoints;
    Point currentQueryPoint;
};

#endif // MAINWINDOW_H
