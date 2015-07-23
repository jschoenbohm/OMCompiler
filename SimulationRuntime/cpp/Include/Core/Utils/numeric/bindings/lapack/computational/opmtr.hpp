//
// Copyright (c) 2002--2010
// Toon Knapen, Karl Meerbergen, Kresimir Fresl,
// Thomas Klimpel and Rutger ter Borg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// THIS FILE IS AUTOMATICALLY GENERATED
// PLEASE DO NOT EDIT!
//

#ifndef BOOST_NUMERIC_BINDINGS_LAPACK_COMPUTATIONAL_OPMTR_HPP
#define BOOST_NUMERIC_BINDINGS_LAPACK_COMPUTATIONAL_OPMTR_HPP

#include <boost/assert.hpp>
#include <Core/Utils/numeric/bindings/begin.hpp>
#include <Core/Utils/numeric/bindings/detail/array.hpp>
#include <Core/Utils/numeric/bindings/detail/if_left.hpp>
#include <Core/Utils/numeric/bindings/is_column_major.hpp>
#include <Core/Utils/numeric/bindings/is_mutable.hpp>
#include <Core/Utils/numeric/bindings/lapack/workspace.hpp>
#include <Core/Utils/numeric/bindings/remove_imaginary.hpp>
#include <Core/Utils/numeric/bindings/size.hpp>
#include <Core/Utils/numeric/bindings/stride.hpp>
#include <Core/Utils/numeric/bindings/uplo_tag.hpp>
#include <Core/Utils/numeric/bindings/value_type.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_const.hpp>

//
// The LAPACK-backend for opmtr is the netlib-compatible backend.
//
#include <Core/Utils/numeric/bindings/lapack/detail/lapack.h>
#include <Core/Utils/numeric/bindings/lapack/detail/lapack_option.hpp>

namespace boost {
namespace numeric {
namespace bindings {
namespace lapack {

//
// The detail namespace contains value-type-overloaded functions that
// dispatch to the appropriate back-end LAPACK-routine.
//
namespace detail {

//
// Overloaded function for dispatching to
// * netlib-compatible LAPACK backend (the default), and
// * float value-type.
//
template< typename Side, typename Trans >
inline std::ptrdiff_t opmtr( const Side, const char uplo, const Trans,
        const fortran_int_t m, const fortran_int_t n, const float* ap,
        const float* tau, float* c, const fortran_int_t ldc, float* work ) {
    fortran_int_t info(0);
    LAPACK_SOPMTR( &lapack_option< Side >::value, &uplo, &lapack_option<
            Trans >::value, &m, &n, ap, tau, c, &ldc, work, &info );
    return info;
}

//
// Overloaded function for dispatching to
// * netlib-compatible LAPACK backend (the default), and
// * double value-type.
//
template< typename Side, typename Trans >
inline std::ptrdiff_t opmtr( const Side, const char uplo, const Trans,
        const fortran_int_t m, const fortran_int_t n, const double* ap,
        const double* tau, double* c, const fortran_int_t ldc, double* work ) {
    fortran_int_t info(0);
    LAPACK_DOPMTR( &lapack_option< Side >::value, &uplo, &lapack_option<
            Trans >::value, &m, &n, ap, tau, c, &ldc, work, &info );
    return info;
}

} // namespace detail

//
// Value-type based template class. Use this class if you need a type
// for dispatching to opmtr.
//
template< typename Value >
struct opmtr_impl {

    typedef Value value_type;
    typedef typename remove_imaginary< Value >::type real_type;

