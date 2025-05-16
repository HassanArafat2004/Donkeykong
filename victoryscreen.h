#ifndef VICTORYSCREEN_H
#define VICTORYSCREEN_H

#include <QWidget>

namespace Ui {
class VictoryScreen;
}

class VictoryScreen : public QWidget
{
    Q_OBJECT

public:
    explicit VictoryScreen(QWidget *parent = nullptr);
    ~VictoryScreen();

signals:
    void nextLevelClicked();
    void mainMenuClicked();

private:
    Ui::VictoryScreen *ui;
};

#endif // VICTORYSCREEN_H 