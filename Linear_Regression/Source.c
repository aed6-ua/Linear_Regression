#include<stdio.h>
#include<conio.h>
#include<time.h>

#define S 10000

int main()
{
	int i;
	double timeElapsed = 0.0;
	int x[S];
	int y[S];
	int sumX = 0, sumX2 = 0, sumY = 0, sumXY = 0;
	float a, b;
	
	srand(time(0));

	clock_t begin;

	//Default vector
	for (i = 0; i < S; i++)
	{
		x[i] = rand() % S;
		y[i] = rand() % S;
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
	timeElapsed += (double)(end - begin) / CLOCKS_PER_SEC;

	/* Displaying value of a, b and time elapsed */
	printf("Values are: a=%0.2f and b = %0.2f", a, b);
	printf("\nEquation of best fit is: y = %0.2f + %0.2fx", a, b);
	printf("\nTime elapsed si %f seconds", timeElapsed);


	//Output to file
	FILE *stream;
	errno_t err = fopen_s(&stream,"bench_result_c.txt", "a");
	if (err == 0) {
		fprintf(stream, "\n%f", timeElapsed);
		fclose(stream);
	}
	else {
		printf("\nError opening file");
	}
	

	//_getch();
	return(0);
}