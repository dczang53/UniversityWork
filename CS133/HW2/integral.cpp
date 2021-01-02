#include <cmath>
#include <vector>
#include <iostream>
#include <omp.h>
using namespace std;

#define A 0.0
#define B 1.0
#define STEPS 10000

///*
void integral()
{
	float area = 0;
	float stepSize = (B - A) / STEPS;
	#pragma omp parallel for schedule(guided) num_threads(16) reduction(+:area)
	for(int step = 0; step < STEPS; step++)
	{
		area += stepSize * sqrt(A + (step * stepSize))/(1 + pow((A + (step * stepSize)), 3));
	}
	cout << area << endl;
}
//*/
//user + sys: 11ms

/*
void integral()
{
	float area = 0;
	vector<float> stepAreas(STEPS, 0);
	float stepSize = (B - A) / STEPS;
	#pragma omp parallel for schedule(guided) num_threads(16)
	for(int step = 0; step < STEPS; step++)
	{
		stepAreas[step] = stepSize * sqrt(A + (step * stepSize))/(1 + pow((A + (step * stepSize)), 3));
	}
	#pragma omp parallel for schedule(guided) num_threads(16) reduction(+:area)
	for(int step = 0; step < STEPS; step++)
	{
		area += stepAreas[step];
	}
	cout << area << endl;
}
*/
//user + sys: 13ms

int main()
{
	integral();
}

//g++ -o integral -fopenmp integral.cpp
