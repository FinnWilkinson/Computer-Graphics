#ifndef MESH_H
#define MESH_H

#include "Header.h"

class Mesh
{
    public:

        std::string obj_file;
        std::string mtl_file;
        Texture texture;
        bool textured;

        glm::vec3 scaling;
        std::vector<std::string> colour_strings;
        std::vector<Colour> colour_vals;
        std::vector<Vertex> verts;
        std::vector<ModelTriangle> model;
        std::vector<TexturePoint> texturePoints;
        std::vector<glm::vec3> vertexNormals;
        std::vector<BoundingBox> boundingBoxes;

        std::vector<ModelTriangle> originalModel;

        int frameCount;
        int modelSize;

        Mesh()
        {

        }

        Mesh(std::string texture_file, float x_scale, float y_scale, float z_scale, int gridSize)
        {
          this->scaling = glm::vec3(x_scale, y_scale, z_scale);
          this->frameCount = 0;
          this->frameCount = 0;
          boundingBoxes.push_back(BoundingBox());
          GenerativeGeometry(-7.0f, 7.0f, 0, 5, -26.0f, 3.0f, 33);
          originalModel = model;
          std::cout << "----- READY -----" << std::endl;
        }

        Mesh(std::string obj_file, std::string mtl_file, float x_scale, float y_scale, float z_scale)
        {
            this->obj_file = obj_file;
            this->mtl_file = mtl_file;
            textured = false;
            this->scaling = glm::vec3(x_scale, y_scale, z_scale);
            this->frameCount = 0;
            getModel();
            originalModel = model;
            std::cout << "----- READY -----" << std::endl;
        }

        Mesh(std::string obj_file, std::string mtl_file, std::string texture_file, float x_scale, float y_scale, float z_scale)
        {
          this->obj_file = obj_file;
          this->mtl_file = mtl_file;
          texture = Texture(texture_file);
          textured = true;
          this->scaling = glm::vec3(x_scale, y_scale, z_scale);
          this->frameCount = 0;
          getModel();
          originalModel = model;
          std::cout << "----- READY -----" << std::endl;
        }

        void GenerativeGeometry(float minX, float maxX, int minY, int maxY, float minZ, float maxZ, int modelSize)
        {
          this->verts.clear();
          this->model.clear();
          this->modelSize = modelSize;
          generateModel(minX, maxX, minY, maxY, minZ, maxZ);
        }

        void cullTris(Camera& camera, DrawingWindow& window)
        {
          model = originalModel;
          //bounding box culling
          for (int i = 0; i < (int) boundingBoxes.size(); i++) {
            if(boundingBoxes[i].culling(camera, window) == false){
              for (int j = 0; j < (int) boundingBoxes[i].containedTriIndexes.size(); j++) {
                model[boundingBoxes[i].containedTriIndexes[j]].visible = false;
              }
            }
          }
          nearFarCulling(camera, window);
          backFaceCulling(camera);
          //triFragmentation(camera, window);
        }

        void repositionModel(const float& value, const char& axis)
        {
          std::cout << "REPOSITION MODEL" << std::endl;
          //tris in model
          for(int i = 0; i < (int) model.size(); i++){
            for(int j = 0; j < 3; j++){
              if(axis == 'x') model[i].vertices[j].x += value;
              else if(axis == 'y') model[i].vertices[j].y += value;
              else if(axis == 'z') model[i].vertices[j].z += value;
            }
          }
          //verts of model
          for(int i = 0; i < (int) verts.size(); i++){
            if(axis == 'x') verts[i].position.x += value;
            else if(axis == 'y') verts[i].position.y += value;
            else if(axis == 'z') verts[i].position.z += value;
          }
          //bounding box values
          for (int i = 0; i < (int) boundingBoxes.size(); i++) {
            for (int j = 0; j < (int) boundingBoxes[i].triVerts.size(); j++) {
              if(axis == 'x') boundingBoxes[i].triVerts[j].x += value;
              else if(axis == 'y') boundingBoxes[i].triVerts[j].y += value;
              else if(axis == 'z') boundingBoxes[i].triVerts[j].z += value;
            }
            boundingBoxes[i].updateBoxCoords();
          }

          std::cout << "DONE" << std::endl;
        }

