#include "bossfight.h"
#include <QRandomGenerator>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QEasingCurve>
#include <QScrollBar>
#include <QTimeLine>
#include <QGraphicsItemAnimation>
#include <QGraphicsView>
#include <QMovie>
#include <QPointer>
#include <QApplication>

BossFight::BossFight(QGraphicsScene *scene, int level, QObject *parent)
    : QObject(parent), scene(scene), level(level), currentState(BossState::INTRO),
    isTransitioning(false), currentAnimation(nullptr), playerSprite(nullptr), bossSprite(nullptr),
    whipAttackAnimation(nullptr), whipAttackSprite(nullptr), bossHealthDisplay(nullptr),
    playerHealthDisplay(nullptr), sequenceDisplay(nullptr), vsText(nullptr),
    backgroundOverlay(nullptr), background(nullptr)
{
    bossHealth = 100;
    playerHealth = 100;
    
    // Initialize whip attack animation
    whipAttackAnimation = new QMovie(":/images/whipattack.gif");
    whipAttackAnimation->setScaledSize(QSize(100, 100));
    whipAttackSprite = new QGraphicsPixmapItem();
    whipAttackSprite->setVisible(false);
    scene->addItem(whipAttackSprite);
    
    connect(whipAttackAnimation, &QMovie::frameChanged, this, [this]() {
        if (whipAttackSprite) {
            whipAttackSprite->setPixmap(whipAttackAnimation->currentPixmap());
        }
    });
}

BossFight::~BossFight()
{
    cleanup();
    delete whipAttackAnimation;
    delete whipAttackSprite;
}

void BossFight::generateSequence()
{
    buttonSequence.clear();
    currentSequenceIndex = 0;
    clearSequenceIndicators();

    int sequenceLength = getSequenceLength();
    QList<Qt::Key> possibleKeys = {Qt::Key_Left, Qt::Key_Right, Qt::Key_Up, Qt::Key_Down, Qt::Key_Space};
    
    // Create sequence container with improved visuals and positioning
    qreal centerX = scene->width() / 2;
    qreal startX = centerX - (sequenceLength * 80) / 2;
    qreal containerY = 280;
    
    QGraphicsRectItem *sequenceContainer = new QGraphicsRectItem();
    sequenceContainer->setRect(startX - 20, containerY, sequenceLength * 80 + 40, 90);
    
    // Add gradient background
    QLinearGradient gradient(0, 0, 0, 90);
    gradient.setColorAt(0, QColor(40, 40, 40, 200));
    gradient.setColorAt(1, QColor(20, 20, 20, 200));
    sequenceContainer->setBrush(gradient);
    
    // Add subtle border
    sequenceContainer->setPen(QPen(QColor(100, 100, 100, 150), 2));
    scene->addItem(sequenceContainer);

    // Generate and display sequence with improved button visuals
    for (int i = 0; i < sequenceLength; ++i) {
        buttonSequence.append(possibleKeys[QRandomGenerator::global()->bounded(possibleKeys.size())]);
        
        // Create button background with improved visuals
        QPainterPath path;
        path.addRoundedRect(QRectF(0, 0, 70, 70), 10, 10);
        QGraphicsPathItem *bgRect = new QGraphicsPathItem(path);
        
        // Add gradient to button
        QLinearGradient buttonGradient(0, 0, 0, 70);
        buttonGradient.setColorAt(0, QColor(70, 70, 70));
        buttonGradient.setColorAt(1, QColor(50, 50, 50));
        bgRect->setBrush(buttonGradient);
        
        bgRect->setPen(QPen(QColor(100, 100, 100), 2));
        bgRect->setPos(startX + i * 80, containerY + 10);
        scene->addItem(bgRect);
        
        // Create button text with improved visuals
        QGraphicsTextItem *indicator = new QGraphicsTextItem();
        indicator->setDefaultTextColor(Qt::lightGray);
        indicator->setFont(QFont("Arial", 32, QFont::Bold));
        indicator->setPlainText(getButtonSymbol(buttonSequence.last()));
        
        qreal textX = startX + i * 80 + (70 - indicator->boundingRect().width()) / 2;
        qreal textY = containerY + 10 + (70 - indicator->boundingRect().height()) / 2;
        indicator->setPos(textX, textY);
        
        // Add glow effect
        QGraphicsDropShadowEffect *buttonGlow = new QGraphicsDropShadowEffect();
        buttonGlow->setColor(Qt::white);
        buttonGlow->setBlurRadius(10);
        buttonGlow->setOffset(0);
        indicator->setGraphicsEffect(buttonGlow);
        
        scene->addItem(indicator);
        sequenceIndicators.append(indicator);
    }

    // Update UI with better positioning
    if (sequenceDisplay) {
        sequenceDisplay->setPlainText("MATCH THE SEQUENCE!");
        sequenceDisplay->setDefaultTextColor(QColor(255, 255, 100));
        sequenceDisplay->setFont(QFont("Arial", 36, QFont::Bold));
        sequenceDisplay->setPos((scene->width() - sequenceDisplay->boundingRect().width()) / 2, 220);
    }
    
    // Add a timer to transition to player input state
    QTimer::singleShot(2000, this, [this]() {
        transitionToState(BossState::PLAYER_INPUT);
    });
}

