#include "private/helper.h"

#include "private/casting.h"
#include "private/types.h"

#include "pPack/GUICreator.h"
#include "pPack/GUIReader.h"
#include "pPack/windowManager.h"

#include "stb_image.h"

#include <string>


using namespace ::pPack;
using namespace ::pPack::gui_creator;
using namespace ::pPack::window_manager;
using namespace ::pPack::gui_creator_private;
using namespace ::pPack::vector;


namespace {

GUIElement* readElement = nullptr;



}; // namespace




namespace pPack {
namespace gui_creator_private {


void DeleteChildren(GUIBase* elem, GUIBase*& clicked, GUIBase*& hovered) {
  for (int i = 0; i < elem->children.size(); i++) {
    GUIElement* child = elem->children[i];
    DeleteChildren(child, clicked, hovered);

    if (child->GetDeletion() && !child->GetPageCreated()) {
      if (child == clicked)
        clicked = nullptr;
      if (child == hovered)
        hovered = nullptr;
      delete(child);
      elem->children.erase(elem->children.begin() + i);
      i--;
    }
  }
}



BlockReturn BlockFunction(Window* window, int key, int action, int mods) {
  if (readElement != nullptr && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    switch (key) {
      case GLFW_KEY_UP:

        if (readElement->insertCharPos.y > 0) {
          readElement->insertCharPos.y--;
        }

        return BLOCK;
      case GLFW_KEY_DOWN:
        readElement->insertCharPos.y++;

        return BLOCK;
      case GLFW_KEY_LEFT:
        if (readElement->insertCharPos.x > 0)
          readElement->insertCharPos.x--;
        else if (readElement->insertCharPos.y > 0) {
          readElement->insertCharPos.y--;
          readElement->insertCharPos.x = -1;
        }

        return BLOCK;
      case GLFW_KEY_RIGHT:
        readElement->insertCharPos.x++;

        return BLOCK;

      case GLFW_KEY_BACKSPACE:
        if (readElement->insertCharPos.z > 0) {
          readElement->data.erase(readElement->data.begin() + readElement->insertCharPos.z - 1);
          readElement->insertCharPos.x--;
        }

        return BLOCK;
      case GLFW_KEY_DELETE:
        if (readElement->insertCharPos.z < readElement->data.size()) {
          readElement->data.erase(readElement->data.begin() + readElement->insertCharPos.z);
        }

        return BLOCK;
    }
  }




  switch (key) {

    case GLFW_KEY_ENTER:

      return ALLOW;

    case GLFW_KEY_ESCAPE:
      if (action == GLFW_RELEASE) {
        window->SetReadFunction(nullptr);
        SetReadElement(nullptr);
        return UNBLOCK;
      }
      break;
  }
  return BLOCK;
}


void ReadFunction(Window* window, unsigned int c) {
  if (readElement == nullptr || c > 255) {
    return;
  }

  readElement->data.insert(readElement->data.begin() + readElement->insertCharPos.z, c);
  readElement->insertCharPos.x++;
}


void SetReadElement(GUIElement* elem) {
  if (readElement != nullptr) {
    readElement->insertCharPos = {-1, -1, 0};
  }

  readElement = elem;
  if (elem != nullptr) {
    elem->insertCharPos = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), 0};
  }
}





bool FitPositionIn(Vector4 area, Vector2& pos) {
  bool b = false;
  if (pos.x < area.x) {
    pos.x = area.x;
    b = true;
  } else if (pos.x > area.z) {
    pos.x = area.z;
    b = true;
  }

  if (pos.y < area.y) {
    pos.y = area.y;
    b = true;
  } else if (pos.y > area.w) {
    pos.y = area.w;
    b = true;
  }

  return b;
}


float SignOf(float i) {
  return i < 0 ? -1.f : 1.f;
}



bool FileExists(const char* fileName) {
  struct _stat buffer;
  if (_stat(fileName, &buffer) == 0) {
    return true;
  }
  return false;
}



char const* ToNextInstanceOf(char const* str, char what) {
  if (str == nullptr) {
    return nullptr;
  }

  while (true) {
    if (*str == what)
      return str;
    if (*str == '\0')
      return nullptr;
    str++;
  }
}



