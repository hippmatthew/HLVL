#include "include/signature.hpp"

namespace hlvl
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

Signature Signature::operator | (const Signature& signature) const
{
  return Signature(bits | signature.bits);
}

Signature Signature::operator & (const Signature & signature) const
{
  return Signature(bits & signature.bits);
}

Signature Signature::operator ~ () const
{
  return Signature(~bits);
}

bool Signature::operator == (const Signature& signature) const
{
  return bits == signature.bits;
}

bool Signature::operator != (const Signature& signature) const
{
  return bits != signature.bits;
}

} // namespace hlvl