#pragma once

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// http://stackoverflow.com/questions/10526950/symbolic-differentiation-using-expression-templates-in-c
// // https://en.wikipedia.org/wiki/Differentiation_rules#Derivatives_of_trigonometric_functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace ctd {
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Must-have forward declarations
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	/*struct constant;
	template<class, class> struct multiply;*/
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Expression Wrapper class - i think we dont need this anymore
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	/*template <class SubExpr>
	struct expr_wrapper {
		
		const SubExpr se;
		
		expr_wrapper(): se{SubExpr{}} {}
		expr_wrapper(const SubExpr &se): se{se} {}
		
		// get value under this wrapper
		inline double value(int id, double x) const {
			return se.value(x, id);
		}
		
		// differentiate expression tree under this wrapper
		inline double diff(int id, double x) const {
			return se.diff(id, x);
		}
		
		// fancy operator for this->value
		inline double operator()(int id, double x) const {
			return value(id, x);
		}
		
		// prefix negative sign must be in class scope
		
		template <class output_type = 
			expr_wrapper<
				multiply<
					expr_wrapper<constant>, expr_wrapper<SubExpr>
				>
			>
		>
		inline output_type operator-() const;
		
	};*/
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// constant class - actual literals in expression tree
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	struct constant {
		
		const double data;
		
		constant(double value): 
			data{value} 
		{}
		
		double value(int, double) const {
			return data;
		}
		
		double diff(int, double) const {
			return 0.0;
		}
	};
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// unknown - symbolic variables
	// hopefully ID and Value are template variables 
	// so ternary is also compile time
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	template <int ID, int Value>
	struct unknown {
		
		double value(int in_id, double x) const {
			return in_id == ID ? x : Value;
		}
		
		double diff(int in_id, double) const {
			return in_id == ID ? 1.0 : 0.0;
		}
	};
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// symbol class
	// ID: identity of symbolic variable
	// Value: constant value for partial derivative
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	template <int ID = 0>
	struct symbol {
		
		template <int Value = 0>
		using value = unknown<ID, Value>;
	};

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Function
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	/*
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
		typename output_type = 
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
		typename output_type = 
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
		typename output_type = 
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
	*/
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Operator logic
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	// binary operator base class
	// OperandA and OperandB should be declared only once: base class
	template <
		class OperandA, 
		class OperandB
	>
	struct binary_operator {
		
		const OperandA a;
		const OperandB b;
		
		binary_operator(const OperandA a, const OperandB b): 
			a{a}, b{b} 
		{}
	};
	
	// add
	template <
		class OperandA, 
		class OperandB
	>
	struct add: public binary_operator<OperandA, OperandB> {
		
		using bin_op_inst = binary_operator<OperandA, OperandB>;
		
		add(const OperandA a, const OperandB b): 
			bin_op_inst(a, b) 
		{}
		
		double value(int id, double x) const {
			return bin_op_inst::a.value(id, x) + bin_op_inst::b.value(id, x);
		}
		
		double diff(int id, double x) const {
			return bin_op_inst::a.diff(id, x) + bin_op_inst::b.diff(id, x);
		}
	};
	
	/*
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
		class bin_op_inst = binary_operator<Base, Exponent>
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
	*/
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Operator syntax
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	/*
	// -expr
	template <class SubExpr>
	template <class output_type>
	inline output_type expr_wrapper<SubExpr>::operator-() const {
		return (-1) * (*this);
	}
	*/
	// expr + expr
	template<
		class SubExprA, 
		class SubExprB, 
		class add_inst = add<SubExprA, SubExprB>
	>
	add_inst operator+(
		const SubExprA sea,
		const SubExprB seb
	){
		return add_inst{sea, seb};
	}
	/*
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
	
	// expr - const
	template<class SubExpr>
	inline auto operator-(const expr_wrapper<SubExpr> &se, const constant &c)
	-> decltype(se - expr_wrapper<constant>{c})
	{
		return se - expr_wrapper<constant>{c};
	}
	
	// expr * expr
	template<
		class SubExprA, class SubExprB,
		class multiply_inst = multiply<
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
		class divide_inst = divide<
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
		class exponential_inst = exponential<
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
	*/
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Syntactic sugar evaluation
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	/*
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
	*/
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Predefined expression types
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	//expr_wrapper<unknown> x;
	
}