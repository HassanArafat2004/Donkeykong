#ifndef ARTIFACT_H
#define ARTIFACT_H

#include <QGraphicsPixmapItem>
#include <QObject>

class Artifact : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Artifact(QObject *parent = nullptr);
    bool isCollected() const { return collected; }
    void collect();

signals:
    void artifactCollected();  // Signal emitted when artifact is collected

private:
    bool collected;
};

#endif // ARTIFACT_H 