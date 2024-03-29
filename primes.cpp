#include "primes.h"

vector<vector<int>> inverted_pi_mod_pj =
{
 {0, 2, 5, 4, 9, 6, 13, 8, 21, 25,   },
 {2, 0, 3, 9, 8, 7, 4, 14, 25, 15,   },
 {1, 3, 0, 8, 2, 5, 11, 10, 9, 16,   },
 {2, 1, 2, 0, 6, 14, 7, 21, 17, 27,  },
 {1, 2, 6, 6, 0, 4, 3, 16, 12, 20,   },
 {2, 3, 5, 2, 10, 0, 9, 19, 11, 24,  },
 {1, 4, 3, 7, 11, 9, 0, 17, 18, 2,   },
 {2, 2, 4, 1, 4, 3, 5, 0, 27, 29,    },
 {1, 1, 5, 5, 8, 11, 8, 3, 0, 6,     },
 {1, 3, 4, 3, 6, 6, 18, 5, 26, 0     },
};

vector<int> primes =
{
  3,
  5,
  7,
  11,
  13,
  17,
  19,
  23,
  31,
  37,

//  10000019,
//  10000079,
//  10000103,
//  10000121,
//  10000139,
//  10000141,
//  10000169,
//  10000189,
//  10000223,
//  10000229,
};
