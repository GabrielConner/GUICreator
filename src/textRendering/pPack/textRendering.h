#ifndef PPM_INCLUDE_PPACK_TEXT_RENDERING_H
#define PPM_INCLUDE_PPACK_TEXT_RENDERING_H

#include "pPack/vector.h"
#include "glad/glad.h"
#include <vector>


#ifdef _GUI_CREATOR_BUILD_DLL
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif


namespace pPack {
namespace text_rendering {


struct CharacterMetrics {
  vector::IVector2 pixelBottomLeft;
  vector::IVector2 pixelTopRight;

  vector::Vector2 uvBottomLeft;
  vector::Vector2 uvTopRight;

  vector::IVector2 size;
  vector::IVector2 bearing;
  int advance;


  DLL CharacterMetrics();
  DLL CharacterMetrics(vector::IVector2 Size, vector::IVector2 Bearing, float Advance);
};



struct RenderTextInfo {
  float fontScale;
  float lineSize;
  float textWidth;
  float cutoff;

  vector::IVector3* showAt;

  bool centerX;
  bool centerY;

  bool startTop;

  vector::Vector2 startPos;
  vector::Vector4 area;
  vector::Vector4 color;

  vector::IVector2 frameSize;

  DLL RenderTextInfo(vector::IVector2 FrameSize);
};




class Bitmap {
  unsigned int texture;
  vector::IVector2 textureSize;
  int fontSize;

  unsigned char charLow;
  unsigned char charHigh;

  friend DLL Bitmap GenerateBitmap(const char* fontFile, int size, unsigned char low, unsigned char high, int filtering);
  friend DLL void Render(Bitmap& bitmap, std::string& text, RenderTextInfo info);

public:
  std::vector<CharacterMetrics> characterMapping;

  DLL void LoadFromFile(const char* file, int filtering);
  DLL void SaveToFile(const char* file, const char* imageLocation);


  DLL bool Valid();


  DLL void Destroy();


  DLL int GetFontSize();
  DLL unsigned char GetCharLow();
  DLL unsigned char GetCharHigh();


  DLL CharacterMetrics& operator[](unsigned char c);

  DLL Bitmap();
};



DLL void Start(std::string folderLoc);
DLL void End();


DLL Bitmap GenerateBitmap(const char* fontFile, int size, unsigned char low, unsigned char high, int filtering);
DLL void Render(Bitmap& bitmap, std::string& text, RenderTextInfo info);


} // namespace text_rendering
} // namespace pPack

#endif