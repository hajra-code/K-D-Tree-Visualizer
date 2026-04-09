#include "mainwindow.h"
#include <QApplication>
#include <QRandomGenerator>
#include <QTime>
#include <sstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setupUI();
    setWindowTitle("KD-Tree Visualizer");
    resize(1200, 800);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    // Create central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // Left panel for visualization
    treeWidget = new KDTreeWidget(this);
    mainLayout->addWidget(treeWidget, 3);

    // Right panel for controls
    QWidget* controlPanel = new QWidget(this);
    controlPanel->setFixedWidth(300);
    QVBoxLayout* controlLayout = new QVBoxLayout(controlPanel);

    // Information group
    QGroupBox* infoGroup = new QGroupBox("KD-Tree Information", this);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup);

    textInfo = new QTextEdit(this);
    textInfo->setReadOnly(true);
    textInfo->setMaximumHeight(200);
    infoLayout->addWidget(textInfo);

    // Controls group
    QGroupBox* controlGroup = new QGroupBox("Controls", this);
    QVBoxLayout* controlGroupLayout = new QVBoxLayout(controlGroup);

    // Add random points
    QHBoxLayout* randomLayout = new QHBoxLayout();
    randomLayout->addWidget(new QLabel("Points:", this));
    spinNumPoints = new QSpinBox(this);
    spinNumPoints->setRange(1, 100);
    spinNumPoints->setValue(10);
    randomLayout->addWidget(spinNumPoints);

    btnAddRandom = new QPushButton("Add Random", this);
    randomLayout->addWidget(btnAddRandom);
    controlGroupLayout->addLayout(randomLayout);

    // Clear button
    btnClear = new QPushButton("Clear All", this);
    controlGroupLayout->addWidget(btnClear);

    // Build tree button
    btnBuild = new QPushButton("Build KD-Tree", this);
    controlGroupLayout->addWidget(btnBuild);

    // Range search
    QHBoxLayout* rangeLayout = new QHBoxLayout();
    rangeLayout->addWidget(new QLabel("Radius:", this));
    spinRadius = new QDoubleSpinBox(this);
    spinRadius->setRange(10, 500);
    spinRadius->setValue(100);
    spinRadius->setSingleStep(10);
    rangeLayout->addWidget(spinRadius);
    controlGroupLayout->addLayout(rangeLayout);

    btnRangeSearch = new QPushButton("Range Search", this);
    controlGroupLayout->addWidget(btnRangeSearch);

    // Nearest neighbor
    btnNearestNeighbor = new QPushButton("Nearest Neighbor", this);
    controlGroupLayout->addWidget(btnNearestNeighbor);

    controlGroupLayout->addStretch();

    // Add groups to control layout
    controlLayout->addWidget(infoGroup);
    controlLayout->addWidget(controlGroup);
    controlLayout->addStretch();

    // Add control panel to main layout
    mainLayout->addWidget(controlPanel);

    // Connect signals and slots
    connect(btnAddRandom, &QPushButton::clicked,
            this, &MainWindow::onAddRandomPoints);
    connect(btnClear, &QPushButton::clicked,
            this, &MainWindow::onClearAll);
    connect(btnBuild, &QPushButton::clicked,
            this, &MainWindow::onBuildTree);
    connect(btnRangeSearch, &QPushButton::clicked,
            this, &MainWindow::onRangeSearch);
    connect(btnNearestNeighbor, &QPushButton::clicked,
            this, &MainWindow::onNearestNeighbor);

    connect(treeWidget, &KDTreeWidget::pointAdded,
            this, &MainWindow::onPointAdded);
    connect(treeWidget, &KDTreeWidget::rangeQueryPerformed,
            this, &MainWindow::onRangeQueryPerformed);
    connect(treeWidget, &KDTreeWidget::nearestNeighborFound,
            this, &MainWindow::onNearestNeighborFound);

    // Initialize info
    updateTreeInfo();
}

