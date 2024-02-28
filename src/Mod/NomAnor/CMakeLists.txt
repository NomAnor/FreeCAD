
add_subdirectory(App)
if(BUILD_GUI)
    add_subdirectory(Gui)
endif(BUILD_GUI)

set(NomAnor_Scripts
    Init.py
)

if(BUILD_GUI)
    list (APPEND NomAnor_Scripts InitGui.py)
endif(BUILD_GUI)

add_custom_target(NomAnorScripts ALL
    SOURCES ${NomAnor_Scripts}
)

fc_target_copy_resource_flat(NomAnorScripts
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/NomAnor
    ${NomAnor_Scripts}
)

install(
    FILES
        ${NomAnor_Scripts}
    DESTINATION
        Mod/NomAnor
)
