#define GLFW_INCLUDE_NONE
#define WIN32_LEAN_AND_MEAN

#include "pPack/vector.h"
#include "pPack/GUICreator.h"
#include "pPack/WindowManager.h"
#include "pPack/shaderHandling.h"
#include "pPack/textRendering.h"
#include "pPack/timer.h"
#include "pPack/animateValue.h"
#include "private/GUIReader.h"

#include "private/helper.h"
#include "private/casting.h"
#include "private/types.h"

#include "stb_image.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

#include <Windows.h>


using namespace ::pPack;
using namespace ::pPack::gui_creator;
using namespace ::pPack::gui_creator_private;
using namespace ::pPack::window_manager;
using namespace ::pPack::vector;
using namespace ::pPack::shader_handling;
using namespace ::pPack::timer;


namespace {
// Rendering
// ----------

void DrawBodyObject(GUIBody* base, float aspect, bool widthWay);
void DrawObject(GUIElement* obj, float aspect, IVector2 windowSize, Vector2 windowScaler, bool widthWay, Vector4 drawArea);

// ----------
// Rendering



  // external
Window window = Window();
ShaderHandler shader;
ShaderHandler borderShader;
Timer* singletonTimer = nullptr;
text_rendering::Bitmap bitmap;


// rendering stuff
unsigned int VAO, VBO, elementInfoUBO, transformInfoUBO;
float const square[16] = {
  -1.0f, -1.0f, 0.f, 0.f,
  1.0f, -1.0f, 1.f, 0.f,
  -1.0f, 1.0f, 0.f, 1.f,
  1.0f, 1.0f, 1.f, 1.f
};


// mappings
std::map<std::string, std::map<std::string, void(*)(GUIElement*)>> functionMapping = std::map<std::string, std::map<std::string, void(*)(GUIElement*)>>();
std::map<std::string, std::map<std::string, void(*)(GUIElement*, int)>> cyclicMapping = std::map<std::string, std::map<std::string, void(*)(GUIElement*, int)>>();

std::map<std::string, GUIBody*> pageMapping = std::map<std::string, GUIBody*>();


// runtime info
GUIBody* selectedPage = nullptr;
std::string selectedPageName = std::string();
void (*updateFunc)(double) = nullptr;
void (*closeFunc)() = nullptr;


GUIBase* hovered = nullptr;
GUIBase* clicked = nullptr;


}; // namespace







namespace pPack {
namespace gui_creator {



bool Start(StartAppInfo info) {
  CHAR pathBuffer[512];
  GetModuleFileNameA(NULL, pathBuffer, 512 / sizeof(TCHAR));
  std::filesystem::path pathProp = std::string(pathBuffer);
  std::string path = (pathProp.parent_path().concat("\\")).lexically_normal().string();
  SetGlobalPath(path);


  if (!glfwInit()) {
    return false;
  }

  window.Open(info.width, info.height, info.title, info.windowHints, info.windowHintCount);
  window.SetAsContext();

  if (!gladLoadGL()) {
    window.Close();
    glfwTerminate();

    return false;
  }

  window.SetSwapInterval(0);


  std::string fontLoc = path + info.font;

  text_rendering::Start(path);
  bitmap = text_rendering::GenerateBitmap(fontLoc.c_str(), info.fontSize, info.fontLow, info.fontHigh, info.fontFiltering);


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  singletonTimer = &Timer::GetSingleton();
  //singletonTimer->SetUpdateDelay(1.0 / 120.0);


  std::string vertLoc = path + "./shaders/guiCreator/shader.vert";
  std::string fragLoc = path + "./shaders/guiCreator/shader.frag";
  std::string borderFragLoc = path + "./shaders/guiCreator/border.frag";

  const char* vertLocC = vertLoc.c_str();
  const char* fragLocC = fragLoc.c_str();
  const char* borderFragLocC = borderFragLoc.c_str();

  ShaderCreateInfo shaderInfo[] = { ShaderCreateInfo(&vertLocC, 1, GL_VERTEX_SHADER), ShaderCreateInfo(&fragLocC, 1, GL_FRAGMENT_SHADER)};
  ShaderCreateInfo borderShaderInfo[] = { ShaderCreateInfo(&vertLocC, 1, GL_VERTEX_SHADER), ShaderCreateInfo(&borderFragLocC, 1, GL_FRAGMENT_SHADER) };

  shader = ShaderHandler::CreateShader("GUICreator__baseShader", shaderInfo, 2);
  borderShader = ShaderHandler::CreateShader("GUICreator__borderShader", borderShaderInfo, 2);


  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);


