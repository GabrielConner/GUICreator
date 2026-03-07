#include "private/GUIReader.h"
#include "private/dataTypes.h"

#include "pPack/vector.h"

#include <filesystem>
#include <vector>
#include <sstream>
#include <iomanip>
#include <set>
#include <algorithm>
#include <numeric>

#include <libxml/parser.h>

using namespace ::pPack;
using namespace ::pPack::vector;
using namespace ::pPack::gui_creator_private;


namespace {

void TrimString(std::string& s);
void* CopyMem(void* dst, const void* src, size_t size);
bool ToBool(char* str);
Vector2 ToVector2(char* str);
std::vector<GUINode*> GenerateGUIFromNode(GUINode* parent, xmlNode* xmlNode, std::set<std::string> idSet);

}; // namespace



namespace pPack {
namespace gui_creator_private {



void* GenerateGUIFromFile(const char* file) {
  if (!std::filesystem::exists(file)) {
    return nullptr;
  }

  std::set<std::string> idSet = std::set<std::string>();


  std::vector<GUINode*> nodeList = std::vector<GUINode*>();

  xmlDoc* document = xmlReadFile(file, nullptr, 0);
  xmlNode* root = nullptr;

  if (document == nullptr) {
    return nullptr;
  }

  root = xmlDocGetRootElement(document);
  if (root == nullptr) {
    return nullptr;
  }

  nodeList = GenerateGUIFromNode(nullptr, root, idSet);

  size_t allocLen = sizeof(uint32_t);
  for (const auto& node : nodeList) {
    allocLen += sizeof(RawGUINode);
    allocLen += node->type.size() + 1;

    for (const auto& attrib : node->attributes) {
      allocLen += attrib.first.size() + 1;
      allocLen += attrib.second.size() + 1;
    }

    allocLen += node->type.size() + 1;
    allocLen += node->data.size() + 1;
  }


  void* mem = calloc(allocLen + sizeof(size_t), 1);
  void* insertAt = mem;

  uint32_t objCount = nodeList.size();

  insertAt = CopyMem(insertAt, &allocLen, sizeof(size_t));
  insertAt = CopyMem(insertAt, &objCount, sizeof(uint32_t));


  for (const auto& node : nodeList) {
    std::vector<GUINode*>::iterator it = std::find(nodeList.begin(), nodeList.end(), node->parent);
    int parentIndex = -1;
    if (it != nodeList.end()) {
      parentIndex = it - nodeList.begin();
    }
    RawGUINode raw = RawGUINode(node->position, node->scale, node->attributes.size(), parentIndex);

    insertAt = CopyMem(insertAt, &raw, sizeof(RawGUINode));

    for (const auto& attrib : node->attributes) {
      insertAt = CopyMem(insertAt, attrib.first.c_str(), attrib.first.size() + 1);
      insertAt = CopyMem(insertAt, attrib.second.c_str(), attrib.second.size() + 1);
    }

    insertAt = CopyMem(insertAt, node->type.c_str(), node->type.size() + 1);
    insertAt = CopyMem(insertAt, node->data.c_str(), node->data.size() + 1);
  }


  return mem;
}



void FreeGUI(void* ptr) {
  free(ptr);
}



}; // namespace gui_creator_private
}; // namespace pPack








namespace {


void TrimString(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());

  std::string newStr = std::string();
  newStr.reserve(s.size());
  bool ws = false;
  bool slash = false;
  for (char c : s) {
    if (std::isspace(c)) {
      if (slash || !ws) {
        newStr += c;
        ws = true;
      }
      continue;
    }
    ws = false;

    if (c == '\\') {
      if (slash) {
        newStr += c;
        slash = false;
      } else {
        slash = true;
      }
      continue;
    }

    slash = false;
    newStr += c;
  }

  s = newStr;
}


void* CopyMem(void* dst, const void* src, size_t size) {
  memcpy(dst, src, size);
  return ((char*)dst) + size;
}

