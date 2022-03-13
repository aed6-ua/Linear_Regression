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

		mov esi x			//puntero a vector x
		mov edi y			//puntero a vector y
		mov ecx 0			//loop counters
		xor xmm0, xmm0		//xmm0 a 0		
		xor xmm1, xmm1		//xmm1 a 0
		xor xmm3, xmm3		//xmm3 a 0
		xor xmm5, xmm5		//xmm5 a 0

		loop_start:
			addps xmm0, [esi+ecx*16]		//Sumar 4 floats de 32 bits (vector x) a xmm0 (sumx)
			addps xmm1, [edi+ecx*16]		//Sumar 4 floats de 32 bits (vector y) a xmm1 (sumY)
			movaps xmm2, [esi+ecx*16]		//Mover 4 floats (vector x) a xmm2
			mulps xmm2, xmm2				//multiplicar 4 floats en xmm2 (X^2)
			addps xmm3, xmm2				//Sumar en xmm3 (sumX^2)
			movaps xmm4, [esi+ecx*16]		//Poner las x en xmm4
			mulps xmm4, [edi+ecx*16]		//Multiplicar por las y (XY)
			addps xmm5, xmm4				//sumXY
    }
}