  glGenBuffers(1, &elementInfoUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, elementInfoUBO);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, elementInfoUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);


  glGenBuffers(1, &transformInfoUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, transformInfoUBO);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, transformInfoUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);


  return true;
}



void Update() {
  while (!window.ShouldClose()) {
    if (selectedPage == nullptr) {
      return;
    }

    window.ResetKeys();

    //singletonTimer->WaitForNextUpdate();
    window.SetAsContext();

    glfwPollEvents();

    if (!window.GetValidity()) {
      window.SetViewport();
      window.SetValidity(true);
    }


    if (updateFunc != nullptr) {
      updateFunc(singletonTimer->GetDeltaTime());
    }



    Vector2 mousePos = window.GetMousePosition().ConvertTo<float>();
    InputInfo button = window.GetInput(GLFW_MOUSE_BUTTON_1);
    float yScroll = (float)window.GetMouseScrollY();


    if (clicked != nullptr) {
      if (button.released) {

        if (clicked->onRelease != nullptr) {
          clicked->onRelease(clicked);
        }
        if (clicked->onCycle != nullptr) {
          clicked->onCycle(clicked, 2);
        }

        clicked = nullptr;
      } else if (button.held) {
        if (clicked->onHold != nullptr) {
          clicked->onHold(clicked);
        }
      }
    }

    if (hovered != nullptr) {
      if (!LazyPositionInElement(mousePos, dynamic_cast<GUIElement*>(hovered))) {
        if (hovered->onLeave != nullptr) {
          hovered->onLeave(hovered);
        }
        if (hovered->onCycle != nullptr) {
          hovered->onCycle(hovered, 3);
        }

        hovered->_isHovered = false;
        hovered = nullptr;
        clicked = nullptr;
      }
    }



    GUIBase* elem = selectedPage->TriggerTree(mousePos, button.pressed, yScroll, false);
    if (elem != nullptr) {
      if (button.pressed) {
        clicked = elem;
      }
      hovered = elem;
    }


    DeleteChildren(selectedPage, clicked, hovered);


    glBindVertexArray(VAO);
    selectedPage->Update();
    glBindVertexArray(0);


    window.Swap();
    singletonTimer->Advance();
    animate_value::AnimateValueUpdate();

  }
}



void End() {
  if (closeFunc != nullptr) {
    closeFunc();
  }

  animate_value::DeleteAllAnimations();

  for (auto& [name, page] : pageMapping) {
    for (GUIElement* elem : page->elements) {
      glDeleteTextures(1, &elem->texture);
      delete(elem);
    }

    delete(page);
  }

  bitmap.Destroy();

  window.Close();

  text_rendering::End();

  glfwTerminate();

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shader);
}



