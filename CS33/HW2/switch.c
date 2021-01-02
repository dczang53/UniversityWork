int switch_prob(int x, int n) {
	int result = x;

	switch(n) {
	case 60:
	case 62:
	result *= 8;
	return;
	case 63:
	result >> 3;
	return;
	case 64:
	result << 4;
	result -= x;
	case 65:
	result *= result;
	default:
	result += 75;
	}

	return result;
}