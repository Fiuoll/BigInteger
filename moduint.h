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
  vector<int> mui;
public:
  ModUInt (const BigUInt &a = BigUInt ());
  ModUInt (const ModUInt &a);
  ModUInt operator+ (const ModUInt &a);
  ModUInt operator- (const ModUInt &a);
  ModUInt operator* (const ModUInt &a);
  int operator[] (const unsigned int i) const;
  int &operator[] (const unsigned int i);

  BigInt to_bigint ()
  {
    BigInt a;
    BigInt b;
    vector<int> x;
    x.resize (mui.size ());
    for (unsigned int i = 0; i < PRIME_BASE; i++)
      {
        x[i] = mui[i];
        for (unsigned int j = 0; j < i; j++)
          {
            x[i] = (x[i] - x[j]) * inverted_pi_mod_pj[j][i];

            x[i] = (x[i] + primes[i]) % primes[i];
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
    return a.to_biguint();
  }
};




#endif // MODUINT_H
