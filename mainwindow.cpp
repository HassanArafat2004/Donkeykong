#include "mainwindow.h"
#include "platform.h"
#include "ladder.h"
#include "boss.h"
#include "barrel.h"
#include "player.h"
#include "artifact.h"
#include "retrybutton.h"
#include "startmenu.h"
#include "gameoverscreen.h"
#include "victoryscreen.h"
#include "homingprojectile.h"

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
    : QMainWindow(parent), view(new QGraphicsView(this)), level1Scene(nullptr),
    shopScene(new QGraphicsScene(this)), bossFightScene(new QGraphicsScene(this)),
    gameOverScene(new QGraphicsScene(this)), victoryScene(new QGraphicsScene(this)),
    startMenu(new StartMenu), gameOverScreen(new GameOverScreen),
    victoryScreen(new VictoryScreen), player(nullptr), artifact(nullptr),
    currentBoss(nullptr), coins(0), lives(3), isInvincible(false)
{
    setCentralWidget(view);
    setFixedSize(1000, 900);

    // Set up scenes
    shopScene->setSceneRect(0, 0, 1000, 900);
    bossFightScene->setSceneRect(0, 0, 1000, 900);
    gameOverScene->setSceneRect(0, 0, 1000, 900);
    victoryScene->setSceneRect(0, 0, 1000, 900);

    // Add UI screens to their scenes
    gameOverScene->addWidget(gameOverScreen);
    victoryScene->addWidget(victoryScreen);

    // Set initial scene
    view->setScene(startMenu->scene());
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Initialize displays
    livesDisplay = nullptr;
    scoreDisplay = nullptr;
    coinsDisplay = nullptr;

    coins = 0;

    // Connect StartMenu signals to our slots
    connect(startMenu, &StartMenu::startGameClicked, this, &MainWindow::startGame);
    connect(startMenu, &StartMenu::shopClicked, this, &MainWindow::showShop);
    connect(startMenu, &StartMenu::exitClicked, this, &MainWindow::exitGame);

    // Connect GameOverScreen signals
    connect(gameOverScreen, &GameOverScreen::retryClicked, this, &MainWindow::retryGame);
    connect(gameOverScreen, &GameOverScreen::mainMenuClicked, this, &MainWindow::resetGame);

    // Connect VictoryScreen signals
    connect(victoryScreen, &VictoryScreen::nextLevelClicked, this, &MainWindow::startBossFight);
    connect(victoryScreen, &VictoryScreen::mainMenuClicked, this, &MainWindow::resetGame);

    // Set up shop buttons
    buyInvincibilityButton = new QPushButton("Buy Invincibility (50 coins)");
    buyInvincibilityButton->setGeometry(400, 300, 200, 50);
    shopScene->addWidget(buyInvincibilityButton);

    QPushButton *backButton = new QPushButton("Back to Menu");
    backButton->setGeometry(400, 400, 200, 50);
    shopScene->addWidget(backButton);

    connect(buyInvincibilityButton, &QPushButton::clicked, this, &MainWindow::buyInvincibility);
    connect(backButton, &QPushButton::clicked, this, [this]() { view->setScene(startMenu->scene()); });
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
                    delete barrel;
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

    // Show game over screen
    view->setScene(gameOverScene);

    delete player;
    player = nullptr;
    delete artifact;
    artifact = nullptr;
}

