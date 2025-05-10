#include "gameview.h"
#include <QGraphicsRectItem>

GameView::GameView(QWidget *parent)
    : QGraphicsView(parent), m_currentLevel(0)
{
    // Create the scene
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 780, 580);
    setScene(m_scene);

    // Set black background
    setBackgroundBrush(Qt::black);

    // Set up the view
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(800, 600);

    // Create return button
    m_returnButton = new QPushButton("Return to Menu", this);
    m_returnButton->setGeometry(10, 10, 150, 30);
    m_returnButton->setStyleSheet("background-color: #FF5555; color: white;");
    connect(m_returnButton, &QPushButton::clicked, this, &GameView::returnToMenuClicked);
}

GameView::~GameView()
{
    clearLevel();
    delete m_scene;
}

void GameView::startLevel(int level)
{
    m_currentLevel = level;
    clearLevel();
    createLevel(level);
}

void GameView::createLevel(int level)
{
    // Create a simple level with platforms and ladders

    // Create platforms (red)
    // Bottom platform
    Platform *bottomPlatform = new Platform(0, 550, 780, 30);
    m_scene->addItem(bottomPlatform);
    m_platforms.append(bottomPlatform);

    // Middle platforms
    Platform *platform1 = new Platform(100, 450, 680, 20);
    m_scene->addItem(platform1);
    m_platforms.append(platform1);

    Platform *platform2 = new Platform(0, 350, 680, 20);
    m_scene->addItem(platform2);
    m_platforms.append(platform2);

    Platform *platform3 = new Platform(100, 250, 680, 20);
    m_scene->addItem(platform3);
    m_platforms.append(platform3);

    Platform *platform4 = new Platform(0, 150, 680, 20);
    m_scene->addItem(platform4);
    m_platforms.append(platform4);

    // Create ladders (yellow)
    Ladder *ladder1 = new Ladder(200, 450, 30, 100);
    m_scene->addItem(ladder1);
    m_ladders.append(ladder1);

    Ladder *ladder2 = new Ladder(550, 350, 30, 100);
    m_scene->addItem(ladder2);
    m_ladders.append(ladder2);

    Ladder *ladder3 = new Ladder(200, 250, 30, 100);
    m_scene->addItem(ladder3);
    m_ladders.append(ladder3);

    Ladder *ladder4 = new Ladder(550, 150, 30, 100);
    m_scene->addItem(ladder4);
    m_ladders.append(ladder4);

    // Add a level indicator text
    QGraphicsTextItem *levelText = new QGraphicsTextItem(QString("Level %1").arg(level));
    levelText->setDefaultTextColor(Qt::white);
    levelText->setFont(QFont("Arial", 20));
    levelText->setPos(350, 10);
    m_scene->addItem(levelText);
}

void GameView::clearLevel()
{
    // Clear all platforms
    for (auto platform : m_platforms) {
        m_scene->removeItem(platform);
        delete platform;
    }
    m_platforms.clear();

    // Clear all ladders
    for (auto ladder : m_ladders) {
        m_scene->removeItem(ladder);
        delete ladder;
    }
    m_ladders.clear();

    // Clear all other items
    QList<QGraphicsItem*> itemsToRemove;
    for (auto item : m_scene->items()) {
        if (item->type() != QGraphicsTextItem::Type) {  // Keep text items
            itemsToRemove.append(item);
        }
    }

    for (auto item : itemsToRemove) {
        m_scene->removeItem(item);
        delete item;
    }
}
