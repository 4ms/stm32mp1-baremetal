#pragma once
#include "iterator_helpers.hh"
#include <tuple>

// zip
//
// Use in range-based for loops
// Given N iterable containers (such as arrays) return a tuple of size N:
// [ a[index], b[index], c[index], ... ]
//
// For example:
//
// for (auto & [in, out, param] : zip(in_array, out_array, param_array) {
//     out = in * calc_something(param.xyz);
// }
//
// Source:
// Pretty much from https://committhis.github.io/2020/10/14/zip-iterator.html
// with the counting index stuff and some C++20 fixes added by me (Dan Green)

namespace Zip
{
namespace details
{
using IteratorHelper::any_match;
using IteratorHelper::select_access_type_for;
using IteratorHelper::select_iterator_for;

template<typename... Iters>
class ZipIterator {
public:
	using ValueT = std::tuple<select_access_type_for<Iters>...>;

	ZipIterator() = delete;

	ZipIterator(Iters &&...iters)
		: m_iters{std::forward<Iters>(iters)...}
	{}

	auto operator++() -> ZipIterator &
	{
		std::apply([](auto &...args) { ((args += 1), ...); }, m_iters);
		return *this;
	}

	auto operator++(int) -> ZipIterator
	{
		auto tmp = *this;
		++*this;
		return tmp;
	}

	friend auto operator!=(ZipIterator const &lhs, ZipIterator const &rhs)
	{
		return !(lhs == rhs);
	}

	friend auto operator==(ZipIterator const &lhs, ZipIterator const &rhs)
	{
		// This method is a tiny bit faster (one compare per loop), but of course things break if first element isn't the largest:
		// return (std::get<1>(m_iters) == std::get<1>(other.m_iters));

		// This way adds a compare for each iterator, per loop, but is safer
		return any_match(lhs.m_iters, rhs.m_iters);
	}

	auto operator*() -> ValueT
	{
		return std::apply([](auto &&...args) { return ValueT{*args...}; }, m_iters);
	}

private:
	std::tuple<Iters...> m_iters;
};

// Zipper
template<typename... T>
class Zipper {
public:
	using ZipType = ZipIterator<select_iterator_for<T>...>;

	//Constructor
	template<typename... Args>
	explicit Zipper(Args &&...args)
		: m_args{std::forward<Args>(args)...}
	{}

	auto begin() -> ZipType
	{
		return std::apply([](auto &&...args) { return ZipType{std::begin(args)...}; }, m_args);
	}
	auto end() -> ZipType
	{
		return std::apply([](auto &&...args) { return ZipType{std::end(args)...}; }, m_args);
	}

private:
	std::tuple<T...> m_args;
};
} // namespace details
} // namespace Zip

template<typename... T>
auto zip(T &&...t)
{
	return Zip::details::Zipper<T...>{std::forward<T>(t)...};
}