void HandleGeneralAttributes(GUIBase* obj, int attributeCount, StringHandler& handler) {
  for (int i = 0; i < attributeCount; i++) {
    char const* key = handler.GetNext();
    char const* value = handler.GetNext();

    if (strcmp(key, "primaryColor") == 0) {
      obj->primaryColor = ToVector4(value);
    } else if (strcmp(key, "secondaryColor") == 0) {
      obj->secondaryColor = ToVector4(value);
    } else if (strcmp(key, "borderColor") == 0) {
      obj->borderColor = ToVector4(value);
    } else if (strcmp(key, "gradientStart") == 0) {
      obj->gradientStart = ToVector2(value);
    } else if (strcmp(key, "gradientX") == 0) {
      obj->gradientX = ToBool(value);
    } else if (strcmp(key, "gradientY") == 0) {
      obj->gradientY = ToBool(value);
    } else if (strcmp(key, "manhattanGradient") == 0) {
      obj->manhattanGradient = ToBool(value);
    } else if (strcmp(key, "border") == 0) {
      obj->borderEnable = ToBool(value);
    } else if (strcmp(key, "borderThickness") == 0) {
      obj->borderThickness = atof(value);
    } else if (strcmp(key, "gradientStep") == 0) {
      obj->gradientStep = atof(value);
    } else if (strcmp(key, "gradientDistance") == 0) {
      obj->gradientDistance = atof(value);
    } else if (strcmp(key, "centerTextX") == 0) {
      obj->centerTextX = ToBool(value);
    } else if (strcmp(key, "centerTextY") == 0) {
      obj->centerTextY = ToBool(value);
    } else if (strcmp(key, "textColor") == 0) {
      obj->textColor = ToVector4(value);
    } else if (strcmp(key, "padding") == 0) {
      obj->padding = ToVector2(value);
    } else if (strcmp(key, "textSize") == 0) {
      obj->textSize = atof(value);
    } else if (strcmp(key, "textCutoff") == 0) {
      obj->textCutoff = atof(value);
    } else if (strcmp(key, "scrollStep") == 0) {
      obj->scrollStep = atof(value);
    } else if (strcmp(key, "scrolledAmount") == 0) {
      obj->scrolledAmount = atof(value);
    } else if (strcmp(key, "minScroll") == 0) {
      obj->minScroll = atof(value);
    } else if (strcmp(key, "maxScroll") == 0) {
      obj->maxScroll = atof(value);
    } else if (strcmp(key, "image") == 0) {
      if (!FileExists(value)) {
        continue;
      }

      int width, height, channels, rChannels = 4;
      void* pixelData = stbi_load(value, &width, &height, &channels, rChannels);

      glGenTextures(1, &obj->texture);
      glBindTexture(GL_TEXTURE_2D, obj->texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      glBindTexture(GL_TEXTURE_2D, 0);
      stbi_image_free(pixelData);
    }
  }


}



void HandleElementAttributes(std::string pageName, GUIElement* element, int attributeCount, StringHandler& handler) {
  for (int i = 0; i < attributeCount; i++) {
    char const* key = handler.GetNext();
    char const* value = handler.GetNext();
    if (key == nullptr || value == nullptr)
      return;


    if (strcmp(key, "id") == 0) {
      element->id = value;
    } else if (strcmp(key, "stick") == 0) {
      element->stick = ToBool(value);
    } else if (strcmp(key, "stretch") == 0) {
      element->stretch = ToBool(value);
    } else if (strcmp(key, "stuck") == 0) {
      element->stuck = ToBool(value);
    } else if (strcmp(key, "inputField") == 0) {
      element->inputField = ToBool(value);
    } else if (strcmp(key, "enabled") == 0) {
      element->enabled = ToBool(value);
    } else if (strcmp(key, "onClick") == 0) {
      element->onClick = ToFunction(pageName, value);
    } else if (strcmp(key, "onRelease") == 0) {
      element->onRelease = ToFunction(pageName, value);
    } else if (strcmp(key, "onEnter") == 0) {
      element->onEnter = ToFunction(pageName, value);
    } else if (strcmp(key, "onLeave") == 0) {
      element->onLeave = ToFunction(pageName, value);
    } else if (strcmp(key, "onHold") == 0) {
      element->onHold = ToFunction(pageName, value);
    } else if (strcmp(key, "onCycle") == 0) {
      element->onCycle = ToCyclic(pageName, value);
    } else if (strcmp(key, "placeholder") == 0) {
      element->placeholder = value;
    }
  }
}



std::string TryGetStringFrom(char const* const str) {
  if (str == nullptr)
    return std::string();
  return std::string(str);
}



bool PrecompileData(std::string fileName, void** ppBuffer, size_t* bufferSize) {
  if (!FileExists(fileName.c_str()) || ppBuffer == nullptr || bufferSize == nullptr) {
    return false;
  }

  size_t* gui = (size_t*)guiReader_GenerateGUIFromFile(fileName.c_str());

  *bufferSize = *gui;

  if (*bufferSize == 0) {
    guiReader_FreeGUI(gui);
    return false;
  }

  *ppBuffer = malloc(*bufferSize);
  memcpy(*ppBuffer, gui + 1, *bufferSize);
  guiReader_FreeGUI(gui);

  return true;
}

}; // namespace gui_creator_private
}; // namespace pPack