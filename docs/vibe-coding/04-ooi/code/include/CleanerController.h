#pragma once
#include "types.h"

class CleanerController {
private:
    CleanMode mode{CleanMode::OFF};

public:
    void setMode(CleanMode m);
    CleanMode getMode() const;
};