void SetPage(std::string pageName, std::string file) {
  if (pageName.size() == 0 || pageName == "") {
    return;
  }

  if (pageMapping.contains(pageName)) {
    GUIBody* page = pageMapping[pageName];

    for (GUIElement* elem : page->elements) {
      glDeleteTextures(1, &elem->texture);
      delete(elem);
    }

    delete(page);
  }


  uint8_t* fileBuffer = nullptr;
  size_t bufferSize = 0;

  if (!PrecompileData(file, (void**)&fileBuffer, &bufferSize) || bufferSize == 0 || fileBuffer == nullptr) {
    return;
  }


  uint32_t objectCount = *(uint32_t*)fileBuffer - 1;


  std::vector<GUIBase*> objsCreated = std::vector<GUIBase*>();

  GUIRawObject* curObj = (GUIRawObject*)(fileBuffer + sizeof(int));

  GUIBody* createdBody = new GUIBody();
  objsCreated.push_back(createdBody);
  pageMapping[pageName] = createdBody;

  StringHandler stringHandler = curObj->GetStringStart();

  HandleGeneralAttributes(createdBody, curObj->attributeCount, stringHandler);

  createdBody->type = TryGetStringFrom(stringHandler.GetNext());
  createdBody->data = TryGetStringFrom(stringHandler.GetNext());

  createdBody->SetCurrent();

  curObj = (GUIRawObject*)stringHandler.GetCurrentEnd();

  GUIElement* createdElement = nullptr;
  for (int i = 0; i < objectCount; i++) {
    createdElement = new GUIElement();

    createdElement->pageCreated = true;

    objsCreated.push_back(createdElement);
    createdBody->elements.push_back(createdElement);
    stringHandler.SetTo(curObj->GetStringStart());


    createdElement->transform.position = curObj->position;
    createdElement->transform.scale = curObj->scale;

    if (curObj->parentIndex != -1) {
      createdElement->SetParent(objsCreated[curObj->parentIndex]);
      createdElement->CopyViewAttributes(createdElement->parent);
    }

    HandleGeneralAttributes(createdElement, curObj->attributeCount, stringHandler);
    stringHandler.Reset();
    HandleElementAttributes(pageName, createdElement, curObj->attributeCount, stringHandler);


    createdElement->type = TryGetStringFrom(stringHandler.GetNext());
    createdElement->data = TryGetStringFrom(stringHandler.GetNext());

    createdElement->SetCurrent();

    curObj = (GUIRawObject*)stringHandler.GetCurrentEnd();
  }



  free(fileBuffer);
}



void SetTextInputElement(GUIElement* elem) {
  if (elem == nullptr) {
    window.SetBlockFunction(nullptr);
    window.SetReadFunction(nullptr);
    SetReadElement(nullptr);
  } else {
    window.SetBlockFunction(BlockFunction);
    window.SetReadFunction(ReadFunction);
    SetReadElement(elem);
  }
}




void OpenPage(std::string pageName) {
  if (!pageMapping.contains(pageName)) {
    return;
  }

  selectedPageName = pageName;
  selectedPage = pageMapping[pageName];
}



GUIBase* GetObjectByID(std::string id) {
  if (selectedPage == nullptr) {
    return nullptr;
  }

  if (id == "body") {
    return selectedPage;
  }

  for (GUIElement* elem : selectedPage->elements) {
    if (elem->id == id) {
      return elem;
    }
  }

  return nullptr;
}



void SetUpdateFunction(void(*func)(double deltaTime)) {
  updateFunc = func;
}




void SetCloseFunction(void(*func)()) {
  closeFunc = func;
}



bool LazyPositionInElement(Vector2 pos, GUIElement* ele) {
  if (ele == nullptr) {
    return true;
  }

  Vector2 bl = ele->drawTransform.position - ele->drawTransform.scale;
  Vector2 tr = ele->drawTransform.position + ele->drawTransform.scale;

  return pos.x >= bl.x && pos.y >= bl.y && pos.x <= tr.x && pos.y <= tr.y;
}




// GUIBase
//------------------------------------------------------------------------------------------

void GUIBase::RemoveChild(GUIElement* Child) {
  if (Child == nullptr) {
    return;
  }

  std::vector<GUIElement*>::iterator search = std::find(children.begin(), children.end(), Child);
  if (search == children.end()) {
    return;
  }
  children.erase(search);
}


GUIShaderInformation GUIBase::ShaderInformation() {
  return GUIShaderInformation {primaryColor, secondaryColor,
  gradientStart, gradientStep, gradientDistance, gradientX,
  gradientY, manhattanGradient};
}


bool GUIBase::TreeEnabled() {
  if (parent == nullptr) {
    return true;
  }
  if (!enabled) {
    return false;
  }

  return parent->TreeEnabled();
}



