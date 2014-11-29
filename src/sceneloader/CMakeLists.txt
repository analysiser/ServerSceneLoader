add_executable(sceneloader main.cpp scene_loader.cpp scene_loader.h)
target_link_libraries(sceneloader utils core)

#~ if(APPLE)
    #~ target_link_libraries(sceneloader SDLmain)
#~ endif()

install(TARGETS sceneloader DESTINATION ${PROJECT_SOURCE_DIR}/..)
