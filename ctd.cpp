#include <iostream>
#include <cmath>

namespace ctd {
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Must-have forward declarations
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	struct constant;
	template<class, class> struct multiply;
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Expression Wrapper class
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	template <class SubExpr>
	struct expr_wrapper {
		const SubExpr se;
		expr_wrapper(): se{SubExpr{}} {}
		expr_wrapper(const SubExpr &se): se{se} {}
		inline double value(double x) const {
			return se.value(x);
		}
		inline double diff(double x) const {
			return se.diff(x);
		}
		inline double operator()(double x) const {
			return value(x);
		}
		template <class output_type = 
			expr_wrapper<
				multiply<
					expr_wrapper<constant>, expr_wrapper<SubExpr>
				>
			>
		>
		inline output_type operator-() const;
	};
	
	struct constant {
		const double __value;
		constant(double value): __value{value} {}
		inline double value(double) const {
			return __value;
		}
		inline double diff(double) const {
			return 0.0;
		}
	};
	
	struct unknown {
		inline double value(double x) const {
			return x;
		}
		inline double diff(double) const {
			return 1.0;
		}
	};

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Function
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	// sin(f(x))
	template <class SubExpr>
	struct func_sin {
		const SubExpr arg;
		func_sin(const SubExpr arg): arg{arg} {}
		inline double value(double x) const {
			return std::sin(arg.value(x));
		}
		inline double diff(double x) const {
			return std::cos(arg.value(x)) * arg.diff(x);
		}
	};
	
	template <
		typename SubExpr, 
		/*alias*/ typename output_type = 
			expr_wrapper<func_sin<expr_wrapper<SubExpr>>>
	>
	inline output_type
	sin(const expr_wrapper<SubExpr> &a){
		return output_type{func_sin<expr_wrapper<SubExpr>>{a}};
	}
	
	inline expr_wrapper<func_sin<constant>> sin(double a){
		return expr_wrapper<func_sin<constant>>
			{func_sin<constant>{constant{a}}};
	}
	
	inline auto sin(int a) -> decltype(sin(static_cast<double>(a))) {
		return sin(static_cast<double>(a));
	}
	
	// cos(f(x))
	template <class SubExpr>
	struct func_cos {
		const SubExpr arg;
		func_cos(const SubExpr arg): arg{arg} {}
		inline double value(double x) const {
			return std::cos(arg.value(x));
		}
		inline double diff(double x) const {
			return (-std::sin(arg.value(x))) * arg.diff(x);
		}
	};
	
	template <
		typename SubExpr, 
		/*alias*/ typename output_type = 
			expr_wrapper<func_cos<expr_wrapper<SubExpr>>>
	>
	inline output_type
	cos(const expr_wrapper<SubExpr> &a){
		return output_type{func_cos<expr_wrapper<SubExpr>>{a}};
	}
	
	inline expr_wrapper<func_cos<constant>> cos(double a){
		return expr_wrapper<func_cos<constant>>
			{func_cos<constant>{constant{a}}};
	}
	
	inline auto cos(int a) -> decltype(cos(static_cast<double>(a))) {
		return cos(static_cast<double>(a));
	}

	// log(f(x))
	template <class SubExpr>
	struct func_log {
		const SubExpr arg;
		func_log(const SubExpr arg): arg{arg} {}
		inline double value(double x) const {
			return std::log(arg.value(x));
		}
		inline double diff(double x) const {
			return arg.diff(x)/arg.value(x);
		}
	};
	
	template <
		typename SubExpr, 
		/*alias*/ typename output_type = 
			expr_wrapper<func_log<expr_wrapper<SubExpr>>>
	>
	inline output_type
	log(const expr_wrapper<SubExpr> &a){
		return output_type{func_log<expr_wrapper<SubExpr>>{a}};
	}
	
	inline expr_wrapper<func_log<constant>> log(double a){
		return expr_wrapper<func_log<constant>>
			{func_log<constant>{constant{a}}};
	}
	
