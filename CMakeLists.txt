cmake_minimum_required(VERSION 3.7)
project(OSG_GPS_Simulator
        VERSION 0.0.0
        LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 17)


find_package( OpenThreads )
find_package( osg )
find_package( osgDB )
find_package( osgGA )
find_package( osgUtil )
find_package( osgText )
find_package( osgViewer )
find_package( osgAnimation )
macro( config_project PROJNAME LIBNAME )
 include_directories( ${${LIBNAME}_INCLUDE_DIR} )
 target_link_libraries( ${PROJNAME} ${${LIBNAME}_LIBRARY} )
endmacro()

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/files")
add_executable( ${PROJECT_NAME} 
src/main.cpp 
src/MyManipulator.cpp 
include/MyManipulator.h 
src/PickHandler.cpp 
include/PickHandler.h 
src/Skybox.cpp 
include/Skybox.h
src/UniverseObjects.cpp 
include/UniverseObjects.h
src/Animator.cpp 
include/Animator.h
src/MathLib.cpp
include/MathLib.h
src/GPSTracker.cpp
include/GPSTracker.h
include/InfoRenderer.h)

config_project( ${PROJECT_NAME} OPENTHREADS )
config_project( ${PROJECT_NAME} OSGANIMATION )
config_project( ${PROJECT_NAME} OSGGA )
config_project( ${PROJECT_NAME} OSG )
config_project( ${PROJECT_NAME} OSGTEXT )
config_project( ${PROJECT_NAME} OSGDB )
config_project( ${PROJECT_NAME} OSGUTIL )
config_project( ${PROJECT_NAME} OSGVIEWER )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/files/
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/files/)
