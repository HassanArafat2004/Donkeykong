#ifndef FASTBARREL_H
#define FASTBARREL_H

#include "barrel.h"

class FastBarrel : public Barrel {
    Q_OBJECT
public:
    FastBarrel();
    ~FastBarrel() override = default;

private:
    void move() override;
};

#endif // FASTBARREL_H 