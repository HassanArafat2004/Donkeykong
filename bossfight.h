#ifndef BOSSFIGHT_H
#define BOSSFIGHT_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QTimer>
#include <QList>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QGraphicsPixmapItem>
#include <QMovie>
#include <QPushButton>
#include <QGraphicsProxyWidget>

class BossFight : public QObject
{
    Q_OBJECT

public:
    explicit BossFight(QGraphicsScene *scene, int level, QObject *parent = nullptr);
    ~BossFight();

    void start();
    void handleKeyPress(Qt::Key key);
    void cleanup();

    enum class BossState {
        INTRO,
        SEQUENCE_DISPLAY,
        PLAYER_INPUT,
        TRANSITION,
        VICTORY,
        DEFEAT
    };

signals:
    void gameOver();
    void victory();
    void healthChanged(int newHealth);
    void sequenceStarted();
    void sequenceCompleted(bool success);
    void returnToMainMenu();

protected:
    virtual void generateSequence();
    virtual void onCorrectSequence();
    virtual void onWrongButton();
    virtual int getSequenceLength() const;
    virtual int getPlayerDamage() const;
    virtual int getBossDamage() const;

private slots:
    void startNextSequence();
    void playIntroAnimation();
    void playAttackAnimation(bool isPlayerAttacking);
    void showVictoryScreen();
    void showGameOverScreen();

private:
    QGraphicsScene *scene;
    int level;
    int bossHealth;
    int playerHealth;
    int currentSequenceIndex;
    BossState currentState;
    QList<Qt::Key> buttonSequence;
    bool isTransitioning;
    
    // Character sprites
    QGraphicsPixmapItem *playerSprite;
    QGraphicsPixmapItem *bossSprite;
    QMovie *whipAttackAnimation;
    QGraphicsPixmapItem *whipAttackSprite;
    
    // UI Elements
    QGraphicsTextItem *bossHealthDisplay;
    QGraphicsTextItem *playerHealthDisplay;
    QGraphicsTextItem *sequenceDisplay;
    QGraphicsTextItem *vsText;
    QList<QGraphicsRectItem*> bossHealthBars;
    QList<QGraphicsRectItem*> playerHealthBars;
    QList<QGraphicsTextItem*> sequenceIndicators;
    QGraphicsRectItem *backgroundOverlay;
    QGraphicsPixmapItem *background;
    
    QParallelAnimationGroup *currentAnimation;
    
    void updateHealthDisplays();
    void updateSequenceIndicators(int index);
    void clearSequenceIndicators();
    QString getButtonSymbol(Qt::Key key) const;
    void setupUI();
    void transitionToState(BossState newState);
    void createVisualFeedback(const QString &text, const QColor &color);
    bool isValidInput(Qt::Key key) const;
    void checkGameState();
    void setupCharacters();
    void createHealthBar(QGraphicsRectItem* &container, QList<QGraphicsRectItem*> &bars, 
                        const QPointF &pos, const QColor &color, bool isPlayer);
    void createStylizedButton(QPushButton* button, const QString& text, 
                            const QPoint& pos, const QSize& size);
};

class Level1Boss : public BossFight
{
    Q_OBJECT
public:
    explicit Level1Boss(QGraphicsScene *scene, QObject *parent = nullptr);

protected:
    int getSequenceLength() const override { return 4; }
    int getPlayerDamage() const override { return 20; }
    int getBossDamage() const override { return 25; }
};

#endif 