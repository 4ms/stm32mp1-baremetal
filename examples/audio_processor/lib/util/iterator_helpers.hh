#pragma once
#include <iterator>
#include <tuple>

namespace IteratorHelper
{

template<typename T>
using select_iterator_for = std::conditional_t<std::is_const_v<std::remove_reference_t<T>>,
											   typename std::decay_t<T>::const_iterator,
											   typename std::decay_t<T>::iterator>;

#ifdef COUNTZIP_SUPPORT_VECTORBOOL
	//Optional if we want to handle vector<bool>:
	#include <vector>
template<typename Iter>
using select_access_type_for = std::conditional_t<std::is_same_v<Iter, std::vector<bool>::iterator> ||
													  std::is_same_v<Iter, std::vector<bool>::const_iterator>,
												  typename std::iterator_traits<Iter>::value_type,
												  typename std::iterator_traits<Iter>::reference>;

#else

template<typename Iter>
using select_access_type_for = typename std::iterator_traits<Iter>::reference;

#endif

template<typename... Args, std::size_t... Index>
auto any_match_impl(std::tuple<Args...> const &lhs, std::tuple<Args...> const &rhs, std::index_sequence<Index...>)
	-> bool
{
	return (... | (std::get<Index>(lhs) == std::get<Index>(rhs)));
}

template<typename... Args>
auto any_match(std::tuple<Args...> const &lhs, std::tuple<Args...> const &rhs) -> bool
{
	return any_match_impl(lhs, rhs, std::index_sequence_for<Args...>{});
}

} // namespace IteratorHelper
