#ifndef PPM_INCLUDE_PPACK_TIMER_H
#define PPM_INCLUDE_PPACK_TIMER_H

#include <chrono>

#ifdef _GUI_CREATOR_BUILD_DLL
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif


namespace pPack {
namespace timer {

class Timer {
  double totalTime;
  double deltaTime;
  double timeScale;
  size_t updateDelay;

  std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> startPoint;


public:

  static DLL Timer& GetSingleton();



  DLL void Advance();
  DLL void Advance(double delta);

  DLL double GetDeltaTime() const;
  DLL double GetTimeScale() const;
  DLL size_t GetUpdateDelay() const;
  DLL double GetTotalTime() const;

  DLL void SetTimeScale(double Scale);
  DLL void SetUpdateDelay(double Rate);

  DLL void WaitForNextUpdate();
  DLL void ResetDeltaTime();


  DLL Timer();

};

}; // namespace timing
}; // namespace pPack


#endif