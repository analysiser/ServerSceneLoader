/**
 * @file scene_loader.hpp
 * @brief parse an xml description of scene file, load into a Scene class
 *        generate mesh, materials. Make Scene ready for tracing
 *
 * @author Xiao Li (xiaol2)
 */

#ifndef _SCENELOADER_HPP
#define _SCENELOADER_HPP
#include "scene.h"

using namespace _AZ_;

bool load_scene( Scene *scene, const char * xmlInputFile );

#endif