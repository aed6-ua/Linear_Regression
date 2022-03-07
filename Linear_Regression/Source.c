#include<stdio.h>
#include<conio.h>
#include<time.h>

#define S 10000

int main()
{
	int n, i;
	double time = 0.0;
	int x[S];
	int y[S];
	int sumX = 0, sumX2 = 0, sumY = 0, sumXY = 0;
	float a, b;
	/* Input */
	/*printf("How many data points?\n");
	scanf_s("%d", &n);
	printf("Enter data:\n");
	for (i = 1;i <= n;i++)
	{
		printf("x[%d]=", i);
		scanf_s("%f", &x[i]);
		printf("y[%d]=", i);
		scanf_s("%f", &y[i]);
	}*/
	clock_t begin;

	//Default vector
	for (i = 0; i < S; i++)
	{
		x[i] = i;
		y[i] = i;
	}

	//Start timer
	begin = clock();
	for (int j = 0; j < 100000; ++j) {
		/* Calculating Required Sum */
		for (i = 1;i < S;i++)
		{
			sumX = sumX + x[i];
			sumX2 = sumX2 + x[i] * x[i];
			sumY = sumY + y[i];
			sumXY = sumXY + x[i] * y[i];
		}
	}
		
	

	/* Calculating a and b */
	b = (float)(S * sumXY - sumX * sumY) / (S * sumX2 - sumX * sumX);
	a = (float)(sumY - b * sumX) / S;
	
	//Stop timer
	clock_t end = clock();
	time += (double)(end - begin) / CLOCKS_PER_SEC;

	/* Displaying value of a, b and time elapsed */
	printf("Values are: a=%0.2f and b = %0.2f", a, b);
	printf("\nEquation of best fit is: y = %0.2f + %0.2fx", a, b);
	printf("\nTime elapsed si %f seconds", time);
	_getch();
	return(0);
}