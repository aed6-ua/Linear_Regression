
#include <cstdlib>
#include <iostream>
#include <fstream>

#define S 10000
float a, b;
double timeElapsed = 0;

void ensamblador() {

	int vectorsize = S;
	int x[S];
	int y[S];
	int dividendo;
	int divisor;
	int sumX;
	int sumY;

	srand(time(0));

	for (int i = 0; i < S; i++)
	{
		x[i] = rand() % S;
		y[i] = rand() % S;
	}
	clock_t begin = clock();
	int contador = 0;
	int repeticiones = 0;
	int resto = 0;

	__asm {
		xor edi, edi			//Contador loop función

		start :
			
			mov eax, vectorsize		//tamano vector
			xor edx, edx
			xor esi, esi
			xor ebx, ebx
			xorps xmm0, xmm0		//xmm0 a 0		
			xorps xmm1, xmm1		//xmm1 a 0
			xorps xmm3, xmm3		//xmm3 a 0
			xorps xmm5, xmm5		//xmm5 a 0

			mov ecx, 4
			div ecx					//Calcular número de repeticiones necesarias
			mov repeticiones, eax
			add edx, eax
			mov resto, edx			//Cantidad de números sobrantes (<4) + repeticiones (numeros totales)

			mov eax, vectorsize
			xor ecx, ecx				//loop counters

		loop_start :
			movdqu xmm6, [x + esi]		//Cargar 128 bits desde la dirección de memoria del vector X
			paddd xmm0, xmm6			//Sumar 4 enteros de 32 bits (vector x) a xmm0 (sumx)

			movdqu xmm7, [y + esi]		//Cargar 128 bits desde la dirección de memoria del vector X
			paddd xmm1, xmm7			//Sumar 4 enteros de 32 bits (vector y) a xmm1 (sumY)

			movdqa xmm2, xmm6			//Copiamos los 4 enteros X a xmm2
			pmulld xmm2, xmm6			//elevar al cuadrado 4 enteros en xmm2 (X^2)
			paddd xmm3, xmm2			//Acumular los resultados en xmm3 (sumX^2)

			movdqa xmm4, xmm6			//Copiamos los 4 enteros X a xmm4
			pmulld xmm4, xmm7			//Multiplicar por las Y (XY)
			paddd xmm5, xmm4			//sumXY en xmm5

			inc ecx					//Incrementar contador
			mov esi, ecx			//Copiar el valor del contador
			shl esi, 4				//Multìplicar el valor del contador por 16
			cmp ecx, repeticiones	//comparar contador con numero de repeticiones
			jl loop_start
			cmp ecx, resto
			je terminar
			
		sobrantes:
			mov eax, [x + esi]
			xorps xmm2, xmm2
			movd xmm2, eax
			paddd xmm0, xmm2
			mov eax, [y + esi]
			xorps xmm4, xmm4
			movd xmm4, eax
			paddd xmm1, xmm4
			pmulld xmm4, xmm2
			paddd xmm5, xmm4
			pmulld xmm2, xmm2
			paddd xmm3, xmm2

			inc ecx
			mov esi, ecx
			shl esi, 2
			cmp ecx, resto
			jl sobrantes

			
		terminar:
			mov ecx, 0				//Poner ecx a 0

			phaddd xmm0, xmm0
			phaddd xmm0, xmm0		//sumX final

			phaddd xmm1, xmm1
			phaddd xmm1, xmm1		//sumY final
			movd ebx, xmm1

			phaddd xmm3, xmm3
			phaddd xmm3, xmm3		//sumX^2 final

			phaddd xmm5, xmm5
			phaddd xmm5, xmm5		//sumXY final
			movd eax, xmm5

			imul eax, vectorsize			//S*sumXY
			movd ecx, xmm0
			imul ebx, ecx				//sumY*sumX
			sub eax, ebx				//(S * sumXY - sumX * sumY)
			mov dividendo, eax
			movd ebx, xmm3
			imul ebx, vectorsize		//S*sumX^2
			imul ecx, ecx				//sumX*sumX

			sub ebx, ecx				//(S * sumX2 - sumX * sumX)
			mov divisor, ebx
			
			//b=(S * sumXY - sumX * sumY) / (S * sumX2 - sumX * sumX)
			fild dividendo  //Cargar el dividendo en la pila del coprocesador (y convertirlo a float)
			fidiv divisor		//Dividir el float del tope de la pila por un entero de la memoria
			fst b			//Almacenar el número de la cima de la pila a la dirección de memoria b

			movd sumX, xmm0
			fmul sumX			//b*sumX
			movd sumY, xmm1
			fsubr sumY		//(sumY - b * sumX)
			fdiv vectorsize	//(sumY - b * sumX) / S	
			fstp a			//Guardar a

			inc edi
			cmp edi, 100000
			jl start

	}
	//Stop timer
	clock_t end = clock();
	timeElapsed += (double)(end - begin) / CLOCKS_PER_SEC;
}



int main()
{



	ensamblador();





	/* Displaying value of a, b and time elapsed */
	std::cout << "Values are: a = " << a << " and b = " << b;
	std::cout << "\nEquation of best fit is: y = " << a << " + " << b << "x";
	std::cout << "\nTime elapsed is " << timeElapsed << "seconds";

	//Escribir en un fichero la salida
	std::ofstream outfile;

	outfile.open("bench_result_sse_integer.txt", std::ios_base::app);
	if (outfile.is_open()) {
		outfile << "\n" << timeElapsed;
	}
	else {
		std::cout << "Fail opening the file" << std::endl;
	}

	//getchar();
	return(0);
}