
#include <cstdlib>
#include <iostream>
#include <fstream>

#define S 10000


int main() {
	
	float a, b;
	int contador;
	int vectorsize = S;
	int doublevectorsize = S * 2;
	int vector[S*2];
	int dividendo;
	int divisor;
	int sumX;
	int sumY;

	srand(time(0));

	for (int i = 0; i < S * 2; i += 2)
	{
		vector[i] = rand() % S;
		vector[i + 1] = rand() % S;
	}

	clock_t begin = clock();
	contador = 0;
	__asm {
	start:
		
		mov ecx, 0		//Inicializar contador del bucle
		mov ebx, 0		//sumX
		mov edx, 0		//sumX^2
		mov edi, 0		//sumY
		mov esi, 0		//sumXY	


	loopstart:
			
			add ebx, [vector + ecx*4]				//sumX
			add edi, [vector + ecx*4 + 4]		//sumY
			mov eax, [vector + ecx*4]
			imul eax, eax					//X^2
			add edx, eax					//Guardar sumX^2
			mov eax, [vector + ecx*4]
			imul eax, [vector + ecx*4 + 4]		//XY
			add esi, eax					//Guardar sumXY
			add ecx, 2						//Incrementar contador
			mov eax, doublevectorsize					//Numero de repeticiones
			cmp ecx, eax
			jl loopstart				//Seguir si contador<S

		imul esi, vectorsize		//S*sumXY
		mov eax, ebx
		imul eax, edi	//sumX*sumY
		sub esi, eax	//(S * sumXY - sumX * sumY)
		mov dividendo, esi
		imul edx, vectorsize	//S*sumX2
		mov eax, ebx
		imul eax, ebx	//sumX*sumX
		sub edx, eax	//(S * sumX2 - sumX * sumX)
		mov divisor, edx
		fild dividendo  //Cargar el dividendo en la pila del coprocesador (y convertirlo a float)
		fidiv divisor		//Dividir el float del tope de la pila por un entero de la memoria
		fst b			//Almacenar el número de la cima de la pila a la dirección de memoria b

		mov sumX, ebx
		fmul sumX		//b * sumX
		mov sumY, edi
		fsubr sumY		//(sumY - b * sumX)
		fdiv vectorsize	//(sumY - b * sumX) / S	
		fstp a			//Guardar a
		add contador, 1
		cmp contador, 100000
		jl start
	}
	//Stop timer
	clock_t end = clock();
	double time = (double)(end - begin) / CLOCKS_PER_SEC;

	/* Displaying value of a, b and time elapsed */
	std::cout << "Values are: a = " << a << " and b = " << b;
	std::cout << "\nEquation of best fit is: y = " << a <<" + " << b << "x";
	std::cout << "\nTime elapsed is " << time << "seconds";

	//Escribir en un fichero la salida
	std::ofstream outfile;

	outfile.open("bench_result_assembler.txt", std::ios_base::app);
	if (outfile.is_open()) {
		outfile << "\n" << time;
	}
	else {
		std::cout << "Fail opening the file" << std::endl;
	}

	//getchar();
	return(0);
}