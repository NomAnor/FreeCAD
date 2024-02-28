
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${COIN3D_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
)

set(NomAnorGui_LIBS
    NomAnor
    FreeCADGui
    PartDesignGui
)

qt_add_resources(NomAnor_QRC_SRCS Resources/NomAnor.qrc)

SET(NomAnorGui_SRCS
    ${NomAnor_QRC_SRCS}
    AppNomAnorGui.cpp
    PreCompiled.cpp
    PreCompiled.h
    Command.cpp
    Workbench.cpp
    Workbench.h
)

SET(NomAnorGuiIcon_SVG
    Resources/icons/NomAnorWorkbench.svg
)

add_library(NomAnorGui SHARED ${NomAnorGui_SRCS} ${NomAnorGuiIcon_SVG})
target_link_libraries(NomAnorGui ${NomAnorGui_LIBS})

SET_BIN_DIR(NomAnorGui NomAnorGui /Mod/NomAnor)
SET_PYTHON_PREFIX_SUFFIX(NomAnorGui)

fc_copy_sources(NomAnorGui "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}/Mod/NomAnor" ${NomAnorGuiIcon_SVG})

install(TARGETS NomAnorGui DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${NomAnorGuiIcon_SVG} DESTINATION "${CMAKE_INSTALL_DATADIR}/Mod/NomAnor/Resources/icons")