void MainWindow::showVictoryScreen() {
    // Store current level for the victory screen to know what to do next
    bool isLevel1 = (view->scene() == level1Scene);
    
    // Clean up current scene
    if (level1Scene) {
        cleanup(level1Scene);
    }
    if (level2Scene) {
        cleanup(level2Scene);
    }
    
    // Set up victory screen with appropriate button functionality
    connect(victoryScreen, &VictoryScreen::nextLevelClicked, this, [this, isLevel1]() {
        if (isLevel1) {
            // If coming from Level 1, proceed to Level 2
            setupLevel2Scene();
            view->setScene(level2Scene);
        } else {
            // If coming from Level 2, maybe show final victory or return to menu
            resetGame();
        }
    });
    
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

    // Start new game
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
    // Stop all barrels and timers in current scene
    QGraphicsScene *currentScene = view->scene();
    
    if (currentScene == level1Scene) {
        QTimer *barrelTimer1 = level1Scene->property("barrelTimer1").value<QTimer*>();
        QTimer *barrelTimer2 = level1Scene->property("barrelTimer2").value<QTimer*>();
        
        if (barrelTimer1) barrelTimer1->stop();
        if (barrelTimer2) barrelTimer2->stop();

        // Remove all existing barrels
        QList<QGraphicsItem *> items = currentScene->items();
        for (QGraphicsItem *item : items) {
            if (Barrel *barrel = dynamic_cast<Barrel *>(item)) {
                currentScene->removeItem(barrel);
                delete barrel;
            }
        }

        // Start boss fight for Level 1
        startBossFight();
        
    } else if (currentScene == level2Scene) {
        QTimer *regularBarrelTimer = level2Scene->property("regularBarrelTimer").value<QTimer*>();
        QTimer *fastBarrelTimer = level2Scene->property("fastBarrelTimer").value<QTimer*>();
        QTimer *projectileTimer = level2Scene->property("projectileTimer").value<QTimer*>();
        
        if (regularBarrelTimer) regularBarrelTimer->stop();
        if (fastBarrelTimer) fastBarrelTimer->stop();
        if (projectileTimer) projectileTimer->stop();

        // Remove all existing barrels and projectiles
        QList<QGraphicsItem *> items = currentScene->items();
        for (QGraphicsItem *item : items) {
            if (dynamic_cast<Barrel *>(item) || dynamic_cast<HomingProjectile *>(item)) {
                currentScene->removeItem(item);
                delete item;
            }
        }

        // For Level 2, show victory screen directly (or implement Level 2 boss fight if desired)
        showVictoryScreen();
    }
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
    
    // Connect signals
    connect(currentBoss, &BossFight::gameOver, this, &MainWindow::showGameOver);
    connect(currentBoss, &BossFight::victory, this, &MainWindow::showVictoryScreen);
    connect(currentBoss, &BossFight::returnToMainMenu, this, &MainWindow::resetGame);
    
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
    if (currentBoss) {
        currentBoss->handleKeyPress(static_cast<Qt::Key>(event->key()));
        event->accept();
        return;
    }
    
    // Handle other key events for the main game
    QMainWindow::keyPressEvent(event);
}

