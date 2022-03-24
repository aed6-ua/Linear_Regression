// Bench_master.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS			//Para evitar que se queje del localtime
#endif
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define S 10000		//Número de puntos que se usan para calcular la recta de regresión
#define D false;		//Para depuración (leer más abajo).
float a, b;


/*README
Para depurar descomentar los getch() del main. Sirven para parar después de la ejecución de cada benchmark y comprobar los resultados.

Para comprobar que los resultados de los cálculos están bien cambiar el valor D a true. En este modo todas las rectas de regresión
deberían ser y = 0 + 1x.
*/





double benchC(int r) {
	bool debug = D;
	int i;
	double timeElapsed = 0.0;
	int *x = new int[S];
	int *y = new int[S];
	int sumX = 0, sumX2 = 0, sumY = 0, sumXY = 0;

	srand(time(0));

	clock_t begin;

	//Vectores de puntos
	if (debug)
		for (i = 0; i < S; i++)
		{
			x[i] = i;
			y[i] = i;
		}
	else
		for (i = 0; i < S; i++)
		{
			x[i] = rand() % S;
			y[i] = rand() % S;
		}

	//Start timer
	begin = clock();

	//Bucle de repeticiones totales del benchmark
	for (int j = 0; j < r; ++j) {
		//Calculando sumatorios
		for (i = 1;i < S;i++)
		{
			sumX = sumX + x[i];
			sumX2 = sumX2 + x[i] * x[i];
			sumY = sumY + y[i];
			sumXY = sumXY + x[i] * y[i];
		}
	}


	//Calculando valores a y b
	b = (float)(S * sumXY - sumX * sumY) / (S * sumX2 - sumX * sumX);
	a = (float)(sumY - b * sumX) / S;

	//Stop timer
	clock_t end = clock();
	timeElapsed += (double)(end - begin) / CLOCKS_PER_SEC;

	delete[] x;
	delete[] y;

	return timeElapsed;
}

double benchAssembler(int r) {
	bool debug = D;
	int contador;
	int vectorsize = S;
	int doublevectorsize = S * 2;
	int vector[S * 2];
	int dividendo;
	int divisor;
	int sumX;
	int sumY;

	srand(time(0));

	//Vector de puntos (en este caso usamos un unico vector - cada punto X.Y ocupa dos posiciones)
	if (debug)
		for (int i = 0; i < S * 2; i += 2)
		{
			vector[i] = i;
			vector[i + 1] = i;
		}
	else
		for (int i = 0; i < S * 2; i += 2)
		{
			vector[i] = rand() % S;
			vector[i + 1] = rand() % S;
		}

	//Start timer
	clock_t begin = clock();

	//Contador para repeticiones totales del benchmark
	contador = 0;
	__asm {
	start:

		mov ecx, 0		//Inicializar contador del bucle
			mov ebx, 0		//sumX
			mov edx, 0		//sumX^2
			mov edi, 0		//sumY
			mov esi, 0		//sumXY	


			loopstart:

		add ebx, [vector + ecx * 4]				//sumX
			add edi, [vector + ecx * 4 + 4]		//sumY
			mov eax, [vector + ecx * 4]
			imul eax, eax					//X^2
			add edx, eax					//Guardar sumX^2
			mov eax, [vector + ecx * 4]
			imul eax, [vector + ecx * 4 + 4]		//XY
			add esi, eax					//Guardar sumXY
			add ecx, 2						//Incrementar contador
			mov eax, doublevectorsize		//Numero de repeticiones (puntos del vector)
			cmp ecx, eax
			jl loopstart				//Seguir si contador<S

			imul esi, vectorsize		//S*sumXY
			mov eax, ebx
			imul eax, edi				//sumX*sumY
			sub esi, eax				//(S * sumXY - sumX * sumY)
			mov dividendo, esi
			imul edx, vectorsize		//S*sumX2
			mov eax, ebx
			imul eax, ebx				//sumX*sumX
			sub edx, eax				//(S * sumX2 - sumX * sumX)
			mov divisor, edx
			fild dividendo				//Cargar el dividendo en la pila del coprocesador (y convertirlo a float)
			fidiv divisor				//Dividir el float del tope de la pila por un entero de la memoria
			fst b						//Almacenar el número de la cima de la pila a la dirección de memoria b

			mov sumX, ebx
			fmul sumX		//b * sumX
			mov sumY, edi
			fsubr sumY		//(sumY - b * sumX)
			fdiv vectorsize	//(sumY - b * sumX) / S	
			fstp a			//Guardar a
			add contador, 1 //Incrementar contador de repeticiones del benchmark
			mov eax, r
			cmp contador, eax
			jl start		//Repetir el benchmark
	}
	
	//Stop timer
	clock_t end = clock();
	double time = (double)(end - begin) / CLOCKS_PER_SEC;

	return time;
}

