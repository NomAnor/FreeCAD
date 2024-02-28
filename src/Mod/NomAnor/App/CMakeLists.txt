
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIRS}
)

set(NomAnor_LIBS
    FreeCADApp
    PartDesign
)

SET(NomAnor_SRCS
    AppNomAnor.cpp
    PreCompiled.cpp
    PreCompiled.h
)

add_library(NomAnor SHARED ${NomAnor_SRCS})
target_link_libraries(NomAnor ${NomAnor_LIBS})


SET_BIN_DIR(NomAnor NomAnor /Mod/NomAnor)
SET_PYTHON_PREFIX_SUFFIX(NomAnor)

install(TARGETS NomAnor DESTINATION ${CMAKE_INSTALL_LIBDIR})
