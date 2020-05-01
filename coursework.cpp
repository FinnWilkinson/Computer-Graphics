#include "Header.h"

using namespace std;
using namespace glm;

#define WIDTH 640
#define HEIGHT 480

void handleEvent(SDL_Event event);
void flythrough_and_rotate();
void animateSimple();
void animateFancy();
void reDrawFrame(int frame_count);
void PPMWrite(int frameCount);
void combineScene();


// --- FEATURES --- //
DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
Camera camera(0.0f, 2.5f, 30.0f);
Mesh cornell_box("Models/cornell-box/cornell-box.obj", "Models/cornell-box/cornell-box.mtl", "Models/HackspaceLogo/test.ppm", 1.0f, 1.0f, 1.0f);
Mesh hackspace("Models/HackspaceLogo/logo.obj", "Models/HackspaceLogo/materials.mtl", "Models/HackspaceLogo/test.ppm", 0.01f, 0.01f, 0.01f);
Mesh sphere1("Models/Sphere/sphere.obj", "Models/Sphere/sphere.mtl", 0.06f, 0.06f, 0.06f);
Mesh sphere2("Models/Sphere/sphere2.obj", "Models/Sphere/sphere2.mtl", 0.03f, 0.03f, 0.03f);

//Mesh generatedModel("Models/cornell-box/checker.ppm", 1.0f, 1.0f, 1.0f, 33);

Rasteriser rasteriser;
Raytracer raytracer;

bool screenUpdate = false;

int main(int argc, char *argv[])
{
  combineScene();
  rasteriser = Rasteriser(cornell_box, window);
  raytracer = Raytracer(cornell_box, vec3(-0.24f, 5.0f, 2.0f), 500.0f);
  SDL_Event event;
  int succ = system("clear");
  if(succ == 0) cout << "---- COMPUTER GRAPHICS 2020 ----" << endl;
  while(true)
  {
    if(window.pollForInputEvents(&event)) handleEvent(event);
    if(screenUpdate){
      window.clearPixels();
      if(rasteriser.wireframeOn || rasteriser.rasteriseOn) rasteriser.draw(camera, window);
      if(raytracer.raytracerOn) raytracer.render(window, camera);
      screenUpdate = false;
     }
    window.renderFrame();
  }
  return 0;
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
    if(event.key.keysym.sym == SDLK_c){
      cout << "CLEAR PIXELS" << endl;
      window.clearPixels();
    }
    else if(event.key.keysym.sym == SDLK_o){
      camera.orbit();
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_UP){
      camera.performTranslation(0.01f, 'y');
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_DOWN){
      camera.performTranslation(-0.01f, 'y');
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_RIGHT){
      camera.performTranslation(-0.01f, 'x');
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_LEFT){
      camera.performTranslation(0.01f, 'x');
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_z){
      camera.performTranslation(-0.1f, 'z');
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_x){
      camera.performTranslation(0.1f, 'z');
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_KP_8){
      camera.performRotation(-1.0f, 'x');
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_KP_2){
      camera.performRotation(1.0f, 'x');
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_KP_6){
      camera.performRotation(-1.0f, 'y');
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_KP_4){
      camera.performRotation(1.0f, 'y');
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_2){
      cout << "DRAW WIREFRAME" << endl;
      rasteriser.wireframeOn = true;
      rasteriser.rasteriseOn = false;
      raytracer.raytracerOn = false;
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_3){
      cout << "FILL ON" << endl;
      rasteriser.wireframeOn = true;
      rasteriser.rasteriseOn = true;
      raytracer.raytracerOn = false;
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_4){
      std::cout << "RAYTRACER ON" << std::endl;
      rasteriser.wireframeOn = false;
      rasteriser.rasteriseOn = false;
      raytracer.raytracerOn = true;
      screenUpdate = true;
    }
    else if(event.key.keysym.sym == SDLK_r){
      PPMWrite(0);
      cout << "PPM FILE WRITE COMPLETE" << endl;
    }
    else if(event.key.keysym.sym == SDLK_a){
      cout << "ANIMATION STARTED" << endl;
      if(raytracer.raytracerOn == true){
        animateFancy();
      }
      else{
        animateSimple();
      }
      cout << "ANIMATION ENDED" << endl;
    }
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN) cout << "MOUSE CLICK" << endl;
}

