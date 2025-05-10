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
#include "retrybutton.h"
#include "startmenu.h"

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
    QGraphicsScene *gameOverScene;
    QGraphicsScene *victoryScene;
    QGraphicsScene *shopScene;
    QGraphicsScene *bossFightScene;

    StartMenu *startMenu;
    RetryButton *retryButton;
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
    void setupShopScene();
    void resetGame();
};

#endif
