#include "startmenu.h"
#include "ui_startmenu.h"

StartMenu::StartMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StartMenu)
    , menuScene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    menuScene->setSceneRect(0, 0, 1000, 900);
    
    // Create a container widget for the menu
    QWidget *menuContainer = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(menuContainer);
    layout->addWidget(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Add the container to the scene
    menuScene->addWidget(menuContainer);
    
    // Set the menu container to fill the scene
    menuContainer->setGeometry(0, 0, 1000, 900);
    
    // Connect button signals to our signals
    connect(ui->startButton, &QPushButton::clicked, this, &StartMenu::startGameClicked);
    connect(ui->shopButton, &QPushButton::clicked, this, &StartMenu::shopClicked);
    connect(ui->exitButton, &QPushButton::clicked, this, &StartMenu::exitClicked);
}

StartMenu::~StartMenu()
{
    delete ui;
} 