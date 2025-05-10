#include "mainwindow.h"
#include "platform.h"
#include "ladder.h"
#include "boss.h"
#include "barrel.h"
#include "player.h"
#include "artifact.h"
#include "retrybutton.h"
#include "startmenu.h"

#include <QGraphicsTextItem>
#include <QTimer>
#include <QTime>
#include <QBrush>
#include <QPen>
#include <QDebug>
// #include <QRandomGenerator>
#include <QKeyEvent>
#include <QPixmap>
#include <QLinearGradient>
#include <QColor>
#include <QVariant>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    view = new QGraphicsView(this);
    startMenu = new StartMenu(this);
    gameOverScene = new QGraphicsScene(this);
    victoryScene = new QGraphicsScene(this);
    shopScene = new QGraphicsScene(this);
    bossFightScene = new QGraphicsScene(this);
    bossFightScene->setSceneRect(0, 0, 1000, 900);

    // Set up view properties
    view->setSceneRect(0, 0, 1000, 900);
    view->setFixedSize(1000, 900);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setRenderHint(QPainter::Antialiasing);
    view->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    
    // Allow mouse clicks but prevent view movement
    view->setDragMode(QGraphicsView::NoDrag);
    view->setTransformationAnchor(QGraphicsView::NoAnchor);
    view->setResizeAnchor(QGraphicsView::NoAnchor);
    view->setAlignment(Qt::AlignCenter);
    
    setCentralWidget(view);
    
    // Set window properties to prevent resizing
    setFixedSize(1000, 900);
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);

    // Set the initial scene to the start menu
    view->setScene(startMenu->scene());

    gameOverScene->setSceneRect(0, 0, 1000, 900);
    victoryScene->setSceneRect(0, 0, 1000, 900);
    shopScene->setSceneRect(0, 0, 1000, 900);
    bossFightScene->setSceneRect(0, 0, 1000, 900);

    coins = 0;

    // Connect StartMenu signals to our slots
    connect(startMenu, &StartMenu::startGameClicked, this, &MainWindow::startGame);
    connect(startMenu, &StartMenu::shopClicked, this, &MainWindow::showShop);
    connect(startMenu, &StartMenu::exitClicked, this, &MainWindow::exitGame);

    retryButton = new RetryButton(gameOverScene);
    connect(retryButton, &RetryButton::clicked, this, &MainWindow::retryGame);

    buyInvincibilityButton = new QPushButton("Buy Invincibility (50 coins)");
    buyInvincibilityButton->setGeometry(400, 300, 200, 50);
    shopScene->addWidget(buyInvincibilityButton);

    QPushButton *backButton = new QPushButton("Back to Menu");
    backButton->setGeometry(400, 400, 200, 50);
    shopScene->addWidget(backButton);

    connect(buyInvincibilityButton, &QPushButton::clicked, this, &MainWindow::buyInvincibility);
    connect(backButton, &QPushButton::clicked, this, [this]() { view->setScene(startMenu->scene()); });

    currentBoss = nullptr;
}

MainWindow::~MainWindow() {}

void MainWindow::startGame() {
    setupLevel1Scene();
    view->setScene(level1Scene);
}

void MainWindow::exitGame() {
    close();
}

