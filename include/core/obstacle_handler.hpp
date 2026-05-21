#pragma once
#include "interface/i_proximity_sensor.hpp"
#include "interface/i_drive_train.hpp"
#include "interface/i_output_log.hpp"
#include "common/types.hpp"

class ObstacleHandler {
public:
    // front sensor detection is handled by CleaningEngine; ObstacleHandler
    // only decides the avoidance path using left_ and right_.
    ObstacleHandler(
        IProximitySensor& left,
        IProximitySensor& right,
        IDriveTrain&      drive,
        IOutputLog&       log
    );

    void startForward();
    void avoidAndReturn();

private:
    AvoidPath selectPath() const;
    void      executeTurn(MotorCommand turnCmd);
    void      executeBackAndTurn();

    IProximitySensor& left_;
    IProximitySensor& right_;
    IDriveTrain&      drive_;
    IOutputLog&       log_;
};
