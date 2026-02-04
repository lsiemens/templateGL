add_library(__PROJECT___core_obj OBJECT
    looplog.cpp frame_timer.cpp
    model.cpp camera.cpp
    object.cpp shaders.cpp)

target_include_directories(__PROJECT___core_obj PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(__PROJECT___core_obj PRIVATE __PROJECT___warnings)
