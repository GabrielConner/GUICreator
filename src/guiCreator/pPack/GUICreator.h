#ifndef PPM_INCLUDE_PPACK_GUI_CREATOR_H
#define PPM_INCLUDE_PPACK_GUI_CREATOR_H

#include <vector>
#include <string>

#include "pPack/vector.h"
#include "pPack/textRendering.h"
#include "pPack/WindowManager.h"


#ifdef _GUI_CREATOR_BUILD_DLL
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif



namespace pPack {
namespace gui_creator {

// Transform
// --------------------------------------------------
struct Transform {
  vector::Vector2 position;
  vector::Vector2 scale;


  DLL Transform& operator += (const Transform& transform);
  DLL vector::Vector2 Move(vector::Vector2 pos);


  DLL Transform();
};
// --------------------------------------------------
// Transform





// GUIShaderInformation
// --------------------------------------------------
struct GUIShaderInformation {
  vector::Vector4 primaryColor;
  vector::Vector4 secondaryColor;
  vector::Vector2 gradientStart;

  float gradientStep;
  float gradientDistance;


  int gradientX;
  int gradientY;
  int manhattan;


  //padding
  int a, b, c;
};
// --------------------------------------------------
// GUIShaderInformation





// GUIBase
// --------------------------------------------------
class GUIElement;
struct  GUIBase {
public:

  vector::Vector4 _primaryColor;
  vector::Vector4 _secondaryColor;
  vector::Vector4 _borderColor;
  vector::Vector2 _gradientStart;


  float _borderThickness;


  float _gradientStep;
  float _gradientDistance;


  bool _gradientX;
  bool _gradientY;
  bool _manhattanGradient;
  bool _borderEnable;


  float _textSize;
  float _textCutoff;
  vector::Vector4 _textColor;
  vector::Vector2 _padding;


  bool _centerTextX;
  bool _centerTextY;

  bool _isHovered;


  std::vector<GUIElement*> children;
  vector::Vector4 primaryColor;
  vector::Vector4 secondaryColor;
  vector::Vector4 borderColor;
  vector::Vector2 gradientStart;


  std::string type;
  std::string data;


  GUIBase* parent;



  void (*onClick)(GUIBase* element);
  void (*onRelease)(GUIBase* element);


  void (*onEnter)(GUIBase* element);
  void (*onLeave)(GUIBase* element);


  void (*onHold)(GUIBase* element);


  void (*onCycle)(GUIBase* element, int val);

  unsigned int texture;
  float borderThickness;


  float gradientStep;
  float gradientDistance;

  float minScroll;
  float maxScroll;

  float scrollStep;
  float scrolledAmount;


  bool gradientX;
  bool gradientY;
  bool manhattanGradient;
  bool borderEnable;


  bool centerTextX;
  bool centerTextY;

  bool enabled;


  float textSize;
  float textCutoff;


  vector::Vector4 textColor;
  vector::Vector2 padding;





  DLL void RemoveChild(GUIElement* Child);


  DLL GUIShaderInformation ShaderInformation();


  DLL bool TreeEnabled();
  DLL virtual GUIBase* TriggerTree(vector::Vector2 mousePos, bool click, float& yScroll, bool noGrid);


  DLL virtual void Reset();
  DLL virtual void SetCurrent();


  DLL GUIBase();
};
// --------------------------------------------------
// GUIBase





// GUIBody
// --------------------------------------------------
struct GUIBody : public GUIBase {
public:

  std::vector<GUIElement*> elements;


  DLL void Update();


  DLL GUIBody();
};
// --------------------------------------------------





// GUIElement
// --------------------------------------------------
class GUIElement : public GUIBase {
  friend DLL void Update();
  friend DLL void SetPage(std::string pageName, std::string file);


  friend GUIBase;

  bool pageCreated;
  bool toggleDeletion;


public:

  Transform _transform;

  bool _stretch;
  bool _stick;
  bool _stuck;


  bool stretch;
  bool stick;
  bool stuck;

  bool inputField;

  bool noGrid;

  Transform transform;
  Transform drawTransform;

  ::pPack::vector::IVector3 insertCharPos;

  std::string id;
  std::string placeholder;

  DLL GUIBase* TriggerTree(vector::Vector2 mousePos, bool click, float& yScroll, bool noGrid) override;


  DLL void Reset() override;
  DLL void SetCurrent() override;


  DLL void SetDeletion(bool b);
  DLL bool GetDeletion();

  DLL bool GetPageCreated();
  
  DLL void SetParent(GUIBase* Parent);


  DLL vector::Vector2 MoveIntoLocal(vector::Vector2 pos);
  DLL vector::Vector2 LazyMoveIntoLocal(vector::Vector2 pos);


  DLL void CopyViewAttributes(GUIBase* obj);


  DLL GUIElement();
};
// --------------------------------------------------
// GUIElement





// StartAppInfo
// --------------------------------------------------
struct StartAppInfo {
  int width;
  int height;
  std::string title;
  window_manager::WindowCreateHint* windowHints;
  int windowHintCount;

  std::string font;
  int fontSize;
  unsigned char fontLow;
  unsigned char fontHigh;
  int fontFiltering;

  DLL StartAppInfo();
};
// --------------------------------------------------
// StartAppInfo







// Functions
// --------------------------------------------------
// --------------------------------------------------

DLL bool Start(StartAppInfo info);
DLL void Update();
DLL void End();

DLL void SetTextInputElement(GUIElement* elem);

DLL void SetFunction(std::string space, std::string name, void(*func)(GUIBase*));
DLL void SetCyclicFunction(std::string space, std::string name, void(*func)(GUIBase*, int));

DLL void SetPage(std::string pageName, std::string file);
DLL void OpenPage(std::string pageName);

DLL void SetUpdateFunction(void(*func)(double deltaTime));
DLL void SetCloseFunction(void(*func)());

DLL void (*ToFunction(std::string pageName, const char* str))(gui_creator::GUIBase*);
DLL void (*ToCyclic(std::string pageName, const char* str))(gui_creator::GUIBase*, int);

DLL GUIBase* GetObjectByID(std::string id);

DLL bool LazyPositionInElement(vector::Vector2 pos, GUIElement* ele);


DLL const std::string& GetGlobalPath();

// --------------------------------------------------
// --------------------------------------------------
// Functions

}; // namespace gui_creator
}; // namespace pPack



#endif