void BossFight::onCorrectSequence()
{
    bossHealth -= getBossDamage();
    updateHealthDisplays();
    createVisualFeedback("Hit!", Qt::green);
    
    // Play whip attack animation
    if (whipAttackSprite && whipAttackAnimation) {
        whipAttackSprite->setPos(playerSprite->pos().x() + 50, playerSprite->pos().y());
        whipAttackSprite->setVisible(true);
        whipAttackAnimation->start();
        
        // Hide the whip attack after animation completes
        QTimer::singleShot(whipAttackAnimation->frameCount() * 50, this, [this]() {
            if (whipAttackSprite) {
                whipAttackSprite->setVisible(false);
                whipAttackAnimation->stop();
            }
        });
    }
    
    playAttackAnimation(true);
    
    if (bossHealth <= 0) {
        transitionToState(BossState::VICTORY);
    } else {
        generateSequence();
    }
}

void BossFight::onWrongButton()
{
    if (isTransitioning) return;

    if (currentSequenceIndex < sequenceIndicators.size()) {
        auto currentIndicator = sequenceIndicators[currentSequenceIndex];
        if (currentIndicator) {
            currentIndicator->setDefaultTextColor(Qt::red);
            currentIndicator->setFont(QFont("Arial", 32, QFont::Bold));
            
            QSequentialAnimationGroup *shakeGroup = new QSequentialAnimationGroup(this);
            QPropertyAnimation *shakeAnim = new QPropertyAnimation(currentIndicator, "pos");
            shakeAnim->setDuration(100);
            
            QPointF originalPos = currentIndicator->pos();
            for (int i = 0; i < 6; i++) {
                shakeAnim->setKeyValueAt(i/6.0, originalPos + QPointF((i % 2) * 10 - 5, 0));
            }
            shakeAnim->setKeyValueAt(1, originalPos);
            
            shakeGroup->addAnimation(shakeAnim);
            shakeGroup->start(QAbstractAnimation::DeleteWhenStopped);
            
            // Store a weak pointer to the indicator
            QPointer<QGraphicsTextItem> weakIndicator = currentIndicator;
            QTimer::singleShot(300, this, [this, weakIndicator]() {
                if (!weakIndicator.isNull() && weakIndicator->scene()) {
                    weakIndicator->setDefaultTextColor(Qt::yellow);
                    weakIndicator->setFont(QFont("Arial", 28, QFont::Bold));
                }
            });
        }
    }
    
    playerHealth -= getPlayerDamage();
    updateHealthDisplays();
    
    // Only play attack animation if we're not transitioning to game over
    if (playerHealth > 0) {
        playAttackAnimation(false);
    }
    
    if (playerHealth <= 0) {
        // Ensure health doesn't go below 0 for display purposes
        playerHealth = 0;
        updateHealthDisplays();
        
        // Transition to defeat state
        transitionToState(BossState::DEFEAT);
    }
}

