#include <stdio.h>
#include <math.h>

#include "renderers.h"

static int mouse_x;
static int mouse_y;
static int mouse_down;
static int mouse_right_down;

static int object_id = 1;
static bool use_deferred = true;

static void swap_renderer() {
  
  camera* cam = entity_get("camera");
  light* sun = entity_get("sun");
  light* backlight = entity_get("backlight");
  
  if (use_deferred) {
    
    deferred_renderer_finish();
    forward_renderer_init();
    forward_renderer_set_camera(cam);
    forward_renderer_set_shadow_light(sun);
    forward_renderer_set_shadow_texture( shadow_mapper_depth_texture() );
    forward_renderer_add_light(sun);
    forward_renderer_add_light(backlight);
    
    use_deferred = false;
    
  } else {
    
    forward_renderer_finish();
    deferred_renderer_init();
    deferred_renderer_set_camera(cam);
    deferred_renderer_set_shadow_light(sun);
    deferred_renderer_set_shadow_texture( shadow_mapper_depth_texture() );
    deferred_renderer_add_light(sun);
    deferred_renderer_add_light(backlight);
    
    use_deferred = true;
  }

}

static bool any_button_pressed = false;

static void switch_renderer_event(ui_button* b, SDL_Event event) {
  
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    
    if (ui_button_contains_position(b, v2(event.motion.x, event.motion.y))) {
      any_button_pressed = true;
      b->pressed = true;
    }
  
  } else if (event.type == SDL_MOUSEBUTTONUP) {
    
    if (b->pressed) {
      any_button_pressed = false;
      b->pressed = false;
      if ((strcmp(ui_elem_name(b), "forward_renderer") == 0) && (use_deferred)) {
        swap_renderer();
      }
      if ((strcmp(ui_elem_name(b), "deferred_renderer") == 0) && (!use_deferred)) {
        swap_renderer();
      }
      
    }
  }
}

static void switch_object_event(ui_button* b, SDL_Event event) {
  
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    
    if (ui_button_contains_position(b, v2(event.motion.x, event.motion.y))) {
      any_button_pressed = true;
      b->pressed = true;
    }
  
  } else if (event.type == SDL_MOUSEBUTTONUP) {
    
    if (b->pressed) {
      any_button_pressed = false;
      b->pressed = false;
      
      char* name = ui_elem_name(b);
      if (strcmp(name, "piano") == 0) {
        object_id = 1;
      } else if (strcmp(name, "cello") == 0) {
        object_id = 0;
      } else if (strcmp(name, "imrod") == 0) {
        object_id = 2;
      } else if (strcmp(name, "dino") == 0) {
        object_id = 3;
      }
      
    }
  }
  
}

static vector3 ik_target;