GUIBase* GUIBase::TriggerTree(Vector2 mousePos, bool click, float& yScroll, bool noGrid) {
  if (!enabled) {
    return nullptr;
  }

  GUIBase* elemClicked = nullptr;
  for (int i = children.size() - 1; i >= 0; i--) {
    GUIElement* child = children[i];
    if (noGrid && !child->noGrid) {
      continue;
    }


    elemClicked = child->TriggerTree(mousePos, click, yScroll, noGrid);
    if (elemClicked != nullptr) {

      if (scrollStep != 0 && yScroll != 0) {
        scrolledAmount -= scrollStep * yScroll;

        if (minScroll < 1.0f && scrolledAmount < minScroll) {
          scrolledAmount = minScroll;
        }
        if (maxScroll > -1.0f && scrolledAmount > maxScroll) {
          scrolledAmount = maxScroll;
        }
        yScroll = 0;
      }

      return elemClicked;
    }
  }
  if (scrollStep != 0 && yScroll != 0) {
    scrolledAmount -= scrollStep * yScroll;
    if (minScroll < 1.0f && scrolledAmount < minScroll) {
      scrolledAmount = minScroll;
    }
    if (maxScroll > -1.0f && scrolledAmount > maxScroll) {
      scrolledAmount = maxScroll;
    }
    yScroll = 0;
  }
  if (parent != nullptr) {
    return nullptr;
  }


  if (!_isHovered) {
    if (onEnter != nullptr) {
      onEnter(this);
    }
    if (onCycle != nullptr) {
      onCycle(this, 0);
    }
    _isHovered = true;
  }

  if (click) {
    if (onClick != nullptr) {
      onClick(this);
    }

    if (onCycle != nullptr) {
      onCycle(this, 1);
    }
  }

  return this;
}



void GUIBase::Reset() {
  primaryColor = _primaryColor;
  secondaryColor = _secondaryColor;
  borderColor = _borderColor;
  gradientStart = _gradientStart;
  borderThickness = _borderThickness;
  gradientStep = _gradientStep;
  gradientDistance = _gradientDistance;
  gradientX = _gradientX;
  gradientY = _gradientY;
  manhattanGradient = _manhattanGradient;
  borderEnable = _borderEnable;

  textSize = _textSize;
  textCutoff = _textCutoff;
  textColor = _textColor;

  centerTextX = _centerTextX;
  centerTextY = _centerTextY;

  padding = _padding;
}

void GUIBase::SetCurrent() {
  _primaryColor = primaryColor;
  _secondaryColor = secondaryColor;
  _borderColor = borderColor;
  _gradientStart = gradientStart;
  _borderThickness = borderThickness;
  _gradientStep = gradientStep;
  _gradientDistance = gradientDistance;
  _gradientX = gradientX;
  _gradientY = gradientY;
  _manhattanGradient = manhattanGradient;
  _borderEnable = borderEnable;

  _textSize = textSize;
  _textColor = textColor;
  _textCutoff = textCutoff;
  _padding = padding;
  _centerTextX = centerTextX;
  _centerTextY = centerTextY;
}



GUIBase::GUIBase() : children(), primaryColor(1.0f), secondaryColor(0.f, 0.f, 0.f, 1.0f), borderColor(0.f, 0.f, 0.f, 1.0f), gradientStart(0, 0),
type(), data(), parent(nullptr), texture(0), borderThickness(0.05f), gradientStep(0.f), gradientDistance(1.0f), minScroll(1.0f), maxScroll(-1.0f),
scrollStep(0), scrolledAmount(0), gradientX(false), gradientY(false), manhattanGradient(false), borderEnable(false),
enabled(true), textColor(0, 0, 0, 1), padding(), textSize(1.0f), textCutoff(0.f),
onClick(nullptr), onRelease(nullptr), onEnter(nullptr), onLeave(nullptr), onHold(nullptr), onCycle(nullptr),
centerTextX(true), centerTextY(true), _primaryColor(0.f), _secondaryColor(0.f), _isHovered(false),
_borderColor(0.f,0.f,0.f,1.f), _gradientStart(), _borderThickness(0.05f), _gradientDistance(1.f), _gradientX(false), _gradientY(false), _manhattanGradient(false),
_borderEnable(false), _centerTextX(true), _centerTextY(true), _gradientStep(0.f),
_textSize(1.0f), _textCutoff(0.f), _textColor(0, 0, 0, 1), _padding()
{}


