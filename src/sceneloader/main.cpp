/**
 * @file main.cpp
 * @brief SceneLoader entry
 *
 * @author Xiao Li (xiaol2)
 */

//#ifdef __APPLE__
//#include <GLKit/GLKMath.h>
//#endif
//
//#include <SDL.h>

#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "utils/tiny_obj_loader.h"
#include "utils/tiny_obj_loader.cc"
#include "scene_loader.h"


using namespace std;
using namespace tinyobj;

/**
 * Struct of the program options.
 */
struct Options
{
    const char *input_obj_file_name;
};

/**
 * Parses args into an Options struct. Returns true on success, false on failure.
 */
static bool parse_args( Options* opt, int argc, char* argv[] )
{
    // TODO: Print usage
    if ( argc < 2 ) {
        //print_usage( argv[0] );
        return false;
    }
    
    opt->input_obj_file_name = argv[1];
    return true;
}

static void printObjInfo(vector<shape_t> &shapes)
{
    for (size_t i = 0; i < shapes.size(); i++) {
        printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
        printf("shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
        assert((shapes[i].mesh.indices.size() % 3) == 0);
        for (size_t f = 0; f < shapes[i].mesh.indices.size(); f++) {
            printf("  idx[%ld] = %d\n", f, shapes[i].mesh.indices[f]);
        }
        
        printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
        assert((shapes[i].mesh.positions.size() % 3) == 0);
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            printf("  v[%ld] = (%f, %f, %f)\n", v,
                   shapes[i].mesh.positions[3*v+0],
                   shapes[i].mesh.positions[3*v+1],
                   shapes[i].mesh.positions[3*v+2]);
        }
    }
}

int main(int argc, char* argv[])
{
    printf("Hello World!\n");
    
    Options opt;
    
    if ( !parse_args( &opt, argc, argv ) ) {
        return 1;
    }
    
    // load obj file to tiny obj loader
    std::string inputfile = opt.input_obj_file_name;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> mtls;
    
    std::string err = tinyobj::LoadObj(shapes, mtls, inputfile.c_str());
    
    if (!err.empty()) {
        std::cerr << err << std::endl;
        exit(1);
    }
    
    printObjInfo(shapes);
    
    
    
    return 0;
}


