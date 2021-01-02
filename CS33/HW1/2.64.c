int any_odd_one(unsigned x)
{
	/*create a mask to omit all even position digits (0x55555555 is 1010101...0101 [32 bits]), then check if there are no 1's*/
	return ((x & 0x55555555) != 0x00000000);
}