        //when combining models, the indecies of the trianlges, verts it contains need to be updated
        void updateBoundingIndexes(int triShiftValue, int vertShiftValue)
        {
          for (int i = 0; i < (int) boundingBoxes.size(); i++) {
            for (int j = 0; j < (int) boundingBoxes[i].containedTriIndexes.size(); j++) {
              boundingBoxes[i].containedTriIndexes[j] += triShiftValue;
            }
          }
          for (int i = 0; i < (int) boundingBoxes.size(); i++) {
            for (int j = 0; j < (int) boundingBoxes[i].containedVertIndexes.size(); j++) {
              boundingBoxes[i].containedVertIndexes[j] += vertShiftValue;
            }
          }
          for (int i = 0; i < (int) verts.size(); i++) {
            for (int j = 0; j < (int) verts[i].triangles.size(); j++) {
              verts[i].triangles[j] += triShiftValue;
            }
          }
        }

    private:

        void getModel()
        {
            getColourStrings();
            getColourVals();
            getVerts();

            int boundingIndex = -1;
            std::ifstream obj_stream;
            char buffer[256];
            obj_stream.open(this->obj_file, std::ifstream::in);
            char delim = ' ';
            int colour_index = -1;
            int len = colour_vals.size();
            int triangle_index = 0;

            while(obj_stream.getline(buffer, 256))
            {
                std::string line(buffer);
                if(line.substr(0,1).compare("o") == 0){
                  boundingBoxes.push_back(BoundingBox());
                  boundingIndex++;
                }
                if(line.substr(0, 6).compare("usemtl") == 0){
                    std::string colour = split(line, delim)[1];
                    for(int i = 0; i < len; i++){
                        if(colour_vals[i].name == colour){
                            colour_index = i;
                        }
                    }
                }
                else if(line.substr(0, 1).compare("f") == 0){

                  std::string vertex1 = split(line, delim)[1];
                  std::string vertex2 = split(line, delim)[2];
                  std::string vertex3 = split(line, delim)[3];

                  int v1 = std::stoi(split(vertex1, '/')[0]) - 1;
                  int v2 = std::stoi(split(vertex2, '/')[0]) - 1;
                  int v3 = std::stoi(split(vertex3, '/')[0]) - 1;

                  ModelTriangle triangle(verts[v1].position, verts[v2].position, verts[v3].position);
                  if(colour_index != -1) triangle.colour = colour_vals[colour_index];

                  if(!(split(vertex1, '/')[1].empty()) && texturePoints.size() > 0 && textured){
                    int t1 = std::stoi(split(vertex1, '/')[1]) - 1;
                    int t2 = std::stoi(split(vertex2, '/')[1]) - 1;
                    int t3 = std::stoi(split(vertex3, '/')[1]) - 1;
                    triangle.texture_points[0] = texturePoints[t1];
                    triangle.texture_points[1] = texturePoints[t2];
                    triangle.texture_points[2] = texturePoints[t3];
                    triangle.textured = true;
                  }

                  if(vertexNormals.size() > 0){
                    int n1 = std::stoi(split(vertex1, '/')[2]) - 1;
                    int n2 = std::stoi(split(vertex2, '/')[2]) - 1;
                    int n3 = std::stoi(split(vertex3, '/')[2]) - 1;
                    triangle.vertex_normals[0] = vertexNormals[n1];
                    triangle.vertex_normals[1] = vertexNormals[n2];
                    triangle.vertex_normals[2] = vertexNormals[n3];
                  }

                  //add triangle to correct bounding box
                  if(boundingIndex >= 0) boundingBoxes[boundingIndex].addTri(triangle, triangle_index, v1, v2, v3);
                  model.push_back(triangle);
                  verts[v1].triangles.push_back(triangle_index);
                  verts[v2].triangles.push_back(triangle_index);
                  verts[v3].triangles.push_back(triangle_index);
                  triangle_index++;
                }
            }
            if(vertexNormals.size() == 0) getVertexNormals();
            //update all bounding boxes
            if(boundingIndex >= 0){
              for (int i = 0; i < (int) boundingBoxes.size(); i++) {
                  boundingBoxes[i].updateBoxCoords();
              }
            }
        }

        void getColourStrings()
        {
            std::ifstream obj_stream;
            char buffer[256];
            obj_stream.open(this->obj_file, std::ifstream::in);
            std::string sub_str = "";
            char delim = ' ';

            while(obj_stream.getline(buffer, 256))
            {
                std::string line(buffer);
                sub_str = line.substr(0, 6);
                if(sub_str.compare("usemtl") == 0){
                    colour_strings.push_back(split(line, delim)[1]);
                }
            }
        }

