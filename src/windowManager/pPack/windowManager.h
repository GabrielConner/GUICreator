#ifndef PPM_INCLUDE_PPACK_WINDOW_MANAGER_H
#define PPM_INCLUDE_PPACK_WINDOW_MANAGER_H
#define GLFW_INCLUDE_NONE

#include "GLFW\glfw3.h"
#include <string>
#include <map>
#include "pPack/vector.h"


#ifdef _GUI_CREATOR_BUILD_DLL
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif




namespace pPack {
namespace window_manager {


enum BlockReturn {BLOCK = 0, ALLOW, UNBLOCK};


struct InputInfo {
  bool pressed;
  bool held;
  bool released;
  int mods;

  InputInfo() : pressed(false), held(false), released(false), mods(0) {}
  InputInfo(bool Pressed, bool Held, bool Released, int Mods) : pressed(Pressed), held(Held), released(Released), mods(Mods) {}
};

struct WindowCreateHint {
  int hint;
  int value;
};



class Window {
  friend void _windowResizedCallback(GLFWwindow* Window, int Width, int Height);
  friend void _mouseScrollCallback(GLFWwindow* glWindow, double xoffset, double yoffset);
  friend void _charCallback(GLFWwindow* glWindow, unsigned int codepoint);
  friend void _keyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods);
  friend void _closeCallback(GLFWwindow* Window);
  friend void _mouseCallback(GLFWwindow* glWindow, int Button, int Action, int Mods);
  friend void _mouseMoveCallback(GLFWwindow* glWindow, double xPos, double yPos);
  friend void _mouseEnterCallback(GLFWwindow* glWindow, int Entered);
  friend void _windowFocusCallback(GLFWwindow* glWindow, int Focused);
  friend void _windowMaximizeCallback(GLFWwindow* glWindow, int Maximized);
  friend void _windowIconifyCallback(GLFWwindow* glWindow, int Maximized);



  int width;
  int height;
  double aspectRatio;
  std::string title;

  std::map<int, InputInfo> keyMapping;
  std::map<int, InputInfo> mouseMapping;

  double realMouseX;
  double realMouseY;

  double mouseX;
  double mouseY;

  double mouseScrollX;
  double mouseScrollY;

  bool mouseOver;
  bool hasFocus;
  bool maximized;
  bool visible;

  bool valid;
  bool shouldClose;

  BlockReturn (*blockUntil)(Window*, int, int, int);
  void (*readFunction)(Window*, unsigned int);

public:
  GLFWwindow* windowRef;

  DLL static Window* GetCurrentContext();



  InputInfo GetInput(int Key) {
    return keyMapping[Key];
  }



  DLL void SetSize(int Width, int Height);
  DLL void SetTitle(std::string Title);
  DLL void SetFocus();
  DLL void SetMouseScroll(double X, double Y);
  DLL void SetMousePosition(double X, double Y);
  DLL void SetValidity(bool Valid);

  DLL void SetSwapInterval(int i);
  DLL void SetShouldClose(bool set);
  DLL void SetBlockFunction(BlockReturn (*func)(Window*, int, int, int));
  DLL void SetReadFunction(void (*func)(Window*, unsigned int));


  DLL void RequestFocus();


  DLL int GetWidth() const;
  DLL int GetHeight() const;

  DLL bool WidthLarger() const;

  DLL double GetMouseScrollX() const;
  DLL double GetMouseScrollY() const;


  DLL::pPack::vector::IVector2 GetSize() const;


  DLL double GetAspectRatio() const;

  DLL std::string GetTitle() const;
  DLL bool GetMaximized() const;
  DLL bool GetVisible() const;
  DLL bool GetValidity() const;


  DLL::pPack::vector::DVector2 GetMousePosition() const;
  DLL::pPack::vector::DVector2 GetRealMousePosition() const;



  DLL void Open(int Width, int Height, std::string Title, WindowCreateHint* Hints, int HintCount);
  DLL void Close();


  DLL bool IsOpen();
  DLL bool ShouldClose();


  DLL void Show();


  DLL void Maximize();
  DLL void Restore();
  DLL void Minimize();


  DLL void ResetKeys();

  DLL void Swap();

  DLL void SetAsContext();

  DLL void SetViewport();


  DLL Window();
  DLL ~Window();
};




}; // namespace window_manager
}; // namespace pPack


#endif