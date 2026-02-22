#ifndef PPM_INCLUDE_PRIVATE_TYPES_H
#define PPM_INCLUDE_PRIVATE_TYPES_H

#include "pPack/vector.h"

namespace pPack {
namespace gui_creator_private {

// StringHandler
// --------------------------------------------------
struct StringHandler {
  char const* stringList;
  char const* curString;
  char const* foundEnd;




  char const* const GetNext();
  char const* GetCurrentEnd() const;


  void SetTo(char const* ptr);
  void Reset();


  StringHandler() = delete;
  StringHandler(char const* StringList);
};
// --------------------------------------------------
// StringHandler





// GUIRawObject
// --------------------------------------------------
struct GUIRawObject {
  vector::Vector2 position;
  vector::Vector2 scale;


  uint32_t attributeCount;
  uint32_t parentIndex;


  char const* GetStringStart();
};
// --------------------------------------------------
// GUIRawObject





// TransformInformation
// --------------------------------------------------
struct TransformInformation {
  vector::Vector2 position;
  vector::Vector2 scale;
  vector::Vector2 sizeFix;


  TransformInformation() = delete;
  TransformInformation(vector::Vector2 Pos, vector::Vector2 Scale, vector::Vector2 SizeFix) : position(Pos), scale(Scale), sizeFix(SizeFix) {}
};
// --------------------------------------------------


}; // namespace gui_creator_private
}; // namespace pPack

#endif