        void getColourVals()
        {
            std::ifstream mtl_stream;
            char buffer[256];
            char delim = ' ';
            int length = colour_strings.size();
            std::string newmtl_str;
            float red, green, blue;
            mtl_stream.open(this->mtl_file, std::ifstream::in);

            while(mtl_stream.getline(buffer, 256)){
                std::string line(buffer);
                std::string newmtl_str = split(line, delim)[0];
                if(newmtl_str.compare("newmtl") == 0){
                    for(int i = 0; i < length; i++){
                        if(split(line, delim)[1] == colour_strings[i]){
                            std::string name(split(line, delim)[1]);
                            mtl_stream.getline(buffer, 256);
                            std::string vals(buffer);
                            red = std::stof(split(vals, delim)[1]) * 255;
                            green = std::stof(split(vals, delim)[2]) * 255;
                            blue = std::stof(split(vals, delim)[3]) * 255;
                            Colour newColour(name, red, green, blue);
                            std::string surf_type(split(vals, delim)[0]);
                            if(surf_type.compare("Ks") == 0){
                              newColour.specular = true;
                              mtl_stream.getline(buffer, 256);
                              std::string exponent(buffer);
                              newColour.specular_exponent = std::stof(split(exponent, delim)[1]);
                              mtl_stream.getline(buffer, 256);
                              std::string reflective_index(buffer);
                              newColour.reflective_index = std::stof(split(reflective_index, delim)[1]);
                              mtl_stream.getline(buffer, 256);
                              std::string transparency(buffer);
                              newColour.transparency = std::stof(split(transparency, delim)[1]);
                              mtl_stream.getline(buffer, 256);
                              std::string refractive_index(buffer);
                              newColour.refractive_index = std::stof(split(refractive_index, delim)[1]);
                            }
                            colour_vals.push_back(newColour);
                        }
                    }
                }
            }
        }

        void getVerts()
        {
            std::ifstream obj_stream;
            char buffer[256];
            char delim = ' ';
            obj_stream.open(this->obj_file, std::ifstream::in);

            while(obj_stream.getline(buffer, 256))
            {
                std::string line(buffer);
                if(line.substr(0, 1).compare("v") == 0){
                  if(line.substr(1, 1).compare("t") == 0){
                    float x = std::stof(split(line, delim)[1]);
                    float y = std::stof(split(line, delim)[2]);
                    TexturePoint point(x, y);
                    texturePoints.push_back(point);
                  }
                  else if(line.substr(1, 1).compare("n") == 0){
                    float x = std::stof(split(line, delim)[1]);
                    float y = std::stof(split(line, delim)[2]);
                    float z = std::stof(split(line, delim)[3]);
                    glm::vec3 normal(x, y, z);
                    vertexNormals.push_back(normal);
                  }
                  else{
                    float x = std::stof(split(line, delim)[1]);
                    float y = std::stof(split(line, delim)[2]);
                    double z = std::stod(split(line, delim)[3]);
                    Vertex vert(glm::vec3(x, y, z) * scaling);
                    verts.push_back(vert);
                  }
                }
            }
        }

        void getVertexNormals()
        {
            int len = verts.size();

            for(int i = 0; i < len; i++){
                Vertex vert = verts[i];
                glm::vec3 vert_normal(0.0f, 0.0f, 0.0f);

                float no_triangles = vert.triangles.size();

                for(int j = 0.0f; j < no_triangles; j++){
                    vert_normal += model[vert.triangles[j]].normal;
                }

                vert_normal = vert_normal * (1.0f / no_triangles);
                vert_normal = glm::normalize(vert_normal);

                for(int j = 0; j < no_triangles; j++){
                    for(int n = 0; n < 3; n++){
                        if(model[vert.triangles[j]].vertices[n] == vert.position){
                            model[vert.triangles[j]].vertex_normals[n] = vert_normal;
                        }
                    }
                }
            }
        }

