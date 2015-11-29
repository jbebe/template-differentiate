#pragma once

#include <string>
#include <sstream>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// http://stackoverflow.com/questions/10526950/symbolic-differentiation-using-expression-templates-in-c
// // https://en.wikipedia.org/wiki/Differentiation_rules#Derivatives_of_trigonometric_functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace ctd {
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Negatable - workaround tp make any number negative
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	template <typename ChildType> struct negatable;
	template <class, class> struct multiply;
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// constant class - actual literals in expression tree
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	struct constant {
		
		const double data;
		
		constant(double value): data{value} {}
		constant(int value): data{static_cast<double>(value)} {}
		
		double value(int, double) const {
			return data;
		}
		
		double diff(int, double) const {
			return 0.0;
		}
		
		std::string printValue(int) const {
			if (std::abs(data - round(data)) < 1e-7){
				// data is (almost) a whole numeber
				return std::to_string(static_cast<long long>(round(data)));
			}
			else {
				constexpr int tmpSize = 512;
				char tmp[tmpSize];
				snprintf(tmp, tmpSize-1, "%.3f", data);
				return tmp;
			}
		}
		
		std::string printDiff(int) const {
			return "0";
		}
	};
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// unknown - symbolic variables
	// hopefully ID and Value are template variables 
	// so ternary is also compile time
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	struct unknown_base {};
	
	template <int ID>
	struct unknown: public negatable<unknown<ID>>, unknown_base {
		
		const double data;
		
		unknown(double data = 0.): data{data} {}
		
		double value(int in_id, double x) const {
			return in_id == ID ? x : data;
		}
		
		double diff(int in_id, double) const {
			return in_id == ID ? 1.0 : 0.0;
		}
		
		std::string printValue(int) const {
			return std::string{0, ID};
		}
		
		std::string printDiff(int in_id) const {
			return in_id == ID ? "1" : "0";
		}
	};
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// symbol class
	// ID: identity of symbolic variable
	// Value: constant value for partial derivative
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	template <int ID = 0>
	struct symbol {
		using value = unknown<ID>;
	};
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Unary negation sign - must be declared after operator logic section
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	template <typename ChildType>
	struct negatable {
		multiply<constant, ChildType> operator-() const;
	};

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Function
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	// sin(f(x))
	template <class SubExpr>
	struct func_sin: negatable<func_sin<SubExpr>> {
		
		const SubExpr arg;
		
		func_sin(const SubExpr arg): arg{arg} {}
		
		double value(int id, double x) const {
			return std::sin(arg.value(id, x));
		}
		
		double diff(int id, double x) const {
			return std::cos(arg.value(id, x)) * arg.diff(id, x);
		}
		
		std::string printValue(int id) const {
			std::stringstream ss;
			ss << "sin(" << arg.printValue(id) << ")";
			return ss.str();
		}
		
		std::string printDiff(int id) const {
			std::stringstream ss;
			ss << "cos(" << arg.printValue(id) << ")*(" << arg.printDiff(id) << ")";
			return ss.str();
		}
	};
	
	template <
		typename SubExpr, 
		typename output_type = func_sin<SubExpr>
	>
	output_type sin(const SubExpr a){
		return func_sin<SubExpr>{a};
	}
	
	func_sin<constant> sin(double a){
		return func_sin<constant>{constant{a}};
	}
	
	auto sin(int a) 
	-> decltype(sin(static_cast<double>(a))) 
	{
		return sin(static_cast<double>(a));
	}
	
	// cos(f(x))
	template <class SubExpr>
	struct func_cos: negatable<func_cos<SubExpr>> {

		const SubExpr arg;

		func_cos(const SubExpr arg): arg{arg} {}

		double value(int id, double x) const {
			return std::cos(arg.value(id, x));
		}

		double diff(int id, double x) const {
			return (-std::sin(arg.value(id, x))) * arg.diff(id, x);
		}
		
		std::string printValue(int id) const {
			std::stringstream ss;
			ss << "cos(" << arg.print(id) << ")";
			return ss.str();
		}
		
		std::string printDiff(int id) const {
			std::stringstream ss;
			ss << "-sin(" << arg.printValue(id) << ")*(" << arg.printDiff(id) << ")";
			return ss.str();
		}
	};
	
	template <
		typename SubExpr, 
		typename output_type = func_cos<SubExpr>
	>
	output_type cos(const SubExpr a){
		return output_type{a};
	}
	
	func_cos<constant> cos(double a){
		return func_cos<constant>{func_cos<constant>{constant{a}}};
	}
	
	auto cos(int a) -> decltype(cos(static_cast<double>(a))) {
		return cos(static_cast<double>(a));
	}

	// log(f(x))
	template <class SubExpr>
	struct func_log: negatable<func_log<SubExpr>> {
		
		const SubExpr arg;
		
		func_log(const SubExpr arg): arg{arg} {}
		
		double value(int id, double x) const {
			return std::log(arg.value(id, x));
		}
		
		double diff(int id, double x) const {
			return arg.diff(id, x)/arg.value(id, x);
		}
		
		std::string printValue(int id) const {
			std::stringstream ss;
			ss << "log(" << arg.print(id) << ")";
			return ss.str();
		}
		
		std::string printDiff(int id) const {
			std::stringstream ss;
			ss << "(" << arg.printDiff(id) << ")/(" << arg.printValue(id) << ")";
			return ss.str();
		}
	};
	
	template <
		typename SubExpr, 
		typename output_type = func_log<SubExpr>
	>
	output_type log(const SubExpr a){
		return output_type{a};
	}
	
	func_log<constant> log(double a){
		return func_log<constant>{constant{a}};
	}
	
	auto log(int a) -> decltype(log(static_cast<double>(a))) {
		return log(static_cast<double>(a));
	}
	
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
	struct add: binary_operator<OperandA, OperandB>, negatable<add<OperandA, OperandB>> {
		
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
		
		std::string printValue(int id) const {
			std::stringstream ss;
			ss << "(" << bin_op_inst::a.print(id) << ")+(" << bin_op_inst::b.print(id) << ")";
			return ss.str();
		}
		
		std::string printDiff(int id) const {
			std::stringstream ss;
			ss << "(" << bin_op_inst::a.printDiff(id) << ")+(" << bin_op_inst::b.printDiff(id) << ")";
			return ss.str();
		}
	};
	
	// multiply
	template <
		class OperandA, 
		class OperandB
	>
	struct multiply: binary_operator<OperandA, OperandB>, negatable<multiply<OperandA, OperandB>> {
		
		using bin_op_inst = binary_operator<OperandA, OperandB>;
		
		multiply(const OperandA a, const OperandB b): 
			bin_op_inst(a, b) 
		{}
		
		double value(int id, double x) const {
			return bin_op_inst::a.value(id, x) * bin_op_inst::b.value(id, x);
		}
		
		double diff(int id, double x) const {
			return bin_op_inst::a.diff(id, x) * bin_op_inst::b.value(id, x) 
			+ bin_op_inst::a.value(id, x) * bin_op_inst::b.diff(id, x);
		}
		
		std::string printValue(int id) const {
			std::stringstream ss;
			ss << "(" << bin_op_inst::a.printValue(id) << ")*(" << bin_op_inst::b.printValue(id) << ")";
			return ss.str();
		}
		
		std::string printDiff(int id) const {
			std::stringstream ss;
			ss << "((" << bin_op_inst::a.printDiff(id) << ")*(" 
				<< bin_op_inst::b.printValue(id) << ")+(" 
				<< bin_op_inst::a.printValue(id) << ")*(" 
				<< bin_op_inst::b.printDiff(id) << "))";
			return ss.str();
		}
	};
	
	// divide
	template <
		class OperandA, 
		class OperandB
	>
	struct divide: binary_operator<OperandA, OperandB>, negatable<divide<OperandA, OperandB>> {
		
		using bin_op_inst = binary_operator<OperandA, OperandB>;
		
		divide(const OperandA a, const OperandB b)
		: bin_op_inst(a, b) {}
		
		double value(int id, double x) const {
			return bin_op_inst::a.value(id, x) / bin_op_inst::b.value(id, x);
		}
		
		double diff(int id, double x) const {
			return (bin_op_inst::a.diff(id, x) * bin_op_inst::b.value(id, x) 
					- bin_op_inst::a.value(id, x) * bin_op_inst::b.diff(id, x))
				/ (bin_op_inst::b.value(id, x)*bin_op_inst::b.value(id, x));
		}
		
		std::string printValue(int id) const {
			std::stringstream ss;
			ss << "(" << bin_op_inst::a.print() << ")/(" << bin_op_inst::b.print() << ")";
			return ss.str();
		}
		
		std::string printDiff(int id) const {
			std::stringstream ss;
			ss << "(((" << bin_op_inst::a.printDiff(id) << ")*(" 
				<< bin_op_inst::b.printValue(id) << ")-(" 
				<< bin_op_inst::a.printValue(id) << ")*(" 
				<< bin_op_inst::b.printDiff(id) << "))/((" 
				<< bin_op_inst::b.printValue(id) << ")*(" 
				<< bin_op_inst::b.printValue(id) << ")))";
			return ss.str();
		}
	};
	
	// exponential
	template <
		class Base, 
		class Exponent
	>
	struct exponential : binary_operator<Base, Exponent>, negatable<exponential<Base, Exponent>> {
		
		using bin_op_inst = binary_operator<Base, Exponent>;
		
		exponential(const Base a, const Exponent b)
		: bin_op_inst(a, b) {}
		
		double value(int id, double x) const {
			return std::pow(bin_op_inst::a.value(id, x), bin_op_inst::b.value(id, x));
		}
		
		double diff(int id, double x) const {
			// functional power rule
			return value(id, x)*(
				bin_op_inst::a.diff(id, x)
					*((bin_op_inst::b.value(id, x))/(bin_op_inst::a.value(id, x))) 
				+ bin_op_inst::b.diff(id, x)*std::log(bin_op_inst::a.value(id, x))
			);
		}
		
		std::string printValue(int id) const {
			std::stringstream ss;
			ss << "(" << bin_op_inst::a.print() << ")^(" << bin_op_inst::b.print() << ")";
			return ss.str();
		}
		
		std::string printDiff(int id) const {
			std::stringstream ss;
			ss << "((" << printValue(id) << ")*((" 
				<< bin_op_inst::a.printDiff(id) << ")*((" 
				<< bin_op_inst::b.printValue(id) << ")/(" 
				<< bin_op_inst::a.printValue(id) << "))+((" 
				<< bin_op_inst::b.printDiff(id) << ")*(log(" 
				<< bin_op_inst::a.printValue(id) << "))))";
			return ss.str();
		}
	};
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Operator syntax
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	// -expr
	template <typename ChildType>
	multiply<constant, ChildType> negatable<ChildType>::operator-() const
	{
		return multiply<constant, ChildType>{-1, static_cast<const ChildType&>(*this)};
	}
	
	// expr + expr
	template<
		class SubExprA,
		class SubExprB,
		class add_inst = add<SubExprA, SubExprB>,
		typename std::enable_if<
			std::is_pod<SubExprA>::value == false && std::is_pod<SubExprB>::value == false
		>::type* = nullptr
	>
	add_inst operator+(const SubExprA sea, const SubExprB seb){
		return add_inst{sea, seb};
	}
	
	// const + expr
	template<
		class SubExpr,
		class add_inst = add<constant, SubExpr>
	>
	add_inst operator+(const constant c, const SubExpr se) 
	{
		return add_inst{c, se};
	}
	
	// expr + const
	template<
		class SubExpr,
		class add_inst = add<SubExpr, constant>
	>
	add_inst operator+(const SubExpr se, const constant c) 
	{
		return add_inst{se, c};
	}
	
	// expr - expr
	template<
		class SubExprA, 
		class SubExprB,
		typename std::enable_if<
			std::is_pod<SubExprA>::value == false && std::is_pod<SubExprB>::value == false
		>::type* = nullptr
	>
	auto operator-(const SubExprA sea, const SubExprB seb)
	-> decltype(sea + (-seb))
	{
		return sea + (-seb);
	}
	
	// const - expr
	template<class SubExpr>
	auto operator-(const constant c, const SubExpr se)
	-> decltype(c - se)
	{
		return c - se;
	}
	
	// expr - const
	template<class SubExpr>
	auto operator-(const SubExpr se, const constant c)
	-> decltype(se - c)
	{
		return se - c;
	}

	// expr * expr
	template<
		class SubExprA,
		class SubExprB,
		class multiply_inst = multiply<SubExprA, SubExprB>,
		typename std::enable_if<
			std::is_pod<SubExprA>::value == false && std::is_pod<SubExprB>::value == false
		>::type* = nullptr
	>
	multiply_inst operator*(
		const SubExprA sea, 
		const SubExprB seb
	){
		return multiply_inst{sea, seb};
	}
	
	// const * expr
	template<
		class SubExpr,
		class multiply_inst = multiply<constant, SubExpr>
	>
	multiply_inst operator*(const constant c, const SubExpr se) 
	{
		return multiply_inst{c, se};
	}
	
	// expr * const 
	template<
		class SubExpr,
		class multiply_inst = multiply<SubExpr, constant>
	>
	multiply_inst operator*(const SubExpr se, const constant c)
	{
		return multiply_inst{se, c};
	}
	
	// expr / expr
	template<
		class SubExprA, 
		class SubExprB,
		class divide_inst = divide<SubExprA, SubExprB>,
		typename std::enable_if<
			std::is_pod<SubExprA>::value == false && std::is_pod<SubExprB>::value == false
		>::type* = nullptr
	>
	divide_inst operator/(const SubExprA sea, const SubExprB seb
	){
		return divide_inst{sea, seb};
	}
	
	// const / expr
	template<
		class SubExpr,
		class divide_inst = divide<constant, SubExpr>
	>
	divide_inst operator/(const constant c, const SubExpr se)
	{
		return divide_inst{c, se};
	}
	
	// expr / const 
	template<
		class SubExpr,
		class divide_inst = divide<SubExpr, constant>
	>
	divide_inst operator/(const SubExpr se, const constant c)
	{
		return divide_inst{se, c};
	}
	
	// expr ^ expr
	template<
		class SubExprA, 
		class SubExprB, 
		class exponential_inst = exponential<SubExprA, SubExprB>,
		typename std::enable_if<
			std::is_pod<SubExprA>::value == false && std::is_pod<SubExprB>::value == false
		>::type* = nullptr
	>
	exponential_inst operator^(const SubExprA sea, const SubExprB seb){
		return exponential_inst{sea, seb};
	}
	
	// const ^ expr
	template<
		class SubExpr,
		class exponential_inst = exponential<constant, SubExpr>
	>
	exponential_inst operator^(const constant c, const SubExpr se)
	{
		return exponential_inst{c, se};
	}
	
	// expr ^ const 
	template<
		class SubExpr,
		class exponential_inst = exponential<SubExpr, constant>
	>
	exponential_inst operator^(const SubExpr se, const constant c)
	{
		return exponential_inst{se, c};
	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Commands
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
	template <
		typename ExpressionTree,
		typename std::enable_if<std::is_pod<ExpressionTree>::value == false>::type* = nullptr
	>
	double diff(ExpressionTree et, int id, double x){
		return et.diff(id, x);
	}
	
	template <
		typename Constant, 
		typename std::enable_if<std::is_pod<Constant>::value == true>::type* = nullptr
	>
	double diff(Constant c, int id, double x){
		return 0;
	}
	
	template <
		typename ExpressionTree,
		typename std::enable_if<std::is_pod<ExpressionTree>::value == false>::type* = nullptr
	>
	double value(ExpressionTree et, int id, double x){
		return et.value(id, x);
	}
	
	template <
		typename Constant, 
		typename std::enable_if<std::is_pod<Constant>::value == true>::type* = nullptr
	>
	double value(Constant c, int id, double x){
		return c;
	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// Predefs
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	
#ifndef CTD_NO_PREDEF_SYMS
	auto x = symbol<'x'>::value{};
	auto y = symbol<'y'>::value{};
	auto z = symbol<'z'>::value{};
#endif
	
}