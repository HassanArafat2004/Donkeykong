#ifndef RETRYBUTTON_H
#define RETRYBUTTON_H

#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

class RetryButton : public QPushButton {
    Q_OBJECT

public:
    explicit RetryButton(QGraphicsScene* scene);
    ~RetryButton();

    void showInScene();
    void removeFromScene();
    QGraphicsProxyWidget* getProxyWidget() const { return proxyWidget; }

private:
    QGraphicsScene* parentScene;
    QGraphicsProxyWidget* proxyWidget;
    void setupButton();
};

#endif 