/*!
 * @file
 * Defines `boost::hana::Range`.
 *
 *
 * @copyright Louis Dionne 2014
 * Distributed under the Boost Software License, Version 1.0.
 *         (See accompanying file LICENSE.md or copy at
 *             http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_RANGE_HPP
#define BOOST_HANA_RANGE_HPP

#include <boost/hana/foldable.hpp>
#include <boost/hana/functor.hpp>
#include <boost/hana/integral.hpp>
#include <boost/hana/iterable.hpp>
#include <boost/hana/list.hpp>

#include <type_traits>
#include <utility>


namespace boost { namespace hana {
    //! @ingroup datatypes
    template <typename T, T ...v>
    struct Range { };

    constexpr struct _range {
        template <typename T, T x, T ...v>
        static constexpr Range<T, (v + x)...>
        slide(std::integer_sequence<T, v...>) { return {}; }

        template <typename T, T from, T to>
        constexpr auto operator()(Integral<T, from>, Integral<T, to>) const {
            return slide<T, from>(std::make_integer_sequence<T, to - from>{});
        }
    } range{};

    template <typename T>
    struct Iterable<Range<T>> {
        static constexpr Bool<true> is_empty_impl(Range<T>)
        { return {}; }
    };

    template <typename T, T v, T ...vs>
    struct Iterable<Range<T, v, vs...>> {
        static constexpr Integral<T, v> head_impl(Range<T, v, vs...>)
        { return {}; }

        static constexpr Range<T, vs...> tail_impl(Range<T, v, vs...>)
        { return {}; }

        static constexpr Bool<false> is_empty_impl(Range<T, v, vs...>)
        { return {}; }
    };

    template <typename T, T ...vs>
    struct Functor<Range<T, vs...>> {
        template <typename F>
        static constexpr auto fmap_impl(F f, Range<T, vs...>)
        { return list(f(Integral<T, vs>{})...); }
    };

    template <typename T, T v, T ...vs>
    struct Foldable<Range<T, v, vs...>> {
        template <typename F, typename State>
        static constexpr auto foldl_impl(F f, State s, Range<T, v, vs...> xs)
        { return foldl(f, f(s, head(xs)), tail(xs)); }

        template <typename F, typename State>
        static constexpr auto foldr_impl(F f, State s, Range<T, v, vs...> xs)
        { return f(head(xs), foldr(f, s, tail(xs))); }
    };

    template <typename T>
    struct Foldable<Range<T>> {
        template <typename F, typename State>
        static constexpr auto foldl_impl(F f, State s, Range<T> xs)
        { return s; }

        template <typename F, typename State>
        static constexpr auto foldr_impl(F f, State s, Range<T> xs)
        { return s; }
    };

    // comparison
    namespace detail {
        template <bool ...> struct bool_seq;
        template <bool ...b>
        using and_ = Bool<std::is_same<
            bool_seq<b...>, bool_seq<(b, true)...>
        >::value>;

        template <typename X, X ...xs, typename Y, Y ...ys>
        constexpr and_<(xs == ys)...>
        equal_helper(Range<X, xs...> a, Range<Y, ys...> b, Bool<true>)
        { return {}; }

        template <typename X, X ...xs, typename Y, Y ...ys>
        constexpr Bool<false>
        equal_helper(Range<X, xs...>, Range<Y, ys...>, Bool<false>)
        { return {}; }
    }

    template <typename X, X ...xs, typename Y, Y ...ys>
    constexpr auto operator==(Range<X, xs...> a, Range<Y, ys...> b)
        // decltype is required or Clang says
        // error: invalid argument type 'auto' to unary expression
        -> decltype(detail::equal_helper(a, b, Bool<sizeof...(xs) == sizeof...(ys)>{}))
    { return detail::equal_helper(a, b, Bool<sizeof...(xs) == sizeof...(ys)>{}); }

    template <typename X, X ...xs, typename Y, Y ...ys>
    constexpr auto operator!=(Range<X, xs...> a, Range<Y, ys...> b)
        // decltype is required or Clang says
        // error: invalid argument type 'auto' to unary expression
        -> decltype(!(a == b))
    { return !(a == b); }
}} // end namespace boost::hana

#endif // !BOOST_HANA_RANGE_HPP
