#include <stdio.h>
#include "I2CBus.hpp"
#include "INA237.hpp"

extern "C" void app_main(void)
{

    I2CBus bus;
    bus.begin(21, 22, 400000);

    INA237 ina(bus);

    if (!ina.begin())
    {
        printf("INA237 non detecte\n");
        return;
    }

    printf("INA237 OK\n");

    ina.setCalibration(10000, 2000); // exemple temporaire

    while (true)
    {

        printf("Bus: %.3f V\n", ina.readBusVoltage());
        printf("Current: %.3f A\n", ina.readCurrent());
        printf("Power: %.3f W\n", ina.readPower());
        printf("------------------\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}