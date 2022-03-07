
#include <cstdlib>
#include <iostream>
#include <vector>

#define S 1000


int main() {
	
	int a, b;
	int vectorsize = S;
	std::vector<int> vector;
	vector.reserve(S*2);
	//vector = (int*) malloc(S * sizeof(int) * 2);

	for (int i = 0; i < S*2; i+=2)
	{
		vector.push_back(i);
		vector.push_back(i);
	}

	int* v = vector.data();

	clock_t begin = clock();
	
	__asm {
		mov ecx, 0		//Inicializar contador del bucle
		mov ebx, 0		//sumX
		mov edx, 0		//sumX^2
		mov edi, 0		//sumY
		mov esi, 0		//sumXY	
		mov ebp, v


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
			mov eax, 2000					//Numero de repeticiones
			cmp ecx, eax
			jl loopstart				//Seguir si contador<S

		imul esi, 10000		//S*sumXY
		mov eax, ebx
		imul eax, edi	//sumX*sumY
		sub esi, eax	//(S * sumXY - sumX * sumY)
		imul edx, 10000	//S*sumX2
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
	//free(vector);
	//vector = nullptr;
	/* Displaying value of a, b and time elapsed */
	std::cout << "Values are: a = " << a << " and b = " << b;
	std::cout << "\nEquation of best fit is: y = " << a <<" + " << b << "x";
	std::cout << "\nTime elapsed is " << time << "seconds";
	return(0);
}