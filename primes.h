#ifndef PRIMES_H
#define PRIMES_H
#include "vector"
#include "array"
#include "biguint.h"
#define PRIME_BASE primes.size ()
extern vector<int> primes;
extern vector<vector<int>> inverted_pi_mod_pj;

void compute_r();

#endif // PRIMES_H