    //
    // Static member function for user-defined workspaces, that
    // * Deduces the required arguments for dispatching to LAPACK, and
    // * Asserts that most arguments make sense.
    //
    template< typename Side, typename VectorAP, typename VectorTAU,
            typename MatrixC, typename WORK >
    static std::ptrdiff_t invoke( const Side side, const char uplo,
            const VectorAP& ap, const VectorTAU& tau, MatrixC& c,
            detail::workspace1< WORK > work ) {
        namespace bindings = ::boost::numeric::bindings;
        typedef tag::column_major order;
        typedef typename result_of::trans_tag< VectorAP, order >::type trans;
        BOOST_STATIC_ASSERT( (bindings::is_column_major< MatrixC >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< VectorAP >::type >::type,
                typename remove_const< typename bindings::value_type<
                VectorTAU >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (boost::is_same< typename remove_const<
                typename bindings::value_type< VectorAP >::type >::type,
                typename remove_const< typename bindings::value_type<
                MatrixC >::type >::type >::value) );
        BOOST_STATIC_ASSERT( (bindings::is_mutable< MatrixC >::value) );
        BOOST_ASSERT( bindings::size(work.select(real_type())) >=
                min_size_work( side, bindings::size_row(c),
                bindings::size_column(c) ));
        BOOST_ASSERT( bindings::size_column(c) >= 0 );
        BOOST_ASSERT( bindings::size_minor(c) == 1 ||
                bindings::stride_minor(c) == 1 );
        BOOST_ASSERT( bindings::size_row(c) >= 0 );
        BOOST_ASSERT( bindings::stride_major(c) >= std::max< std::ptrdiff_t >(1,
                bindings::size_row(c)) );
        return detail::opmtr( side, uplo, trans(), bindings::size_row(c),
                bindings::size_column(c), bindings::begin_value(ap),
                bindings::begin_value(tau), bindings::begin_value(c),
                bindings::stride_major(c),
                bindings::begin_value(work.select(real_type())) );
    }

    //
    // Static member function that
    // * Figures out the minimal workspace requirements, and passes
    //   the results to the user-defined workspace overload of the
    //   invoke static member function
    // * Enables the unblocked algorithm (BLAS level 2)
    //
    template< typename Side, typename VectorAP, typename VectorTAU,
            typename MatrixC >
    static std::ptrdiff_t invoke( const Side side, const char uplo,
            const VectorAP& ap, const VectorTAU& tau, MatrixC& c,
            minimal_workspace ) {
        namespace bindings = ::boost::numeric::bindings;
        typedef tag::column_major order;
        typedef typename result_of::trans_tag< VectorAP, order >::type trans;
        bindings::detail::array< real_type > tmp_work( min_size_work( side,
                bindings::size_row(c), bindings::size_column(c) ) );
        return invoke( side, uplo, ap, tau, c, workspace( tmp_work ) );
    }

    //
    // Static member function that
    // * Figures out the optimal workspace requirements, and passes
    //   the results to the user-defined workspace overload of the
    //   invoke static member
    // * Enables the blocked algorithm (BLAS level 3)
    //
    template< typename Side, typename VectorAP, typename VectorTAU,
            typename MatrixC >
    static std::ptrdiff_t invoke( const Side side, const char uplo,
            const VectorAP& ap, const VectorTAU& tau, MatrixC& c,
            optimal_workspace ) {
        namespace bindings = ::boost::numeric::bindings;
        typedef tag::column_major order;
        typedef typename result_of::trans_tag< VectorAP, order >::type trans;
        return invoke( side, uplo, ap, tau, c, minimal_workspace() );
    }

    //
    // Static member function that returns the minimum size of
    // workspace-array work.
    //
    template< typename Side >
    static std::ptrdiff_t min_size_work( const Side side,
            const std::ptrdiff_t m, const std::ptrdiff_t n ) {
        return std::max< std::ptrdiff_t >( 1, bindings::detail::if_left( side,
                n, m ) );
    }
};


//
// Functions for direct use. These functions are overloaded for temporaries,
// so that wrapped types can still be passed and used for write-access. In
// addition, if applicable, they are overloaded for user-defined workspaces.
// Calls to these functions are passed to the opmtr_impl classes. In the
// documentation, most overloads are collapsed to avoid a large number of
// prototypes which are very similar.
//

//
// Overloaded function for opmtr. Its overload differs for
// * User-defined workspace
//
template< typename Side, typename VectorAP, typename VectorTAU,
        typename MatrixC, typename Workspace >
inline typename boost::enable_if< detail::is_workspace< Workspace >,
        std::ptrdiff_t >::type
opmtr( const Side side, const char uplo, const VectorAP& ap,
        const VectorTAU& tau, MatrixC& c, Workspace work ) {
    return opmtr_impl< typename bindings::value_type<
            VectorAP >::type >::invoke( side, uplo, ap, tau, c, work );
}

//
// Overloaded function for opmtr. Its overload differs for
// * Default workspace-type (optimal)
//
template< typename Side, typename VectorAP, typename VectorTAU,
        typename MatrixC >
inline typename boost::disable_if< detail::is_workspace< MatrixC >,
        std::ptrdiff_t >::type
opmtr( const Side side, const char uplo, const VectorAP& ap,
        const VectorTAU& tau, MatrixC& c ) {
    return opmtr_impl< typename bindings::value_type<
            VectorAP >::type >::invoke( side, uplo, ap, tau, c,
            optimal_workspace() );
}

} // namespace lapack
} // namespace bindings
} // namespace numeric
} // namespace boost

#endif