void flythrough_and_rotate()
{
  int frame_count = 0;
  PPMWrite(frame_count);
  frame_count++;
  for(int i = 0; i < 50; i++)
  {
    raytracer.light.position.z -= 0.1f;
    reDrawFrame(frame_count);
    frame_count++;
  }
  camera.orbit();
  for(int i = 0; i < 125; i++)
  {
    camera.performRotation(2.88f, 'y');
    reDrawFrame(frame_count);
    frame_count++;
  }
  for(int i = 0; i < 125; i++)
  {
    camera.performTranslation(-0.48f, 'z');
    reDrawFrame(frame_count);
    frame_count++;
  }
}

void animateSimple()
{
  int frame_count = 0;
  //int sizeOfGrid = 33;
  //move left then right then centre
  for (int i = 0; i < 25; i++) {
    camera.performTranslation(-0.1f, 'x');
    //rasteriser.animateModel(frame_count, sizeOfGrid);
    reDrawFrame(frame_count);
    frame_count++;
  }
  for (int i = 0; i < 50; i++) {
    camera.performTranslation(0.1f, 'x');
    //rasteriser.animateModel(frame_count, sizeOfGrid);
    reDrawFrame(frame_count);
    frame_count++;
  }
  for (int i = 0; i < 25; i++) {
    camera.performTranslation(-0.1f, 'x');
    //rasteriser.animateModel(frame_count, sizeOfGrid);
    reDrawFrame(frame_count);
    frame_count++;
  }

  //move down then up then centre
  for (int i = 0; i < 25; i++) {
    camera.performTranslation(-0.1f, 'y');
    //rasteriser.animateModel(frame_count, sizeOfGrid);
    reDrawFrame(frame_count);
    frame_count++;
  }
  for (int i = 0; i < 50; i++) {
    camera.performTranslation(0.1f, 'y');
    //rasteriser.animateModel(frame_count, sizeOfGrid);
    reDrawFrame(frame_count);
    frame_count++;
  }
  for (int i = 0; i < 25; i++) {
    camera.performTranslation(-0.1f, 'y');
    //rasteriser.animateModel(frame_count, sizeOfGrid);
    reDrawFrame(frame_count);
    frame_count++;
  }

  //zoom out until culled
  for (int i = 0; i < 30; i++) {
    camera.performTranslation(1.0f, 'z');
    //rasteriser.animateModel(frame_count, sizeOfGrid);
    reDrawFrame(frame_count);
    frame_count++;
  }
  //zoom in until culled
  for (int i = 0; i < 70; i++) {
    camera.performTranslation(-1.0f, 'z');
    //rasteriser.animateModel(frame_count, sizeOfGrid);
    reDrawFrame(frame_count);
    frame_count++;
  }
}

void animateFancy()
{
  int frame_count = 0;
  camera.orbit();
  for (int i = 0; i < 25; i++) {
    camera.performRotation(1.2f, 'y');
    raytracer.animateLight(frame_count);
    reDrawFrame(frame_count);
    frame_count++;
  }
  for (int i = 0; i < 50; i++) {
    camera.performRotation(-1.2f, 'y');
    raytracer.animateLight(frame_count);
    reDrawFrame(frame_count);
    frame_count++;
  }
  for (int i = 0; i < 25; i++) {
    camera.performRotation(1.2f, 'y');
    raytracer.animateLight(frame_count);
    reDrawFrame(frame_count);
    frame_count++;
  }
  for (int i = 0; i < 200; i++) {
    camera.performRotation(1.8f, 'y');
    raytracer.animateLight(frame_count);
    reDrawFrame(frame_count);
    frame_count++;
  }
  camera.orbit();
}

