#include <omp.h>
#include <queue>
#include <iostream>
using namespace std;

void quicksort(int *a, int p, int r)
{
	if(p >= r)
		return;
	int mid = a[r];
	queue<int> lessThan;
	queue<int> greaterThanEquals;
	//#pragma omp parallel
	//{
		//#pragma omp single
		//{
			for(int i = p; i < r; i++)
			{
				//#pragma omp task //firstprivate(i)
				//{
					if(a[i] < mid)
						lessThan.push(a[i]);
					else
						greaterThanEquals.push(a[i]);
				//}
			}
		//}
	//}
	int curr = p;
	while(!lessThan.empty())
	{
		a[curr] = lessThan.front();
		curr++;
		lessThan.pop();
	}
	a[curr] = mid;
	mid = curr;
	curr++;
	while(!greaterThanEquals.empty())
	{
		a[curr] = greaterThanEquals.front();
		curr++;
		greaterThanEquals.pop();
	}
	#pragma omp parallel
	{
		#pragma omp single
		{
			#pragma omp task
			quicksort(a, p, mid - 1);
			#pragma omp task
			quicksort(a, mid + 1, r);
		}
	}
	return;
}

int main()
{
	int test[100] = {4, 2, 48, 70, 3, 65, 22, 94, 32, 10, 1, 4, 63, 76, 85, 14, 52, 59, 82, 90, 14, 42, 47, 60, 80, 9, 14, 60, 71, 94, 5, 26, 65, 88, 99, 41, 57, 69, 74, 80, 7, 72, 75, 88, 96, 15, 25, 58, 65, 94, 6, 18, 43, 69, 87, 56, 68, 70, 87, 100, 3, 12, 33, 43, 77, 36, 37, 52, 59, 86, 26, 30, 47, 51, 61, 30, 32, 77, 88, 95, 54, 83, 86, 94, 96, 1, 10, 21, 37, 98, 15, 35, 69, 80, 92, 6, 24, 67, 91, 100};
	quicksort(test, 0, 99);
	for(int i = 0; i < 100; i++)
		cout << test[i] << endl;
}