	inline auto log(int a) -> decltype(log(static_cast<double>(a))) {
		return log(static_cast<double>(a));
	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Operator logic
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	// binary operator base class
	template <class OperandA, class OperandB>
	struct binary_operator {
		const OperandA a;
		const OperandB b;
		binary_operator(const OperandA &a, const OperandB &b)
		: a{a}, b{b} {}
	};
	
	// add
	template <
		class OperandA, 
		class OperandB,
		class bin_op_inst = binary_operator<OperandA, OperandB>
	>
	struct add: binary_operator<OperandA, OperandB> {
		add(const OperandA &a, const OperandB &b): bin_op_inst(a, b) {}
		inline double value(double x) const {
			return bin_op_inst::a.value(x) + bin_op_inst::b.value(x);
		}
		inline double diff(double x) const {
			return bin_op_inst::a.diff(x) + bin_op_inst::b.diff(x);
		}
	};
	
	// multiply
	template <
		class OperandA, 
		class OperandB
	>
	struct multiply: binary_operator<OperandA, OperandB> {
		using bin_op_inst = binary_operator<OperandA, OperandB>;
		multiply(const OperandA &a, const OperandB &b)
		: bin_op_inst(a, b) {}
		inline double value(double x) const {
			return bin_op_inst::a.value(x) * bin_op_inst::b.value(x);
		}
		inline double diff(double x) const {
			return bin_op_inst::a.diff(x) * bin_op_inst::b.value(x) 
				+ bin_op_inst::a.value(x) * bin_op_inst::b.diff(x);
		}
	};
	
	// divide
	template <
		class OperandA, 
		class OperandB,
		class bin_op_inst = binary_operator<OperandA, OperandB>
	>
	struct divide: binary_operator<OperandA, OperandB> {
		divide(const OperandA &a, const OperandB &b)
		: bin_op_inst(a, b) {}
		inline double value(double x) const {
			return bin_op_inst::a.value(x) / bin_op_inst::b.value(x);
		}
		inline double diff(double x) const {
			return (bin_op_inst::a.diff(x) * bin_op_inst::b.value(x) 
					- bin_op_inst::a.value(x) * bin_op_inst::b.diff(x))
				/ (bin_op_inst::b.value(x)*bin_op_inst::b.value(x));
		}
	};
	
	// exponential
	template <
		class Base, class Exponent, 
		/*alias*/ class bin_op_inst = binary_operator<Base, Exponent>
	>
	struct exponential : bin_op_inst {
		exponential(const Base &a, const Exponent &b)
		: bin_op_inst(a, b) {}
		inline double value(double x) const {
			return std::pow(bin_op_inst::a.value(x), bin_op_inst::b.value(x));
		}
		inline double diff(double x) const {
			// functional power rule
			return value(x)*(
				bin_op_inst::a.diff(x)
					*((bin_op_inst::b.value(x))/(bin_op_inst::a.value(x))) 
				+ bin_op_inst::b.diff(x)*std::log(bin_op_inst::a.value(x))
			);
		}
	};
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Operator syntax
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	// -expr
	template <class SubExpr>
	template </*alias*/ class output_type>
	inline output_type expr_wrapper<SubExpr>::operator-() const {
		return (-1) * (*this);
	}
	
	// expr + expr
	template<
		class SubExprA, 
		class SubExprB, 
		/*alias*/ class add_inst = add<
			expr_wrapper<SubExprA>, 
			expr_wrapper<SubExprB>
		>
	>
	inline expr_wrapper<add_inst> operator+(
		const expr_wrapper<SubExprA> &sea,
		const expr_wrapper<SubExprB> &seb
	){
		return expr_wrapper<add_inst>{add_inst{sea, seb}};
	}
	
	// const + expr
	template<class SubExpr>
	inline auto operator+(const constant &c, const expr_wrapper<SubExpr> &se)
	-> decltype(expr_wrapper<constant>{c} + se)
	{
		return expr_wrapper<constant>{c} + se;
	}
	
	// expr + const
	template<class SubExpr>
	inline auto operator+(const expr_wrapper<SubExpr> &se, const constant &c)
	-> decltype(expr_wrapper<constant>{c} + se)
	{
		return expr_wrapper<constant>{c} + se;
	}
	
	// expr - expr
	template<class SubExprA, class SubExprB>
	inline auto operator-(
		const expr_wrapper<SubExprA> &sea,
		const expr_wrapper<SubExprB> &seb
	)
	-> decltype(sea + (-seb))
	{
		return sea + (-seb);
	}
	
	// const - expr
	template<class SubExpr>
	inline auto operator-(const constant &c, const expr_wrapper<SubExpr> &se)
	-> decltype(expr_wrapper<constant>{c} - se)
	{
		return expr_wrapper<constant>{c} - se;
	}
	
	// expr - const --> -expr + const
	template<class SubExpr>
	inline auto operator-(const expr_wrapper<SubExpr> &se, const constant &c)
	-> decltype(se - expr_wrapper<constant>{c})
	{
		return se - expr_wrapper<constant>{c};
	}
	
	// expr * expr
	template<
		class SubExprA, class SubExprB,
		/*alias*/ class multiply_inst = multiply<
			expr_wrapper<SubExprA>, expr_wrapper<SubExprB>
		>
	>
	inline expr_wrapper<multiply_inst> operator*(
		const expr_wrapper<SubExprA> &sea, 
		const expr_wrapper<SubExprB> &seb
	){
		return expr_wrapper<multiply_inst>{multiply_inst{sea, seb}};
	}

	// const * expr
	template<class SubExpr>
	inline auto operator*(const constant &c, const expr_wrapper<SubExpr> &se)
	-> decltype(expr_wrapper<constant>{c} * se)
	{
		return expr_wrapper<constant>{c} * se;
	}
	
	// expr * const 
	template<class SubExpr>
	inline auto operator*(const expr_wrapper<SubExpr> &se, const constant &c)
	-> decltype(expr_wrapper<constant>{c} * se)
	{
		return expr_wrapper<constant>{c} * se;
	}
	
	// expr / expr
	template<
		class SubExprA, class SubExprB,
		/*alias*/ class divide_inst = divide<
			expr_wrapper<SubExprA>, expr_wrapper<SubExprB>
		>
	>
	inline expr_wrapper<divide_inst> operator/(
		const expr_wrapper<SubExprA> &sea, 
		const expr_wrapper<SubExprB> &seb
	){
		return expr_wrapper<divide_inst>{divide_inst{sea, seb}};
	}
	
	// const / expr
	template<class SubExpr>
	inline auto operator/(const constant &c, const expr_wrapper<SubExpr> &se)
	-> decltype(expr_wrapper<constant>{c} / se)
	{
		return expr_wrapper<constant>{c} / se;
	}
	
	// expr / const 
	template<class SubExpr>
	inline auto operator/(const expr_wrapper<SubExpr> &se, const constant &c)
	-> decltype(expr_wrapper<constant>{c} / se)
	{
		return expr_wrapper<constant>{c} / se;
	}
	
	// expr ^ expr
	template<
		class SubExprA, 
		class SubExprB, 
		/*alias*/ class exponential_inst = exponential<
			expr_wrapper<SubExprA>, 
			expr_wrapper<SubExprB>
		>
	>
	inline expr_wrapper<exponential_inst> operator^(
		const expr_wrapper<SubExprA> &sea,
		const expr_wrapper<SubExprB> &seb
	){
		return expr_wrapper<exponential_inst>{exponential_inst{sea, seb}};
	}
	
	// const ^ expr
	template<class SubExpr>
	inline auto operator^(const constant &c, const expr_wrapper<SubExpr> &se)
	-> decltype(expr_wrapper<constant>{c} ^ se)
	{
		return expr_wrapper<constant>{c} ^ se;
	}
	
	// expr ^ const 
	template<class SubExpr>
	inline auto operator^(const expr_wrapper<SubExpr> &se, const constant &c)
	-> decltype(se ^ expr_wrapper<constant>{c})
	{
		return se ^ expr_wrapper<constant>{c};
	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Syntactic sugar evaluation
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	template <class SubExpr>
	struct diff_wrapper : expr_wrapper<SubExpr> {
		diff_wrapper(const expr_wrapper<SubExpr> &cpy)
		: expr_wrapper<SubExpr>{cpy} {}
		inline double operator()(double x) const {
			return expr_wrapper<SubExpr>::diff(x);
		}
	};
	
	struct sdx {} dx;
	struct sdt {} dt;
	struct sdxdt {} dxdt;
	
	inline sdxdt operator/(const sdx&, const sdt&){
		return sdxdt{};
	}
	
	template<class SubExpr>
	inline diff_wrapper<SubExpr> 
	operator*(const expr_wrapper<SubExpr> &sea, const sdxdt&){
		return diff_wrapper<SubExpr>{sea};
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Predefined expression types
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	expr_wrapper<unknown> x;
	
}

#include <cxxabi.h>

int main(int argc, char** argv) {
	using namespace ctd;
	// https://en.wikipedia.org/wiki/Differentiation_rules#Derivatives_of_trigonometric_functions
	
	// demo expression: (this fails, need to debug)
	auto f = 3 + x + 3*x - (3^x) + sin(cos(x^3)) + log(x/((x-3)^3));
	auto fd = f*(dx/dt);
	std::cout.precision(30);
	std::cout << std::fixed << f(5.0) << std::endl;
	std::cout << std::fixed << fd(5.0) << std::endl;
	/*std::cout << "expression template tree: " << std::endl;
	std::cout <<  abi::__cxa_demangle(typeid(f).name(), 0, 0, NULL);*/
	return 0;
}
