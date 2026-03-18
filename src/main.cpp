#include "pPack/GUICreator.h"

#include <iostream>

using namespace ::pPack;
using namespace ::pPack::gui_creator;


void cycle(GUIBase* elem, int i) {
  if (i == 0 || i == 2) {
    elem->primaryColor = elem->_primaryColor * 0.9f;
  } else if (i == 1) {
    elem->primaryColor = elem->_primaryColor * 0.8f;
  } else if (i == 3) {
    elem->primaryColor = elem->_primaryColor;
  }
}

void Delete(GUIBase* elem) {
  ((GUIElement*)elem)->SetDeletion(true);
}



int main() {

  StartAppInfo info = StartAppInfo();

  Start(info);


  SetCyclicFunction("global", "cycle", cycle);

  SetPage("home", GetGlobalPath() + "home.xml");
  OpenPage("home");

  SetPage("home", GetGlobalPath() + "home.xml");
  OpenPage("home");


  GUIElement* elem = new GUIElement();
  elem->transform.scale = 0.5;
  elem->SetParent(GetObjectByID("text"));
  elem->onClick = Delete;



  Update();

  End();

  return 0;
}