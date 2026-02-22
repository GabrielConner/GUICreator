#ifndef GUI_READER_SRC_GUI_READER_H
#define GUI_READER_SRC_GUI_READER_H


#ifdef GUIREADER_DLL

#define DLL __declspec(dllexport)

#else

#define DLL __declspec(dllimport)

#endif

#ifdef __cplusplus
extern "C" {
#endif

DLL void* guiReader_GenerateGUIFromFile(const char* file);
DLL void guiReader_FreeGUI(void* ptr);


#ifdef __cplusplus
}
#endif

#endif