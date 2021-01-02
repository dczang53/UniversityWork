static float u2f(unsigned u)
{
	union {
		unsigned int i;
		float f;
	} stuff;
	stuff.i = u;
	return stuff.f;
}

float fpwr4(int x)
{ 
	/* Result exponent and fraction */
	unsigned exp, frac;
	unsigned u;

	if (x < -63) {
		/* Too small. Return 0.0 */
		exp = 0;
		frac = 0;  
	}
	else if (x < 64) {
		/* Normalized result. */
		exp = (2 * x) + 127;
		frac = 0;
	}
	else {
		/* Too big. Return +oo */
		exp = 255;
		frac = 0;
	}
	/* Pack exp and frac into 32 bits */
	u = exp << 23 | frac;
	/* Return as float */ 
	return u2f(u);
}