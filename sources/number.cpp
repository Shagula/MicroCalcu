#include "../includes/number.hpp"
#include "../includes/core.hpp"
#include <map>
namespace mcalcu
{
	namespace op
	{


		NumObj integer_add(const NumObj& lhs, const NumObj& rhs)
		{
			return Integer::from(Integer::get_value(lhs) + Integer::get_value(rhs));
		}
		NumObj integer_mul(const NumObj& lhs, const NumObj& rhs)
		{
			return Integer::from(Integer::get_value(lhs) * Integer::get_value(rhs));
		}
		NumObj integer_sub(const NumObj& lhs, const NumObj& rhs)
		{
			return Integer::from(Integer::get_value(lhs) - Integer::get_value(rhs));
		}
		NumObj integer_div(const NumObj& lhs, const NumObj& rhs)
		{
			long long a = Integer::get_value(lhs),b= Integer::get_value(rhs);
			if (a % b == 0)
				return Integer::from(a / b);
			return Fraction::from(lhs, rhs);
		}
		NumObj fraction_add(const NumObj& lhs, const NumObj& rhs)
		{
			auto demo = lhs * rhs;
			auto lhs_num = std::static_pointer_cast<Fraction> (lhs)->numerator;
			auto rhs_num = std::static_pointer_cast<Fraction> (rhs)->numerator;
			auto lhs_deno = std::static_pointer_cast<Fraction> (lhs)->denominator;
			auto rhs_deno = std::static_pointer_cast<Fraction> (rhs)->denominator;
			auto num = lhs_num * rhs_deno + rhs_num * lhs_deno;
			return Fraction::from(num, demo)->simplify();
		}
		NumObj fraction_sub(const NumObj& lhs, const NumObj& rhs)
		{
			auto demo = lhs * rhs;
			auto lhs_num = std::static_pointer_cast<Fraction> (lhs)->numerator;
			auto rhs_num = std::static_pointer_cast<Fraction> (rhs)->numerator;
			auto lhs_deno = std::static_pointer_cast<Fraction> (lhs)->denominator;
			auto rhs_deno = std::static_pointer_cast<Fraction> (rhs)->denominator;
			auto num = lhs_num * rhs_deno - rhs_num * lhs_deno;
			return Fraction::from(num, demo)->simplify();
		}
		NumObj fraction_mul(const NumObj& lhs, const NumObj& rhs)
		{
			auto lhs_num = std::static_pointer_cast<Fraction> (lhs)->numerator;
			auto rhs_num = std::static_pointer_cast<Fraction> (rhs)->numerator;
			auto lhs_deno = std::static_pointer_cast<Fraction> (lhs)->denominator;
			auto rhs_deno = std::static_pointer_cast<Fraction> (rhs)->denominator;
			return Fraction::from(lhs_num*rhs_num, lhs_deno*lhs_deno)->simplify();
		}
		NumObj fraction_div(const NumObj& lhs, const NumObj& rhs)
		{
			return fraction_mul(lhs, Fraction::reciprocal(rhs));
		}
		using BinOpFuncType = decltype(&integer_add);

		std::map<std::pair<NumberKind, OpKind>, BinOpFuncType> bin_op_map
		{
			{{INTEGER,ADD},integer_add},{{INTEGER,SUB},integer_sub},{{INTEGER,MUL},integer_mul},{{INTEGER,DIV},integer_div},
			{{FRACTION,ADD},fraction_add},{{FRACTION,SUB},fraction_sub},{{FRACTION,MUL},fraction_mul},{{FRACTION,DIV},fraction_div}
		};
	}

	std::string to_string(NumberKind nk)
	{
		std::map<NumberKind, std::string> table
		{ 
			{INTEGER,"int"},{FRACTION,"frac"},{IRRATIONAL,"irrational"},{COMPOUND,"compound"}
		};
		return table[nk];
	}
	std::string to_string(OpKind ok)
	{
		std::map<OpKind, std::string> table
		{
			{ADD,"add"},{SUB,"sub"},{MUL,"mul"},{DIV,"div"}
		};
		return table[ok];
	}

