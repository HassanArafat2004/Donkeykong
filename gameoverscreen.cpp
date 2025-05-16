#include "gameoverscreen.h"
#include "ui_gameoverscreen.h"
#include <QGraphicsDropShadowEffect>

GameOverScreen::GameOverScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameOverScreen)
{
    ui->setupUi(this);

    // Add glow effect to the game over text
    QGraphicsDropShadowEffect* glowEffect = new QGraphicsDropShadowEffect(this);
    glowEffect->setColor(Qt::red);
    glowEffect->setBlurRadius(20);
    glowEffect->setOffset(0);
    ui->gameOverLabel->setGraphicsEffect(glowEffect);

    // Connect button signals
    connect(ui->retryButton, &QPushButton::clicked, this, &GameOverScreen::retryClicked);
    connect(ui->mainMenuButton, &QPushButton::clicked, this, &GameOverScreen::mainMenuClicked);
}

GameOverScreen::~GameOverScreen()
{
    delete ui;
} 