void renderers_init() {
  
  graphics_viewport_set_dimensions(1280, 720);
  graphics_viewport_set_title("Renderers");

  load_folder("./resources/podium/");
  load_folder("./resources/cello/");
  load_folder("./resources/piano/");
  load_folder("./resources/imrod/");
  load_folder("./resources/dino/");
  
  renderable* r_podium = asset_get("./resources/podium/podium.obj");
  renderable_set_material(r_podium, asset_get("./resources/podium/podium.mat"));
  
  static_object* s_podium = entity_new("podium", static_object);
  s_podium->renderable = r_podium;
  
  renderable* r_cello = asset_get("./resources/cello/cello.obj");
  renderable_set_material(r_cello, asset_get("./resources/cello/cello.mat"));
  
  static_object* s_cello = entity_new("cello", static_object);
  s_cello->renderable = r_cello;
  s_cello->position = v3(0, 3, 0);
  s_cello->rotation = v4_quaternion_mul(s_cello->rotation, v4_quaternion_yaw(-1.7));
  s_cello->scale = v3(0.6, 0.6, 0.6);
  
  renderable* r_piano = asset_get("./resources/piano/piano.obj");
  renderable_set_material(r_piano, asset_get("./resources/piano/piano.mat"));
  
  static_object* s_piano = entity_new("piano", static_object);
  s_piano->renderable = r_piano;
  s_piano->position = v3(1, 5, 0);
  
  renderable* r_dino = asset_get("./resources/dino/dino.obj");
  renderable_set_multi_material(r_dino, asset_get("./resources/dino/dino.mmat"));
  
  static_object* s_dino = entity_new("dino", static_object);
  s_dino->renderable = r_dino;
  s_dino->scale = v3(4,4,4);
  
  renderable* r_imrod = asset_get("./resources/imrod/imrod.smd");
  material* mat_imrod = asset_get("./resources/imrod/imrod_animated.mat");  
  renderable_set_material(r_imrod, mat_imrod);
  
  skeleton* skel_imrod = asset_get("./resources/imrod/imrod.skl");
  animation* ani_imrod = asset_get("./resources/imrod/imrod.ani");
  
  animated_object* a_imrod = entity_new("imrod", animated_object);
  animated_object_load_skeleton(a_imrod, skel_imrod);
  a_imrod->renderable = r_imrod;
  a_imrod->animation = ani_imrod;
  a_imrod->rotation = v4_quaternion_mul(a_imrod->rotation, v4_quaternion_roll(1.57));
  
  /* Put some text on the screen */
  
  ui_button* framerate = ui_elem_new("framerate", ui_button);
  ui_button_move(framerate, v2(10,10));
  ui_button_resize(framerate, v2(30,25));
  ui_button_set_label(framerate, "FRAMERATE");
  ui_button_disable(framerate);
  
  ui_button* renderer = ui_elem_new("renderer", ui_button);
  ui_button_move(renderer, v2(10, graphics_viewport_height() - 35));
  ui_button_resize(renderer, v2(75,25));
  ui_button_set_label(renderer, "Renderer");
  ui_button_disable(renderer);
  
  ui_button* forward_renderer = ui_elem_new("forward_renderer", ui_button);
  ui_button_move(forward_renderer, v2(95, graphics_viewport_height() - 35));
  ui_button_resize(forward_renderer, v2(65,25));
  ui_button_set_label(forward_renderer, "Forward");
  
  ui_button* deferred_renderer = ui_elem_new("deferred_renderer", ui_button);
  ui_button_move(deferred_renderer, v2(170, graphics_viewport_height() - 35));
  ui_button_resize(deferred_renderer, v2(75,25));
  ui_button_set_label(deferred_renderer, "Deferred");
  
  ui_elem_add_event("forward_renderer", switch_renderer_event);
  ui_elem_add_event("deferred_renderer", switch_renderer_event);
  
  ui_button* object = ui_elem_new("object", ui_button);
  ui_button_move(object, v2(10, graphics_viewport_height() - 70));
  ui_button_resize(object, v2(60,25));
  ui_button_set_label(object, "Object");
  ui_button_disable(object);
  
  ui_button* piano = ui_elem_new("piano", ui_button);
  ui_button_move(piano, v2(80, graphics_viewport_height() - 70));
  ui_button_resize(piano, v2(50,25));
  ui_button_set_label(piano, "Piano");
  
  ui_button* cello = ui_elem_new("cello", ui_button);
  ui_button_move(cello, v2(140, graphics_viewport_height() - 70));
  ui_button_resize(cello, v2(50,25));
  ui_button_set_label(cello, "Cello");
  
  ui_button* imrod = ui_elem_new("imrod", ui_button);
  ui_button_move(imrod, v2(200, graphics_viewport_height() - 70));
  ui_button_resize(imrod, v2(50,25));
  ui_button_set_label(imrod, "Imrod");
  
  ui_button* dino = ui_elem_new("dino", ui_button);
  ui_button_move(dino, v2(260, graphics_viewport_height() - 70));
  ui_button_resize(dino, v2(40,25));
  ui_button_set_label(dino, "Dino");
  
  ui_elem_add_event("piano", switch_object_event);
  ui_elem_add_event("cello", switch_object_event);
  ui_elem_add_event("imrod", switch_object_event);
  ui_elem_add_event("dino", switch_object_event);
  
  /* New Camera and light */
  
  camera* cam = entity_new("camera", camera);
  cam->position = v3(25.0, 25.0, 10.0);
  cam->target = v3(0, 5, 0);
  
  light* sun = entity_new("sun", light);
  light_set_type(sun, light_type_spot);
  sun->position = v3(20,23,16);
  sun->ambient_color = v3(0.5, 0.5, 0.5);
  sun->diffuse_color = v3(1.0,  0.894, 0.811);
  sun->specular_color = v3_mul(v3(1.0,  0.894, 0.811), 4);
  sun->power = 5;
  
  light* backlight = entity_new("backlight", light);
  light_set_type(backlight, light_type_point);
  backlight->position = v3(-22,10,-13);
  backlight->ambient_color = v3(0.2, 0.2, 0.2);
  backlight->diffuse_color = v3(0.729, 0.729, 1.0);
  backlight->specular_color = v3_mul(v3(0.729, 0.729, 1.0), 1);
  backlight->power = 2;
  
  /* Renderer Setup */
  
  shadow_mapper_init(sun);
  
  use_deferred = 1;
  swap_renderer();
  
  ik_target = v3(0.0, 0.5, -2.8);
  
}

