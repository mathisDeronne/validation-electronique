idf_component_register(
    SRCS "hal/sensors/drv_ina237.cpp"
         "hal/sensors/drv_ina237_convert.cpp"
         "hal/sensors/drv_temp_sensor.cpp"
         "hal/ihm/drv_ihm.cpp"
         "memory/psram_manager.cpp"
         "memory/flash_storage.cpp"
    INCLUDE_DIRS "hal/sensors/include"
                 "hal/ihm/include"
                 "memory/include"
    REQUIRES config driver
)
