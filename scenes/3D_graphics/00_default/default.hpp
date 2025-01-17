#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_DEFAULT_3D_GRAPHICS

#include <vector>
#include <deque>
#include "scenes/3D_graphics/00_default/Hitbox.hpp"
#include "scenes/3D_graphics/00_default/Character.hpp"

struct gui_scene_structure
{
    bool wireframe = false;
    bool show_hitbox = false;
    bool is_collision = false;
};

/**
 * @brief scene de jeu
 * normalement l axe des x pointe vers l' utilisateur (camera vers -x), y et z forment un repere de face
 */
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
    // void keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods); // we use direct opengl interface because we don’t need callbacks



    void set_gui();
    gui_scene_structure gui_scene;


    // meshes :
    vcl::mesh_drawable mesh_building;
    vcl::mesh_drawable mesh_background;
    vcl::mesh_drawable mesh_game_over;
    Character character;
    Hitbox hitbox_building;
    GLuint texture_building;
    GLuint texture_background;
    GLuint texture_game_over;


    private:
    void load_mesh_building();
    void load_mesh_background();
    void load_mesh_game_over();


    void update_positions(float dt);

    /**
     * @brief adds elements at the end of the positions queue
     */
    void generate_wave();


    /**
     * @brief detects collision with the character
     *
     * @return true if the character collides with an object
     */
    bool detect_collision();

    /**
     * @brief too bad if you're out of field you lose
     *
     * @return true if you lose, false otherwise
     */
    bool out_of_field();

    /**
     * @brief removes out of view objects
     */
    void garbage_collect();

    /**
     * @brief returns something’s height depending on how far it is
     *
     * @param x how far it is
     *
     * @return z 
     */
    float earth_curve(float x);


    //invariant : building_positions[i].x croissant en i
    std::deque<vcl::vec2> building_positions; // deque because we always delete elements first added
    Hitbox get_building_hitbox(int i);

    vcl::timer_basic timer;
    vcl::timer_event timer_generate_wave;

    //constants 
    //general
    const float drop_angle= M_PI / 24; //for the camera
    const float field_length = 8;
    const float field_height = 4;
    const float curve = .005;

    const float camera_offset = 2;
    const float horizon = - 70; // x position where objects are generated
    const float wave_interval = 1.5; //seconds
    //building
    const float building_height = 2;
    const float building_depth = 7; // how far down the building goes (under the field)
    const float building_side = 2; // they're square
    //background
    const float background_side= 80;
    const float background_height= 50;

    float speed = 20;

    bool game_over = false;



};

#endif