        void generateModel(float minX, float maxX, int minY, int maxY, float minZ, float maxZ)
        {
          //initialise grid
          srand(time(0)*849849*77816);
          std::vector<std::vector<glm::vec3>> grid(modelSize, std::vector<glm::vec3> (modelSize, glm::vec3(0.f,0.f,0.f)));
          grid[0][0] = glm::vec3(minX, (float)(rand()%maxY + minY), minZ);
          grid[0][modelSize - 1] = glm::vec3(maxX, (float)(rand()%maxY + minY), minZ);
          grid[modelSize - 1][0] = glm::vec3(minX, (float)(rand()%maxY + minY), maxZ);
          grid[modelSize - 1][modelSize - 1] = glm::vec3(maxX, (float)(rand()%maxY + minY), maxZ);

          //interpolate all points for x and z coord
          std::vector<glm::vec3> lhs = interpolateVec3(grid[0][0], grid[0][modelSize-1], modelSize);
          std::vector<glm::vec3> rhs = interpolateVec3(grid[modelSize-1][0], grid[modelSize-1][modelSize-1], modelSize);
          for (int i = 0; i < modelSize; i++) {
            std::vector<glm::vec3> row = interpolateVec3(lhs[i], rhs[i], modelSize);
            for (int j = 0; j < modelSize; j++) {
              grid[j][i] = row[j];
            }
          }
          //perform diamond square
          grid = diamondSquare(grid, maxY);
          //once diamond square complete, save array points into model as triangles.
          saveToTris(grid);
        }

        std::vector<std::vector<glm::vec3>> diamondSquare(std::vector<std::vector<glm::vec3>> grid, int maxY)
        {
          //bottom = max indecies
          int randomness = maxY;
          int workingGridSize = modelSize;
          const int maxXindex = modelSize - 1;
          const int maxYindex = modelSize - 1;
          glm::vec2 topl;
          glm::vec2 topr;
          glm::vec2 bottoml;
          glm::vec2 bottomr;
          glm::vec2 mid;
          while(workingGridSize != 2){
            //initialise positions for top left working grid
            topl = glm::vec2(0,0);
            topr = glm::vec2(workingGridSize - 1, 0);
            bottoml = glm::vec2(0, workingGridSize - 1);
            bottomr = glm::vec2(workingGridSize - 1, workingGridSize - 1);
            mid = glm::vec2((workingGridSize-1)/2, (workingGridSize-1)/2);
            if(modelSize == workingGridSize){
              grid = squareStep(grid, topl, topr, bottoml, bottomr, randomness);
              //diamondStep 4 times
              grid = diamondStep(grid, topl, mid, bottoml, randomness);//left hand diamond
              grid = diamondStep(grid, bottoml, mid, bottomr, randomness);//bottom diamond
              grid = diamondStep(grid, topr, mid, bottomr, randomness);//right hand diamond
              grid = diamondStep(grid, topl, mid, topr, randomness);//top diamond
            }
            else{
              while(bottomr != glm::vec2(maxXindex, maxYindex)){
                grid = squareStep(grid, topl, topr, bottoml, bottomr, randomness);
                //diamondStep 4 times
                grid = diamondStep(grid, topl, mid, bottoml, randomness);//left hand diamond
                grid = diamondStep(grid, bottoml, mid, bottomr, randomness);//bottom diamond
                grid = diamondStep(grid, topr, mid, bottomr, randomness);//right hand diamond
                grid = diamondStep(grid, topl, mid, topr, randomness);//top diamond
                if(topr.x != maxXindex){
                  //move right
                  topl = glm::vec2(topl.x + (workingGridSize-1), topl.y);;
                  topr = glm::vec2(topr.x + (workingGridSize-1), topr.y);
                  bottoml = glm::vec2(bottoml.x + (workingGridSize-1), bottoml.y);
                  bottomr = glm::vec2(bottomr.x + (workingGridSize-1), bottomr.y);
                  mid = glm::vec2(topl.x + (topr.x-topl.x)/2, topl.y + (bottoml.y-topl.y)/2);
                }
                else{
                  //move down and to left hand edge
                  topl = glm::vec2(0,topl.y + (workingGridSize-1));
                  topr = glm::vec2(workingGridSize-1, topr.y + workingGridSize-1);
                  bottoml = glm::vec2(0, bottoml.y + (workingGridSize-1));
                  bottomr = glm::vec2(workingGridSize-1, bottomr.y + (workingGridSize-1));
                  mid = glm::vec2(topl.x + (topr.x-topl.x)/2, topl.y + (bottoml.y-topl.y)/2);
                }
              }
              grid = squareStep(grid, topl, topr, bottoml, bottomr, randomness);
              //diamondStep 4 times
              grid = diamondStep(grid, topl, mid, bottoml, randomness);//left hand diamond
              grid = diamondStep(grid, bottoml, mid, bottomr, randomness);//bottom diamond
              grid = diamondStep(grid, topr, mid, bottomr, randomness);//right hand diamond
              grid = diamondStep(grid, topl, mid, topr, randomness);//top diamond
            }
            //move right until at edge
            //move down and repeat
            workingGridSize = ((workingGridSize-1)/2)+1;
            randomness = std::max(randomness / 2, 1);
          }
          return grid;
        }

