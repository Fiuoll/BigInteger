#include "biguint.h"

BigUInt::BigUInt(const char *string_of_digits) {init (string_of_digits);}

BigUInt::BigUInt(const string string_of_digits) {init (string_of_digits);}

BigUInt::BigUInt(const unsigned int n)
{
  bui.resize(1, 0);
  bui[0] = n;
}

void BigUInt::init(char *string_of_digits)
{
  clear();
  for (unsigned int i = strlen(string_of_digits); i > 0; i -= NUMBER_OF_DIGITS_IN_BASE)
    {
      string_of_digits[i] = 0;
      bui.push_back (atoi (i>=NUMBER_OF_DIGITS_IN_BASE ? string_of_digits + i - NUMBER_OF_DIGITS_IN_BASE : string_of_digits));
    }

  clear_first_zeroes ();
}

void BigUInt::init(const string string_of_digits)
{
  clear();
  for (int i = string_of_digits.length(); i > 0; i -= NUMBER_OF_DIGITS_IN_BASE)
    {
      if (i < NUMBER_OF_DIGITS_IN_BASE)
        bui.push_back(atoi (string_of_digits.substr(0, i).c_str()));
      else
        bui.push_back(atoi (string_of_digits.substr(i - NUMBER_OF_DIGITS_IN_BASE, NUMBER_OF_DIGITS_IN_BASE).c_str()));
    }

  while (bui.size() > 1 && bui.back() == 0)
    bui.pop_back();
}

void BigUInt::print()
{
  char buf[100];
  sprintf (buf, "%%%02dd", NUMBER_OF_DIGITS_IN_BASE);
  //  printf ("%s\n",buf);
  printf ("%d", bui.empty() ? 0 : bui.back());
  for (int i = (int) bui.size() - 2; i >= 0; i--)
    printf (buf, bui[i]);
  printf ("\n");
}

BigUInt::BigUInt(const BigUInt &a) {bui = a.bui;}

BigUInt BigUInt::operator+(const BigUInt &a) const
{
  BigUInt c;
  int overflow = 0;
  unsigned int maxsize = max (a.size(), size());
  for (unsigned int i = 0; i < maxsize || overflow; i++)
    {
      c[i] = overflow + a[i] + (*this)[i];
      overflow = c[i] >= BASE ? 1 : 0;
      if (overflow)
        c[i] -= BASE;
    }
  return c;
}

BigUInt BigUInt::operator-(const BigUInt &a) const
{
  BigUInt c;
  if (*(this) < a)
    {
      printf ("Wrong values! \n");
      return c;
    }
  int overflow = 0;
  for (unsigned int i = 0; i < size() || overflow; i++)
    {
      c[i] = (*this)[i] - overflow - a[i];
      overflow = c[i] < 0 ? 1 : 0;
      if (overflow)
        c[i] += BASE;
    }
  c.clear_first_zeroes ();
  return c;
}

BigUInt BigUInt::operator*(const BigUInt &a) const
{
  if (a.size() == 1)
    {
      return (*this) * a[0];
    }
  else if (size() == 1)
    {
      return a * (*this)[0];
    }
  else
    {
      BigUInt c;
      int overflow;
      unsigned int j = 0, i = 0;
      for (i = 0; i < a.size(); i++)
        {
          for (overflow = 0, j = 0; j < (*this).size() || overflow; j++)
            {
              long long val = c[i + j] + (long long) a[i] * (*this)[j] + overflow;
              overflow = int (val / BASE);
              c[i + j] = int (val - overflow * BASE);
            }
        }
      c.clear_first_zeroes();
      return c;
    }
}

void BigUInt::operator*=(const BigUInt &a)
{
  (*this) = (*this) * a;
}

void BigUInt::operator+=(int a)
{
  *this = *this + a;
}

void BigUInt::operator+=(const BigUInt &a)
{
  *this = *this + a;
}

BigUInt BigUInt::operator/(int a) const
{
  BigUInt c;
  unsigned int overflow = 0;
  for (int i = size () - 1; i >= 0; i--)
    {
      long long curr = (*this)[i] + (long long) overflow * BASE;
      c[i] = curr / a;
      overflow = curr % a;
    }
  c.clear_first_zeroes ();
  return c;
}

