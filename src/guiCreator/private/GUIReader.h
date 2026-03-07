#ifndef GUI_READER_SRC_GUI_READER_H
#define GUI_READER_SRC_GUI_READER_H

namespace pPack {
namespace gui_creator_private {


void* GenerateGUIFromFile(const char* file);
void FreeGUI(void* ptr);


}; // namespace gui_creator_private
}; // namespace pPack



#endif