        std::vector<std::vector<glm::vec3>> diamondStep(std::vector<std::vector<glm::vec3>> grid, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3, int randomness)
        {
          srand(time(0)+9846189451*98416484);
          float average = (grid[v1.x][v1.y].y + grid[v2.x][v2.y].y + grid[v3.x][v3.y].y) * (float)(1.0/3.0);
          grid[v1.x + (v3.x-v1.x)/2][v1.y + (v3.y-v1.y)/2].y = average + (rand()%randomness);
          return grid;
        }

        std::vector<std::vector<glm::vec3>> squareStep(std::vector<std::vector<glm::vec3>> grid, glm::vec2 topL, glm::vec2 topR, glm::vec2 botL, glm::vec2 botR, int randomness)
        {
          srand(time(0)*846*88888);
          float average = (grid[topL.x][topL.y].y + grid[topR.x][topR.y].y + grid[botL.x][botL.y].y + grid[botR.x][botR.y].y) * (float)(1.0/4.0);
          grid[topL.x + (topR.x - topL.x)/2][topL.y + (botL.y - topL.y)/2].y = average + (rand()%randomness);
          return grid;
        }

        void saveToTris(std::vector<std::vector<glm::vec3>> grid)
        {
          Colour white = Colour(255,255,255);
          //save all verts
          for (int i = 0; i < modelSize; i++) {
            for (int j = 0; j < modelSize; j++) {
              verts.push_back(Vertex(grid[j][i] * scaling));
            }
          }
          //save as triangles
          int triangle_index = 0;
          const int trisPerRow = (modelSize - 1) * 2;
          const int nOfTriRows = modelSize - 1;
          for (int i = 0; i < nOfTriRows; i++) {
            //top half triangles as each row of squares has two sub rows of triangles
            for (int j = 0; j < trisPerRow/2; j++) {
              int v1 =  j + (i*modelSize);
              int v2 =  v1+1;
              int v3 =  ((i+1)*modelSize)+j;
              ModelTriangle tri(verts[v1].position, verts[v2].position, verts[v3].position, white);
              model.push_back(tri);
              boundingBoxes[0].addTri(model[i], i, v1, v2, v3);
              verts[v1].triangles.push_back(triangle_index);
              verts[v2].triangles.push_back(triangle_index);
              verts[v3].triangles.push_back(triangle_index);
              triangle_index++;
            }
            //bottom half triangles
            for (int j = 0; j < trisPerRow/2; j++) {
              int v1 =  j + (i*modelSize) + 1;
              int v3 =  ((i+1)*modelSize)+j;
              int v2 =  v3+1;
              ModelTriangle tri(verts[v1].position, verts[v2].position, verts[v3].position, white);
              model.push_back(tri);
              boundingBoxes[0].addTri(model[i], i, v1, v2, v3);
              verts[v1].triangles.push_back(triangle_index);
              verts[v2].triangles.push_back(triangle_index);
              verts[v3].triangles.push_back(triangle_index);
              triangle_index++;
            }
          }
          if(vertexNormals.size() == 0) getVertexNormals();
        }

        std::vector<glm::vec3> interpolateVec3(glm::vec3 from, glm::vec3 to, int numberOfValues)
        {
          std::vector<glm::vec3> output;
          float vecXStep, vecYStep;
          float vecZStep;
          if(numberOfValues == 0){
            output.push_back(glm::vec3(from[0], from[1], from[2]));
            return output;
          }
          if(numberOfValues == 1){
            output.push_back(glm::vec3(from[0], from[1], from[2]));
            output.push_back(glm::vec3(to[0], to[1], to[2]));
            return output;
          }
          else{
            vecXStep = (to[0] - from[0]) / (numberOfValues);
            vecYStep = (to[1] - from[1]) / (numberOfValues);
            vecZStep = (to[2] - from[2]) / (numberOfValues);
            for (int i = 0; i < numberOfValues; i++) {
              output.push_back(glm::vec3(from[0] + vecXStep*i, from[1] + vecYStep*i, from[2] + vecZStep*i));
            }
          }
          return output;
        }

