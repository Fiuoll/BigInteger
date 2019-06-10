#ifndef PARSER_H
#define PARSER_H
#include "primes.h"
#define PRINTS 0
void compute_r();

int scan_p (const char *filename);
int scan_p (const char *a, const char *b);

void print_p ();
void print_r ();

int parse_command_line (int argc, char **argv);
extern vector<int> primes;
extern vector<vector<int>> inverted_pi_mod_pj;

#endif // PARSER_H
