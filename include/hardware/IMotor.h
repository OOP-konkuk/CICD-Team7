#pragma once

class IMotor {
public:
    virtual ~IMotor() = default;

    virtual void moveForward() = 0;
    virtual void moveBackward() = 0;
    virtual void turnLeft() = 0;
    virtual void turnRight() = 0;
    virtual void stopMoving() = 0;
};
