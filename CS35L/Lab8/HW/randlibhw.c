#include "randlib.h"
#include <immintrin.h>

/* Hardware implementation.  */

/* Return a random value, using hardware operations.  */
extern unsigned long long
hardware_rand64(void)
{
	unsigned long long int x;
	while (!_rdrand64_step(&x))
		continue;
	return x;
}

//Changed "static" to "extern" in line 7 as the function is to used by main
//Removed init and fini functions as they were not necessary