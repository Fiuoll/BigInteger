#include "parser.h"


void compute_r()
{
  // p_i ^ (p_j - 2) = p_i ^ (-1) mod (p_j)
  inverted_pi_mod_pj.resize(PRIME_BASE);
  for (unsigned int i = 0; i < PRIME_BASE; i++)
    {
      inverted_pi_mod_pj[i].resize(PRIME_BASE);
      for (unsigned int j = 0; j < PRIME_BASE; j++)
        {
          inverted_pi_mod_pj[i][j] = binary_power (primes[i], primes[j] - 2, primes[j]);
        }
    }

#if PRINTS
  for (unsigned int i = 0; i < PRIME_BASE; i++)
    {
      for (unsigned int j = 0; j < PRIME_BASE; j++)
        printf ("%d, ", inverted_pi_mod_pj[i][j]);
      printf ("\n");
    }
#endif
}

int scan_p(const char *filename)
{
  FILE *fp = fopen (filename, "r");
  if (!fp)
    {
      printf ("Cannot open file '%s' \n", filename);
      return 1;
    }
  primes.clear();
  int a;
  while ((fscanf (fp, "%d", &a) == 1))
    {
      primes.push_back(a);
    }
  return 0;
}

int scan_p(const char *a, const char *b)
{
  BigUInt n (a);

#if PRINTS
  n.print();
  printf ("\n");
#endif

  unsigned int num_of_primes = atoi (b);
  primes.resize(num_of_primes);

  int k;
  for (unsigned int i = 0; i < num_of_primes; i++, ++n)
    {
      k = n.get_first_non_trivial_divider();
      while (k)
        {
          ++n;
          k = n.get_first_non_trivial_divider();
        }
      primes[i] = n[0];
#if PRINTS
      n.print();
      printf ("\n");
#endif
    }
  return 0;
}

void print_p()
{
  printf ("Prime basis:\n");
  for (unsigned int i = 0; i < PRIME_BASE; i++)
    printf ("p[%d] = %d\n", i + 1, primes[i]);
}

void print_r()
{
  printf ("Inverted values:\n");

  for (unsigned int i = 0; i < PRIME_BASE; i++)
    {
      for (unsigned int j = 0; j < PRIME_BASE; j++)
        {
          printf ("%d ", inverted_pi_mod_pj[i][j]);
        }
      printf ("\n");
    }
}

int parse_command_line(int argc, char **argv)
{
  if (argc == 1)
    {
      printf ("Will be using default prime basis.");
      compute_r();
    }
  else if (argc == 2)
    {
      // ./a.out name_of_file_with_primes
      if (scan_p (argv[1]))
        return 1;
      compute_r();
    }
  else if (argc == 3)
    {
      // ./a.out start_primes number_of_primes
      double dt = clock();
      scan_p (argv[1], argv[2]);
      compute_r();
      dt = (clock() - dt) / CLOCKS_PER_SEC;
      printf ("Calculation of %d prime numbers since %d value %e sec\n",atoi (argv[2]), atoi (argv[1]), dt);
    }
#if PRINTS
  print_p ();
  print_r ();
#endif
  return 0;
}
