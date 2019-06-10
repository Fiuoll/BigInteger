#include <iostream>
#include "biguint.h"
#include "moduint.h"
#include "primes.h"
#include "parser.h"
using namespace std;

#define DEBUG_PRINT 0
#define DEBUG_CHECK 0
int main(int argc, char **argv)
{
  if (parse_command_line (argc, argv))
    return 1;

  char buf1[12345] = {};
  char buf2[12345] = {};
  char op;
  BigInt c ;
//check matrix
#if DEBUG_CHECK
  for (unsigned int i = 0; i < PRIME_BASE; i++)
    {
      for (unsigned int j = 0; j < PRIME_BASE; j++)
        {
          if (i == j)
            continue;
          c = primes[i];
          c *= inverted_pi_mod_pj[i][j];
          if (c % primes[j] != 1)
            {
              printf ("Wrong table!\n");
              return 1;
            }
        }
    }
#endif

  BigUInt max_value = 1;
  for (auto &val : primes)
    max_value *= val;

  printf ("Input 'q' for quit\n");
  printf ("Values and result must be less then ");
  max_value.print();
  printf ("Number of degrees: %d\n", max_value.number_of_degrees());
  printf ("\n");
  printf ("Input [value] [+/-/*] [value]:\n");

  while (1)
    {
      scanf ("%s", buf1);

      if (buf1[0] == 'q')
        break;
      else if (buf1[0] == 'p')
        {
          print_p();
          continue;
        }
      else if (buf1[0] == 'r')
        {
          print_r();
          continue;
        }

      BigInt a (buf1);
#if DEBUG_PRINT
      a.print();
#endif

      scanf(" %c ", &op);
#if DEBUG_PRINT
      printf ("%c\n", op);
#endif
      scanf ("%s", buf2);

      BigInt b (buf2);
#if DEBUG_PRINT
      b.print();
#endif

      printf ("%s %c %s = ", buf1, op, buf2);

      switch (op)
        {
        case '+':
          c = a + b;
          break;
        case '-':
          c = a - b;
          break;
        case '*':
          c = a * b;
          break;
        default:
          printf ("%c\n", op);
          break;
        }
      c.print();

      ModUInt aa (a.to_biguint());
#if DEBUG_PRINT
      BigUInt aaa = aa.to_biguint();
      printf ("a = ");
      aaa.print();
#endif

      ModUInt bb (b.to_biguint());
#if DEBUG_PRINT
      BigUInt bbb = bb.to_biguint();
      printf ("b = ");
      bbb.print();
#endif

      ModUInt cc;

      switch (op)
        {
        case '+':
          cc = aa + bb;
          break;
        case '-':
          cc = aa - bb;
          break;
        case '*':
          cc = aa * bb;
          break;
        default:
          printf ("%c\n", op);
          break;
        }

      BigUInt ccc = cc.to_biguint();
      printf ("%s %c %s = ", buf1, op, buf2);
      ccc.print();

    }

  return 0;
}