	NumObj bin_op(const NumObj& lhs, const NumObj& rhs, OpKind op)
	{
		if (lhs->get_number_kind() == rhs->get_number_kind())
		{// Remeber to do something to convert
			auto result = op::bin_op_map.find({ lhs->get_number_kind(),op });
			if (result == op::bin_op_map.end())
				throw Error("{type:"+to_string(lhs->get_number_kind())+",op:"+to_string(op)+"} operation is not compatible.");
			return result->second(lhs, rhs);
		}
		else
		{
			throw Error("NumObj bin_op(const NumObj& lhs, const NumObj& rhs, OpKind op) not finished yet ");
		}
	}

	NumObj operator+(const NumObj& lhs, const NumObj& rhs)
	{
		return bin_op(lhs, rhs, ADD);
	}

	NumObj operator-(const NumObj& lhs, const NumObj& rhs)
	{
		return bin_op(lhs, rhs, SUB);
	}

	NumObj operator*(const NumObj& lhs, const NumObj& rhs)
	{
		return bin_op(lhs, rhs, MUL);
	}

	NumObj operator/(const NumObj& lhs, const NumObj& rhs)
	{
		return bin_op(lhs, rhs, DIV);
	}

	NumObj Integer::from(long long v)
	{
		return std::make_shared<Integer>(v);
	}
	
	long long Integer::get_value(const NumObj& obj)
	{
		if (obj->get_number_kind() != INTEGER)
		{
			throw Error("Integer::get_value(const NumObj& obj) Only accept Integer object!");
		}
		return std::static_pointer_cast<Integer>(obj)->value;
	}
	std::string Integer::to_string()
	{
		return "< Integer: " + std::to_string(value) + " >";
	}
	NumObj Integer::common_factor(const NumObj& c)
	{
		if (c->get_number_kind() == INTEGER)
		{
			return Integer::from(gcd(value, std::static_pointer_cast<Integer>(c)->value));
		}
		return Integer::from(1);
	}
	NumObj Integer::convert(NumberKind kind)
	{
		switch (get_number_kind())
		{
		case INTEGER:
			return Integer::from(value);
		default:
			throw Error("failed to convert");
		}
	}

	NumObj Integer::simplify()
	{
		return Integer::from(value);
	}

	NumObj Integer::clone()
	{
		return Integer::from(value);
	}

	long double Integer::calcu()
	{
		return (long double)value;
	}

	NumObj Fraction::from(const NumObj& _numerator, const NumObj& _denominator)
	{
		return std::make_shared<Fraction>(_numerator, _denominator);
	}

	NumObj Fraction::reciprocal(const NumObj& frac)
	{
		auto fracc = std::static_pointer_cast<Fraction> (frac);
		return Fraction::from(fracc->denominator, fracc->numerator);
	}

	std::string Fraction::to_string()
	{
		return "<Fraction:{" + numerator->to_string() + "},{" + denominator->to_string() + "}>";
	}

	NumObj Fraction::common_factor(const NumObj& c)
	{
		return NumObj();
	}

	NumObj Fraction::convert(NumberKind kind)
	{
		switch (kind)
		{
		// Warning, accuracy may be lost
		case mcalcu::INTEGER:
			return Integer::from(calcu());
		case mcalcu::FRACTION:
			return clone();
		}
		throw Error("invalid convert");
	}

	NumObj Fraction::simplify()
	{
		numerator = numerator->simplify();
		denominator = denominator->simplify();
		auto factor = numerator->common_factor(denominator);
		numerator = numerator / factor;
		denominator = denominator / factor;
		if (numerator->get_number_kind() == INTEGER && denominator->get_number_kind() == INTEGER)
		{
			long long num = Integer::get_value(numerator), deno = Integer::get_value(denominator);
			if (num % deno == 0)
				return Integer::from(num / deno);
		}
		return clone();
	}

	NumObj Fraction::clone()
	{
		return Fraction::from(numerator->clone(), denominator->clone());
	}

	long double Fraction::calcu()
	{
		return numerator->calcu() / denominator->calcu();
	}

}