bool ToBool(char* str) {
  std::stringstream stream = std::stringstream(str);
  bool b = false;
  stream >> std::boolalpha >> b;

  return b;
}

Vector2 ToVector2(char* str) {
  Vector2 vec = Vector2();
  sscanf(str, "%f,%f", &vec.x, &vec.y);

  return vec;
}




std::vector<GUINode*> GenerateGUIFromNode(GUINode* parent, xmlNode* readNode, std::set<std::string> idSet) {
  std::vector<GUINode*> nodeList = std::vector<GUINode*>();

  GUINode* node = new GUINode();
  node->parent = parent;


  int asInt = 0;
  float asFloat = 0;
  Vector2 asVector2 = 0;



  for (xmlAttr* prop = readNode->properties; prop != nullptr; prop = prop->next) {
    char* name = (char*)prop->name;
    char* value = (char*)prop->children->content;

    if (strcmp(name, "id") == 0) {
      if (idSet.contains(value)) {
        delete(node);
        return nodeList;
      }
      idSet.insert(value);

    } else if (strcmp(name, "gridX") == 0) {
      asInt = atoi(value);
      node->horizontalGrid = std::vector<int>(asInt, 1);
    } else if (strcmp(name, "gridY") == 0) {
      asInt = atoi(value);
      node->verticalGrid = std::vector<int>(asInt, 1);
    } else if (strcmp(name, "scaleX") == 0) {
      asFloat = atof(value);
      node->scale.x = asFloat;
    } else if (strcmp(name, "scaleY") == 0) {
      asFloat = atof(value);
      node->scale.y = asFloat;
    } else if (strcmp(name, "offsetX") == 0) {
      asFloat = atof(value);
      node->position.x = asFloat;
    } else if (strcmp(name, "offsetY") == 0) {
      asFloat = atof(value);
      node->position.y = asFloat;
    } else if (strcmp(name, "noMove") == 0) {
      node->noMove = ToBool(value);
    } else if (strcmp(name, "jumpGridX") == 0) {
      asInt = atoi(value);
      node->jumpGrid.x = asInt;
    } else if (strcmp(name, "jumpGridY") == 0) {
      asInt = atoi(value);
      node->jumpGrid.y = asInt;
    } else if (memcmp(name, "gridX", 5) == 0) {
      asInt = atoi(name + 5);
      if (asInt >= 0 && asInt < node->verticalGrid.size()) {
        node->horizontalGrid[asInt] = atoi(value);
      }
    } else if (memcmp(name, "gridY", 5) == 0) {
      asInt = atoi(name + 5);
      if (asInt >= 0 && asInt < node->verticalGrid.size()) {
        node->verticalGrid[asInt] = atoi(value);
      }
    }

    if (strcmp(name, "padding") == 0) {
      asVector2 = ToVector2(value);
      node->padding = asVector2;
    } else if (strcmp(name, "noGrid") == 0) {
      node->noGrid = ToBool(value);
    }

    node->attributes.insert(std::pair<std::string, std::string>(name, value));
  }

  if (parent != nullptr) {
    std::pair<Vector2, Vector2> transform = parent->GenTransformFor(node);
    node->position += transform.first;
    node->scale *= transform.second;
    parent->children.push_back(node);
  }


  node->gridSize = IVector2(std::accumulate(node->horizontalGrid.begin(), node->horizontalGrid.end(), 0), std::accumulate(node->verticalGrid.begin(), node->verticalGrid.end(), 0));


  nodeList.push_back(node);


  for (xmlNode* child = readNode->children; child != nullptr; child = child->next) {
    if (child->type == XML_ELEMENT_NODE) {
      std::vector<GUINode*> retList = GenerateGUIFromNode(node, child, idSet);
      nodeList.insert(nodeList.end(), retList.begin(), retList.end());
    }

    if (child->type == XML_TEXT_NODE) {
      node->data = (char*)child->content;
    }
  }

  TrimString(node->data);
  node->type = (char*)readNode->name;


  return nodeList;
}


}; // namespace