double benchSSE(int r) {
	bool debug = D
	double timeElapsed = 0;
	int vectorsize = S;
	int x[S];
	int y[S];
	int dividendo;
	int divisor;
	int sumX;
	int sumY;

	srand(time(0));

	//Vector de puntos
	if (debug)
		for (int i = 0; i < S; i++)
		{
			x[i] = i;
			y[i] = i;
		}
	else
		for (int i = 0; i < S; i++)
		{
			x[i] = rand() % S;
			y[i] = rand() % S;
		}

	//Start timer
	clock_t begin = clock();

	//Contador repeticiones del benchmark
	int contador = 0;
	//Repeticiones para completar el vector
	int repeticiones = 0;
	//Como vamos de 4 en 4 es posible que nos queden 3, 2 o 1 elemento sobrante
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
			cmp ecx, resto			//La variable resto contiene las repeticiones a realizar (de 4 en 4) + los puntos sobrantes
			je terminar				//Si la variable resto es igual al contador no es necesario hacer nada más
			

		sobrantes :					//Si han sobrado números los añadiremos de 1 en 1 a los sumatorios
			mov eax, [x + esi]
			xorps xmm2, xmm2
			movd xmm2, eax
			paddd xmm0, xmm2		//sumX + X
			mov eax, [y + esi]
			xorps xmm4, xmm4
			movd xmm4, eax
			paddd xmm1, xmm4		//sumY + Y
			pmulld xmm4, xmm2		//XY
			paddd xmm5, xmm4		//sumXY
			pmulld xmm2, xmm2		//X^2
			paddd xmm3, xmm2		//sumX^2

			inc ecx
			mov esi, ecx
			shl esi, 2
			cmp ecx, resto
			jl sobrantes


		terminar :
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
			cmp edi, r
			jl start

	}
	//Stop timer
	clock_t end = clock();
	timeElapsed += (double)(end - begin) / CLOCKS_PER_SEC;

	return timeElapsed;
}

int main()
{
	double elapsedTime = 0;
	std::cout << std::fixed << std::setprecision(3);

	std::time_t raw_now = time(nullptr);	//Esto es para mostrar fecha y hora del benchmark
	std::tm now = *localtime(&raw_now);

	//Abrir o crear el archivo de los resultados
	std::ofstream outfile;
	outfile.open("benchmark_resultados.txt", std::ios_base::app);
	if (outfile.is_open()) {
		outfile << "\n#TIEMPOS DE LOS BENCHMARKS " << std::put_time(&now, "%x %X\n");
		outfile << "\n#**************************************************";
		outfile << "\n#REPETICIONES	C			ENSAMBLADOR	SSE";
		outfile << "\n#==================================================";
		outfile.close();
	}
	else {
		std::cout << "Error abriendo el archivo" << std::endl;
	}

	//Repetimos los benchmarks varias veces incrementando el nº de repeticiones totales de cada uno
	for (int i = 10000; i < 100001; i += 10000) {
		std::cout << "*************************************";
		std::cout << "\nNUMERO DE ITERACIONES: " << i;
		std::cout << "\n*************************************";
		std::cout << "\n\n";


		//BENCHMARK EN C
		elapsedTime = benchC(i);
		
		std::cout << "BENCHMARK EN C";
		std::cout << "\n===========================================================";
		std::cout << "\nLos valores son: a = " << a << " y b = " << b;
		std::cout << "\n-----------------------------------------------------------";
		std::cout << "\nLa recta de regresión es: y = " << a << " + " << b << "x";
		std::cout << "\n-----------------------------------------------------------";
		std::cout << "\nTime elapsed is " << elapsedTime << "seconds";
		std::cout << "\n\n";

		//Escribir en un fichero la salida
		outfile.open("benchmark_resultados.txt", std::ios_base::app);
		if (outfile.is_open()) {
			outfile << std::fixed << std::setprecision(3) << "\n" << i << "\t\t" << elapsedTime;
			outfile.close();
		}
		else {
			std::cout << "Error abriendo el archivo" << std::endl;
		}

		//getchar();

		//BENCHMARK EN ENSAMBLADOR
		elapsedTime = benchAssembler(i);

		std::cout << "BENCHMARK ENSAMBLADOR";
		std::cout << "\n===========================================================";
		std::cout << "\nLos valores son: a = " << a << " y b = " << b;
		std::cout << "\n-----------------------------------------------------------";
		std::cout << "\nLa recta de regresión es: y = " << a << " + " << b << "x";
		std::cout << "\n-----------------------------------------------------------";
		std::cout << "\nTime elapsed is " << elapsedTime << "seconds";
		std::cout << "\n\n";

		//Escribir en un fichero la salida

		outfile.open("benchmark_resultados.txt", std::ios_base::app);
		if (outfile.is_open()) {
			outfile << std::fixed << std::setprecision(3) << "\t\t" << elapsedTime;
			outfile.close();
		}
		else {
			std::cout << "Error abriendo el archivo" << std::endl;
		}

		//getchar();


		//BENCHMARK EN ENSAMBLADOR CON SSE
		elapsedTime = benchSSE(i);

		std::cout << "BENCHMARK SSE";
		std::cout << "\n===========================================================";
		std::cout << "\nLos valores son: a = " << a << " y b = " << b;
		std::cout << "\n-----------------------------------------------------------";
		std::cout << "\nLa recta de regresión es: y = " << a << " + " << b << "x";
		std::cout << "\n-----------------------------------------------------------";
		std::cout << "\nTime elapsed is " << elapsedTime << "seconds";
		std::cout << "\n\n";

		//Escribir en un fichero la salida

		outfile.open("benchmark_resultados.txt", std::ios_base::app);
		if (outfile.is_open()) {
			outfile << std::fixed << std::setprecision(3) << "\t\t" << elapsedTime;
			outfile.close();
		}
		else {
			std::cout << "Error abriendo el archivo" << std::endl;
		}

		//getchar();
	}
	return(0);
}