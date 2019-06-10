#include "bigint.h"
#include "math.h"
BigInt::BigInt(const char *string_of_digits) {init (string_of_digits);}

BigInt::BigInt(const string string_of_digits) {init (string_of_digits);}

BigInt::BigInt(const int n)
{
  if (n < 0)
    negative = true;
  unsigned int nn = abs(n);
  num = nn;
}

bool BigInt::is_negative ()
{
  return negative;
}

void BigInt::init(char *string_of_digits)
{
  clear();
  if (string_of_digits[0] == '-')
    {
      negative = true;
      num.init (string_of_digits + 1);
    }
  else num.init (string_of_digits);
}

void BigInt::init(const string string_of_digits)
{
  clear();
  if (string_of_digits[0] == '-')
    {
      negative = true;
      num.init (string_of_digits.data() + 1);
    }
  else num.init (string_of_digits);
}

void BigInt::print()
{
  if (negative)
    printf ("-");
  num.print();
}

BigInt::BigInt(const BigInt &a) {negative = a.negative; num = a.num;}

BigInt BigInt::operator+(const BigInt &a) const
{
  BigInt c;
  if (a.negative ^ negative)
    {
      if (num > a.num)
        {
          c.num = num - a.num;
          c.negative = negative;
        }
      else
        {
          c.num = a.num - num;
          c.negative = a.negative;
        }
    }
  else
    {
      c.num = num + a.num;
      c.negative = negative;
    }
  return c;
}

BigInt BigInt::operator-(const BigInt &a) const
{
  BigInt c;
  if (a.negative ^ negative)
    {
      c.num = num + a.num;
      c.negative = negative;
    }
  else
    {
      if (num > a.num)
        {
          c.num = num - a.num;
          c.negative = negative;
        }
      else
        {
          c.num = a.num - num;
          c.negative = !a.negative;
        }
    }
  return c;
}

BigInt BigInt::operator*(const BigInt &a) const
{
  BigInt c;
  c.negative = negative ^ a.negative;
  c.num = num * a.num;
  return c;
}

void BigInt::operator*=(const BigInt &a)
{
  (*this) = (*this) * a;
}

void BigInt::operator+=(int a)
{
  *this = *this + a;
}

void BigInt::operator+=(const BigInt &a)
{
  *this = *this + a;
}

BigInt BigInt::operator/(int a) const
{
  BigInt c;
  c.negative = negative ^ (a < 0 ? true : false);
  a = abs (a);
  c.num = num / a;
  return c;
}

int BigInt::operator%(int a) const
{
  int overflow = num % abs (a);
  return overflow * (negative ^ (a < 0) ? -1 : 1);
}

BigInt BigInt::operator/(const BigInt &a) const
{
  BigInt c;
  c.num = num / a.num;
  c.negative = negative ^ a.negative;
  return c;
}

BigInt BigInt::operator%(const BigInt &a) const
{
  BigInt c;
  c.num = num % a.num;
  c.negative = negative ^ a.negative;
  return c;
}

BigInt BigInt::operator*(int a) const
{
  BigInt c;
  c.num = num * abs (a);
  c.negative = negative ^ (a < 0 ? true : false);
  return c;
}

int& BigInt::operator[](unsigned int n)
{
  return num[n];
}

int BigInt::operator[](unsigned int n) const
{
  return num[n];
}

int BigInt::operator==(const BigInt &a) const
{
  return (num == a.num) && (negative == a.negative);
}

void BigInt::operator=(const int a)
{
  num = abs(a);
  negative = a < 0 ? true : false;
}

int BigInt::operator<(const BigInt &a) const
{
  if (negative ^ a.negative)
    {
      if (negative)
        return 1;
      else
        return 0;
    }
  else
    {
      if (negative)
        return num > a.num;
      else
        return num < a.num;
    }
}

void BigInt::clear()
{
  num.clear();
  negative = false;
}

unsigned int BigInt::size() const
{
  return num.size ();
}

//zero if prime
int BigInt::get_first_non_trivial_divider()
{
  BigInt c = (*this) / 2;
  BigInt q("3");

  if (!((*this)[0] & 1))
    return 2;

  for (; q < c; q += 2)
    {
      if ((*this) % q == (int)0)
        return q[0];
    }
  return 0;
}

bool BigInt::is_prime()
{
  return !get_first_non_trivial_divider();
}

int BigInt::number_of_degrees()
{
  return num.number_of_degrees();
}

bool BigInt::is_int()
{
  return num.is_int();
}

int binary_power(BigInt a, int n, int mod)
{
  BigInt res = 1;
  while (n)
    {
      if (n & 1)
        res *= a;
      a *= a;
      n >>= 1;
      res = res % mod;
      a   = a % mod;
    }

  return res % mod;
}
