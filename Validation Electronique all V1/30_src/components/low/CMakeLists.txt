idf_component_register(
    SRCS "hal/i2c/drv_i2c.cpp"
         "hal/sensors/drv_ina237.cpp"
         "hal/sensors/drv_ina237_convert.cpp"
         "hal/sensors/drv_temp_sensor.cpp"
         "hal/ihm/drv_ihm.cpp"
         "hal/adc/drv_adc.cpp"
         "hal/uart/drv_uart.cpp"
         "memory/psram_manager.cpp"
         "memory/flash_storage.cpp"
    INCLUDE_DIRS "hal/i2c/include"
                 "hal/sensors/include"
                 "hal/ihm/include"
                 "hal/adc/include"
                 "hal/uart/include"
                 "memory/include"
    REQUIRES config driver
)
