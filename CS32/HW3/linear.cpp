bool allTrue(const double a[], int n)
{
	if (n <= 0)
		return true;
	else if (somePredicate(a[0]))
		return allTrue(a + 1, n - 1);
	else
		return false;
}

int countFalse(const double a[], int n)
{
	if (n <= 0)
		return 0;
	else if (!somePredicate(a[0]))
		return 1 + countFalse(a + 1, n - 1);
	else
		return countFalse(a + 1, n - 1);
}

int firstFalse(const double a[], int n)
{
	if (n <= 0)
		return -1;
	if (somePredicate(a[0]))
	{
		int x = firstFalse(a + 1, n - 1);
		if (x == -1)
			return -1;
		else
			return 1 + x;
	}	
	else
		return 0;
}

int indexOfMin(const double a[], int n)
{
	if (n <= 0)
		return -1;
	else if (n >= 2)
	{
		int x = indexOfMin(a + 1, n - 1);
		if (a[0] <= a[x + 1])
			return 0;
		else
			return 1 + x;
	}
	else if (n == 2)
	{
		if (a[0] <= a[1])
			return 0;
		else
			return 1;
	}
	else
		return 0;
}

bool includes(const double a1[], int n1, const double a2[], int n2)
{
	if (n2 <= 0)
		return true;
	else if (n1 <= 0)
		return false;
	else if (a1[0] == a2[0])
		return includes(a1 + 1, n1 - 1, a2 + 1, n2 - 1);
	else
		return includes(a1 + 1, n1 - 1, a2, n2);
}