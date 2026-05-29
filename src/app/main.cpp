#include "app/console_log.hpp"
#include "app/cli_command_source.hpp"
#include "core/cleaning_engine.hpp"
#include "interface/i_proximity_sensor.hpp"
#include "interface/i_dust_sensor.hpp"
#include "interface/i_drive_train.hpp"
#include "interface/i_cleaning_unit.hpp"
#include <array>
#include <string>
#include <chrono>

namespace {

// Demo hardware implementations: sensors always report clear/clean;
// actuators log their commands via IOutputLog.

struct DemoSensor final : public IProximitySensor {
    bool isBlocked() const override { return false; }
};

struct DemoDustSensor final : public IDustSensor {
    bool isDirty() const override { return false; }
};

struct DemoDriveTrain final : public IDriveTrain {
    explicit DemoDriveTrain(IOutputLog& log) : log_(log) {}
    void execute(MotorCommand cmd) override {
        static constexpr std::array<const char*, 5> names{
            "FORWARD", "BACKWARD", "TURN_LEFT", "TURN_RIGHT", "STOP"};
        log_.write(std::string("[MOTOR] ") + names[static_cast<std::size_t>(cmd)]);
    }
    IOutputLog& log_;
};

struct DemoCleaningUnit final : public ICleaningUnit {
    explicit DemoCleaningUnit(IOutputLog& log) : log_(log) {}
    void setIntensity(CleaningMode mode) override {
        static constexpr std::array<const char*, 3> names{"OFF", "STANDARD", "BOOST"};
        log_.write(std::string("[CLEANER] ") + names[static_cast<std::size_t>(mode)]);
    }
    IOutputLog& log_;
};

}  // namespace

int main() {
    ConsoleLog        log;
    CliCommandSource  commands;

    log.write("=== RVC 제어 소프트웨어 ===");
    log.write("'on'을 입력하여 전원을 켜세요. 실행 중 'off'를 입력하면 종료됩니다.");
    commands.waitForPowerOn();

    DemoSensor       front, left;
    DemoDustSensor   dust;
    DemoDriveTrain   drive(log);
    DemoCleaningUnit cleaner(log);

    CleaningEngine engine(
        front, left, dust,
        drive, cleaner, log, commands,
        std::chrono::milliseconds(100)  // 100ms loop for human-readable output
    );

    engine.run();
    return 0;
}
