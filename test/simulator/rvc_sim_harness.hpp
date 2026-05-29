#pragma once
#include "core/cleaning_engine.hpp"
#include "sim_proximity_sensor.hpp"
#include "sim_dust_sensor.hpp"
#include "sim_drive_train.hpp"
#include "sim_cleaning_unit.hpp"
#include "sim_output_log.hpp"
#include "sim_command_source.hpp"

// RVCSimHarness — test harness for full end-to-end system simulation.
//
// Wires all six Sim implementations into a CleaningEngine.
// Tests control the environment through Inject* methods and verify
// system output through Query* methods, then call run() to execute.
//
// Usage pattern:
//   RVCSimHarness h;
//   h.injectFrontObstacle(true);      // environment setup
//   h.scheduleCommand(UserCommand::POWER_OFF);
//   h.run();
//   EXPECT_TRUE(h.driveHasCommand(MotorCommand::STOP));
class RVCSimHarness {
public:
    RVCSimHarness() : engine_(front_, left_, dust_, drive_, cleaner_, log_, cmds_) {}

    // -----------------------------------------------------------------------
    // Environment injection API — call before run()
    // -----------------------------------------------------------------------

    // Set steady-state front/left sensor values
    void setFrontBlocked(bool b)  { front_.setBlocked(b); }
    void setLeftBlocked(bool b)   { left_.setBlocked(b); }

    // Set steady-state dust sensor value
    void setDirty(bool d) { dust_.setDirty(d); }

    // Inject a one-shot sensor event (consumed by next poll, then reverts to steady-state)
    void injectFrontObstacle(bool blocked)  { front_.enqueueEvent(blocked); }
    void injectLeftObstacle(bool blocked)   { left_.enqueueEvent(blocked); }
    void injectDustEvent(bool dirty)        { dust_.enqueueEvent(dirty); }

    // Schedule a user command (consumed in order by the event loop)
    void scheduleCommand(UserCommand cmd) { cmds_.sendCommand(cmd); }

    // Schedule a "no-op" loop tick (loop runs once without consuming a command)
    void schedulePause() { cmds_.sendPause(); }

    // -----------------------------------------------------------------------
    // System execution
    // -----------------------------------------------------------------------
    void run() { engine_.run(); }

    // -----------------------------------------------------------------------
    // State query API — call after run()
    // -----------------------------------------------------------------------

    // Drive train queries
    MotorCommand currentDriveCommand() const         { return drive_.currentCommand(); }
    bool driveHasCommand(MotorCommand cmd) const     { return drive_.hasCommand(cmd); }
    std::size_t driveCommandCount() const            { return drive_.commandCount(); }
    MotorCommand driveCommandAt(std::size_t i) const { return drive_.commandAt(i); }
    std::size_t driveFirstIndexOf(MotorCommand cmd) const { return drive_.firstIndexOf(cmd); }

    // Cleaner queries
    CleaningMode currentCleaningMode() const          { return cleaner_.currentMode(); }
    bool cleanerHasMode(CleaningMode m) const         { return cleaner_.hasMode(m); }
    std::size_t cleanerModeCount() const              { return cleaner_.modeCount(); }
    CleaningMode cleanerModeAt(std::size_t i) const   { return cleaner_.modeAt(i); }

    // Log queries
    bool logContains(std::string_view sub) const     { return log_.contains(sub); }
    std::size_t logMessageCount() const              { return log_.messageCount(); }

    // Sensor call-count introspection (verifies sensor masking during rotation)
    int frontSensorCalls() const  { return front_.callCount(); }
    int leftSensorCalls() const   { return left_.callCount(); }
    int dustSensorCalls() const   { return dust_.callCount(); }

    void resetSensorCallCounts() {
        front_.resetCallCount();
        left_.resetCallCount();
        dust_.resetCallCount();
    }

    // Direct access for advanced assertions (prefer the query methods above)
    SimProximitySensor& frontSensor()  { return front_; }
    SimProximitySensor& leftSensor()   { return left_; }
    SimDustSensor&      dustSensor()   { return dust_; }
    SimDriveTrain&      driveTrain()   { return drive_; }
    SimCleaningUnit&    cleaningUnit() { return cleaner_; }
    SimOutputLog&       outputLog()    { return log_; }
    SimCommandSource&   commandSrc()   { return cmds_; }

private:
    SimProximitySensor front_, left_;
    SimDustSensor      dust_;
    SimDriveTrain      drive_;
    SimCleaningUnit    cleaner_;
    SimOutputLog       log_;
    SimCommandSource   cmds_;
    CleaningEngine     engine_;
};
