#ifndef STARTMENU_H
#define STARTMENU_H

#include <QWidget>
#include <QPushButton>
#include <QGraphicsScene>

namespace Ui {
class StartMenu;
}

class StartMenu : public QWidget
{
    Q_OBJECT

public:
    explicit StartMenu(QWidget *parent = nullptr);
    ~StartMenu();
    
    QGraphicsScene* scene() const { return menuScene; }

signals:
    void startGameClicked();
    void shopClicked();
    void exitClicked();

private:
    Ui::StartMenu *ui;
    QGraphicsScene *menuScene;
    QPushButton *startButton;
    QPushButton *shopButton;
    QPushButton *exitButton;
};

#endif // STARTMENU_H 