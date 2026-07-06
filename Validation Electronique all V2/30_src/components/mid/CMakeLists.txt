idf_component_register(
    SRCS "i2c_manager.cpp"
         "ble_manager.cpp"
         "usb_manager.cpp"
    INCLUDE_DIRS "include"
    REQUIRES config
)
