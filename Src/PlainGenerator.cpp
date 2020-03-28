
#include "PlainGenerator.h"

#include <nstd/Unicode.h>

String PlainGenerator::escapeChar(uint32 c)
{
  return Unicode::toString(c);
}
