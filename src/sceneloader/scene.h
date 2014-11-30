/**
 * @file scene.h
 * @brief scene file, stores Scene class, used for storing scene status, specifically for data structures
 *
 * @author Xiao Li (xiaol2)
 */

#ifndef _SCENE_HPP
#define _SCENE_HPP

#include <string>
#include <vector>
#include <map>

#include "utils/tiny_obj_loader.h"

using namespace std;


namespace _AZ_ {

//    typedef vector<vector<tinyobj::shape_t> >
    
    class Scene
    {
    public:
        Scene() {};
        ~Scene() {};
        
        void addMesh(tinyobj::shape_t &shape)
        {
            meshes.push_back(shape);
        }
        
        void addMesh(vector<tinyobj::shape_t> &shapes)
        {
            meshes.insert(meshes.end(), shapes.begin(), shapes.end());
        }
        
        void addMaterial(tinyobj::material_t &mtl)
        {
            mtls.push_back(mtl);
        }
        
        void addMaterial(vector<tinyobj::material_t> &materials)
        {
            mtls.insert(mtls.end(), materials.begin(), materials.end());
        }
        
        void reset();
        
        // Scene variables
        vector<tinyobj::shape_t> meshes;
        vector<tinyobj::material_t> mtls;
    };

    
}

#endif