#ifndef PPM_INCLUDE_PRIVATE_CASTING_H
#define PPM_INCLUDE_PRIVATE_CASTING_H

#include "pPack/vector.h"
#include "pPack/GUICreator.h"


namespace pPack {
namespace gui_creator_private {

bool ToBool(std::string str);
vector::Vector4 ToVector4(const char* str);
vector::Vector2 ToVector2(const char* str);



}; // namespace gui_creator_private
}; // namespace pPack


#endif