//------------------------------------------------------------------------------------------



// GUIBody
//------------------------------------------------------------------------------------------

void GUIBody::Update() {
  glClearColor(0, 0, 0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  float aspect = window.GetAspectRatio();
  bool widthWay = window.WidthLarger();

  DrawBodyObject(this, aspect, widthWay);


  // Gets current window scale, in display settings
  float xscale, yscale;
  glfwGetWindowContentScale(Window::GetCurrentContext()->windowRef, &xscale, &yscale);

  IVector2 windowSize = window.GetSize();

  for (GUIElement* child : children) {
    child->drawTransform = Transform();
    DrawObject(child, aspect, windowSize, {xscale, yscale}, widthWay, {-1.0, -1.0, 1.0, 1.0});
  }
}


GUIBody::GUIBody() : GUIBase() {}


//------------------------------------------------------------------------------------------



// GUIElement
//------------------------------------------------------------------------------------------

GUIBase* GUIElement::TriggerTree(Vector2 mousePos, bool click, float& yScroll, bool noGrid) {
  if (!enabled) {
    return nullptr;
  }

  bool inSelf = LazyPositionInElement(mousePos, this);

  GUIBase* desc = GUIBase::TriggerTree(mousePos, click, yScroll, !inSelf);
  if (desc != nullptr) {
    return desc;
  }

  if (inSelf) {
    if (!_isHovered) {
      if (onEnter != nullptr) {
        onEnter(this);
      }
      if (onCycle != nullptr) {
        onCycle(this, 0);
      }
      _isHovered = true;
    }

    if (click) {
      if (inputField) {
        SetTextInputElement(this);
      }

      if (onClick != nullptr) {
        onClick(this);
      }

      if (onCycle != nullptr) {
        onCycle(this, 1);
      }

    }

    return this;
  }
  return nullptr;
}



void GUIElement::Reset() {
  stretch = _stretch;
  stick = _stick;
  stuck = _stuck;
  transform = _transform;

  GUIBase::Reset();
}



void GUIElement::SetCurrent() {
  _stretch = stretch;
  _stick = stick;
  _stuck = stuck;
  _transform = transform;

  GUIBase::SetCurrent();
}



void GUIElement::SetDeletion(bool b) {
  toggleDeletion = b;

  if (b == true) {
    for (GUIElement* child : children) {
      child->SetDeletion(true);
    }
  }
}



bool GUIElement::GetDeletion() {
  return toggleDeletion;
}

bool GUIElement::GetPageCreated() {
  return pageCreated;
}




void GUIElement::SetParent(GUIBase* Parent) {
  if (Parent == nullptr) {
    return;
  }
  if (parent != nullptr) {
    parent->RemoveChild(this);
  }

  parent = Parent;
  Parent->children.push_back(this);
}


Vector2 GUIElement::MoveIntoLocal(Vector2 pos) {
  GUIElement* elemParent = dynamic_cast<GUIElement*>(parent);

  if (elemParent == nullptr) {
    return pos;
  }

  pos = elemParent->MoveIntoLocal(pos);
  return elemParent->transform.Move(pos);
}


Vector2 GUIElement::LazyMoveIntoLocal(Vector2 pos) {
  GUIElement* elemParent = dynamic_cast<GUIElement*>(parent);
  if (elemParent == nullptr) {
    return pos;
  }

  return (pos - elemParent->drawTransform.position) / elemParent->drawTransform.scale;
}


void GUIElement::CopyViewAttributes(GUIBase* obj) {
  if (obj == nullptr) {
    return;
  }

  primaryColor = obj->primaryColor;
  secondaryColor = obj->secondaryColor;
  borderColor = obj->borderColor;
  gradientStart = obj->gradientStart;
  texture = obj->texture;
  borderThickness = obj->borderThickness;
  gradientStep = obj->gradientStep;
  gradientDistance = obj->gradientDistance;
  gradientX = obj->gradientX;
  gradientY = obj->gradientY;
  manhattanGradient = obj->manhattanGradient;
  borderEnable = obj->borderEnable;
  centerTextX = obj->centerTextX;
  centerTextY = obj->centerTextY;
  textSize = obj->textSize;
  textCutoff = obj->textCutoff;
  textColor = obj->textColor;
  padding = obj->padding;
}



GUIElement::GUIElement() : GUIBase(), pageCreated(false), id(), placeholder(), transform(), drawTransform(), stretch(true), stick(false),
stuck(false), inputField(false), noGrid(false), toggleDeletion(false), _transform(), _stretch(true), insertCharPos(-1, -1, 0),
_stick(false), _stuck(false)
{}

//------------------------------------------------------------------------------------------



// StartAppInfo
//------------------------------------------------------------------------------------------

StartAppInfo::StartAppInfo() : width(1280), height(720), title("GUI App"), windowHints(nullptr),
windowHintCount(0), font("./fonts/CascadiaCode-Bold.ttf"), fontSize(64), fontLow(0), fontHigh(127),
fontFiltering(GL_LINEAR) {}

//------------------------------------------------------------------------------------------



// Transform
//------------------------------------------------------------------------------------------

Transform& Transform::operator += (const Transform& transform) {
  position += transform.position * scale;
  scale *= transform.scale;

  return *this;
}

vector::Vector2 Transform::Move(vector::Vector2 pos) {
  pos -= position;
  pos /= scale;

  return pos;
}

Transform::Transform() : position(), scale(1) {}

//------------------------------------------------------------------------------------------



}; // namespace gui_creator
}; // namespace pPack












