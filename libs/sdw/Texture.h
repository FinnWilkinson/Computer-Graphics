#ifndef TEXTURE_H
#define TEXTURE_H

#include "Header.h"

class Texture {
    public:

        int image_width, image_height;
        std::string texture_file;
        Colour* colours;

        Texture()
        {

        }  

        Texture(std::string texture_file)
        {
            this->texture_file = texture_file;
            
            readInTexture();
        }

        void displayTexture(DrawingWindow& window)
        {
            for(int y = 0; y < image_height; y++){
                for(int x = 0; x < image_width; x++){
                    window.setPixelColour(x, y, colours[x + y*image_width].packedColour());
                }
            }
        }

    private:
        
        void readInTexture()
        {   
            std::ifstream tex_stream;
            char buffer[256];
            tex_stream.open(texture_file, std::ifstream::in);

            tex_stream.getline(buffer, 256);
            tex_stream.getline(buffer, 256);
            tex_stream.getline(buffer, 256);

            std::string line(buffer);

            image_width = std::stoi(split(line, ' ')[0]);
            image_height = std::stoi(split(line, ' ')[1]);
            
            colours = (Colour*)std::malloc(image_width * image_height * sizeof(Colour));

            tex_stream.getline(buffer, 256);

            for(int y = 0; y < image_height; y++){
                for(int x = 0; x < image_width; x++){
                    colours[x + y*image_width].red = tex_stream.get();
                    colours[x + y*image_width].green = tex_stream.get();
                    colours[x + y*image_width].blue = tex_stream.get();
                }
            }
            tex_stream.close();
        }
};

#endif