        void backFaceCulling(Camera& camera)
        {
          for (int i = 0; i < (int) model.size(); i++) {
            if(model[i].visible == true){
              glm::vec3 cameraPos(camera.cameraToWorld[3].x, camera.cameraToWorld[3].y, camera.cameraToWorld[3].z);
              glm::vec3 viewRay = glm::normalize(model[i].vertices[0] - cameraPos);
              if(glm::dot(viewRay, model[i].normal) >= 0){
                model[i].visible = false;
                continue;
              }
            }
          }
        }

        void nearFarCulling(Camera& camera, DrawingWindow& window)
        {
          for (int i = 0; i < (int) model.size(); i++) {
            if(model[i].visible == true){
              int hiddenVerts = 0;
              for (size_t j = 0; j < 3; j++) {
                glm::vec3 tempVert = worldToCamera(camera, window, model[i].vertices[j]);
                float distFromCamera = -tempVert.z - camera.focalLength;

                if(distFromCamera > camera.farPlaneDist || distFromCamera < camera.nearPlaneDist) hiddenVerts ++;
              }
              if(hiddenVerts == 3) model[i].visible = false;
            }
          }
        }

        void triFragmentation(Camera& camera, DrawingWindow& window)
        {
          for (int i = 0; i < (int) originalModel.size(); i++) {
            if(model[i].visible == true){
              int visibleVertices = 3;
              std::vector<glm::vec3> vertexes; //in camera space
              std::vector<int> hiddenVerts;
              for (size_t j = 0; j < 3; j++) {
                glm::vec3 tempPoint = worldToCamera(camera, window, model[i].vertices[j]);
                float distFromCamera = -tempPoint.z - camera.focalLength;
                vertexes.push_back(tempPoint);

                if(distFromCamera > camera.farPlaneDist || distFromCamera < camera.nearPlaneDist){
                   hiddenVerts.push_back(j);
                   visibleVertices--;
                }
                else if (tempPoint.x < (-window.width/2) || tempPoint.x > (window.width/2)){
                   hiddenVerts.push_back(j);
                   visibleVertices--;
                }
                else if (tempPoint.y < (-window.height/2) || tempPoint.y > (window.height/2)){
                   hiddenVerts.push_back(j);
                   visibleVertices--;
                }
              }

              if(visibleVertices == 3) continue;
              else if(visibleVertices == 0) model[i].visible = false;
              else if(visibleVertices == 1){
                fragment1Vert(camera, window, hiddenVerts, vertexes, i);
              }
              else if(visibleVertices == 2){
                fragment2Vert(camera, window, hiddenVerts, vertexes, i);
              }
            }
          }

        }

        void fragment1Vert(Camera& camera, DrawingWindow& window, std::vector<int> hiddenVerts, std::vector<glm::vec3> vertexes, int i)
        {
          int visibleVertex = 3 - hiddenVerts[0] - hiddenVerts[1]; //total to take away from =3 as index start at 0
          //interpolate between visible and hidden
          int numberOfSteps0 = std::max(std::abs(vertexes[visibleVertex].x - vertexes[hiddenVerts[0]].x), std::abs(vertexes[visibleVertex].y - vertexes[hiddenVerts[0]].y));
          int numberOfSteps1 = std::max(std::abs(vertexes[visibleVertex].x - vertexes[hiddenVerts[1]].x), std::abs(vertexes[visibleVertex].y - vertexes[hiddenVerts[1]].y));
          std::vector<glm::vec3> line0 = interpolateVec3(vertexes[visibleVertex], vertexes[hiddenVerts[0]], numberOfSteps0);
          std::vector<glm::vec3> line1 = interpolateVec3(vertexes[visibleVertex], vertexes[hiddenVerts[1]], numberOfSteps1);
          //find new last visible points on these lines, update coords in verticies
          for (int k = 0; k < (int) line0.size(); k++) {
            if(line0[k].x <= (-window.width/2) || line0[k].x >= (window.width/2) -1|| line0[k].y <= (-window.height/2) || line0[k].y >= (window.height/2)-1){
              vertexes[hiddenVerts[0]] = line0[k];
              break;
            }
          }
          for (int k = 0; k < (int) line1.size(); k++) {
            if(line1[k].x <= (-window.width/2) || line1[k].x >= (window.width/2) -1|| line1[k].y <= (-window.height/2) || line1[k].y >= (window.height/2)-1){
              vertexes[hiddenVerts[1]] = line1[k];
              break;
            }
          }
          //convert back to world space, update model[i].verticies
          model[i].vertices[visibleVertex] = cameraToWorld(camera, window, vertexes[visibleVertex]);
          model[i].vertices[hiddenVerts[0]] = cameraToWorld(camera, window, vertexes[hiddenVerts[0]]);
          model[i].vertices[hiddenVerts[1]] = cameraToWorld(camera, window, vertexes[hiddenVerts[1]]);
        }

