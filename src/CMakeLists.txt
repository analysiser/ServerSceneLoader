cmake_minimum_required(VERSION 2.8)
project(azSceneLoader)

include(build/CMakeLists.txt)

include_directories(
    ${PROJECT_SOURCE_DIR}
    #~ ${SDL_INCLUDE_DIR}
    #~ ${GLEW_INCLUDE_DIRS}
    #~ ${PNG_INCLUDE_DIRS}
)


add_subdirectory(utils)
add_subdirectory(sceneloader)
add_subdirectory(core)
add_subdirectory(shapes)
#~ add_subdirectory(application)
#~ add_subdirectory(math)
#~ add_subdirectory(raytracer)
#~ add_subdirectory(scene)
#~ add_subdirectory(tinyxml)

#~ if(APPLE)
    #~ add_subdirectory(SDLmain)
#~ endif()
