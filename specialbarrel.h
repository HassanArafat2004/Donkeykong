#ifndef SPECIALBARREL_H
#define SPECIALBARREL_H

#include "barrel.h"

class SpecialBarrel : public Barrel {
public:
    SpecialBarrel();

    // Override the move function to implement special behavior
    void move() override;

    // Special properties getters/setters
    void setSpeed(int newSpeed);
    int getSpeed() const;

    void setBounce(bool bounce);
    bool isBouncing() const;

    void setZigZag(bool zigzag);
    bool isZigZagging() const;

private:
    int speed;          // Movement speed multiplier
    bool bounce;        // Whether the barrel bounces when it hits edges
    bool zigzag;        // Whether the barrel zigzags while rolling
    int zigzagCounter;  // Counter for zigzag movement
};

#endif // SPECIALBARREL_H
