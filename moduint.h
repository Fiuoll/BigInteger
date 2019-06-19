#ifndef MODUINT_H
#define MODUINT_H
#include <iostream>
#include <vector>
#include "biguint.h"
#include "bigint.h"
#include "primes.h"

extern vector<int> primes;
extern vector<vector<int>> inverted_pi_mod_pj;
using namespace std;

class ModUInt
{
private:
  vector<uint64_t> mui;
public:
  ModUInt (const BigUInt &a = BigUInt ());
  ModUInt (const ModUInt &a);
  ModUInt operator+ (const ModUInt &a);
  ModUInt operator- (const ModUInt &a);
  ModUInt operator* (const ModUInt &a);
  uint64_t operator[] (const unsigned int i) const;
  uint64_t &operator[] (const unsigned int i);

  void clear ()
  {
    for (auto &val : mui)
      val = 0;
  }

  BigInt to_bigint ()
  {
    BigInt a;
    BigInt b;
    vector<BigInt> x;
    x.resize (mui.size ());
    for (unsigned int i = 0; i < PRIME_BASE; i++)
      {
        x[i] = mui[i];
        for (unsigned int j = 0; j < i; j++)
          {
            x[i] = (x[i] - x[j]) * inverted_pi_mod_pj[j][i];

            x[i] = x[i] % primes[i];
            if (x[i] < 0)
              x[i] += primes[i];
          }
      }
    for (unsigned int i = 0; i < PRIME_BASE; i++)
      {
        b = x[i];
        for (unsigned int j = 0; j < i; j++)
          {
            b *= primes[j];
          }
        a += b;
      }
    return a;
  }
  BigUInt to_biguint ()
  {
    return to_bigint().to_biguint();
  }
  uint64_t *data ()
  {
    return mui.data();
  }
};




#endif // MODUINT_H
