#include "matrix.h"
#include <iostream>
#include <array>
#include <chrono>

auto start() { return  std::chrono::high_resolution_clock::now();}
auto duration(std::chrono::steady_clock::time_point start) { 
	auto end = std::chrono::steady_clock::now(); 
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
};

int main(int argc, char const *argv[]) {
	using namespace myMatrix;

	Matrix<float , 10, 10 > m1;
	Matrix<int, 5, 10> m2;
	Matrix<double, 10, 10> m3;
	Matrix<float, 8, 5> m4;
	Matrix<int, 10, 10> m5;

	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 10; ++j) {
			m1[i][j] = (i+1) * (j+1) * .5f;
			m3[i][j] = (i+1) * (j+1) * .25;
			m5[i][j] = (i+1) * (j+1);
		}

	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 10; ++j)
			m2[i][j] = (i + 1) * (j + 1);

	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 5; ++j)
			m4[i][j] = (i + 1) * (j + 1) * .5f;

	std::cout << "Testing operations on following matricies: " << "m1 is float(10x10) , m2 is int(5x10) ,m3 is double(10x10)\n";
	std::cout << "m4 is float (8x5) and m5 is int(10x10) . x is standard matrix multiplication and * is scalar multiplication \n\n";

	auto st = start();
	auto m6 = m2 ^ m1;
	std::cout << "Expression (m2 x m1) took " << duration(st).count() << " Us (micro seconds)"<<std::endl;

	st = start();
	auto m7 = m4 ^ m2 ^ m1 ^ m3;
	std::cout << "Expression (m4 x m2 x m1 x m3 ) took " << duration(st).count() << " Us (micro seconds)" << std::endl;

	st = start();
	auto m8 = m1 + m3 * m1;
	std::cout << "Expression (m1 + m3 * m1) took " << duration(st).count() << " Us (micro seconds)" << std::endl;

	st = start();
	auto m9 = m1 - m1 - m1 - m1;
	std::cout << "Expression (m1 - m1 - m1 - m1) took " << duration(st).count() << " Us (micro seconds)" << std::endl;

	st = start();
	auto m10 = m1 - m3 / m1 * m1;
	std::cout << "Expression (m1 - m3 / m1 * m1) took " << duration(st).count() << " Us (micro seconds)" << std::endl;

	std::cout << "\n\nExample by numbers.\n";
	std::cout << "m1 = \n" << m1 << "\n\n" << "m5 = \n" << m5 << "\n\n";
	std::cout << "m1 + m5 * m1 - m1 = \n";
	std::cout << m1 + m5 * m1 - m1 ;
	std::cin.get();
	return 0;
}