
#include <cstdlib>
#include <iostream>
#include <fstream>

#define S 10000
float a, b;
double timeElapsed = 0;

void ensamblador() {

	int contador;
	int vectorsize = S;
	int x[S];
	int y[S];
	int dividendo;
	int divisor;
	int sumX;
	int sumY;



	for (int i = 0; i < S; i++)
	{
		x[i] = i;
		y[i] = i;
	}
	clock_t begin = clock();
	contador = 0;
	int repeticiones = S / 4;

	__asm {
		//push	ebx			//save EBX(warning :frame pointer register)
							//we save the data in EBX to avoid bugsdue to warnings C4731
		mov edi, 0			//Contador loop función
		start :
		//mov esi, x				//puntero a vector x
		//mov edi, y				//puntero a vector y
			mov ecx, 0				//loop counters
			mov eax, vectorsize		//tamano vector
			mov esi, 0
			mov ebx, 0
			xorps xmm0, xmm0		//xmm0 a 0		
			xorps xmm1, xmm1		//xmm1 a 0
			xorps xmm3, xmm3		//xmm3 a 0
			xorps xmm5, xmm5		//xmm5 a 0

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