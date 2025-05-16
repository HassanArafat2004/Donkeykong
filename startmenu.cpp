#include "startmenu.h"
#include "ui_startmenu.h"
#include <QVBoxLayout>
#include <QGraphicsScene>

StartMenu::StartMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StartMenu)
    , menuScene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    
    // Set up the scene
    menuScene->setSceneRect(0, 0, 1000, 900);
    
    // Set up the widget
    setFixedSize(1000, 900);
    setStyleSheet("QWidget { background-color: #1a1a1a; }");
    
    // Create a container for centering
    QWidget* container = new QWidget;
    container->setFixedSize(1000, 900);
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->addWidget(this, 0, Qt::AlignCenter);
    
    // Add the container to the scene
    menuScene->addWidget(container);
    
    // Connect button signals to our signals
    connect(ui->startButton, &QPushButton::clicked, this, &StartMenu::startGameClicked);
    connect(ui->shopButton, &QPushButton::clicked, this, &StartMenu::shopClicked);
    connect(ui->exitButton, &QPushButton::clicked, this, &StartMenu::exitClicked);
}

StartMenu::~StartMenu()
{
    // Disconnect all signals first
    disconnect(ui->startButton, nullptr, this, nullptr);
    disconnect(ui->shopButton, nullptr, this, nullptr);
    disconnect(ui->exitButton, nullptr, this, nullptr);

    // Remove this widget from the scene if it's still in one
    if (scene()) {
        scene()->removeItem(scene()->items().first());
    }

    // Delete the scene if we own it
    if (menuScene) {
        menuScene->clear();  // Clear all items from the scene
        delete menuScene;
        menuScene = nullptr;
    }

    // Delete UI last, and only if it exists
    if (ui) {
        delete ui;
        ui = nullptr;
    }
} 