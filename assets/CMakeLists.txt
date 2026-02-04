add_library(__PROJECT___assets INTERFACE)

set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(ASSET_FILES
    "${ASSETS_DIR}/vertex.glsl"
    "${ASSETS_DIR}/fragment.glsl")

target_compile_definitions(__PROJECT___assets INTERFACE
    __PROJECT___ASSETS_SUBDIR="assets")

add_custom_target(__PROJECT___stage_assets
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:__PROJECT__>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ASSET_FILES}
        "$<TARGET_FILE_DIR:__PROJECT__>/assets"
    SOURCES ${ASSET_FILES}
    VERBATIM)
