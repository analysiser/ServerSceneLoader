/**
 * @file scene_loader.hpp
 * @brief parse an xml description of scene file, load into a Scene class
 *        generate mesh, materials. Make Scene ready for tracing
 *
 * @author Xiao Li (xiaol2)
 */

#ifndef _SCENELOADER_HPP
#define _SCENELOADER_HPP

bool load_scene( const char * xmlInputFile );

#endif