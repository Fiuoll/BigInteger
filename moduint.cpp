#include "moduint.h"
extern vector<int> primes;
extern vector<vector<int>> inverted_pi_mod_pj;

ModUInt::ModUInt(const BigUInt &a)
{
  mui.resize(PRIME_BASE);
  for (unsigned int i = 0; i < PRIME_BASE; i++)
    {
      mui[i] = a % primes[i];
    }
}

ModUInt::ModUInt(const ModUInt &a)
{
  for (unsigned int i = 0; i < PRIME_BASE; i++)
    mui[i] = a[i];
}

ModUInt ModUInt::operator+(const ModUInt &a)
{
  ModUInt q;
  for (unsigned int i = 0; i < PRIME_BASE; i++)
    {
      q[i] = (mui[i] + a[i]) % primes[i];
    }
  return q;
}

ModUInt ModUInt::operator-(const ModUInt &a)
{
  ModUInt q;
  for (unsigned int i = 0; i < PRIME_BASE; i++)
    {
      q[i] = (mui[i] - a[i] + primes[i]) % primes[i];
    }
  return q;
}

ModUInt ModUInt::operator*(const ModUInt &a)
{
  ModUInt q;
  for (unsigned int i = 0; i < PRIME_BASE; i++)
    {
      q[i] = (mui[i] * a[i]) % primes[i];
    }
  return q;
}

int ModUInt::operator[](const unsigned int i) const
{
  return i < PRIME_BASE ? mui[i] : 0;
}

int &ModUInt::operator[](const unsigned int i)
{
  return mui[i];
}
