add_library(pilot_wave_core_obj OBJECT
    looplog.cpp frame_timer.cpp
    model.cpp camera.cpp
    object.cpp shaders.cpp)

target_include_directories(pilot_wave_core_obj PUBLIC ${PROJECT_SOURCE_DIR}/src)
#target_link_libraries(pilot_wave_core_obj PRIVATE pilot_wave_options)
