// 2020-06-18, Jan de Cuveland <cuveland@compeng.uni-frankfurt.de>

#ifndef STOPWATCH_H
#define STOPWATCH_H

/**
 * Stopwatch
 *
 * Stopwatch class. This class returns the real and cpu time between the start
 * and stop events. Modeled after Root's TStopwatch class.
 */

#include <chrono>
#include <ctime>
#include <stdexcept>

class Stopwatch {
public:
  Stopwatch() { Start(); }

  void Start(bool reset = true) {
    if (reset) {
      fState = State::undefined;
      fTotalRealTime = 0;
      fTotalCpuTime = 0;
      fCounter = 0;
    }
    if (fState != State::running) {
      fStartRealTime = std::chrono::steady_clock::now();
      fStartCpuTime = std::clock();
    }
    fState = State::running;
    fCounter++;
  }

  void Stop() {
    fStopRealTime = std::chrono::steady_clock::now();
    fStopCpuTime = std::clock();

    if (fState == State::running) {
      fTotalRealTime +=
          std::chrono::duration<double>(fStopRealTime - fStartRealTime).count();
      fTotalCpuTime += double(fStopCpuTime - fStartCpuTime) / CLOCKS_PER_SEC;
    }
    fState = State::stopped;
  }

  void Continue() {
    if (fState == State::undefined)
      throw std::runtime_error("stopwatch not started");

    if (fState == State::stopped) {
      fTotalRealTime -=
          std::chrono::duration<double>(fStopRealTime - fStartRealTime).count();
      fTotalCpuTime -= double(fStopCpuTime - fStartCpuTime) / CLOCKS_PER_SEC;
    }

    fState = State::running;
  }

  int Counter() const { return fCounter; }

  double RealTime() {
    if (fState == State::undefined)
      throw std::runtime_error("stopwatch not started");

    if (fState == State::running)
      Stop();

    return fTotalRealTime;
  }

  double CpuTime() {
    if (fState == State::undefined)
      throw std::runtime_error("stopwatch not started");

    if (fState == State::running)
      Stop();

    return fTotalCpuTime;
  }

  void Reset() {
    ResetCpuTime();
    ResetRealTime();
  }

  void ResetRealTime(double time = 0) {
    Stop();
    fTotalRealTime = time;
  }

  void ResetCpuTime(double time = 0) {
    Stop();
    fTotalCpuTime = time;
  }

  void Print(const char *option = "") {
    double realt = RealTime();
    double cput = CpuTime();

    int hours = int(realt / 3600);
    realt -= hours * 3600;
    int min = int(realt / 60);
    realt -= min * 60;
    int sec = int(realt);

    if (realt < 0)
      realt = 0;
    if (cput < 0)
      cput = 0;

    if (option && *option == 'm') {
      if (Counter() > 1) {
        printf("Real time %d:%02d:%06.3f, CP time %.3f, %d slices\n", hours,
               min, realt, cput, Counter());
      } else {
        printf("Real time %d:%02d:%06.3f, CP time %.3f\n", hours, min, realt,
               cput);
      }
    } else if (option && *option == 'u') {
      if (Counter() > 1) {
        printf("Real time %d:%02d:%09.6f, CP time %.3f, %d slices\n", hours,
               min, realt, cput, Counter());
      } else {
        printf("Real time %d:%02d:%09.6f, CP time %.3f\n", hours, min, realt,
               cput);
      }
    } else {
      if (Counter() > 1) {
        printf("Real time %d:%02d:%02d, CP time %.3f, %d slices\n", hours, min,
               sec, cput, Counter());
      } else {
        printf("Real time %d:%02d:%02d, CP time %.3f\n", hours, min, sec, cput);
      }
    }
  }

private:
  enum class State { undefined, stopped, running };

  std::chrono::steady_clock::time_point fStartRealTime; // wall clock start time
  std::chrono::steady_clock::time_point fStopRealTime;  // wall clock stop time
  std::clock_t fStartCpuTime;                           // cpu start time
  std::clock_t fStopCpuTime;                            // cpu stop time
  double fTotalRealTime;                                // total real time
  double fTotalCpuTime;                                 // total cpu time
  State fState;                                         // stopwatch state
  int fCounter; // number of times the stopwatch was started
};

#endif
