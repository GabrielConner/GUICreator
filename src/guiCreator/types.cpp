#include "private/types.h"



namespace pPack {
namespace gui_creator_private {


// StringHandler
//------------------------------------------------------------------------------------------

char const* const StringHandler::GetNext() {
  if (curString == nullptr)
    return nullptr;

  char const* temp = curString;

  while (true) {
    if (*curString == '\0') {
      curString++;
      foundEnd = curString;

      break;
    }
    curString++;
  }


  return temp;
}


char const* StringHandler::GetCurrentEnd() const {
  return foundEnd;
}


void StringHandler::SetTo(char const* ptr) {
  stringList = ptr;
  curString = ptr;
  foundEnd = ptr;
}


void StringHandler::Reset() {
  curString = stringList;
  foundEnd = curString;
}


StringHandler::StringHandler(char const* StringList) : stringList(StringList), curString(StringList), foundEnd(StringList) {}

//------------------------------------------------------------------------------------------



// GUIRawObject
//------------------------------------------------------------------------------------------

char const* GUIRawObject::GetStringStart() {
  return (char const*)(this + 1);
}

//------------------------------------------------------------------------------------------


}; // namespace gui_creator_private
}; // namespace pPack