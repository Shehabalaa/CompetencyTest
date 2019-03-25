#ifndef MATRIX_H
#define MATRIX_H

#include <array>
#include <cassert>
#include <complex>
#include <ostream>
#include <type_traits>

namespace myMatrix {
	using dims_type = std::pair<std::size_t, std::size_t>; // alias for cleaner code for type that carray matrix size

	// Matrix Expression to support Recursive Type Composition 
	template <typename B>
	struct MatExpression {
		auto operator()(const std::size_t& i) const { return static_cast<B const &>(*this)(i); }
		dims_type getDims() const { return static_cast<B const &>(*this).getDims(); }
	};

	// Matrix of any numerical type wirth number of rows = R and number of colums = C
	template <typename T, std::size_t R, std::size_t C>
	class Matrix : public MatExpression<Matrix<T, R, C>> {
		std::array<T, R*C> matrix; // <array> class  will be basic class container
	public:
		using array_value_type = T;
		//Constructors
		Matrix() :matrix{ 0 } {};
		Matrix(std::array<T, R*C> ia) :matrix(ia) {}
		Matrix(std::initializer_list<T> il) : matrix{ 0 } { // construct form intializer list
			assert(il.size() == R * C); std::size_t j = 0;
			for (auto i : il) { matrix[j++] = i; }
		}

		template <typename B>
		Matrix(const MatExpression<B>& m) { //Matrix can be constructed from MatExpression 
			for (std::size_t i = 0; i < R*C; ++i)
				matrix[i] = m(i);
		}

		// Getters
		constexpr auto getSize() const { return matrix.size(); }
		constexpr auto getRows() const { return R; }
		constexpr auto getCols() const { return C; }
		constexpr auto getDims() const { return dims_type{ R,C }; }

		// Operator Overloading

		//Using () for accecing matrix elments this will help in exprission templates using lambda functions
		const T & operator() (const std::size_t& i) const { return matrix[i]; }

		template <typename B>
		const MatExpression<B>& operator=(const MatExpression<B>& m) { //Matrix can be assigned from MatExpression like copy constructor
			for (std::size_t i = 0; i < R*C; ++i)
				matrix[i] = m(i);
			return m;
		}

		//Support [i][j] syntax: Pick the row returning array then use array default slice operator to pick from colum 
		//this will will be easeir than matrix_instant(i*C+j) instead matrix_instant[i][j] for user of class 'where C is number of colmns'
		std::array<T, C> & operator[](std::size_t r) { return *reinterpret_cast<std::array<T, C>*>(&matrix[r*C]); }
		
	};

	// helper functions for matrix restricions on arithmatic operations
	bool scalarOpCheck(const dims_type& dims1, const dims_type& dims2) { return dims1.first == dims2.first && dims1.second == dims2.second; }
	bool multiplyOpCheck(const dims_type& dims1, const dims_type& dims2) { return dims1.second == dims2.first; }

	// General class for any binary arithmetic operation that stores lamba functions
	template <typename MorAO1, typename MorAO2, typename F> //  'MorAO' means ArithmiticOperations or Matrix and F type of lambda function
	class ArithOperation : public MatExpression< ArithOperation<MorAO1, MorAO2, F> > {
		const MorAO1& op1;
		const MorAO2& op2;
		F opr;
		dims_type newDims; // this will help in getting dimentions of resulting matrix from this operations
	public:
		ArithOperation(const MorAO1 & m1, const MorAO2 & m2, F oper, dims_type dims) : op1(m1), op2(m2), opr(oper), newDims(dims) { };
		auto operator()(const std::size_t& i) const { return opr(i); }
		dims_type getDims() const { return newDims; }
	};

	// Overloading general arithmetic operators between matricies
	template <typename MorAO1, typename MorAO2> 
	auto operator+(const MorAO1 & op1,const MorAO2 & op2) {
		assert(scalarOpCheck(op1.getDims(), op2.getDims())); // ensure they are  equal in Dimentions
		auto lambda = [&op1, &op2](std::size_t i) { return op1(i) + op2(i); };
		return ArithOperation <MorAO1, MorAO2, decltype(lambda)>(op1, op2, lambda, op1.getDims());
	}

	template <typename MorAO1, typename MorAO2> 
	auto operator-(const MorAO1 & op1, const MorAO2 & op2) {
		assert(scalarOpCheck(op1.getDims(), op2.getDims()));
		auto lambda = [&op1, &op2](std::size_t i) { return op1(i) - op2(i); };
		return ArithOperation <MorAO1, MorAO2, decltype(lambda)>(op1, op2, lambda, op1.getDims());
	}

	template <typename MorAO1, typename MorAO2>
	auto operator*(const MorAO1 & op1, const MorAO2 & op2) {
		assert(scalarOpCheck(op1.getDims(), op2.getDims()));
		auto lambda = [&op1, &op2](std::size_t i) { return op1(i) * op2(i); };
		return ArithOperation <MorAO1, MorAO2, decltype(lambda)>(op1, op2, lambda, op1.getDims());
	}

	template <typename MorAO1, typename MorAO2>
	auto operator/(const MorAO1 & op1, const MorAO2 & op2) {
		assert(scalarOpCheck(op1.getDims(), op2.getDims()));
		auto lambda = [&op1, &op2](std::size_t i) { return op1(i) / op2(i); };
		return ArithOperation <MorAO1, MorAO2, decltype(lambda)>(op1, op2, lambda, op1.getDims());
	}

	template <typename MorAO1, typename MorAO2>
	auto operator+=(MorAO1 & op1, const MorAO2 & op2) {op1 = op1 + op2;return op1;}

	template <typename MorAO1, typename MorAO2>
	auto operator-=(const MorAO1 & op1, const MorAO2 & op2) {op1 = op1 - op2;return op1;}

	template <typename MorAO1, typename MorAO2>
	auto operator*=(const MorAO1 & op1, const MorAO2 & op2) { op1 = op1 * op2; return op1;}

	template <typename MorAO1, typename MorAO2>
	auto operator/=(MorAO1 & op1, const MorAO2 & op2) {op1 = op1 / op2;return op1;}

	// User ^ operoator for  standard matrix multiplicatio
	template <typename T1, typename T2, std::size_t I, std::size_t J,std::size_t K>
	auto operator^(const Matrix<T1,I,J> & op1, const Matrix<T2, J, K> & op2) {
		assert(multiplyOpCheck(op1.getDims(), op2.getDims()));
		typedef std::common_type<T1, T2>::type bigger_type;
		Matrix <bigger_type,I, K> result{};
		for (std::size_t r = 0; r < I; r++)
			for (std::size_t c = 0; c < K; c++)
				for (std::size_t i = 0; i < J; i++)
					result[r][c] += op1(r*J+i) * op2 (i*K + c);
		return result;
	}


	// overload << operator for printing 
	template <typename B>
	std::ostream & operator<<(std::ostream & out,const MatExpression<B> & m) {
		dims_type dims = m.getDims();
		std::cout <<"Resulting matrix size " <<dims.first << 'x' << dims.second << std::endl;
		for (std::size_t r = 0; r < dims.first; ++r) {
			for (std::size_t c = 0; c < dims.second; ++c) {
				out << m(r*dims.second +c) << " ";
			}
			out << std::endl;
		}
		return out;
	}
};  // namespace myMatrix
#endif MATRIX_H