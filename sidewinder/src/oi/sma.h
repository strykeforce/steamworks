#pragma once

#include <cstddef>

namespace sidewinder {
namespace oi {

class SMA {
 private:
  size_t period_;
  double* window_;  // array holding values we're averaging
  double* head_;    // points to oldest elememnt
  double* tail_;    // points to newest element
  double total_;    // cache total so we don't sum everything each time

  /// Bumps the given pointer up by one and wraps to start of array if needed.
  void inc(double*& p);

  /// Returns how many numbers we have stored.
  ptrdiff_t size() const;

 public:
  SMA(size_t period);
  virtual ~SMA();

  double operator()(const double val);
};

} /* oi */
} /* sidewinder */
