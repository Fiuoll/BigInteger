#ifndef BIGINT_H
#define BIGINT_H

#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include "biguint.h"

using namespace std;
extern vector<int> primes;
extern vector<vector<int>> inverted_pi_mod_pj;

class BigInt
{
private:
  //false if positive
  //true if negative value
  BigUInt num;
  bool negative = false;
public:
  BigInt(const char *string_of_digits = "0");
  BigInt(const string string_of_digits);
  BigInt(const int n);

  bool is_negative ();

  void init (char *string_of_digits);
  void init (const string string_of_digits);
  void print ();
  BigInt (const BigInt &a);
  BigInt operator+ (const BigInt &a) const;
  BigInt operator- (const BigInt &a) const;
  BigInt operator* (int a) const;
  BigInt operator* (const BigInt &a) const;
  BigInt operator/ (int a) const;
  void operator*= (const BigInt &a);
  void operator+= (int a);
  void operator+= (const BigInt &a);
  int operator% (int a) const;
  BigInt operator/ (const BigInt &a) const;
  BigInt operator% (const BigInt &a) const;
  int& operator[] (unsigned int n);
  int operator[] (unsigned int n) const;
  int operator== (const BigInt &a) const;
  void operator= (const int a);
  int operator< (const BigInt &a) const;

  void clear ();
  int get_first_non_trivial_divider ();
  bool is_prime ();
  int number_of_degrees ();
  bool is_int ();
  unsigned int size() const;
  BigUInt to_biguint ()
  {
    if (negative)
      printf ("Warning: Value is negative but converted as positive\n");
    return num;
  }
};

//a**n
int binary_power (BigInt a, int n, int mod);
#endif // BIGINT_H
