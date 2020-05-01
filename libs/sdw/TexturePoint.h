#ifndef TEXTUREPOINT_H
#define TEXTUREPOINT_H

class TexturePoint
{
  public:
    glm::vec2 point;

    TexturePoint()
    {
    }

    TexturePoint(float xPos, float yPos)
    {
      point = glm::vec2(xPos, yPos);
    }

    void print()
    {
    }
};

#endif