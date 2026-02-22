#ifndef PPM_INCLUDE_PPACK_SHADER_HANDLING_H
#define PPM_INCLUDE_PPACK_SHADER_HANDLING_H

#include <string>

#include "pPack/vector.h"


#ifdef _GUI_CREATOR_BUILD_DLL
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif


namespace pPack {
namespace shader_handling {
struct ShaderCreateInfo {
  const char** shaders;
  unsigned int shaderCount;
  unsigned int type;

  ShaderCreateInfo(const char** s, unsigned int sC, unsigned int t) : shaders(s), shaderCount(sC), type(t) {}
};

class ShaderHandler {
  unsigned int internal;

  static unsigned int currentShader;

public:


  DLL void Active();

  DLL operator int() {
    return internal;
  }

  DLL ShaderHandler() {
    internal = 0;
  }
  DLL ShaderHandler(unsigned int s) {
    internal = s;
  }



  static DLL bool FileExists(std::string file);

  static DLL unsigned int GetCurrentShader() {
    return currentShader;
  }
  static DLL void SetCurrentShader(unsigned int s);

  static DLL unsigned int GetShader(std::string loc);

  static DLL unsigned int CreateShader(std::string name, ShaderCreateInfo* shaderInfo, size_t shaderCount);

  static DLL void SetBool(std::string name, bool value);
  static DLL void SetUInt(std::string name, unsigned int value);
  static DLL void SetInt(std::string name, int value);
  static DLL void SetFloat(std::string name, float value);
  static DLL void SetVector2(std::string name, float x, float y);
  static DLL void SetVector2(std::string name, vector::Vector2 vec);
  static DLL void SetVector3(std::string name, float x, float y, float z);
  static DLL void SetVector3(std::string name, vector::Vector3 vec);
  static DLL void SetVector4(std::string name, float x, float y, float z, float w);
  static DLL void SetVector4(std::string name, vector::Vector4 vec);
  static DLL void SetMat4(std::string name, const float* Mat);


  static DLL void SetBool(const char* name, bool value);
  static DLL void SetUInt(const char* name, unsigned int value);
  static DLL void SetInt(const char* name, int value);
  static DLL void SetFloat(const char* name, float value);
  static DLL void SetVector2(const char* name, float x, float y);
  static DLL void SetVector2(const char* name, vector::Vector2 vec);
  static DLL void SetVector3(const char* name, float x, float y, float z);
  static DLL void SetVector3(const char* name, vector::Vector3 vec);
  static DLL void SetVector4(const char* name, float x, float y, float z, float w);
  static DLL void SetVector4(const char* name, vector::Vector4 vec);
  static DLL void SetMat4(const char* name, const float* Mat);
};
} //namespace shader_handling
} //namespace pPack

#endif