namespace {


// Rendering


void DrawBodyObject(GUIBody* base, float aspect, bool widthWay) {
  shader.Active();

  Vector2 sizeFix = Vector2(1);

  if (widthWay) {
    sizeFix.x /= aspect;
  } else {
    sizeFix.y *= aspect;
  }

  GUIShaderInformation shaderInfo = base->ShaderInformation();

  glNamedBufferData(elementInfoUBO, sizeof(GUIShaderInformation), nullptr, GL_STREAM_DRAW);
  glNamedBufferSubData(elementInfoUBO, 0, sizeof(GUIShaderInformation), &shaderInfo);

  TransformInformation transformInfo = TransformInformation(0, 1, sizeFix);

  glNamedBufferData(transformInfoUBO, sizeof(TransformInformation), nullptr, GL_STREAM_DRAW);
  glNamedBufferSubData(transformInfoUBO, 0, sizeof(TransformInformation), &transformInfo);


  if (base->texture != 0) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, base->texture);
    ShaderHandler::SetInt("texTarget", 0);
    ShaderHandler::SetBool("useTexture", true);
  } else {
    ShaderHandler::SetBool("useTexture", false);
  }

  ShaderHandler::SetVector4("area", {-1.0, -1.0, 1.0, 1.0});


  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


  if (base->borderEnable) {
    borderShader.Active();

    ShaderHandler::SetVector4("borderColor", base->borderColor);
    ShaderHandler::SetFloat("borderThickness", base->borderThickness);


    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }
}




