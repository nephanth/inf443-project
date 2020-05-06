#pragma once

#include "main/scene_base/base.hpp"


#ifdef SCENE_3D_GRAPHICS
#include <vector>

// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool wireframe = false;

};

struct scene_model : scene_base
{

    /** A part must define two functions that are called from the main function:
     * setup_data: called once to setup data before starting the animation loop
     * frame_draw: called at every displayed frame within the animation loop
     *
     * These two functions receive the following parameters
     * - shaders: A set of shaders.
     * - scene: Contains general common object to define the 3D scene. Contains in particular the camera.
     * - data: The part-specific data structure defined previously
     * - gui: The GUI structure allowing to create/display buttons to interact with the scene.
    */

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void update_positions();
    void set_gui();

    // visual representation of a surface
    vcl::mesh_drawable terrain;
    vcl::mesh_drawable trunk;
    vcl::mesh_drawable foliage;
    vcl::mesh_drawable plant;
    vcl::mesh_drawable flower;

    GLuint terrain_texture_id;
    GLuint plant_texture_id;
    GLuint flower_texture_id;

    uint tree_number;
    uint plant_number;
    uint flower_number;

    std::vector<vcl::vec3> tree_positions;
    std::vector<vcl::vec3> plant_positions;
    std::vector<vcl::vec3> flower_positions;


    gui_scene_structure gui_scene;
};

#endif


