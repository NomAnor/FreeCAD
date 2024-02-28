
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIRS}
)
link_directories(${OCC_LIBRARY_DIR})

set(NomAnor_LIBS
    FreeCADApp
    PartDesign
    ${OCC_LIBRARIES}
    ${OCC_DEBUG_LIBRARIES}
)

SET(NomAnor_SRCS
    AppNomAnor.cpp
    PreCompiled.cpp
    PreCompiled.h
    Feature.h
    Feature.cpp
)

add_library(NomAnor SHARED ${NomAnor_SRCS})
target_link_libraries(NomAnor ${NomAnor_LIBS})


SET_BIN_DIR(NomAnor NomAnor /Mod/NomAnor)
SET_PYTHON_PREFIX_SUFFIX(NomAnor)

install(TARGETS NomAnor DESTINATION ${CMAKE_INSTALL_LIBDIR})
