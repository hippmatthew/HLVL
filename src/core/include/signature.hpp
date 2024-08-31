#ifndef physp_core_signature_hpp
#define physp_core_signature_hpp

namespace pp
{

class Signature
{
  public:
    Signature() = default;
    Signature(unsigned long);
    Signature(const Signature&) = default;
    Signature(Signature&&) = default;

    ~Signature() = default;

    Signature& operator = (const Signature&) = default;
    Signature& operator = (Signature&&) = default;
    Signature& operator = (unsigned long);
    Signature& operator |= (const Signature&);
    Signature& operator &= (const Signature&);

    Signature operator | (const Signature&) const;
    Signature operator & (const Signature&) const;
    Signature operator ~ () const;

    bool operator == (const Signature&) const;
    bool operator != (const Signature&) const;

  private:
    unsigned long bits = 0x0ul;
};

} // namespace pp

#endif // physp_core_signature_hpp