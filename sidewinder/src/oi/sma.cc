#include "sma.h"

#include <cassert>
#include <cstddef>

using namespace sidewinder::oi;

SMA::SMA(size_t period)
    : period_(period),
      window_(new double[period]),
      head_(nullptr),
      tail_(nullptr),
      total_(0.0) {
  assert(period >= 1);
}

SMA::~SMA() { delete[] window_; }

void SMA::inc(double*& p) {
  if (++p >= window_ + period_) {
    p = window_;
  }
}

ptrdiff_t SMA::size() const {
  if (head_ == nullptr) {
    return 0;
  }
  if (head_ == tail_) return period_;
  return (period_ + tail_ - head_) % period_;
}

double SMA::operator()(const double val) {
  // add a value to the average, pushing one out if neccessary
  // special case for initialization
  if (head_ == nullptr) {
    head_ = window_;
    *head_ = val;
    tail_ = head_;
    inc(tail_);
    total_ = val;
  } else {
    // were we already full?
    if (head_ == tail_) {
      // oldest value drops out of window so update total cache
      total_ -= *head_;
      // make room
      inc(head_);
    }

    // write the value in the next spot.
    *tail_ = val;
    inc(tail_);

    // update our total cache
    total_ += val;
  }

  // calculate average
  return total_ / static_cast<double>(size());
}
