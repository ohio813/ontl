/**\file*********************************************************************
 *                                                                     \brief
 *  26 Numerics library [numerics]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_NUMERICS
#define NTL__STLX_NUMERICS
#pragma once

#include "iterator.hxx"

namespace std
{
 /**\defgroup  lib_numeric ************ 26 Numerics library [numerics]
  *@{
  **/

  // 26.6.1 Accumulate [accumulate]
  template <class InputIterator, class T>
  __forceinline
  T
    accumulate(InputIterator first, InputIterator last, T init)
  {
    while(first != last){
      init = init + *first;
      ++first;
    }
    return init;
  }

  // 26.6.1 Accumulate [accumulate]
  template <class InputIterator, class T, class BinaryOperation>
  __forceinline
  T
    accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op)
  {
    while(first != last){
      init = binary_op(init, *first);
      ++first;
    }
    return init;
  }

  /**@} lib_numeric */
} // namespace std

#endif // NTL__STLX_NUMERICS
