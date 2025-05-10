#include "retrybutton.h"

RetryButton::RetryButton(QGraphicsScene* scene) 
    : QPushButton("Retry"), parentScene(scene), proxyWidget(nullptr)
{
    setupButton();
    hide();
}

RetryButton::~RetryButton()
{
    if (proxyWidget) {
        proxyWidget->setWidget(nullptr);
        if (parentScene) {
            parentScene->removeItem(proxyWidget);
        }
        proxyWidget = nullptr;
    }
}

void RetryButton::setupButton()
{
    setGeometry(400, 400, 200, 50);
    setStyleSheet("QPushButton {"
                 "    background-color: #4CAF50;"
                 "    border: none;"
                 "    color: white;"
                 "    padding: 15px 32px;"
                 "    text-align: center;"
                 "    text-decoration: none;"
                 "    font-size: 16px;"
                 "    margin: 4px 2px;"
                 "    border-radius: 8px;"
                 "}"
                 "QPushButton:hover {"
                 "    background-color: #45a049;"
                 "}"
                 "QPushButton:pressed {"
                 "    background-color: #3d8b40;"
                 "}");
}

void RetryButton::showInScene()
{
    if (parentScene) {
        if (!proxyWidget) {
            proxyWidget = parentScene->addWidget(this);
            proxyWidget->setPos(400, 450);
        }
        show();
    }
}

void RetryButton::removeFromScene()
{
    hide();
    
    if (proxyWidget) {
        proxyWidget->setWidget(nullptr);
        if (parentScene) {
            parentScene->removeItem(proxyWidget);
        }
        proxyWidget = nullptr;
    }
}
