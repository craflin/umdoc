
#include "PlainGenerator.h"

#include <nstd/Unicode.hpp>

String PlainGenerator::escapeChar(uint32 c)
{
  return Unicode::toString(c);
}
