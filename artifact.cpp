#include "artifact.h"
#include <QPixmap>

Artifact::Artifact(QObject *parent) : QObject(parent), QGraphicsPixmapItem() {
    setPixmap(QPixmap(":/images/artifact.png").scaled(40, 40));
    collected = false;
}

void Artifact::collect() {
    if (!collected) {
        collected = true;
        emit artifactCollected();
        setVisible(false);
    }
} 