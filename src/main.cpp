#include <iostream>
#include <thread>
#include "RVCOrchestrator.h"
#include "controller/RVCPowerController.h"
#include "controller/MotorController.h"
#include "controller/CleanerController.h"
#include "controller/MovementPolicyController.h"
#include "controller/CleaningPolicyController.h"
#include "handler/CLIHandler.h"

int main() {
    CLIHandler cli;
    RVCPowerController powerCtrl;
    MotorController motorCtrl;
    CleanerController cleanerCtrl;
    MovementPolicyController movCtrl;
    CleaningPolicyController cleaningPolicyCtrl;

    RVCOrchestrator orc(cli, &powerCtrl, &motorCtrl, &cleanerCtrl, &movCtrl, &cleaningPolicyCtrl);

    // UC1: Power On
    std::cout << "전원을 켜려면 Enter, 종료하려면 'q'를 입력하세요.\n";
    std::string input;
    std::getline(std::cin, input);
    if (input == "q") return 0;

    orc.powerOn();

    // UC8: 별도 스레드에서 'q' 입력 감지 → powerOff
    std::thread inputThread([&orc]() {
        std::string cmd;
        while (std::getline(std::cin, cmd)) {
            if (cmd == "q" || cmd == "Q") {
                orc.powerOff();
                break;
            }
        }
    });
    inputThread.detach();

    // UC2: 자동 청소 시작 (내부에서 while(systemRunning) 루프 돌며 UC3~UC7 포함)
    orc.performCleaning();

    return 0;
}
