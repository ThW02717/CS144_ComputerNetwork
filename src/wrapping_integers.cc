#include "wrapping_integers.hh"
#include "debug.hh"
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <algorithm>
using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
 

  return Wrap32( static_cast<uint32_t>( ( zero_point.raw_value_ + n ) % ( (uint64_t)UINT32_MAX + 1 ) ) );
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // calculate offset（From zero_point to raw_value_)
  uint32_t offset = raw_value_ >= zero_point.raw_value_
                      ? raw_value_ - zero_point.raw_value_
                      : raw_value_ + (1UL << 32) - zero_point.raw_value_;

  // Closest base: logical operation （操場第幾圈的開頭）
  uint64_t base = checkpoint & ~( (1ULL << 32) - 1 ); // checkpoint - checkpoint % 2^32

  // possible answer
  uint64_t candidate1 = base + offset;
  uint64_t candidate2 = candidate1 + (1ULL << 32);
  std::vector<uint64_t> candidates = { candidate1, candidate2 };

  // Special case（base - 2^32）
  if (candidate1 >= (1ULL << 32)) {
    candidates.push_back(candidate1 - (1ULL << 32));
  }

  // Correct candidate
  return *std::min_element(candidates.begin(), candidates.end(),
                           [=](uint64_t a, uint64_t b) {
                             return std::abs((int64_t)(a - checkpoint)) < std::abs((int64_t)(b - checkpoint));
                           });
}