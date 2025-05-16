#include "victoryscreen.h"
#include "ui_victoryscreen.h"
#include <QGraphicsDropShadowEffect>

VictoryScreen::VictoryScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VictoryScreen)
{
    ui->setupUi(this);

    // Add glow effect to the victory text
    QGraphicsDropShadowEffect* glowEffect = new QGraphicsDropShadowEffect(this);
    glowEffect->setColor(Qt::green);
    glowEffect->setBlurRadius(20);
    glowEffect->setOffset(0);
    ui->victoryLabel->setGraphicsEffect(glowEffect);

    // Connect button signals
    connect(ui->nextLevelButton, &QPushButton::clicked, this, &VictoryScreen::nextLevelClicked);
    connect(ui->mainMenuButton, &QPushButton::clicked, this, &VictoryScreen::mainMenuClicked);
}

VictoryScreen::~VictoryScreen()
{
    delete ui;
} 