light* selected_light = NULL;
static void select_light(int x, int y) {
  
  float x_clip =  ((float)x / graphics_viewport_width()) * 2 - 1;
  float y_clip = -(((float)y / graphics_viewport_height()) * 2 - 1);
  
  light* light_ents[10];
  int num_light_ents;
  entities_get(light_ents, &num_light_ents, light);
  
  camera* cam = entity_get("camera");
  matrix_4x4 viewm = camera_view_matrix(cam);
  matrix_4x4 projm = camera_proj_matrix(cam, graphics_viewport_ratio() );
  
  selected_light = NULL;
  float range = 0.1;
  
  for(int i = 0; i < num_light_ents; i++) {
    
    light* l = light_ents[i];
    
    vector4 light_pos = v4(l->position.x, l->position.y, l->position.z, 1);
    light_pos = m44_mul_v4(viewm, light_pos);
    light_pos = m44_mul_v4(projm, light_pos);
    
    light_pos = v4_div(light_pos, light_pos.w);
    
    float distance = v2_dist(v2(light_pos.x, light_pos.y), v2(x_clip, y_clip));
    if (distance < range) {
      range = distance;
      selected_light = l;
      debug("Selected Light %i", i);
    }
  }
  
}

static bool g_down = false;
static bool t_down = false;
static bool w_down = false;
static bool a_down = false;
static bool s_down = false;
static bool d_down = false;

void renderers_event(SDL_Event event) {
  
  camera* cam = entity_get("camera");
  light* sun = entity_get("sun");
  static_object* s_cello = entity_get("cello");

  switch(event.type){
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_g) g_down = true;
    if (event.key.keysym.sym == SDLK_t) t_down = true;
  break;
  
  case SDL_KEYUP:
    if (event.key.keysym.sym == SDLK_g) g_down = false;
    if (event.key.keysym.sym == SDLK_t) t_down = false;
  break;

  case SDL_MOUSEBUTTONDOWN:
    if (event.button.button == SDL_BUTTON_WHEELUP) {
      cam->position = v3_sub(cam->position, v3_normalize(cam->position));
    }
    if (event.button.button == SDL_BUTTON_WHEELDOWN) {
      cam->position = v3_add(cam->position, v3_normalize(cam->position));
    }
  break;
  
  case SDL_MOUSEBUTTONUP:
    
    if (event.button.button == SDL_BUTTON_RIGHT) {
      select_light(event.motion.x, event.motion.y);
    }
    
  break;
  
  case SDL_MOUSEMOTION:
    if (!any_button_pressed) {
      mouse_x = event.motion.xrel;
      mouse_y = event.motion.yrel;
    }
  break;
  }
    
}

void renderers_update() {
  
  camera* cam = entity_get("camera");
  light* sun = entity_get("sun");

  Uint8 keystate = SDL_GetMouseState(NULL, NULL);
  if (keystate & SDL_BUTTON(1)) {
  
    float a1 = -(float)mouse_x * 0.005;
    float a2 = (float)mouse_y * 0.005;
    
    cam->position = m33_mul_v3(m33_rotation_y( a1 ), cam->position );
    
    vector3 rotation_axis = v3_normalize(v3_cross( v3_sub(cam->position, cam->target) , v3(0,1,0) ));
    
    cam->position = m33_mul_v3(m33_rotation_axis_angle(rotation_axis, a2 ), cam->position );
  }
  
  /*
  if (keystate & SDL_BUTTON(3)) {
    
    matrix_4x4 view = camera_view_matrix(cam);
    matrix_4x4 proj = camera_proj_matrix(cam, graphics_viewport_ratio());
    matrix_4x4 inv_view = m44_inverse(view);
    matrix_4x4 inv_proj = m44_inverse(proj);
    
    vector3 ik_screen = ik_target;
    ik_screen = m44_mul_v3(view, ik_screen);
    ik_screen = m44_mul_v3(proj, ik_screen);
    
    ik_screen.x +=  (float)mouse_x * 0.001;
    ik_screen.y += -(float)mouse_y * 0.001;
    
    vector3 ik_world = ik_screen;
    ik_world = m44_mul_v3(inv_proj, ik_world);
    ik_world = m44_mul_v3(inv_view, ik_world);
    
    ik_target = ik_world;
  }
  */
  
  if (g_down && selected_light) {
    vector3 move_dir = v3_sub(cam->target, cam->position); move_dir.y = 0;
    
    vector3 move_forward = v3_normalize(move_dir);
    vector3 move_left = v3_cross(move_forward, v3(0,1,0));
    
    vector3 move = v3_add( v3_mul(move_forward, -mouse_y), v3_mul(move_left, mouse_x));
    
    selected_light->position.x += (float)move.x * 0.1;
    selected_light->position.z += (float)move.z * 0.1;
  }
  
  if (t_down && selected_light) {
    selected_light->position.y += (float)-mouse_y * 0.1;
  }
  
  mouse_x = 0;
  mouse_y = 0;
  
  ui_button* framerate = ui_elem_get("framerate");
  ui_button_set_label(framerate, frame_rate_string());
  
  animated_object* imrod = entity_get("imrod");
  animated_object_update(imrod, 0.1);
  
  /*
  bone* thigh_r = skeleton_bone_name(imrod->pose, "thigh_r");
  bone* foot_r = skeleton_bone_name(imrod->pose, "foot_r");
  matrix_4x4 inv_world = m44_inverse(m44_world(imrod->position, imrod->scale, imrod->rotation));
  
  vector3 local_target = m44_mul_v3(inv_world, ik_target);
  
  inverse_kinematics_solve(thigh_r, foot_r, local_target);
  */
  
}

