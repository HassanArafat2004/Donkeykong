#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include "platform.h"
#include "ladder.h"

class GameView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GameView(QWidget *parent = nullptr);
    ~GameView();

    void startLevel(int level);

signals:
    void returnToMenuClicked();

private:
    void createLevel(int level);
    void clearLevel();

    QGraphicsScene *m_scene;
    QPushButton *m_returnButton;

    // Level design
    QList<Platform*> m_platforms;
    QList<Ladder*> m_ladders;

    int m_currentLevel;
};

#endif // GAMEVIEW_H