void MainWindow::setupLevel1Scene() {
    level1Scene = new QGraphicsScene(this);
    level1Scene->setSceneRect(0, 0, 1000, 900);
    
    QPixmap backgroundImage(":/images/back.png");
    if (backgroundImage.isNull()) {
        QLinearGradient gradient(0, 0, 0, 900);
        gradient.setColorAt(0, QColor(25, 25, 112));
        gradient.setColorAt(1, QColor(47, 79, 79));
        level1Scene->setBackgroundBrush(gradient);
    } else {
        backgroundImage = backgroundImage.scaled(1000, 900, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        level1Scene->setBackgroundBrush(QBrush(backgroundImage));
    }

    QGraphicsTextItem *levelText = new QGraphicsTextItem("LEVEL 1");
    levelText->setDefaultTextColor(Qt::white);
    levelText->setFont(QFont("Arial", 24, QFont::Bold));
    levelText->setPos((1000 - levelText->boundingRect().width()) / 2, 20);
    level1Scene->addItem(levelText);

    livesDisplay = new QGraphicsTextItem("Lives: 3");
    livesDisplay->setDefaultTextColor(Qt::white);
    livesDisplay->setFont(QFont("Arial", 14));
    livesDisplay->setPos(10, 10);
    level1Scene->addItem(livesDisplay);

    scoreDisplay = new QGraphicsTextItem("Score: 0");
    scoreDisplay->setDefaultTextColor(Qt::white);
    scoreDisplay->setFont(QFont("Arial", 14));
    scoreDisplay->setPos(880, 10);
    level1Scene->addItem(scoreDisplay);

    coinsDisplay = new QGraphicsTextItem(QString("Coins: %1").arg(coins));
    coinsDisplay->setDefaultTextColor(Qt::white);
    coinsDisplay->setFont(QFont("Arial", 14));
    coinsDisplay->setPos(450, 10);
    level1Scene->addItem(coinsDisplay);

    int platformHeight = 20;

    level1Scene->addItem(new Platform(200, 100, 600, platformHeight));
    level1Scene->addItem(new Platform(100, 200, 600, platformHeight));
    level1Scene->addItem(new Platform(250, 300, 600, platformHeight));
    level1Scene->addItem(new Platform(100, 400, 600, platformHeight));
    level1Scene->addItem(new Platform(250, 500, 600, platformHeight));
    level1Scene->addItem(new Platform(0, 650, 1000, platformHeight));

    level1Scene->addItem(new Ladder(280, 120, 40, 80));
    level1Scene->addItem(new Ladder(650, 220, 40, 80));
    level1Scene->addItem(new Ladder(300, 320, 40, 80));
    level1Scene->addItem(new Ladder(670, 420, 40, 80));
    level1Scene->addItem(new Ladder(300, 520, 40, 130));

    player = new Player();
    player->setPos(50, 625);
    level1Scene->addItem(player);
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    player->setFocus();

    connect(player, &Player::livesChanged, this, &MainWindow::updateLivesDisplay);
    connect(player, &Player::scoreChanged, this, &MainWindow::updateScoreDisplay);
    connect(player, &Player::playerDied, this, &MainWindow::showGameOver);

    artifact = new Artifact();
    artifact->setPos(150, 170);
    level1Scene->addItem(artifact);
    connect(artifact, &Artifact::artifactCollected, this, &MainWindow::onArtifactCollected);

    Boss *boss = new Boss();
    boss->setPos(300, 20);
    level1Scene->addItem(boss);

    QTimer *barrelTimer1 = new QTimer(this);
    connect(barrelTimer1, &QTimer::timeout, this, [=]() {
        if (player && !player->isGameOver() && level1Scene) {
            Barrel *barrel = new Barrel();
            barrel->setPos(boss->x() + 20, boss->y() + boss->boundingRect().height());
            level1Scene->addItem(barrel);
        }
    });
    barrelTimer1->start(2500);

    QTimer *barrelTimer2 = new QTimer(this);
    connect(barrelTimer2, &QTimer::timeout, this, [=]() {
        if (player && !player->isGameOver() && level1Scene) {
            Barrel *barrel = new Barrel();
            barrel->setPos(boss->x() + 30, boss->y() + boss->boundingRect().height());
            level1Scene->addItem(barrel);
        }
    });
    barrelTimer2->start(5000);

    level1Scene->setProperty("barrelTimer1", QVariant::fromValue(barrelTimer1));
    level1Scene->setProperty("barrelTimer2", QVariant::fromValue(barrelTimer2));

    connect(player, &Player::playerDied, this, [=]() {
        if (barrelTimer1) barrelTimer1->stop();
        if (barrelTimer2) barrelTimer2->stop();
        
        if (level1Scene) {
            QList<QGraphicsItem *> items = level1Scene->items();
            for (QGraphicsItem *item : items) {
                if (Barrel *barrel = dynamic_cast<Barrel *>(item)) {
                    level1Scene->removeItem(barrel);
                    barrel->deleteLater();
                }
            }
        }
    });
}

void MainWindow::updateLivesDisplay(int newLives) {
    if (livesDisplay) {
        livesDisplay->setPlainText(QString("Lives: %1").arg(newLives));
    }
}

void MainWindow::updateScoreDisplay(int newScore) {
    if (scoreDisplay) {
        scoreDisplay->setPlainText(QString("Score: %1").arg(newScore));
    }
}

void MainWindow::showGameOver() {
    // Clean up boss fight scene if active
    if (currentBoss) {
        currentBoss->cleanup();
        delete currentBoss;
        currentBoss = nullptr;
    }

    if (level1Scene) {
        QTimer *timer1 = level1Scene->property("barrelTimer1").value<QTimer*>();
        QTimer *timer2 = level1Scene->property("barrelTimer2").value<QTimer*>();
        if (timer1) timer1->stop();
        if (timer2) timer2->stop();
    }

    int finalScore = 0;
    if (player) {
        finalScore = player->getScore();
    }

    if (level1Scene) {
        if (player) {
            level1Scene->removeItem(player);
        }
        if (artifact) {
            level1Scene->removeItem(artifact);
        }

        QList<QGraphicsItem*> items = level1Scene->items();
        for (QGraphicsItem* item : items) {
            level1Scene->removeItem(item);
            delete item;
        }
        
        delete level1Scene;
        level1Scene = nullptr;
    }

    gameOverScene->clear();
    
    QGraphicsTextItem *gameOverText = new QGraphicsTextItem("GAME OVER");
    gameOverText->setFont(QFont("Arial", 32, QFont::Bold));
    gameOverText->setDefaultTextColor(Qt::red);
    gameOverText->setPos((1000 - gameOverText->boundingRect().width()) / 2, 300);
    gameOverScene->addItem(gameOverText);

    QGraphicsTextItem *scoreText = new QGraphicsTextItem(QString("Final Score: %1").arg(finalScore));
    scoreText->setFont(QFont("Arial", 24));
    scoreText->setDefaultTextColor(Qt::white);
    scoreText->setPos((1000 - scoreText->boundingRect().width()) / 2, 350);
    gameOverScene->addItem(scoreText);

    view->setScene(gameOverScene);

    retryButton->showInScene();

    delete player;
    player = nullptr;
    delete artifact;
    artifact = nullptr;
}

void MainWindow::showVictoryScreen() {
    victoryScene->clear();
    QGraphicsTextItem *winText = new QGraphicsTextItem("YOU WIN!");
    winText->setFont(QFont("Arial", 32, QFont::Bold));
    winText->setDefaultTextColor(Qt::green);
    winText->setPos((1000 - winText->boundingRect().width()) / 2, 400);
    victoryScene->addItem(winText);
    view->setScene(victoryScene);
}

void MainWindow::resetGame() {
    view->setScene(startMenu->scene());
}

void MainWindow::retryGame() {
    // Clean up boss fight scene if active
    if (currentBoss) {
        currentBoss->cleanup();
        delete currentBoss;
        currentBoss = nullptr;
    }

    if (level1Scene) {
        QTimer *timer1 = level1Scene->property("barrelTimer1").value<QTimer*>();
        QTimer *timer2 = level1Scene->property("barrelTimer2").value<QTimer*>();
        if (timer1) timer1->stop();
        if (timer2) timer2->stop();

        QList<QGraphicsItem*> items = level1Scene->items();
        for (QGraphicsItem* item : items) {
            level1Scene->removeItem(item);
            delete item;
        }
        delete level1Scene;
        level1Scene = nullptr;
    }

    // Reset game state
    if (player) {
        delete player;
        player = nullptr;
    }
    if (artifact) {
        delete artifact;
        artifact = nullptr;
    }

    // Hide retry button
    if (retryButton) {
        retryButton->removeFromScene();
    }

    // Reset scene
    setupLevel1Scene();
    view->setScene(level1Scene);
}

void MainWindow::setupShopScene() {
    shopScene->clear();
    shopScene->setBackgroundBrush(QBrush(Qt::black));
    QGraphicsTextItem *shopTitle = new QGraphicsTextItem("SHOP");
    shopTitle->setDefaultTextColor(Qt::white);
    shopTitle->setFont(QFont("Arial", 32, QFont::Bold));
    shopTitle->setPos((1000 - shopTitle->boundingRect().width()) / 2, 150);
    shopScene->addItem(shopTitle);

    coinsDisplay = new QGraphicsTextItem(QString("Coins: %1").arg(coins));
    coinsDisplay->setDefaultTextColor(Qt::white);
    coinsDisplay->setFont(QFont("Arial", 14));
    coinsDisplay->setPos(10, 10);
    shopScene->addItem(coinsDisplay);

    buyInvincibilityButton->setGeometry(400, 300, 200, 50);
    shopScene->addWidget(buyInvincibilityButton);
}

void MainWindow::showShop() {
    setupShopScene();
    view->setScene(shopScene);
}

void MainWindow::buyInvincibility() {
    if (coins >= 50) {
        coins -= 50;
        coinsDisplay->setPlainText(QString("Coins: %1").arg(coins));
        player->activateInvincibility();
    }
}

void MainWindow::onArtifactCollected() {
    // Stop all barrels and timers
    QTimer *barrelTimer1 = level1Scene->property("barrelTimer1").value<QTimer*>();
    QTimer *barrelTimer2 = level1Scene->property("barrelTimer2").value<QTimer*>();
    
    if (barrelTimer1) barrelTimer1->stop();
    if (barrelTimer2) barrelTimer2->stop();
    
    // Remove all existing barrels
    QList<QGraphicsItem *> items = level1Scene->items();
    for (QGraphicsItem *item : items) {
        if (Barrel *barrel = dynamic_cast<Barrel *>(item)) {
            level1Scene->removeItem(barrel);
            delete barrel;
        }
    }
    
    // Save player's current state
    int currentLives = player->getLives();
    int currentScore = player->getScore();
    
    // Create transition effect
    QGraphicsRectItem *overlay = new QGraphicsRectItem(0, 0, level1Scene->width(), level1Scene->height());
    overlay->setBrush(QBrush(QColor(0, 0, 0)));
    overlay->setPen(Qt::NoPen);
    
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0.0);
    overlay->setGraphicsEffect(opacityEffect);
    
    level1Scene->addItem(overlay);
    
    QPropertyAnimation *fadeIn = new QPropertyAnimation(opacityEffect, "opacity");
    fadeIn->setDuration(1000);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    
    connect(fadeIn, &QPropertyAnimation::finished, this, [this, currentLives, currentScore, overlay]() {
        if (overlay) {
            if (overlay->graphicsEffect()) {
                delete overlay->graphicsEffect();
            }
            level1Scene->removeItem(overlay);
            delete overlay;
        }
        startBossFight();
        if (currentBoss) {
            connect(currentBoss, &BossFight::gameOver, this, &MainWindow::showGameOver);
            connect(currentBoss, &BossFight::victory, this, &MainWindow::showVictoryScreen);
            
            // Update UI elements for boss fight
            if (livesDisplay) {
                livesDisplay->setPlainText(QString("Lives: %1").arg(currentLives));
            }
            if (scoreDisplay) {
                scoreDisplay->setPlainText(QString("Score: %1").arg(currentScore));
            }
        }
    });
    
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::startBossFight() {
    if (!bossFightScene) {
        bossFightScene = new QGraphicsScene(this);
        bossFightScene->setSceneRect(0, 0, 1000, 900);
    }
    
    // Set up background for boss fight
    QLinearGradient gradient(0, 0, 0, 900);
    gradient.setColorAt(0, QColor(20, 20, 50));
    gradient.setColorAt(1, QColor(50, 20, 20));
    bossFightScene->setBackgroundBrush(gradient);
    
    // Create the boss fight instance
    currentBoss = new Level1Boss(bossFightScene, this);
    
    // Switch to boss fight scene
    view->setScene(bossFightScene);
    
    // Ensure the main window has focus and can receive key events
    this->setFocus();
    this->activateWindow();
    
    // Start the boss fight with a slight delay
    QTimer::singleShot(500, [this]() {
        if (currentBoss) {
            currentBoss->start();
        }
    });
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (currentBoss && view->scene() == bossFightScene) {
        // Debug output to check key events
        qDebug() << "Key pressed:" << event->key();
        currentBoss->handleKeyPress(static_cast<Qt::Key>(event->key()));
        event->accept();  // Mark the event as handled
        return;  // Don't pass to parent
    }
    QMainWindow::keyPressEvent(event);
}