void DrawObject(GUIElement* obj, float aspect, IVector2 windowSize, Vector2 windowScaler, bool widthWay, Vector4 drawArea) {
  if (!obj->enabled) {
    return;
  }

  shader.Active();

  obj->drawTransform += obj->transform;
  if (obj->parent != nullptr) {
    obj->drawTransform.position.y += obj->parent->scrolledAmount;
  }

  float fixedPosX = obj->drawTransform.position.x;
  float scaleX = obj->drawTransform.scale.x;

  float fixedPosY = obj->drawTransform.position.y;
  float scaleY = obj->drawTransform.scale.y;

  float fontWindow = bitmap.GetFontSize() * 20.f;
  Vector2 sizeFix = Vector2(1, 1);


  if (widthWay) {
    if (obj->stick && obj->drawTransform.position.x != 0) {
      float one = SignOf(obj->drawTransform.position.x);

      fixedPosX = (one - scaleX / aspect) - ((one - (fixedPosX + scaleX)) / aspect);
    } else if (obj->stuck) {
      fixedPosX /= aspect;
    }

    sizeFix.x /= aspect;

    if (!obj->stretch) {
      scaleX /= aspect;
    }
  } else {
    if (obj->stick && obj->drawTransform.position.y != 0) {
      float one = SignOf(obj->drawTransform.position.y);

      fixedPosY = (one - scaleY * aspect) - ((one - (fixedPosY + scaleY)) * aspect);

    } else if (obj->stuck) {
      fixedPosY *= aspect;
    }

    sizeFix.y = aspect;


    if (!obj->stretch) {
      scaleY *= aspect;
    }
  }

  Vector2 bl = {fixedPosX - scaleX, fixedPosY - scaleY};
  Vector2 tr = {fixedPosX + scaleX, fixedPosY + scaleY};

  bool cb = FitPositionIn(drawArea, bl);
  bool ct = FitPositionIn(drawArea, tr);
  if (cb && ct) {
    return;
  }



  GUIShaderInformation shaderInfo = obj->ShaderInformation();

  glNamedBufferData(elementInfoUBO, sizeof(GUIShaderInformation), nullptr, GL_STREAM_DRAW);
  glNamedBufferSubData(elementInfoUBO, 0, sizeof(GUIShaderInformation), &shaderInfo);

  TransformInformation transformInfo = TransformInformation({fixedPosX, fixedPosY}, {scaleX, scaleY}, sizeFix * 0.5f);

  glNamedBufferData(transformInfoUBO, sizeof(TransformInformation), nullptr, GL_STREAM_DRAW);
  glNamedBufferSubData(transformInfoUBO, 0, sizeof(TransformInformation), &transformInfo);

  if (obj->texture != 0) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj->texture);
    ShaderHandler::SetInt("texTarget", 0);
    ShaderHandler::SetBool("useTexture", true);
  } else {
    ShaderHandler::SetBool("useTexture", false);
  }

  ShaderHandler::SetVector4("area", drawArea);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


  Vector4 innerArea = {bl.x, bl.y, tr.x, tr.y};

  if (obj->data.size() != 0 || obj->placeholder.size() != 0) {
    text_rendering::RenderTextInfo info = text_rendering::RenderTextInfo(IVector2(windowSize.x / windowScaler.x, windowSize.y / windowScaler.y));
    info.color = obj->textColor;
    info.startPos = {fixedPosX - (scaleX * (1 - obj->padding.x)), fixedPosY + (obj->centerTextY ? 0.f : scaleY) - (scaleY * obj->padding.y) + obj->scrolledAmount};
    info.fontScale = (obj->textSize * (float)(widthWay ? windowSize.y : windowSize.x)) / fontWindow;
    info.centerX = obj->centerTextX;
    info.centerY = obj->centerTextY;
    info.textWidth = 2 * scaleX * (1 - obj->padding.x);
    info.cutoff = obj->textCutoff;
    info.area = innerArea;

    if (obj->inputField) {
      info.showAt = &obj->insertCharPos;
    }

    std::string* str = &obj->data;
    if (obj->data.size() == 0 && obj->insertCharPos.y == -1)
      str = &obj->placeholder;

    text_rendering::Render(bitmap, *str, info);
  }



  for (GUIElement* child : obj->children) {
    child->drawTransform = Transform();
    child->drawTransform += obj->drawTransform;
    DrawObject(child, aspect, windowSize, windowScaler, widthWay, innerArea);
  }



  if (obj->borderEnable) {
    borderShader.Active();

    glNamedBufferData(transformInfoUBO, sizeof(TransformInformation), nullptr, GL_STREAM_DRAW);
    glNamedBufferSubData(transformInfoUBO, 0, sizeof(TransformInformation), &transformInfo);

    ShaderHandler::SetVector4("borderColor", obj->borderColor);
    ShaderHandler::SetFloat("borderThickness", obj->borderThickness);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }


  obj->drawTransform.position = {fixedPosX, fixedPosY};
  obj->drawTransform.scale = {scaleX, scaleY};
}



} // namespace