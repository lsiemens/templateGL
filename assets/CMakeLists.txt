add_library(pilot_wave_assets INTERFACE)

set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(ASSET_FILES
    "${ASSETS_DIR}/vertex.glsl"
    "${ASSETS_DIR}/fragment.glsl")

target_compile_definitions(pilot_wave_assets INTERFACE
    PILOT_WAVE_ASSETS_SUBDIR="assets")

add_custom_target(pilot_wave_stage_assets
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:pilot_wave>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ASSET_FILES}
        "$<TARGET_FILE_DIR:pilot_wave>/assets"
    SOURCES ${ASSET_FILES}
    VERBATIM)