void MainWindow::setupLevel2Scene() {
    level2Scene = new QGraphicsScene(this);
    level2Scene->setSceneRect(0, 0, 1000, 900);
    
    // Set background
    QPixmap backgroundImage(":/images/back.png");
    if (backgroundImage.isNull()) {
        QLinearGradient gradient(0, 0, 0, 900);
        gradient.setColorAt(0, QColor(25, 25, 112));
        gradient.setColorAt(1, QColor(47, 79, 79));
        level2Scene->setBackgroundBrush(gradient);
    } else {
        backgroundImage = backgroundImage.scaled(1000, 900, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        level2Scene->setBackgroundBrush(QBrush(backgroundImage));
    }

    // Level 2 text
    QGraphicsTextItem *levelText = new QGraphicsTextItem("LEVEL 2");
    levelText->setDefaultTextColor(Qt::white);
    levelText->setFont(QFont("Arial", 24, QFont::Bold));
    levelText->setPos((1000 - levelText->boundingRect().width()) / 2, 20);
    level2Scene->addItem(levelText);

    // Display elements
    livesDisplay = new QGraphicsTextItem("Lives: " + QString::number(player->getLives()));
    livesDisplay->setDefaultTextColor(Qt::white);
    livesDisplay->setFont(QFont("Arial", 14));
    livesDisplay->setPos(10, 10);
    level2Scene->addItem(livesDisplay);

    scoreDisplay = new QGraphicsTextItem("Score: " + QString::number(player->getScore()));
    scoreDisplay->setDefaultTextColor(Qt::white);
    scoreDisplay->setFont(QFont("Arial", 14));
    scoreDisplay->setPos(880, 10);
    level2Scene->addItem(scoreDisplay);

    coinsDisplay = new QGraphicsTextItem(QString("Coins: %1").arg(coins));
    coinsDisplay->setDefaultTextColor(Qt::white);
    coinsDisplay->setFont(QFont("Arial", 14));
    coinsDisplay->setPos(450, 10);
    level2Scene->addItem(coinsDisplay);

    // Different platform layout for Level 2 - Classic Donkey Kong style
    int platformHeight = 20;
    
    // Ground platform - Full width
    level2Scene->addItem(new Platform(0, 800, 1000, platformHeight));
    
    // Bottom platform - Full width
    level2Scene->addItem(new Platform(0, 675, 900, platformHeight));
    
    // Second platform - Extended to the right
    level2Scene->addItem(new Platform(200, 550, 800, platformHeight));
    
    // Third platform - Extended on both sides (full width)
    level2Scene->addItem(new Platform(0, 425, 800, platformHeight));
    
    // Fourth platform - Extended to the right
    level2Scene->addItem(new Platform(200, 300, 800, platformHeight));
    
    // Top platform - Where boss stands
    level2Scene->addItem(new Platform(350, 175, 300, platformHeight));

    // Ladders positioned for strategic climbing
    // Bottom section ladders
    level2Scene->addItem(new Ladder(150, 695, 40, 105));   // Left
    level2Scene->addItem(new Ladder(850, 695, 40, 105));   // Right

    // Second section ladders
    level2Scene->addItem(new Ladder(500, 570, 40, 105));   // Middle
    level2Scene->addItem(new Ladder(900, 570, 40, 105));   // Right

    // Third section ladders
    level2Scene->addItem(new Ladder(150, 445, 40, 105));   // Left
    level2Scene->addItem(new Ladder(600, 445, 40, 105));   // Right

    // Fourth section ladders
    level2Scene->addItem(new Ladder(300, 320, 40, 105));   // Left
    level2Scene->addItem(new Ladder(850, 320, 40, 105));   // Right

    // Final approach ladder
    level2Scene->addItem(new Ladder(450, 195, 40, 105));   // Center to top platform

    // Player setup at bottom left
    player->setPos(50, 775);
    level2Scene->addItem(player);
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    player->setFocus();

    // Artifact placement - moved to a more challenging position
    artifact = new Artifact();
    artifact->setPos(750, 270);  // Right side of fourth platform
    level2Scene->addItem(artifact);
    connect(artifact, &Artifact::artifactCollected, this, &MainWindow::onArtifactCollected);

    // Boss setup - centered on top platform
    Boss *boss = new Boss();
    boss->setPos(350, 100);  // Centered on top platform
    level2Scene->addItem(boss);

    // Alternating barrel types with different timing
    QTimer *regularBarrelTimer = new QTimer(this);
    connect(regularBarrelTimer, &QTimer::timeout, this, [=]() {
        if (player && !player->isGameOver() && level2Scene) {
            Barrel *barrel = new Barrel();
            barrel->setPos(boss->x() + 20, boss->y() + boss->boundingRect().height());
            level2Scene->addItem(barrel);
        }
    });
    regularBarrelTimer->start(4000);  // Slower regular barrels

    QTimer *fastBarrelTimer = new QTimer(this);
    connect(fastBarrelTimer, &QTimer::timeout, this, [=]() {
        if (player && !player->isGameOver() && level2Scene) {
            FastBarrel *fastBarrel = new FastBarrel();
            fastBarrel->setPos(boss->x() + 30, boss->y() + boss->boundingRect().height());
            level2Scene->addItem(fastBarrel);
        }
    });
    fastBarrelTimer->start(6000);  // Fast barrels less frequent

    level2Scene->setProperty("regularBarrelTimer", QVariant::fromValue(regularBarrelTimer));
    level2Scene->setProperty("fastBarrelTimer", QVariant::fromValue(fastBarrelTimer));

    // Set up projectile timer
    QTimer *projectileTimer = new QTimer(this);
    connect(projectileTimer, &QTimer::timeout, this, [=]() {
        if (player && !player->isGameOver() && level2Scene) {
            // Create a homing projectile targeting the player's current position
            HomingProjectile *projectile = new HomingProjectile(player->pos());
            projectile->setPos(boss->x() + boss->boundingRect().width()/2, 
                             boss->y() + boss->boundingRect().height()/2);
            level2Scene->addItem(projectile);
        }
    });
    projectileTimer->start(3000);  // Spawn projectile every 3 seconds

    level2Scene->setProperty("projectileTimer", QVariant::fromValue(projectileTimer));

    // Clean up on player death
    connect(player, &Player::playerDied, this, [=]() {
        if (regularBarrelTimer) regularBarrelTimer->stop();
        if (fastBarrelTimer) fastBarrelTimer->stop();
        if (projectileTimer) projectileTimer->stop();
        
        if (level2Scene) {
            QList<QGraphicsItem *> items = level2Scene->items();
            for (QGraphicsItem *item : items) {
                if (dynamic_cast<Barrel *>(item) || dynamic_cast<HomingProjectile *>(item)) {
                    level2Scene->removeItem(item);
                    delete item;
                }
            }
        }
    });
}

void MainWindow::cleanup(QGraphicsScene* scene) {
    if (!scene) return;
    
    // Stop all timers
    QTimer *timer1 = scene->property("barrelTimer1").value<QTimer*>();
    QTimer *timer2 = scene->property("barrelTimer2").value<QTimer*>();
    QTimer *regularTimer = scene->property("regularBarrelTimer").value<QTimer*>();
    QTimer *fastTimer = scene->property("fastBarrelTimer").value<QTimer*>();
    QTimer *projectileTimer = scene->property("projectileTimer").value<QTimer*>();
    
    if (timer1) timer1->stop();
    if (timer2) timer2->stop();
    if (regularTimer) regularTimer->stop();
    if (fastTimer) fastTimer->stop();
    if (projectileTimer) projectileTimer->stop();
    
    // Remove and delete all items
    QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem* item : items) {
        scene->removeItem(item);
        delete item;
    }
}

