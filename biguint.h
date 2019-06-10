#ifndef BIGUINT_H
#define BIGUINT_H

#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#define BASE (1000 * 1000 * 1000)
#define NUMBER_OF_DIGITS_IN_BASE 9
using namespace std;
extern vector<int> primes;
extern vector<vector<int>> inverted_pi_mod_pj;

class BigUInt
{
private:
  vector<int> bui;
  void clear_first_zeroes ();
public:
  unsigned int size () const;
  BigUInt(const char *string_of_digits = "0");
  BigUInt(const string string_of_digits);
  BigUInt(const unsigned int n);
  void init (char *string_of_digits);
  void init (const string string_of_digits);
  void print ();
  BigUInt (const BigUInt &a);
  BigUInt operator+ (const BigUInt &a) const;
  BigUInt operator- (const BigUInt &a) const;
  BigUInt operator* (int a) const;
  BigUInt operator* (const BigUInt &a) const;
  BigUInt operator/ (int a) const;
  void operator*= (const BigUInt &a);
  void operator+= (int a);
  void operator+= (const BigUInt &a);
  int operator% (int a) const;
  BigUInt operator/ (const BigUInt &a) const;
  BigUInt operator% (const BigUInt &a) const;
  int& operator[] (unsigned int n);
  int operator[] (unsigned int n) const;
  int operator== (const BigUInt &a) const;
  void operator= (const int a);
  int operator< (const BigUInt &a) const;
  int operator>(const BigUInt &a) const;
  void operator++ ();
  void clear ();
  int get_first_non_trivial_divider ();
  bool is_prime ();
  int number_of_degrees ();
  bool is_int ();
};

//a**n
int binary_power (BigUInt a, int n, int mod);
#endif // BIGUINT_H