void renderers_render() {

  light* sun = entity_get("sun");
  light* backlight = entity_get("backlight");

  static_object* s_podium = entity_get("podium");
  
  static_object* s_piano = entity_get("piano");
  static_object* s_cello = entity_get("cello");
  static_object* s_dino = entity_get("dino");
  animated_object* a_imrod = entity_get("imrod");
  
  shadow_mapper_begin();
  shadow_mapper_render_static(s_podium);
  if (object_id == 0) {
    shadow_mapper_render_static(s_cello);
  } else if (object_id == 1) { 
    shadow_mapper_render_static(s_piano);
  } else if (object_id == 2) {
    shadow_mapper_render_animated(a_imrod);
  } else if (object_id == 3) {
    shadow_mapper_render_static(s_dino);
  }
  shadow_mapper_end();
  
  if (use_deferred) {
    
    deferred_renderer_begin();
    deferred_renderer_render_static(s_podium);
    
    if (object_id == 0) {
      deferred_renderer_render_static(s_cello);
    } else if (object_id == 1) {
      deferred_renderer_render_static(s_piano);
    } else if (object_id == 2) {
      deferred_renderer_render_animated(a_imrod);
    } else if (object_id == 3) {
      deferred_renderer_render_static(s_dino);
    }
    
    deferred_renderer_render_light(sun);
    deferred_renderer_render_light(backlight);
    
    if (selected_light != NULL) {
      deferred_renderer_render_axis(m44_world(selected_light->position, v3_one(), v4_quaternion_id()));
    }
    
    deferred_renderer_end();
    
  } else {
  
    forward_renderer_begin();
    
    forward_renderer_render_static(s_podium);
    
    if (object_id == 0) {
      forward_renderer_render_static(s_cello);
    } else if (object_id == 1) {
      forward_renderer_render_static(s_piano);
    } else if (object_id == 2) {
      forward_renderer_render_animated(a_imrod);
      
      matrix_4x4 foot_pos = bone_transform(skeleton_bone_name(a_imrod->pose, "foot_r"));
      
      /*
      vector3 pos = m44_mul_v3(foot_pos, v3_zero());
      pos = m44_mul_v3(m44_world(a_imrod->position, a_imrod->scale, a_imrod->rotation), pos);
      
      glDisable(GL_DEPTH_TEST);
      glPointSize(5.0);
      glBegin(GL_POINTS);
        glColor3f(0,0,1);
        glVertex3f(pos.x, pos.y, pos.z);
        glColor3f(0,1,0);
        glVertex3f(ik_target.x, ik_target.y, ik_target.z);
      glEnd();
      glColor3f(1,1,1);
      glPointSize(1.0);
      glEnable(GL_DEPTH_TEST);
      */
      
    } else if (object_id == 3) {
      forward_renderer_render_static(s_dino);
    }
    
    forward_renderer_render_light(sun);
    forward_renderer_render_light(backlight);
    
    if (selected_light != NULL) {
      forward_renderer_render_axis(m44_world(selected_light->position, v3_one(), v4_quaternion_id()));
    }
    
    forward_renderer_end();
  }
  
}

void renderers_finish() {
  
  if (use_deferred) {
    deferred_renderer_finish();
  } else {
    forward_renderer_finish();
  }

  shadow_mapper_finish();
  
}

int main(int argc, char **argv) {
  
  corange_init("../../core_assets");
  
  renderers_init();
  
  int running = 1;
  SDL_Event event;
  
  while(running) {
    
    frame_begin();
    
    while(SDL_PollEvent(&event)) {
      switch(event.type){
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_ESCAPE) { running = 0; }
        if (event.key.keysym.sym == SDLK_PRINT) { graphics_viewport_screenshot(); }
        break;
      case SDL_QUIT:
        running = 0;
        break;
      }
      renderers_event(event);
      ui_event(event);
    }
    
    renderers_update();
    ui_update();
    
    renderers_render();
    ui_render();
    SDL_GL_SwapBuffers(); 
    
    frame_end();
  }
  
  renderers_finish();
  
  corange_finish();
  
  return 0;
}
