#ifndef GUI_READER_SRC_DATA_TYPES_H
#define GUI_READER_SRC_DATA_TYPES_H

#include <string>
#include <vector>
#include <map>
#include "pPack/vector.h"


namespace pPack {
namespace gui_creator_private {


struct GUINode {
public:

  std::vector<int> horizontalGrid;
  std::vector<int> verticalGrid;
  ::pPack::vector::IVector2 gridPosition;
  ::pPack::vector::IVector2 gridSize;


  ::pPack::vector::Vector2 position;
  ::pPack::vector::Vector2 scale;
  ::pPack::vector::Vector2 padding;
  ::pPack::vector::IVector2 jumpGrid;
  bool noMove;
  bool noGrid;


  std::string type;
  std::string data;
  std::map<std::string, std::string> attributes;
  std::vector<GUINode*> children;
  GUINode* parent;


  std::pair<::pPack::vector::Vector2, ::pPack::vector::Vector2> GenTransformFor(GUINode* obj);

  GUINode();
};



struct RawGUINode {
  ::pPack::vector::Vector2 position;
  ::pPack::vector::Vector2 scale;
  uint32_t attributeCount;
  uint32_t parentIndex;

  RawGUINode(::pPack::vector::Vector2 Position, ::pPack::vector::Vector2 Scale, uint32_t AttributeCount, uint32_t ParentIndex) : position(Position), scale(Scale),
    attributeCount(AttributeCount), parentIndex(ParentIndex) {}
};


}; // namespace gui_creator_private
}; // namespace pPack

#endif