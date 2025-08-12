cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Simulation)

find_package(Geant4 REQUIRED ui_all vis_all)

find_package(NEST REQUIRED)
include_directories(${NEST_INCLUDE_DIRS})
include_directories(/home/nickfar20/gcem/include)

include(${Geant4_USE_FILE})
include_directories(include)
#include_directories(/home/nickfar20/DMG4/DMG4/DMG4-install/include)
#link_directories(/home/nickfar20/DMG4/DMG4/DMG4-install/lib)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

file(GLOB MACRO_FILES
	"*.mac"
)

file(COPY ${MACRO_FILES} DESTINATION ${PROJECT_BINARY_DIR})

add_executable(uclatpc_CAD uclatpc_CAD.cc ${sources} ${headers})
target_link_libraries(uclatpc_CAD ${Geant4_LIBRARIES} NEST::NESTCore NEST::NESTG4) #DMG4

# set_target_properties(uclatpc_CAD PROPERTIES BUILD_WITH_INSTALL_RUNPATH TRUE INSTALL_RUNPATH "/home/nickfar20/DMG4/DMG4/DMG4-install/lib")

add_custom_target(Simulation DEPENDS uclatpc_CAD)
