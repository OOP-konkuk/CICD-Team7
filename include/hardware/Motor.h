#pragma once

class Motor {
public:
    Motor() = default;
    virtual ~Motor() = default;

    virtual void moveForward();
    virtual void moveBackward();
    virtual void turnLeft();
    virtual void turnRight();
    virtual void stopMoving();
};