void MainWindow::onAddRandomPoints() {
    int numPoints = spinNumPoints->value();
    QRandomGenerator* rand = QRandomGenerator::global();

    for (int i = 0; i < numPoints; ++i) {
        double x = 100.0 + rand->generateDouble() * 800.0;  // 100.0 to 900.0
        double y = 100.0 + rand->generateDouble() * 600.0;  // 100.0 to 700.0
        Point p(x, y, allPoints.size() + 1);

        allPoints.push_back(p);
        treeWidget->addPoint(p);
    }

    updateTreeInfo();
}

void MainWindow::onClearAll() {
    allPoints.clear();
    kdtree.clear();
    treeWidget->clearPoints();
    updateTreeInfo();
}

void MainWindow::onBuildTree() {
    kdtree.build(allPoints);
    treeWidget->setTree(&kdtree);
    updateTreeInfo();
}

void MainWindow::onRangeSearch() {
    if (allPoints.empty()) return;

    // Use last clicked point or random point
    if (currentQueryPoint.id == -1) {
        QRandomGenerator* rand = QRandomGenerator::global();
        int idx = rand->bounded(allPoints.size());
        currentQueryPoint = allPoints[idx];
    }

    treeWidget->setQueryPoint(currentQueryPoint);
    treeWidget->setQueryRadius(spinRadius->value());
    treeWidget->performRangeQuery();
}

void MainWindow::onNearestNeighbor() {
    if (allPoints.empty()) return;

    if (currentQueryPoint.id == -1) {
        QRandomGenerator* rand = QRandomGenerator::global();
        int idx = rand->bounded(allPoints.size());
        currentQueryPoint = allPoints[idx];
    }

    treeWidget->setQueryPoint(currentQueryPoint);
    treeWidget->performNearestNeighbor();
}

void MainWindow::onPointAdded(const Point& point) {
    allPoints.push_back(point);
    currentQueryPoint = point;
    updateTreeInfo();
}

void MainWindow::onRangeQueryPerformed(const std::vector<Point>& points) {
    std::stringstream ss;
    ss << "Range Query Results:\n";
    ss << "Query Point: (" << currentQueryPoint.x << ", "
       << currentQueryPoint.y << ")\n";
    ss << "Radius: " << spinRadius->value() << "\n";
    ss << "Points found: " << points.size() << "\n";

    for (size_t i = 0; i < points.size(); ++i) {
        ss << i+1 << ". Point " << points[i].id
           << ": (" << points[i].x << ", " << points[i].y << ")\n";
    }

    textInfo->append(QString::fromStdString(ss.str()));
}

void MainWindow::onNearestNeighborFound(const Point& point) {
    std::stringstream ss;
    ss << "Nearest Neighbor:\n";
    ss << "Query Point: (" << currentQueryPoint.x << ", "
       << currentQueryPoint.y << ")\n";

    if (point.id != -1) {
        ss << "Nearest: Point " << point.id
           << " at (" << point.x << ", " << point.y << ")\n";
        ss << "Distance: " << point.distance(currentQueryPoint) << "\n";
    } else {
        ss << "No nearest neighbor found!\n";
    }

    textInfo->append(QString::fromStdString(ss.str()));
}

void MainWindow::updateTreeInfo() {
    std::stringstream ss;
    ss << "KD-Tree Status:\n";
    ss << "Total Points: " << allPoints.size() << "\n";
    ss << "Tree Built: " << (kdtree.isEmpty() ? "No" : "Yes") << "\n";

    if (!allPoints.empty()) {
        ss << "\nPoints List:\n";
        for (size_t i = 0; i < allPoints.size(); ++i) {
            ss << i+1 << ". Point " << allPoints[i].id
               << ": (" << allPoints[i].x << ", " << allPoints[i].y << ")\n";
        }
    }

    textInfo->setText(QString::fromStdString(ss.str()));
}
