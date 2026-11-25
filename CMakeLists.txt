enable_language(CXX)

add_cfe_app(battery_monitor
    fsw/src/cfs_app.cpp
    fsw/src/battery_logic.cpp
)

target_compile_features(battery_monitor PRIVATE cxx_std_17)