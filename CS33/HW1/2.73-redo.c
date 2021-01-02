#include <stdio.h>

#include <limits.h>

int saturating_add(int x, int y)
{
	int sum;
	/*find the sum and store in "sum", and create a "mask" to omit binary digits (under the condition that the sum overflowed)*/
	int mask = ~((__builtin_sadd_overflow (x, y, &sum))-1);
	/*check if the sum is positive of negative*/
	int neg = (sum & INT_MIN) >> ((sizeof(int)<<3)-1);
	/*return result*/
	return (sum & ~mask) + (INT_MIN & (mask & ~neg)) + (INT_MAX & (mask & neg));
}