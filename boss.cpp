#include "boss.h"
#include <QPixmap>
#include <QMovie>

Boss::Boss() {
    animation = new QMovie(":/images/monkey.gif");
    animation->setScaledSize(QSize(80, 80));
    animation->start();
    connect(animation, &QMovie::frameChanged, this, &Boss::updateFrame);
    
}

Boss::~Boss() {
    delete animation;
}

void Boss::updateFrame() {
    setPixmap(animation->currentPixmap());
}
