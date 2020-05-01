#ifndef COLOUR_H
#define COLOUR_H

#include "Header.h"

class Colour
{
  public:
    std::string name;
    int red;
    int green;
    int blue;
    float brightness;
    bool specular;
    float specular_exponent;
    float reflective_index = 0.0f;
    float transparency = 0.0f;
    float refractive_index = 0.0f;

    Colour()
    {
      name = "";
      red = 255;
      green = 255;
      blue = 255;
      brightness = 1.0f;
      specular = false;
    }

    Colour(int r, int g, int b)
    {
      name = "";
      red = r;
      green = g;
      blue = b;
      brightness = 1.0f;
      specular = false;
    }

    Colour(int r, int g, int b, float point_brightness)
    {
      name = "";
      red = r;
      green = g;
      blue = b;
      brightness = point_brightness;
      specular = false;
    }

    Colour(std::string n, int r, int g, int b)
    {
      name = n;
      red = r;
      green = g;
      blue = b;
      brightness = 1.0f;
      specular = false;
    }

    uint32_t packedColour()
    {
      int r = red * brightness;
      int g = green * brightness;
      int b = blue * brightness;

      return (255 << 24) + (r << 16) + (g << 8) + (b);
    }
};

std::ostream& operator<<(std::ostream& os, const Colour& colour)
{
    os << colour.name << " [" << colour.red << ", " << colour.green << ", " << colour.blue << "]" << std::endl;
    return os;
}

#endif
