idf_component_register(
    SRCS "MainApp.cpp"
         "MonitoringService.cpp"
         "AlarmService.cpp"
         "EventLoggingService.cpp"
         "ConfigService.cpp"
         "app_power_monitor_logic.cpp"
    INCLUDE_DIRS "include"
    REQUIRES config low mid
)
