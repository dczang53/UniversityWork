#include <limits.h>

int saturating_add(int x, int y)
{
	/*find the sum (regularly with possiblity for overflow)*/
	int sum = x + y;
	/*find the number of bits to shift (arithmetically)*/
	int toshift = (sizeof(int)<<3) - 1;
	/*create a "mask" to omit binary digits (under the condition that the sum overflowed)*/
	int mask = ((~((x & INT_MIN)^(y & INT_MIN))) & ((sum & INT_MIN)^(x & INT_MIN))) >> toshift;
	/*check if the sum is positive of negative*/
	int neg = (sum & INT_MIN) >> toshift;
	/*return result*/
	return (sum & ~mask) + (INT_MIN & (mask & ~neg)) + (INT_MAX & (mask & neg));
}