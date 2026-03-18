#ifndef PPM_INCLUDE_PRIVATE_HELPER_H
#define PPM_INCLUDE_PRIVATE_HELPER_H

#include "pPack/GUICreator.h"
#include "pPack/windowManager.h"
#include "private/types.h"

#include <string>

namespace pPack {
namespace gui_creator_private {

void DeleteChildren(::pPack::gui_creator::GUIBase* elem, ::pPack::gui_creator::GUIBase*& clicked, ::pPack::gui_creator::GUIBase*& hovered);

::pPack::window_manager::BlockReturn BlockFunction(::pPack::window_manager::Window* window, int key, int action, int mods);
void ReadFunction(::pPack::window_manager::Window* window, unsigned int c);
void SetReadElement(::pPack::gui_creator::GUIElement* elem);

bool FitPositionIn(vector::Vector4 area, vector::Vector2& pos);
float SignOf(float i);
bool FileExists(const char* fileName);
bool PrecompileData(std::string fileName, void** ppBuffer, size_t* bufferSize);
char const* ToNextInstanceOf(char const* str, char what);
void HandleGeneralAttributes(gui_creator::GUIBase* body, int attributeCount, gui_creator_private::StringHandler& handler);
void HandleElementAttributes(std::string pageName, gui_creator::GUIElement* element, int attributeCount, gui_creator_private::StringHandler& handler);
std::string TryGetStringFrom(char const* const str);


void SetGlobalPath(std::string path);
}; // namespace gui_creator_private
}; // namespace pPack

#endif