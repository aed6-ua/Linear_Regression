// Linear_Regression_SSE.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdlib>
#include <iostream>
#include <fstream>

#define S 10000


int main()
{
	int a, b, contador;
	int vectorsize = S;
	float x[S];
	float y[S];

	for (int i = 0; i < S * 2; i += 2)
	{
		x[i] = (float)i;
		y[i] = (float)i;
	}

	clock_t begin = clock();
	contador = 0;
    __asm {
        push	ebx             ; save EBX(warning :frame pointer register)
                                ; we save the data in EBX to avoid bugsdue to warnings C4731
	start:
		mov esi, x			//puntero a vector x
		mov edi, y			//puntero a vector y
		mov ecx, 0			//loop counters
		mov eax, vectorsize	//cantidad de repeticiones
		mov ebx, 0			//contador outer_loop
		xor xmm0, xmm0		//xmm0 a 0		
		xor xmm1, xmm1		//xmm1 a 0
		xor xmm3, xmm3		//xmm3 a 0
		xor xmm5, xmm5		//xmm5 a 0

		loop_start:
			addps xmm0, [esi + ecx * 16]		//Sumar 4 floats de 32 bits (vector x) a xmm0 (sumx)
			addps xmm1, [edi + ecx * 16]		//Sumar 4 floats de 32 bits (vector y) a xmm1 (sumY)
			movaps xmm2, [esi + ecx * 16]		//Mover 4 floats (vector x) a xmm2
			mulps xmm2, xmm2					//multiplicar 4 floats en xmm2 (X^2)
			addps xmm3, xmm2					//Sumar en xmm3 (sumX^2)
			movaps xmm4, [esi + ecx * 16]		//Poner las x en xmm4
			mulps xmm4, [edi + ecx * 16]		//Multiplicar por las y (XY)
			addps xmm5, xmm4					//sumXY
			cmp ecx, eax						//comparar contador con numero de repeticiones
			inc ecx								//incrementar contador
			jle loop_start 

		mov ecx, 0
		add_loop:
			movaps xmm2, xmm0
			shufps xmm0, xmm0, 93h
			addss xmm2, xmm0				//sumX
			movaps xmm4, xmm1
			shufps, xmm1, xmm1, 93h
			addss xmm4, xmm1				//sumY
			movaps xmm6, xmm3
			shufps xmm3, xmm3, 93h
			addss xmm6, xmm3				//sumX^2
			movaps xmm7, xmm5
			shufps xmm5, xmm5, 93h
			addss xmm7, xmm5				//sumXY
			cmp ecx, 4
			inc ecx
			jl add_loop

		mulss xmm7, eax					//S*sumXY
		mulss xmm8, xmm4, xmm2			//sumX*sumY
		subss xmm7, xmm8				//(S * sumXY - sumX * sumY)
		mulss xmm3, eax					//S*sumX^2
		mulss xmm0, xmm2, xmm2			//sumX*sumX
		subss xmm3, xmm0				//(S * sumX2 - sumX * sumX)
		divss xmm7, xmm3				//(S * sumXY - sumX * sumY) / (S * sumX2 - sumX * sumX)
		movss b, xmm7

		mulss xmm2, xmm7				//b*sumX
		subss xmm4, xmm2				//(sumY - b * sumX)
		divss xmm4, eax					//(sumY - b * sumX) / S	
		movss a, xmm4

		cmp ebx, 100000
		inc ebx
		jl start

    }
}

