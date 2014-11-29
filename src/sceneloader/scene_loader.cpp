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
bool load_scene( const char * xmlInputFile )
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
            string mesh_file_name;
            parse_attrib_string(elem, true, STR_FILENAME, &mesh_file_name);
            
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