void BossFight::start()
{
    bossHealth = 100;
    playerHealth = 100;
    currentSequenceIndex = 0;
    isTransitioning = false;
    
    setupUI();
    updateHealthDisplays();
    
    sequenceDisplay->setPlainText("Get Ready!");
    sequenceDisplay->setDefaultTextColor(Qt::yellow);
    
    QTimer::singleShot(1500, [this]() {
        generateSequence();
    });
}

void BossFight::setupUI()
{
    // Set up background
    QPixmap bgPixmap(":/images/arena.png");
    if (bgPixmap.isNull()) {
        QLinearGradient gradient(0, 0, 0, scene->height());
        gradient.setColorAt(0, QColor(20, 20, 50));
        gradient.setColorAt(1, QColor(50, 20, 20));
        scene->setBackgroundBrush(gradient);
    } else {
        background = scene->addPixmap(bgPixmap.scaled(scene->width(), scene->height()));
        background->setZValue(-1);
    }

    // Setup characters
    setupCharacters();

    // Create health bars with better positioning
    QGraphicsRectItem* playerHealthContainer = nullptr;
    QGraphicsRectItem* bossHealthContainer = nullptr;
    createHealthBar(playerHealthContainer, playerHealthBars, QPointF(50, 30), Qt::green, true);
    createHealthBar(bossHealthContainer, bossHealthBars, QPointF(scene->width() - 450, 30), Qt::red, false);

    // Health text displays with better positioning
    playerHealthDisplay = new QGraphicsTextItem("YOUR HP: 100%");
    playerHealthDisplay->setDefaultTextColor(Qt::green);
    playerHealthDisplay->setFont(QFont("Arial", 16, QFont::Bold));
    playerHealthDisplay->setPos(50, 5);
    scene->addItem(playerHealthDisplay);

    bossHealthDisplay = new QGraphicsTextItem("BOSS HP: 100%");
    bossHealthDisplay->setDefaultTextColor(Qt::red);
    bossHealthDisplay->setFont(QFont("Arial", 16, QFont::Bold));
    bossHealthDisplay->setPos(scene->width() - 450, 5);
    scene->addItem(bossHealthDisplay);

    // Sequence display area with better positioning
    sequenceDisplay = new QGraphicsTextItem("GET READY!");
    sequenceDisplay->setDefaultTextColor(QColor(255, 255, 100));
    sequenceDisplay->setFont(QFont("Arial", 36, QFont::Bold));
    sequenceDisplay->setPos((scene->width() - sequenceDisplay->boundingRect().width()) / 2, 120);
    scene->addItem(sequenceDisplay);

    // Ensure view settings
    if (QGraphicsView* view = scene->views().first()) {
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setDragMode(QGraphicsView::NoDrag);
        view->setTransformationAnchor(QGraphicsView::NoAnchor);
        view->setResizeAnchor(QGraphicsView::NoAnchor);
        view->setAlignment(Qt::AlignCenter);
        view->setFocusPolicy(Qt::StrongFocus); // Allow keyboard focus while maintaining mouse interaction
    }
    
    // Make sure the main window has focus for key events
    if (QWidget* window = scene->views().first()->window()) {
        window->setFocus();
    }
}

void BossFight::updateHealthDisplays()
{
    // Update text displays
    bossHealthDisplay->setPlainText(QString("BOSS HP: %1%").arg(bossHealth));
    playerHealthDisplay->setPlainText(QString("YOUR HP: %1%").arg(playerHealth));
    
    // Update boss health bars
    int bossBarCount = qRound(bossHealth / 10.0);
    for (int i = 0; i < bossHealthBars.size(); i++) {
        if (i < bossBarCount) {
            QColor barColor = bossHealth > 50 ? Qt::red : QColor(255, 50, 50);
            bossHealthBars[i]->setBrush(barColor);
        } else {
            bossHealthBars[i]->setBrush(QColor(40, 40, 40));
        }
    }
    
    // Update player health bars
    int playerBarCount = qRound(playerHealth / 10.0);
    for (int i = 0; i < playerHealthBars.size(); i++) {
        if (i < playerBarCount) {
            QColor barColor = playerHealth > 50 ? Qt::green : QColor(255, 165, 0);
            playerHealthBars[i]->setBrush(barColor);
        } else {
            playerHealthBars[i]->setBrush(QColor(40, 40, 40));
        }
    }
}

