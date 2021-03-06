#ifdef _WIN32
  #include "SDL/SDL_syswm.h"
#endif

#include "bool.h"
#include "error.h"
#include "timing.h"

#include "assets/image.h"

#include "graphics_manager.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

static SDL_Surface* screen;

static int window_width;
static int window_height;

static int window_flags;
static bool window_vsync;
static int window_multisamples;
static int window_multisamplesbuffs;

static void graphics_viewport_start() {
  
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, window_vsync);
  
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, window_multisamples);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, window_multisamplesbuffs);
  
  screen = SDL_SetVideoMode(window_width, window_height, 0, window_flags);
  if (screen == NULL) {
    char* errorstring = SDL_GetError();
    error("Could not create SDL window: %s", errorstring);
  }

  glViewport(0, 0, window_width, window_height);
  
}

void graphics_manager_init() {

  int error = SDL_Init(SDL_INIT_VIDEO);
  if (error == -1) {
    error("Cannot initialize SDL video!");
  }

  window_width = DEFAULT_WIDTH;
  window_height = DEFAULT_HEIGHT;
  window_flags = SDL_OPENGL;
  window_vsync = 1;
  window_multisamples = 4;
  window_multisamplesbuffs = 1;
  
  graphics_viewport_set_title("Corange");
  graphics_viewport_start();
  
  SDL_GL_PrintInfo();
  SDL_GL_LoadExtensions();

  SDL_WM_UseResourceIcon();
}

void graphics_manager_finish() {
  
  SDL_WM_DeleteResourceIcon();
  
  SDL_FreeSurface(screen);

}

void graphics_set_multisamples(int multisamples) {
  window_multisamples = multisamples;
  if (multisamples > 0) {
    window_multisamplesbuffs = 1;
  } else {
    window_multisamplesbuffs = 0;
  }
}

int graphics_get_multisamples() {
  return window_multisamples;
}

void graphics_viewport_restart() {
  
  SDL_WM_CreateTempContext();
  
  graphics_viewport_start();
  
  SDL_WM_DeleteTempContext();
  
}

void graphics_viewport_set_title(char* title) {
  SDL_WM_SetCaption(title, title);    
}

void graphics_set_vsync(bool vsync) {
  window_vsync = vsync;
  graphics_viewport_restart();
}

void graphics_set_fullscreen(bool fullscreen) {

  if (fullscreen) {
    window_flags |= SDL_FULLSCREEN;
  } else {
    window_flags &= !SDL_FULLSCREEN;
  }
  
  graphics_viewport_restart();
}

bool graphics_get_vsync() {
  return window_vsync;
}

bool graphics_get_fullscreen() {
  if (window_flags & SDL_FULLSCREEN) {
    return true;
  } else {
    return false;
  }
}

void graphics_viewport_set_height(int height) {
  window_height = height;
  graphics_viewport_restart();
}

void graphics_viewport_set_width(int width) {
  window_width = width;
  graphics_viewport_restart();
}

void graphics_viewport_set_dimensions(int width, int height) {
  window_width = width;
  window_height = height;
  graphics_viewport_restart();
}

int graphics_viewport_height() {
  return window_height;
}

int graphics_viewport_width() {
  return window_width;
}

float graphics_viewport_ratio() {
  return (float)window_height / (float)window_width;
}

static char timestamp_string[64];
static char screenshot_string[256];

void graphics_viewport_screenshot() {
  
  unsigned char* image_data = malloc( sizeof(unsigned char) * graphics_viewport_width() * graphics_viewport_height() * 4 );
  glReadPixels( 0, 0, graphics_viewport_width(), graphics_viewport_height(), GL_RGBA, GL_UNSIGNED_BYTE, image_data ); 
  
  image* i = image_new(graphics_viewport_width(), graphics_viewport_height(), image_data);
  
  free(image_data);
  
  timestamp_sm(timestamp_string);

  screenshot_string[0] = '\0';
  strcat(screenshot_string, "./corange_");
  strcat(screenshot_string, timestamp_string);
  strcat(screenshot_string, ".tga");
  
  image_write_to_file(i, screenshot_string);
  
  image_delete(i);
  
}
