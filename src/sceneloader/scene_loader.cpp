#include "scene_loader.h"

#include "utils/tiny_obj_loader.h"
#include "utils/tinyxml.h"

#include <iostream>
#include <map>
#include <cstring>
#include <exception>

using namespace std;


struct StrCompare
{
    bool operator() ( const char* s1, const char* s2 ) const
    {
        return strcmp( s1, s2 ) < 0;
    }
};


static const char *STR_MESH = "mesh";
static const char *STR_FILENAME = "filename";

#pragma mark - parsing xml attributes helpers
/* */
static void print_error_header( const TiXmlElement* base )
{
    std::cout << "ERROR, " << base->Row() << ":" << base->Column() << "; "
    << "in " << base->Value() << ", ";
}

static void parse_attrib_string( const TiXmlElement* elem, bool required, const char* name, const char** val )
{
    const char* att = elem->Attribute( name );
    if ( !att && required ) {
        print_error_header( elem );
        std::cout << "missing '" << name << "'.\n";
        throw std::exception();
    } else if ( att ) {
        *val = att;
    }
}

static void parse_attrib_string( const TiXmlElement* elem, bool required, const char* name, std::string* val )
{
    const char* att = 0;
    parse_attrib_string( elem, required, name, &att );
    if ( att ) {
        *val = att;
    }
}

#pragma  mark - load scene main function
static void printObjInfo(vector<tinyobj::shape_t> &shapes)
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


bool load_scene( Scene *scene, const char * xmlInputFile )
{
    TiXmlDocument doc( xmlInputFile );
    const TiXmlElement* root = 0;
    const TiXmlElement* elem = 0;
    
    // try to load the document
    if ( !doc.LoadFile() ) {
        std::cout << "ERROR, " << doc.ErrorRow() << ":" << doc.ErrorCol() << "; "
        << "parse error: " << doc.ErrorDesc() << "\n";
        return false;
    }
    
    // check for root element
    root = doc.RootElement();
    if ( !root ) {
        std::cout << "No root element.\n";
        return false;
    }
    
    
    try {
        // parse the meshes
        elem = root->FirstChildElement( STR_MESH );
        while (elem) {
            
            // Find an elem of mesh, parse the element to real mesh
            // mesh_file_name is used for obj file name
            string mesh_file_name;
            parse_attrib_string(elem, true, STR_FILENAME, &mesh_file_name);
            
            // Use obj loader parse the obj file and write to corresponding data structure
            // load obj file to tiny obj loader
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> mtls;
            
            std::string err = tinyobj::LoadObj(shapes, mtls, mesh_file_name.c_str());
            
            if (!err.empty()) {
                std::cerr << err << std::endl;
                exit(1);
            }
//            printObjInfo(shapes);
            
            scene->addMesh(shapes);
            scene->addMaterial(mtls);
            
            elem = elem->NextSiblingElement( STR_MESH );
        }

    }
    catch ( std::bad_alloc const& ) {
        std::cout << "Out of memory error while loading scene\n.";
        return false;
    } catch ( ... ) {
        return false;
    }
    
    return true;
    
    
}