void reDrawFrame(int frame_count)
{
  window.clearPixels();
  if(rasteriser.wireframeOn || rasteriser.rasteriseOn) rasteriser.draw(camera, window);
  if(raytracer.raytracerOn) raytracer.render(window, camera);
  window.renderFrame();
  PPMWrite(frame_count);
}

void PPMWrite(int frameCount)
{
    std::string filename = "frame_" + std::to_string(frameCount);
    std::ofstream file;
    file.open(filename, std::ios::out | std::ios::binary);
    file.clear();
    file << "P6" << "\n";
    file << std::to_string(window.width) << " " << std::to_string(window.height) << "\n";
    file << "255" << "\n";

    char* pixels =  (char*)malloc(3 * window.width * window.height * sizeof(char));
    int index = 0;

    for(int y = 0; y < window.height; y++)
    {
        for(int x = 0; x < window.width; x++)
        {
            uint32_t pixel_colour = window.getPixelColour(x, y);
            int red = (pixel_colour >> 16) & 0xFF;
            int green = (pixel_colour >> 8) & 0xFF;
            int blue = (pixel_colour) & 0xFF;

            pixels[index] = red;
            pixels[index + 1] = green;
            pixels[index + 2] = blue;

            index +=3;
        }
    }

    file.write(pixels, (3 * window.width * window.height));
    file.close();
    frameCount++;
}

void combineScene()
{
  // //move hackspace logo
  hackspace.repositionModel(-5.5f, 'z');
  hackspace.repositionModel(-3.0f, 'x');
  hackspace.updateBoundingIndexes(cornell_box.model.size(), cornell_box.verts.size());
  cornell_box.model.insert(cornell_box.model.end(), hackspace.model.begin(), hackspace.model.end());
  cornell_box.boundingBoxes.insert(cornell_box.boundingBoxes.end(), hackspace.boundingBoxes.begin(), hackspace.boundingBoxes.end());
  cornell_box.verts.insert(cornell_box.verts.end(), hackspace.verts.begin(), hackspace.verts.end());

  //import Sphere 1
  sphere1.repositionModel(-1.5f, 'z');
  sphere1.repositionModel(-1.55f, 'x');
  sphere1.repositionModel(1.1f, 'y');
  sphere1.updateBoundingIndexes(cornell_box.model.size(), cornell_box.verts.size());
  cornell_box.model.insert(cornell_box.model.end(), sphere1.model.begin(), sphere1.model.end());
  cornell_box.boundingBoxes.insert(cornell_box.boundingBoxes.end(), sphere1.boundingBoxes.begin(), sphere1.boundingBoxes.end());
  cornell_box.verts.insert(cornell_box.verts.end(), sphere1.verts.begin(), sphere1.verts.end());

  sphere2.repositionModel(-1.5f, 'z');
  sphere2.repositionModel(1.0f, 'x');
  sphere2.repositionModel(2.1f, 'y');
  sphere2.updateBoundingIndexes(cornell_box.model.size(), cornell_box.verts.size());
  cornell_box.model.insert(cornell_box.model.end(), sphere2.model.begin(), sphere2.model.end());
  cornell_box.boundingBoxes.insert(cornell_box.boundingBoxes.end(), sphere2.boundingBoxes.begin(), sphere2.boundingBoxes.end());
  cornell_box.verts.insert(cornell_box.verts.end(), sphere2.verts.begin(), sphere2.verts.end());

  // import generatedModel
  //  generatedModel.repositionModel(-7.0f, 'y');
  //  generatedModel.updateBoundingIndexes(cornell_box.model.size(), cornell_box.verts.size());
  //  cornell_box.model.insert(cornell_box.model.end(), generatedModel.model.begin(), generatedModel.model.end());
  //  cornell_box.boundingBoxes.insert(cornell_box.boundingBoxes.end(), generatedModel.boundingBoxes.begin(), generatedModel.boundingBoxes.end());
  //  cornell_box.verts.insert(cornell_box.verts.end(), generatedModel.verts.begin(), generatedModel.verts.end());

   cornell_box.originalModel = cornell_box.model;

}
