//
//  LRay -- Lua-scriptable Ray Tracer
//  Simon Broadhead
//

#include <windows.h>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/timer.hpp>
#include <boost/filesystem.hpp>
#include <omp.h>

#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>

#include "common.h"
#include "math/math.h"
#include "image/image.h"
#include "image/targa.h"
#include "renderer/scene.h"
#include "renderer/accel/kdtree.h"
#include "renderer/raytrace.h"
#include "luascene/luascene.h"

namespace fs = boost::filesystem3;

//
//

int image_width, image_height;
int num_threads;
unsigned char *pixels;
raytrace::imaging::Image *image;

//
//

void render_scene(boost::shared_ptr<raytrace::LuaScene> lua_scene,
  raytrace::SceneInfo scene_info, std::string const& path, 
  std::string const& stem)
{
  using namespace raytrace;

  boost::timer timer;

  // Rendering animation simply evaluates the Lua script at various clock
  // values and renders each one individually
  for (int i = 0; i < scene_info.frames; ++i) {
    double clock;
    
    if (scene_info.frames > 1) {
      clock = (i/((double)scene_info.frames-1))
        * (scene_info.end_clock - scene_info.start_clock)
        + scene_info.start_clock;
    } else {
      clock = scene_info.start_clock;
    }

    std::cout << "FRAME [" << std::setw(4) << (i + 1) << "; clock="
      << std::setprecision(5) << std::fixed << clock << "] ";
    boost::shared_ptr<Scene> s = lua_scene->SceneAt(clock);
    if (!s) {
      Error("Nothing to render in frame %d!", i);
    }

    // Build a kd-tree for acceleration
    KdTree kdtree(s->Entities());

    int j = 0;
    int total = 0;
    double progress = 0.0;
    double progress_step = 0.25;
    int image_size = scene_info.image_width * scene_info.image_height;

    // Each block of pixels can be done in a separate thread
#pragma omp parallel shared(image, pixels, scene_info, image_size, progress, total) private(j)
    {
#pragma omp single
      std::cout << " [Rendering; " << omp_get_num_threads() << " thread(s)] ";

      // Give 30 pixel blocks to each thread
#pragma omp for schedule (static,30)
      for (j = 0; j < image_size; ++j) {
#pragma omp critical
        {
          ++total;
          if ((double)total / image_size > progress) {
            std::cout << (int)(progress * 100) << "%.." << std::flush;
            progress += progress_step;
          }
        }

        int x = j % scene_info.image_width;
        int y = j / scene_info.image_width;
        
        Color c = RayTracePixel(*s, kdtree,
          scene_info.image_width, scene_info.image_height, x, y, 16,
          scene_info.samples);

        // Fill in the array that OpenGL will display as we are rendering.
        // We don't care about thread synchronization, since two threads will
        // never write to the same array position.
        unsigned char *p = &pixels[4*(y*scene_info.image_width+x)];
        *p++ = c.r * 255;
        *p++ = c.g * 255;
        *p++ = c.b * 255;

        image->set_pixel(x, y, c);
      }
    }
    std::cout << "100%\n";

    // Build the output path
    string filename;
    if (scene_info.frames > 1) {
      filename = (boost::format("%s%04d.tga") % stem % i).str();
    } else {
      filename = string(stem) + ".tga";
    }
    fs::path output_path(path);
    output_path /= filename;

    // Write the rendered image to disk
    write_targa(output_path.string(), *image);
  }

  std::cout << "Done! Total time taken was "
    << std::setprecision(2) << timer.elapsed() << "sec.\n";

  boost::this_thread::sleep(boost::posix_time::seconds(5));
  exit(0);
}

// The window was resized
void reshape(int width, int height) {
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0.0, width, height, 0.0);
  glMatrixMode(GL_MODELVIEW);
}

// Write the raw pixels to the window
void display() {
  glDrawPixels(image_width, image_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glutSwapBuffers();
  glutPostRedisplay();
}

void init() {
  pixels = new unsigned char[image_width * image_height * 4];
  for (int i = 0; i < image_width * image_height * 4; i+=4) {
    pixels[i] = 32;
    pixels[i+1] = 32;
    pixels[i+2] = 32;
    pixels[i+3] = 255;
  }

  image = new raytrace::imaging::Image(image_width, image_height);
}

//////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
  std::cout << "LRay - Lua Scriptable Ray Tracer\n"
               "By Simon Broadhead\n\n";

  // Handle command line input
  //
  if (argc != 3) {
    std::cout << "Usage:\n"
      "raytrace <infile> <outdir>\n\n";
    return 1;
  }

  // Read the scene file
  //
  string input_file = argv[1];
  string output_dir = argv[2];
  fs::path input_path(input_file);
  string stem = input_path.stem().string();
 
  if (!fs::exists(input_path)) {
    std::cout << "The file '" << input_path.string() << "' does not exist.\n";
    return 1;
  }

  // Verify that the required support files exist in the working directory
  const char *required[] = { "luascene.lua", "cube.obj" };
  for (int i = 0; i < sizeof(required)/sizeof(required[0]); ++i) {
    if (!fs::exists(required[i])) {
      std::cout << "The required support file '" << required[i] <<
        "' could not be found in the working directory. Make sure they are "
        "present in the directory that the program is being invoked from.";
      return 1;
    }
  }

  std::cout << "Reading scene file...\n";
  raytrace::SceneInfo info;
  boost::shared_ptr<raytrace::LuaScene> lua_scene =
    raytrace::LuaScene::ReadSceneFile(input_file.c_str(), &info);

  image_height = info.image_height;
  image_width = info.image_width;

  if (!lua_scene) {
    std::cout << "The Lua script failed to produce a renderable scene.\n";
    return 1;
  } else {
    std::cout << "OK, rendering!\n";

    // Set up the rendering environment
    init();

    // Set up GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(image_width, image_height);


    glutCreateWindow("LRay");

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    // Launch the rendering in a second thread so that GLUT can
    // have the main thread for its rendering loop
    boost::thread t(render_scene, lua_scene, info, output_dir, stem);

    glutMainLoop();
  }

  return 0;
}