void BossFight::cleanup()
{
    // Stop any ongoing animations first
    if (currentAnimation) {
        currentAnimation->stop();
        delete currentAnimation;
        currentAnimation = nullptr;
    }
    
    if (whipAttackAnimation) {
        whipAttackAnimation->stop();
        delete whipAttackAnimation;
        whipAttackAnimation = nullptr;
    }

    // Clear sequences and indicators
    buttonSequence.clear();
    clearSequenceIndicators();
    
    // Clean up UI elements only if they're still in the scene
    QList<QGraphicsItem*> itemsToRemove;
    
    auto addIfValid = [this, &itemsToRemove](QGraphicsItem* item) {
        if (item && item->scene() == scene) {
            itemsToRemove.append(item);
        }
    };
    
    addIfValid(bossHealthDisplay);
    addIfValid(playerHealthDisplay);
    addIfValid(sequenceDisplay);
    addIfValid(vsText);
    addIfValid(playerSprite);
    addIfValid(bossSprite);
    addIfValid(whipAttackSprite);
    addIfValid(backgroundOverlay);
    addIfValid(background);

    // Remove and delete items
    for (auto item : itemsToRemove) {
        scene->removeItem(item);
        delete item;
    }
    
    // Clean up health bars
    for (auto bar : bossHealthBars) {
        if (bar && bar->scene() == scene) {
            scene->removeItem(bar);
            delete bar;
        }
    }
    
    for (auto bar : playerHealthBars) {
        if (bar && bar->scene() == scene) {
            scene->removeItem(bar);
            delete bar;
        }
    }
    
    // Clear lists
    bossHealthBars.clear();
    playerHealthBars.clear();
    
    // Reset all pointers
    bossHealthDisplay = nullptr;
    playerHealthDisplay = nullptr;
    sequenceDisplay = nullptr;
    vsText = nullptr;
    playerSprite = nullptr;
    bossSprite = nullptr;
    whipAttackSprite = nullptr;
    backgroundOverlay = nullptr;
    background = nullptr;
    
    // Reset state
    isTransitioning = false;
    currentSequenceIndex = 0;
}

void BossFight::playIntroAnimation()
{
    // Create a dramatic intro animation
    QGraphicsTextItem* introText = new QGraphicsTextItem("BOSS FIGHT");
    introText->setDefaultTextColor(Qt::red);
    introText->setFont(QFont("Arial", 72, QFont::Bold));
    introText->setPos((scene->width() - introText->boundingRect().width()) / 2,
                     (scene->height() - introText->boundingRect().height()) / 2);
    introText->setOpacity(0);
    scene->addItem(introText);

    QPropertyAnimation* fadeIn = new QPropertyAnimation(introText, "opacity");
    fadeIn->setDuration(1000);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::OutCubic);

    QPropertyAnimation* fadeOut = new QPropertyAnimation(introText, "opacity");
    fadeOut->setDuration(1000);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::InCubic);

    QSequentialAnimationGroup* introAnim = new QSequentialAnimationGroup(this);
    introAnim->addAnimation(fadeIn);
    introAnim->addPause(1000);
    introAnim->addAnimation(fadeOut);

    connect(introAnim, &QSequentialAnimationGroup::finished, this, [this, introText]() {
        scene->removeItem(introText);
        delete introText;
        transitionToState(BossState::SEQUENCE_DISPLAY);
        generateSequence();
    });

    introAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void BossFight::transitionToState(BossState newState)
{
    if (isTransitioning) return;
    
    isTransitioning = true;
    currentState = newState;
    
    switch (newState) {
        case BossState::SEQUENCE_DISPLAY:
            if (sequenceDisplay) {
                sequenceDisplay->setPlainText("Watch the sequence!");
                sequenceDisplay->setDefaultTextColor(Qt::yellow);
            }
            break;
            
        case BossState::PLAYER_INPUT:
            if (sequenceDisplay) {
                sequenceDisplay->setPlainText("Your turn!");
                sequenceDisplay->setDefaultTextColor(Qt::green);
            }
            break;
            
        case BossState::VICTORY:
        case BossState::DEFEAT:
            // Stop any ongoing animations or timers
            if (currentAnimation) {
                currentAnimation->stop();
                delete currentAnimation;
                currentAnimation = nullptr;
            }
            
            // Clear sequences and indicators before transitioning
            buttonSequence.clear();
            clearSequenceIndicators();
            
            // Disable further input
            isTransitioning = true;
            
            // Create a delayed cleanup to ensure all animations are complete
            QTimer::singleShot(100, this, [this, newState]() {
                // Stop all animations and effects
                QList<QGraphicsItem*> items = scene->items();
                for (QGraphicsItem* item : items) {
                    if (QGraphicsObject* obj = item->toGraphicsObject()) {
                        // Stop any animations on the object
                        obj->setGraphicsEffect(nullptr);
                    }
                }
                
                cleanup();
                
                // Process any pending events before emitting signals
                QApplication::processEvents();
                
                if (newState == BossState::VICTORY) {
                    emit victory();
                } else {
                    emit gameOver();
                }
            });
            return;
            
        default:
            break;
    }
    
    isTransitioning = false;
}

