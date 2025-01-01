/**
 * (c) 2024 Daniel Kramer
 * An easy c++ timer. Only this header file is needed. Refer to the README or
 * https://github.com/drkspace/EasyTimer for information.
 */
#ifndef _EASY_TIMER_HPP
#define _EASY_TIMER_HPP

#include <chrono>
#include <iostream>

/**
 * A class for timing a block of code.
 *
 * Example:
 *     {
 *         eastTimer t;
 *         // Some piece of code
 *         // Will print out the elapsed time
 *     }
 *
 * @tparam Unit The unit (chrono duration) to use for the output time. If a
 * default chrono duration is used (i.e. std::chrono::second), then the unit
 * will be automatically detected. If a custom one is used, then include in the
 * suffix argument.
 * @tparam Clock The type of clock to use.
 */
template <typename Unit = std::chrono::seconds,
          typename Clock = std::chrono::high_resolution_clock>
class easyTimer {
private:
  // The start time
  std::chrono::time_point<Clock> start{};

  // The prefix/suffix to print with the elapsed time.
  std::string prefix, suffix;

  /**
   * Deduces the unit abbreviation if a chrono unit is used.
   *
   * If the unit is custom, then it is recommended to include a unit in the
   * suffix.
   *
   * @returns The unit abbreviation if it is known.
   */
  const char *deduceUnit() {
#if __cplusplus >= 201703L // c++17
    if constexpr
#else // c++11 (doesn't have "if constexpr")
    if
#endif
        (std::is_same<Unit, std::chrono::nanoseconds>::value) {
      return "ns";
    } else if (std::is_same<Unit, std::chrono::microseconds>::value) {
      return "us";
    } else if (std::is_same<Unit, std::chrono::milliseconds>::value) {
      return "ms";
    } else if (std::is_same<Unit, std::chrono::seconds>::value) {
      return "s";
    } else if (std::is_same<Unit, std::chrono::hours>::value) {
      return "h";
    }
#if __cplusplus >= 202002L // c++20
    else if (std::is_same<Unit, std::chrono::days>::value) {
      return "d";
    } else if (std::is_same<Unit, std::chrono::weeks>::value) {
      return "w";
    } else if (std::is_same<Unit, std::chrono::months>::value) {
      return "m";
    } else if (std::is_same<Unit, std::chrono::years>::value) {
      return "y";
    }
#else // The other units didn't exist until c++20
    else {
      return "";
    }
#endif
  }

public:
  /**
   * The default constructor. Will initialize the prefix to "Time Elapsed: "
   */
  easyTimer() : start(Clock::now()), prefix("Time Elapsed: ") {}

  /**
   * A constructor for a custom prefix.
   */
  easyTimer(std::string prefix) : start(Clock::now()), prefix(prefix) {}

  /**
   * A constructor for a custom prefix and suffix.
   */
  easyTimer(std::string prefix, std::string suffix)
      : start(Clock::now()), prefix(prefix), suffix(suffix) {}

  /**
   * The destructor. Will print out the elapsed time when the object goes out of
   * scope.
   */
  ~easyTimer() {
    std::cout << prefix
              << std::chrono::duration<double, typename Unit::period>(
                     Clock::now() - start)
                     .count()
              << deduceUnit() << suffix << std::endl;
  }
};

#endif