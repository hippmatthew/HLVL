#include "src/core/include/signature.hpp"

namespace pp
{

Signature::Signature(unsigned long b) : bits(b) {}

Signature& Signature::operator = (unsigned long b)
{
  bits = b;
  return *this;
}

Signature& Signature::operator |= (const Signature& signature)
{
  if (this == &signature) return *this;

  bits |= signature.bits;

  return *this;
}

Signature& Signature::operator &= (const Signature& signature)
{
  if (this == &signature) return *this;

  bits &= signature.bits;

  return *this;
}

Signature Signature::operator | (const Signature& signature)
{
  return Signature(bits | signature.bits);
}

Signature Signature::operator & (const Signature & signature)
{
  return Signature(bits & signature.bits);
}

bool Signature::operator == (const Signature& signature)
{
  return bits == signature.bits;
}

} // namespace pp