int BigUInt::operator%(int a) const
{
  unsigned int overflow = 0;
  long long curr;
  for (int i = size () - 1; i >= 0; i--)
    {
      curr = (*this)[i] + (long long) overflow * BASE;
      overflow = curr % a;
    }
  return overflow;
}

BigUInt BigUInt::operator/(const BigUInt &a) const
{
  BigUInt c;
  BigUInt d;
  for (int i = size () - 1; i >= 0; --i)
    {
      d *= BASE;
      d[0] = (*this)[i];

      int x = 0;
      int l = 0, r = BASE;
      while (l <= r)
        {
          int m = (l + r) >> 1;
          BigUInt q = a * m;
          if (d < q)
            r = m - 1;
          else
            {
              x = m;
              l = m + 1;
            }
        }
      c[i] = x;
      d = d - a * x;
    }
  c.clear_first_zeroes();
  return c;
}

BigUInt BigUInt::operator%(const BigUInt &a) const
{
  BigUInt c;
  BigUInt d;
  for (int i = size () - 1; i >= 0; --i)
    {
      d *= BASE;
      d[0] = (*this)[i];

      int x = 0;
      int l = 0, r = BASE;
      while (l <= r)
        {
          int m = (l + r) >> 1;
          BigUInt q = a * m;
          if (d < q)
            r = m - 1;
          else
            {
              x = m;
              l = m + 1;
            }
        }
      c[i] = x;
      d = d - a * x;
    }
  c.clear_first_zeroes();
  return d;
}

BigUInt BigUInt::operator*(int a) const
{
  BigUInt c;
  long long val;
  int overflow = 0;
  for (unsigned int i = 0; i < size () || overflow; i++)
    {
      val = overflow + (long long)a * (*this)[i];
      overflow = val / BASE;
      c[i] = val - BASE * overflow;
    }

  void clear_first_zeroes ();
  return c;
}

int& BigUInt::operator[](unsigned int n)
{
  if (n >= bui.size())
    bui.resize(n + 1, 0);
  return bui[n];
}

int BigUInt::operator[](unsigned int n) const
{
  return (n < bui.size() ? bui[n] : 0);
}

int BigUInt::operator==(const BigUInt &a) const
{
  if (size() != a.size())
    return 0;
  for (int i = size() - 1; i >= 0; i--)
    {
      if (a[i] != bui[i])
        return 0;
    }
  return 1;
}

void BigUInt::operator=(const int a)
{
  bui.resize(1);
  bui[0] = a;
}

int BigUInt::operator<(const BigUInt &a) const
{
  if (size() < a.size())
    return 1;
  else if (size() > a.size())
    return  0;
  else
    {
      for (int i = size() - 1; i >= 0; --i)
        {
          if ((*this)[i] < a[i])
            return 1;
          else if ((*this)[i] > a[i])
            return 0;
        }
    }
  return 0;
}

int BigUInt::operator>(const BigUInt &a) const
{
  if (size() > a.size())
    return 1;
  else if (size() < a.size())
    return  0;
  else
    {
      for (int i = size() - 1; i >= 0; --i)
        {
          if ((*this)[i] > a[i])
            return 1;
          else if ((*this)[i] < a[i])
            return 0;
        }
    }
  return 0;
}

void BigUInt::operator++()
{
  for (int &v : bui)
    {
      v++;
      if (v < BASE)
        break;
      else
        v -= BASE;
    }
}

void BigUInt::clear()
{
  bui.clear();
}

unsigned int BigUInt::size() const
{
  return bui.size();
}

void BigUInt::clear_first_zeroes()
{
  while (bui.size() > 1 && bui.back() == 0)
    bui.pop_back();
}
//zero if prime
int BigUInt::get_first_non_trivial_divider()
{
  BigUInt c = (*this) / 2;
  BigUInt q("3");

  if (!((*this)[0] & 1))
    return 2;

  for (; q < c; q += 2)
    {
      if ((*this) % q == (int)0)
        return q.bui[0];
    }
  return 0;
}

bool BigUInt::is_prime()
{
  return !get_first_non_trivial_divider();
}

int BigUInt::number_of_degrees()
{
  int k = 0;
  for (int i = bui[size() - 1]; i; i /= 10)
    k++;
  return  NUMBER_OF_DIGITS_IN_BASE * (size () - 1) + k;
}

bool BigUInt::is_int()
{
  return size () <= 1 ? true : false;
}

int binary_power(BigUInt a, int n, int mod)
{
  BigUInt res = 1;
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
