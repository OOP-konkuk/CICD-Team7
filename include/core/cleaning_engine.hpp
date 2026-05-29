#pragma once
#include <chrono>
#include "interface/i_proximity_sensor.hpp"
#include "interface/i_dust_sensor.hpp"
#include "interface/i_drive_train.hpp"
#include "interface/i_cleaning_unit.hpp"
#include "interface/i_output_log.hpp"
#include "interface/i_command_source.hpp"
#include "core/obstacle_handler.hpp"
#include "core/clean_intensity_controller.hpp"
#include "common/types.hpp"

class CleaningEngine {
public:
    CleaningEngine(
        IProximitySensor& frontSensor,
        IProximitySensor& leftSensor,
        IDustSensor&      dustSensor,
        IDriveTrain&      drive,
        ICleaningUnit&    cleaner,
        IOutputLog&       log,
        ICommandSource&   commands,
        std::chrono::milliseconds loopInterval = std::chrono::milliseconds(0)
    );

    void run();

private:
    void initSequence();
    void cleanLoop();
    void handleObstacle();
    void handleDust();
    void haltSafely(HaltReason reason);

    SystemMode mode_{SystemMode::IDLE};

    IProximitySensor& front_;
    IProximitySensor& left_;
    IDustSensor&      dust_;
    IDriveTrain&      drive_;
    IOutputLog&       log_;
    ICommandSource&   commands_;

    std::chrono::milliseconds loopInterval_;

    ObstacleHandler          obstacleHandler_;
    CleanIntensityController cleanCtrl_;
};
