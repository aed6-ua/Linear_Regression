// Linear_Regression_SSE.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdlib>
#include <iostream>
#include <fstream>

#define S 100
float a, b;


void ensamblador() {

	int contador;
	int vectorsize = S;
	float x[S];
	float y[S];

	for (int i = 0; i < S; i++)
	{
		x[i] = (float)i;
		y[i] = (float)i;
	}
	clock_t begin = clock();
	contador = 0;

	__asm {
		push	ebx			//save EBX(warning :frame pointer register)
							//we save the data in EBX to avoid bugsdue to warnings C4731
		start :
		//mov esi, x				//puntero a vector x
		//mov edi, y				//puntero a vector y
			mov ecx, 0				//loop counters
			mov eax, vectorsize		//tamano vector
			mov ebx, 0				//contador outer_loop
			mov esi, 0
			xorps xmm0, xmm0		//xmm0 a 0		
			xorps xmm1, xmm1		//xmm1 a 0
			xorps xmm3, xmm3		//xmm3 a 0
			xorps xmm5, xmm5		//xmm5 a 0

		loop_start :
			addps xmm0, [x + esi]			//Sumar 4 floats de 32 bits (vector x) a xmm0 (sumx)
			addps xmm1, [y + esi]			//Sumar 4 floats de 32 bits (vector y) a xmm1 (sumY)
			movups xmm2, [x + esi]			//Mover 4 floats (vector x) a xmm2
			mulps xmm2, xmm2					//multiplicar 4 floats en xmm2 (X^2)
			addps xmm3, xmm2					//Sumar en xmm3 (sumX^2)
			movups xmm4, [x + esi]			//Poner las x en xmm4
			mulps xmm4, [y + esi]			//Multiplicar por las y (XY)
			addps xmm5, xmm4					//sumXY
			inc ecx
			mov esi, ecx
			shl esi, 4
			cmp ecx, 100						//comparar contador con numero de repeticiones
			jl loop_start

			mov ecx, 0
		add_loop:
			movups xmm2, xmm0
			shufps xmm0, xmm0, 93h
			addss xmm2, xmm0				//sumX
			movups xmm4, xmm1
			shufps xmm1, xmm1, 93h
			addss xmm4, xmm1				//sumY
			movups xmm6, xmm3
			shufps xmm3, xmm3, 93h
			addss xmm6, xmm3				//sumX^2
			movups xmm7, xmm5
			shufps xmm5, xmm5, 93h
			addss xmm7, xmm5				//sumXY
			inc ecx
			cmp ecx, 4
			jl add_loop

			mulss xmm7, eax					//S*sumXY
			movss xmm1, xmm4
			mulss xmm4, xmm2				//sumX*sumY
			subss xmm7, xmm1				//(S * sumXY - sumX * sumY)
			mulss xmm3, eax					//S*sumX^2
			movss xmm0, xmm2
			mulss xmm0, xmm2				//sumX*sumX
			subss xmm3, xmm0				//(S * sumX2 - sumX * sumX)
			divss xmm7, xmm3				//(S * sumXY - sumX * sumY) / (S * sumX2 - sumX * sumX)
			movss b, xmm7

			mulss xmm2, xmm7				//b*sumX
			subss xmm4, xmm2				//(sumY - b * sumX)
			divss xmm4, eax					//(sumY - b * sumX) / S	
			movss a, xmm4

			inc ebx
			cmp ebx, 100000
			jl start

	}
	//Stop timer
	clock_t end = clock();
	double time = (double)(end - begin) / CLOCKS_PER_SEC;
}



int main()
{
	


	ensamblador();

	
   
	

	/* Displaying value of a, b and time elapsed */
	std::cout << "Values are: a = " << a << " and b = " << b;
	std::cout << "\nEquation of best fit is: y = " << a << " + " << b << "x";
	std::cout << "\nTime elapsed is " << time << "seconds";

	//Escribir en un fichero la salida
	std::ofstream outfile;

	outfile.open("bench_result_sse.txt", std::ios_base::app);
	if (outfile.is_open()) {
		outfile << "\n" << time;
	}
	else {
		std::cout << "Fail opening the file" << std::endl;
	}

	getchar();
	return(0);
}



