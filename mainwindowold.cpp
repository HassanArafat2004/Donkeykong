#include "mainwindow.h"
#include "platform.h"
#include "ladder.h"
#include "boss.h"
#include "barrel.h"

#include <QGraphicsTextItem>
#include <QTimer>
#include <QTime>
#include <QBrush>
#include <QPen>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    view = new QGraphicsView(this);
    menuScene = new QGraphicsScene(this);

    view->setScene(menuScene);
    view->setSceneRect(0, 0, 800, 600); // Fixed scene size
    view->setFixedSize(800, 600);       // Fixed window size
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCentralWidget(view);

    menuScene->setSceneRect(0, 0, 800, 600);

    startButton = new QPushButton("Start Game");
    startButton->setGeometry(300, 200, 200, 50);
    menuScene->addWidget(startButton);

    exitButton = new QPushButton("Exit");
    exitButton->setGeometry(300, 300, 200, 50);
    menuScene->addWidget(exitButton);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::startGame);
    connect(exitButton, &QPushButton::clicked, this, &MainWindow::exitGame);
}

MainWindow::~MainWindow() {}

void MainWindow::startGame() {
    setupLevel1Scene();
    view->setScene(level1Scene);  // Switch to Level 1 scene
}

void MainWindow::exitGame() {
    close();
}

void MainWindow::setupLevel1Scene() {
    level1Scene = new QGraphicsScene(this);
    level1Scene->setSceneRect(0, 0, 800, 600);
    level1Scene->setBackgroundBrush(QBrush(Qt::black));

    // --- Level title ---
    QGraphicsTextItem *levelText = new QGraphicsTextItem("LEVEL 1");
    levelText->setDefaultTextColor(Qt::white);
    levelText->setFont(QFont("Arial", 24, QFont::Bold));
    levelText->setPos(320, 20);
    level1Scene->addItem(levelText);

    // --- Create Platforms ---
    int platformHeight = 20;

    Platform *groundPlatform = new Platform(50, 520, 700, platformHeight);
    level1Scene->addItem(groundPlatform);

    Platform *platform2 = new Platform(150, 420, 600, platformHeight);
    level1Scene->addItem(platform2);

    Platform *platform3 = new Platform(50, 320, 600, platformHeight);
    level1Scene->addItem(platform3);

    Platform *platform4 = new Platform(150, 220, 600, platformHeight);
    level1Scene->addItem(platform4);

    Platform *platform5 = new Platform(250, 120, 300, platformHeight);
    level1Scene->addItem(platform5);

    // --- Create Ladders ---
    Ladder *ladder1 = new Ladder(170, 440, 20, 80); // 2nd to ground
    level1Scene->addItem(ladder1);

    Ladder *ladder2 = new Ladder(600, 340, 20, 80); // 3rd to 2nd
    level1Scene->addItem(ladder2);

    Ladder *ladder3 = new Ladder(170, 240, 20, 80); // 4th to 3rd
    level1Scene->addItem(ladder3);

    Ladder *ladder4 = new Ladder(400, 140, 20, 80); // top to 4th
    level1Scene->addItem(ladder4);

    // --- Create Boss (top character) ---
    Boss *boss = new Boss();
    boss->setPos(360, 50); // Near the top
    level1Scene->addItem(boss);

    // --- Create Barrel Spawner ---
    QTimer *barrelTimer = new QTimer(this);
    connect(barrelTimer, &QTimer::timeout, this, [=]() {
        Barrel *barrel = new Barrel();
        barrel->setPos(boss->x() + 10, boss->y() + 80); // Spawn below Boss
        level1Scene->addItem(barrel);
    });
    barrelTimer->start(3000); // Spawn barrel every 3 seconds
}


//platform.cpp
#include "ladder.h"
#include <QBrush>
#include <QPen>


Ladder::Ladder(qreal x, qreal y, qreal width, qreal height) {
    setRect(0, 0, width, height);
    setPos(x, y);
    setBrush(QBrush(Qt::cyan));
    setPen(QPen(Qt::white, 1));
}


//platform.h
