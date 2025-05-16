#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QTimer>
#include <QGraphicsTextItem>
#include "player.h"
#include "artifact.h"
#include "bossfight.h"
#include "startmenu.h"
#include "gameoverscreen.h"
#include "victoryscreen.h"
#include "fastbarrel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void showGameOver();
    void showVictoryScreen();

private slots:
    void startGame();
    void exitGame();
    void updateLivesDisplay(int newLives);
    void updateScoreDisplay(int newScore);
    void retryGame();
    void showShop();
    void buyInvincibility();
    void startBossFight();
    void onArtifactCollected();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QGraphicsView *view;
    QGraphicsScene *level1Scene;
    QGraphicsScene *level2Scene;
    QGraphicsScene *shopScene;
    QGraphicsScene *bossFightScene;
    QGraphicsScene *gameOverScene;    // Scene for game over screen
    QGraphicsScene *victoryScene;     // Scene for victory screen

    StartMenu *startMenu;
    GameOverScreen *gameOverScreen;
    VictoryScreen *victoryScreen;
    QPushButton *buyInvincibilityButton;

    QGraphicsTextItem *livesDisplay;
    QGraphicsTextItem *scoreDisplay;
    QGraphicsTextItem *coinsDisplay;

    Player *player;
    Artifact *artifact;
    BossFight *currentBoss;
    int coins;
    int lives;
    bool isInvincible;

    void setupLevel1Scene();
    void setupLevel2Scene();
    void setupShopScene();
    void cleanup(QGraphicsScene* scene);
    void resetGame();
};

#endif
