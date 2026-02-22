#include "private/casting.h"

#include "private/helper.h"

#include "pPack/vector.h"
#include "pPack/GUICreator.h"

#include <algorithm>
#include <sstream>


using namespace ::pPack;
using namespace ::pPack::gui_creator;
using namespace ::pPack::gui_creator_private;
using namespace ::pPack::vector;


namespace {

std::map<std::string, std::map<std::string, void(*)(GUIBase*)>> functionMapping = std::map<std::string, std::map<std::string, void(*)(GUIBase*)>>();
std::map<std::string, std::map<std::string, void(*)(GUIBase*, int)>> cyclicMapping = std::map<std::string, std::map<std::string, void(*)(GUIBase*, int)>>();

}; // namespace



namespace pPack {

namespace gui_creator {

void SetFunction(std::string space, std::string name, void(*func)(GUIBase*)) {
  if (func == nullptr) {
    return;
  }

  functionMapping[space][name] = func;
}



void SetCyclicFunction(std::string space, std::string name, void(*func)(GUIBase*,int)) {
  if (func == nullptr) {
    return;
  }

  cyclicMapping[space][name] = func;
}



// Awfulness
void (*ToFunction(std::string pageName, const char* str))(GUIBase*) {
  const char* funcName = ToNextInstanceOf(str, ':');
  if (funcName == nullptr) {
    return functionMapping[pageName][str];
  }
  funcName++;
  return functionMapping[std::string(str, (funcName - str - 1))][funcName];
}



void (*ToCyclic(std::string pageName, const char* str))(GUIBase*, int) {
  const char* funcName = ToNextInstanceOf(str, ':');
  if (funcName == nullptr) {
    return cyclicMapping[pageName][str];
  }
  funcName++;
  return cyclicMapping[std::string(str, (funcName - str - 1))][funcName];
}

}; // namespace gui_creator




namespace gui_creator_private {


bool ToBool(std::string str) {

  // https://stackoverflow.com/a/3613424
  // https://stackoverflow.com/a/2942442
  std::transform(str.begin(), str.end(), str.begin(), static_cast<int(*)(int)>(std::tolower));

  std::istringstream s(str);
  bool b;
  s >> std::boolalpha >> b;
  return b;
}



Vector4 ToVector4(const char* str) {

  Vector4 vec;
  if (str != nullptr)
    vec.x = atof(str);
  str = ToNextInstanceOf(str, ',');
  if (str != nullptr) {
    str++;
    vec.y = atof(str);
  }
  str = ToNextInstanceOf(str, ',');
  if (str != nullptr) {
    str++;
    vec.z = atof(str);
  }
  str = ToNextInstanceOf(str, ',');
  if (str != nullptr) {
    str++;
    vec.w = atof(str);
  }

  return vec;
}



Vector2 ToVector2(const char* str) {

  Vector2 vec;
  if (str != nullptr)
    vec.x = atof(str);
  str = ToNextInstanceOf(str, ',');
  if (str != nullptr) {
    str++;
    vec.y = atof(str);
  }

  int (*f(int))(double);

  return vec;
}



}; // namespace gui_creator_private
}; // namespace pPack