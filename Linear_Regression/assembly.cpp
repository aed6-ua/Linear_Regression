
#include <cstdlib>
#include <iostream>

#define S 10


int main() {
	
	int a, b;
	int vectorsize = S;
	int* vector;
	vector = (int*) malloc(S * sizeof(int) * 2);

	if (vector)
	{
		for (int i = 0; i < S*2; i+=2)
		{
			vector[i] = i;
			vector[i+1] = i;
		}
	}
	else {
		std::cout << "\nAlloc failed" << std::endl;
		return -1;
	}

	clock_t begin = clock();
	
	__asm {
		mov ecx, 0		//Inicializar contador del bucle
		mov ebx, 0		//sumX
		mov edx, 0		//sumX^2
		mov edi, 0		//sumY
		mov esi, 0		//sumXY	
		mov ebp, vector


	loopstart:
			
			add ebx, [ebp + ecx*4]				//sumX
			add edi, [ebp + ecx*4 + 4]		//sumY
			mov eax, [ebp + ecx*4]
			imul eax, eax		//X^2
			add edx, eax					//Guardar sumX^2
			mov eax, [ebp + ecx*4]
			imul eax, [ebp + ecx*4 + 4]		//XY
			add esi, eax					//Guardar sumXY
			add ecx, 2						//Incrementar contador
			mov eax, 20					//Numero de repeticiones
			cmp ecx, eax
			jl loopstart				//Seguir si contador<S

		imul esi, 10		//S*sumXY
		mov eax, ebx
		imul eax, edi	//sumX*sumY
		sub esi, eax	//(S * sumXY - sumX * sumY)
		imul edx, 10	//S*sumX2
		mov eax, ebx
		imul eax, ebx	//sumX*sumX
		sub edx, eax	//(S * sumX2 - sumX * sumX)
		mov eax, esi
		mov esi, edx
		xor edx, edx
		div esi			//(S * sumXY - sumX * sumY) / (S * sumX2 - sumX * sumX)
		mov b, eax		//b

		imul eax, ebx		//b * sumX
		sub edi, eax		//(sumY - b * sumX)
		mov eax, edi
		mov ebx, 10		
		xor edx, edx
		div ebx				//(sumY - b * sumX) / S	
		mov a, eax		//a
	}
	//Stop timer
	clock_t end = clock();
	double time = (double)(end - begin) / CLOCKS_PER_SEC;
	free(vector);
	vector = nullptr;
	/* Displaying value of a, b and time elapsed */
	std::cout << "Values are: a = " << a << " and b = " << b;
	std::cout << "\nEquation of best fit is: y = " << a <<" + " << b << "x";
	std::cout << "\nTime elapsed is " << time << "seconds";
	return(0);
}