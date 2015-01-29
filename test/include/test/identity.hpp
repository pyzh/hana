/*
@copyright Louis Dionne 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_TEST_TEST_IDENTITY_HPP
#define BOOST_HANA_TEST_TEST_IDENTITY_HPP

#include <boost/hana/detail/create.hpp>
#include <boost/hana/logical.hpp>

// instances
#include <boost/hana/applicative.hpp>
#include <boost/hana/comparable.hpp>
#include <boost/hana/functor.hpp>
#include <boost/hana/monad.hpp>
#include <boost/hana/orderable.hpp>


namespace boost { namespace hana {
    namespace test {
        struct Identity;

        template <typename T>
        struct _identity {
            T value;
            struct hana { using datatype = Identity; };
        };

        constexpr detail::create<_identity> identity{};
    }

    template <>
    struct equal_impl<test::Identity, test::Identity> {
        template <typename Id1, typename Id2>
        static constexpr auto apply(Id1 x, Id2 y) {
            return equal(x.value, y.value);
        }
    };

    template <>
    struct less_impl<test::Identity, test::Identity> {
        template <typename Id1, typename Id2>
        static constexpr auto apply(Id1 x, Id2 y) {
            return less(x.value, y.value);
        }
    };

    // Define either one to select which MCD is used:
    //  BOOST_HANA_TEST_FUNCTOR_TRANSFORM_MCD
    //  BOOST_HANA_TEST_FUNCTOR_ADJUST_MCD_MCD
    //  BOOST_HANA_TEST_FUNCTOR_APPLICATIVE_MCD
    //
    // If neither is defined, the MCD used is unspecified.
#ifdef BOOST_HANA_TEST_FUNCTOR_TRANSFORM_MCD
    template <>
    struct transform_impl<test::Identity> {
        template <typename Id, typename F>
        static constexpr auto apply(Id self, F f) {
            return test::identity(f(self.value));
        }
    };
#elif defined(BOOST_HANA_TEST_FUNCTOR_ADJUST_MCD_MCD)
    template <>
    struct adjust_impl<test::Identity> {
        template <typename Id, typename P, typename F>
        static constexpr auto apply(Id self, P p, F f) {
            auto x = eval_if(p(self.value),
                [=](auto _) { return _(f)(self.value); },
                [=](auto _) { return self.value; }
            );
            return test::identity(x);
        }
    };
#else
    template <>
    struct transform_impl<test::Identity>
        : Applicative::transform_impl<test::Identity>
    { };
#endif

    template <>
    struct lift_impl<test::Identity> {
        template <typename X>
        static constexpr auto apply(X x)
        { return test::identity(x); }
    };

    template <>
    struct ap_impl<test::Identity> {
        template <typename F, typename X>
        static constexpr auto apply(F f, X x)
        { return test::identity(f.value(x.value)); }
    };

    // Define either one to select which MCD is used:
    //  BOOST_HANA_TEST_MONAD_FLATTEN_MCD
    //  BOOST_HANA_TEST_MONAD_BIND_MCD
    //
    // If neither is defined, the MCD used is unspecified.
#ifdef BOOST_HANA_TEST_MONAD_FLATTEN_MCD
    template <>
    struct flatten_impl<test::Identity> {
        template <typename Id>
        static constexpr auto apply(Id self)
        { return self.value; }
    };
#else
    template <>
    struct bind_impl<test::Identity> {
        template <typename X, typename F>
        static constexpr auto apply(X x, F f)
        { return f(x.value); }
    };
#endif
}} // end namespace boost::hana

#endif // !BOOST_HANA_TEST_TEST_IDENTITY_HPP