void BossFight::createVisualFeedback(const QString &text, const QColor &color)
{
    QGraphicsTextItem* feedbackText = new QGraphicsTextItem(text);
    feedbackText->setDefaultTextColor(color);
    feedbackText->setFont(QFont("Arial", 48, QFont::Bold));
    feedbackText->setPos((scene->width() - feedbackText->boundingRect().width()) / 2,
                        (scene->height() - feedbackText->boundingRect().height()) / 2);
    
    QGraphicsDropShadowEffect* glow = new QGraphicsDropShadowEffect();
    glow->setColor(color);
    glow->setBlurRadius(20);
    glow->setOffset(0);
    feedbackText->setGraphicsEffect(glow);
    
    scene->addItem(feedbackText);
    
    QPropertyAnimation* scaleAnim = new QPropertyAnimation(feedbackText, "scale");
    scaleAnim->setDuration(500);
    scaleAnim->setStartValue(0.5);
    scaleAnim->setEndValue(1.0);
    scaleAnim->setEasingCurve(QEasingCurve::OutBack);
    
    QPropertyAnimation* fadeAnim = new QPropertyAnimation(feedbackText, "opacity");
    fadeAnim->setDuration(500);
    fadeAnim->setStartValue(0.0);
    fadeAnim->setEndValue(1.0);
    
    QParallelAnimationGroup* animGroup = new QParallelAnimationGroup(this);
    animGroup->addAnimation(scaleAnim);
    animGroup->addAnimation(fadeAnim);
    
    connect(animGroup, &QParallelAnimationGroup::finished, this, [feedbackText]() {
        QTimer::singleShot(1000, feedbackText, [feedbackText]() {
            if (feedbackText->scene()) {
                feedbackText->scene()->removeItem(feedbackText);
            }
            delete feedbackText;
        });
    });
    
    animGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void BossFight::handleKeyPress(Qt::Key key)
{
    if (buttonSequence.isEmpty() || isTransitioning) {
        return;
    }

    if (currentSequenceIndex < buttonSequence.size()) {
        if (key == buttonSequence[currentSequenceIndex]) {
            currentSequenceIndex++;
            
            if (currentSequenceIndex <= sequenceIndicators.size()) {
                auto pressedIndicator = sequenceIndicators[currentSequenceIndex - 1];
                if (pressedIndicator) {
                    pressedIndicator->setDefaultTextColor(QColor(0, 255, 0));
                    pressedIndicator->setFont(QFont("Arial", 32, QFont::Bold));
                    
                    QPropertyAnimation *scaleAnim = new QPropertyAnimation(pressedIndicator, "scale");
                    scaleAnim->setDuration(200);
                    scaleAnim->setStartValue(1.2);
                    scaleAnim->setEndValue(1.0);
                    scaleAnim->start(QAbstractAnimation::DeleteWhenStopped);
                }
            }
            
            updateSequenceIndicators(currentSequenceIndex);
            
            if (currentSequenceIndex >= buttonSequence.size()) {
                if (sequenceDisplay) {
                    sequenceDisplay->setPlainText("Great job!");
                    sequenceDisplay->setDefaultTextColor(Qt::green);
                }
                QTimer::singleShot(500, this, &BossFight::onCorrectSequence);
            }
        } else {
            onWrongButton();
            
            if (sequenceDisplay) {
                sequenceDisplay->setPlainText("Wrong button! Keep trying!");
                sequenceDisplay->setDefaultTextColor(Qt::red);
                
                // Store a weak pointer to sequenceDisplay
                QPointer<QGraphicsTextItem> weakDisplay = sequenceDisplay;
                QTimer::singleShot(1000, this, [this, weakDisplay]() {
                    if (!weakDisplay.isNull() && weakDisplay->scene()) {
                        weakDisplay->setPlainText("Press the keys in sequence!");
                        weakDisplay->setDefaultTextColor(Qt::yellow);
                    }
                });
            }
        }
    }
}

void BossFight::updateSequenceIndicators(int index)
{
    for (int i = 0; i < sequenceIndicators.size(); i++) {
        QGraphicsTextItem* indicator = sequenceIndicators[i];
        QGraphicsDropShadowEffect* glowEffect = new QGraphicsDropShadowEffect();
        
        if (i < index) {
            indicator->setDefaultTextColor(QColor(100, 255, 100));
            glowEffect->setColor(QColor(0, 255, 0));
            glowEffect->setBlurRadius(15);
        } else if (i == index) {
            indicator->setDefaultTextColor(QColor(255, 255, 100));
            glowEffect->setColor(QColor(255, 255, 0));
            glowEffect->setBlurRadius(20);
            
            QGraphicsOpacityEffect *pulseEffect = new QGraphicsOpacityEffect(indicator);
            indicator->setGraphicsEffect(pulseEffect);
            
            QPropertyAnimation *animation = new QPropertyAnimation(pulseEffect, "opacity");
            animation->setDuration(600);
            animation->setStartValue(1.0);
            animation->setEndValue(0.5);
            animation->setLoopCount(-1);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
        } else {
            indicator->setDefaultTextColor(QColor(150, 150, 150));
            glowEffect->setColor(Qt::white);
            glowEffect->setBlurRadius(5);
        }
        
        glowEffect->setOffset(0, 0);
        indicator->setGraphicsEffect(glowEffect);
    }
}

void BossFight::clearSequenceIndicators()
{
    for (auto indicator : sequenceIndicators) {
        scene->removeItem(indicator);
        delete indicator;
    }
    sequenceIndicators.clear();
}

void BossFight::checkGameState()
{
    if (bossHealth <= 0) {
        transitionToState(BossState::VICTORY);
    } else if (playerHealth <= 0) {
        transitionToState(BossState::DEFEAT);
    }
}

QString BossFight::getButtonSymbol(Qt::Key key) const
{
    switch (key) {
        case Qt::Key_Left: return "←";
        case Qt::Key_Right: return "→";
        case Qt::Key_Up: return "↑";
        case Qt::Key_Down: return "↓";
        case Qt::Key_Space: return "⎵";
        default: return "";
    }
}

void BossFight::startNextSequence()
{
    if (currentState == BossState::SEQUENCE_DISPLAY) {
        transitionToState(BossState::PLAYER_INPUT);
    } else {
        currentSequenceIndex = 0;
        generateSequence();
        transitionToState(BossState::SEQUENCE_DISPLAY);
    }
}

int BossFight::getSequenceLength() const
{
    return 4;  // Default sequence length
}

int BossFight::getPlayerDamage() const
{
    return 20;  // Default player damage
}

int BossFight::getBossDamage() const
{
    return 25;  // Default boss damage
}

Level1Boss::Level1Boss(QGraphicsScene *scene, QObject *parent)
    : BossFight(scene, 1, parent)
{
}

void BossFight::setupCharacters()
{
    // Initialize sprite pointers first
    playerSprite = new QGraphicsPixmapItem();
    bossSprite = new QGraphicsPixmapItem();
    
    // Load character sprites
    QPixmap playerPixmap(":/images/player_stance.png");
    QPixmap bossPixmap(":/images/boss_stance.png");
    
    // Set the pixmaps
    playerSprite->setPixmap(playerPixmap);
    bossSprite->setPixmap(bossPixmap);
    
    // Add sprites to scene
    scene->addItem(playerSprite);
    scene->addItem(bossSprite);
    
    // Position the sprites
    playerSprite->setPos(250, 500);  // Left side, on ground
    bossSprite->setPos(650, 500);    // Right side, on ground

    // Add VS text higher up for better visibility
    vsText = new QGraphicsTextItem("VS");
    vsText->setDefaultTextColor(Qt::yellow);
    vsText->setFont(QFont("Arial", 48, QFont::Bold));
    vsText->setPos((scene->width() - vsText->boundingRect().width()) / 2, 200);

    QGraphicsDropShadowEffect *vsGlow = new QGraphicsDropShadowEffect();
    vsGlow->setColor(Qt::red);
    vsGlow->setBlurRadius(20);
    vsGlow->setOffset(0);
    vsText->setGraphicsEffect(vsGlow);

    scene->addItem(vsText);
}

void BossFight::createHealthBar(QGraphicsRectItem* &container, QList<QGraphicsRectItem*> &bars,
                              const QPointF &pos, const QColor &color, bool isPlayer)
{
    // Create container with improved visuals
    container = new QGraphicsRectItem();
    container->setRect(pos.x(), pos.y(), 400, 25);
    container->setBrush(QBrush(QColor(40, 40, 40, 180)));
    container->setPen(QPen(QColor(100, 100, 100, 200), 2));
    scene->addItem(container);

    // Create health segments with better spacing
    qreal barWidth = 38;
    qreal barSpacing = 2;
    qreal barHeight = 21;
    qreal startX = pos.x() + 2;
    qreal startY = pos.y() + 2;

    for (int i = 0; i < 10; i++) {
        QGraphicsRectItem *bar = new QGraphicsRectItem(
            startX + (isPlayer ? i : (9-i)) * (barWidth + barSpacing),
            startY,
            barWidth,
            barHeight
        );
        bar->setPen(Qt::NoPen);
        
        // Add gradient to health bars
        QLinearGradient gradient(0, 0, 0, barHeight);
        if (color == Qt::green) {
            gradient.setColorAt(0, QColor(0, 255, 0));
            gradient.setColorAt(1, QColor(0, 200, 0));
        } else {
            gradient.setColorAt(0, QColor(255, 0, 0));
            gradient.setColorAt(1, QColor(200, 0, 0));
        }
        bar->setBrush(gradient);
        
        scene->addItem(bar);
        bars.append(bar);
    }
}

void BossFight::playAttackAnimation(bool isPlayerAttacking)
{
    if (!playerSprite || !bossSprite) return;

    QGraphicsItem* targetSprite = isPlayerAttacking ? playerSprite : bossSprite;
    QPointF startPos = targetSprite->pos();
    QPointF midPos = startPos + QPointF(isPlayerAttacking ? 200 : -200, 0);
    QPointF endPos = startPos;

    // Create a timeline for the animation
    QTimeLine *timeLine = new QTimeLine(300, this);
    timeLine->setFrameRange(0, 100);
    
    QGraphicsItemAnimation *animation = new QGraphicsItemAnimation(this);
    animation->setItem(targetSprite);
    animation->setTimeLine(timeLine);
    
    // Set up the animation keyframes
    animation->setPosAt(0.0, startPos);
    animation->setPosAt(0.5, midPos);
    animation->setPosAt(1.0, endPos);
    
    connect(timeLine, &QTimeLine::finished, [timeLine, animation]() {
        delete animation;
        timeLine->deleteLater();
    });
    
    timeLine->start();
}

void BossFight::showVictoryScreen()
{
    // Simply emit the victory signal - the MainWindow will handle showing the victory screen
    emit victory();
}

void BossFight::showGameOverScreen()
{
    // Simply emit the gameOver signal - the MainWindow will handle showing the game over screen
    emit gameOver();
}