        void fragment2Vert(Camera& camera, DrawingWindow& window, std::vector<int> hiddenVerts, std::vector<glm::vec3> vertexes, int i)
        {
          ModelTriangle newTri = model[i];
          int visibleVert0;
          int visibleVert1;
          if(hiddenVerts[0] == 0){
            visibleVert0 = 1;
            visibleVert1 = 2;
          }
          else if(hiddenVerts[0] == 1){
            visibleVert0 = 0;
            visibleVert1 = 2;
          }
          else{
            visibleVert0 = 0;
            visibleVert1 = 1;
          }
          //interpolate between visibles and hidden
          int numberOfSteps0 = std::max(std::abs(vertexes[visibleVert0].x - vertexes[hiddenVerts[0]].x), std::abs(vertexes[visibleVert0].y - vertexes[hiddenVerts[0]].y));
          int numberOfSteps1 = std::max(std::abs(vertexes[visibleVert1].x - vertexes[hiddenVerts[0]].x), std::abs(vertexes[visibleVert1].y - vertexes[hiddenVerts[0]].y));
          std::vector<glm::vec3> line0 = interpolateVec3(vertexes[visibleVert0], vertexes[hiddenVerts[0]], numberOfSteps0);
          std::vector<glm::vec3> line1 = interpolateVec3(vertexes[visibleVert1], vertexes[hiddenVerts[0]], numberOfSteps1);
          //find new last visible points on these lines
          int pushedBack = 0;
          for (int k = 0; k < (int) line0.size(); k++) {
            if(line0[k].x <= (-window.width/2) || line0[k].x >= (window.width/2)-1 || line0[k].y <= (-window.height/2) || line0[k].y >= (window.height/2)-1){
              newTri.vertices[0] = cameraToWorld(camera, window, vertexes[visibleVert0]);
              newTri.vertices[1] = cameraToWorld(camera, window, vertexes[visibleVert1]);
              newTri.vertices[2] = cameraToWorld(camera, window, line0[k]);
              model.push_back(newTri);
              pushedBack++;
              break;
            }
          }
          for (int k = 0; k < (int) line1.size(); k++) {
            if(line1[k].x <= (-window.width/2) || line1[k].x >= (window.width/2)-1 || line1[k].y <= (-window.height/2) || line1[k].y >= (window.height/2)-1){
              newTri.vertices[0] = cameraToWorld(camera, window, vertexes[visibleVert1]);
              newTri.vertices[1] = cameraToWorld(camera, window, line1[k]);
              model.push_back(newTri);
              pushedBack++;
              break;
            }
          }
          if(pushedBack == 2) model[i].visible=false;
          else if(pushedBack == 1) model.erase(model.end());
        }

        glm::vec3 worldToCamera(Camera& camera, DrawingWindow& window, glm::vec3& point)
        {
          glm::mat4 worldToCamera =  glm::inverse(camera.cameraToWorld);
          glm::vec4 worldPoint(point.x, point.y, point.z, 1.0f);
          glm::vec4 cameraVert = worldToCamera * worldPoint;
          float f = (camera.focalLength / cameraVert.z);
          float x = -(cameraVert.x * f * window.height/2);
          float y = (cameraVert.y * f * window.height/2);
          float z = cameraVert.z;
          return glm::vec3(x,y,z);
        }

        glm::vec3 cameraToWorld(Camera& camera, DrawingWindow& window, glm::vec3& point)
        {
          float f = (camera.focalLength / point.z);
          float x = -(point.x / (f * (window.height/2)));
          float y = (point.y / (f * (window.height/2)));
          float z = point.z;
          glm::vec4 cameraPoint(x ,y ,z , 1.0f);
          glm::vec4 worldVert = camera.cameraToWorld * cameraPoint;
          return glm::vec3(worldVert.x,worldVert.y,worldVert.z);
        }
};
#endif
