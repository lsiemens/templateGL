add_executable(__PROJECT__
    __PROJECT__.cpp)

target_sources(__PROJECT__ PRIVATE
    $<TARGET_OBJECTS:__PROJECT___core_obj>)

target_link_libraries(__PROJECT__ PRIVATE
    OpenGL::GL
    GLEW::GLEW
    glfw
    
    __PROJECT___core_obj
    __PROJECT___warnings
    __PROJECT___assets)

add_dependencies(__PROJECT__ __PROJECT___stage_assets)
