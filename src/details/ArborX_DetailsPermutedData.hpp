/****************************************************************************
 * Copyright (c) 2017-2022 by the ArborX authors                            *
 * All rights reserved.                                                     *
 *                                                                          *
 * This file is part of the ArborX library. ArborX is                       *
 * distributed under a BSD 3-clause license. For the licensing terms see    *
 * the LICENSE file in the top-level directory.                             *
 *                                                                          *
 * SPDX-License-Identifier: BSD-3-Clause                                    *
 ****************************************************************************/

#ifndef ARBORX_DETAILS_PERMUTED_DATA_HPP
#define ARBORX_DETAILS_PERMUTED_DATA_HPP

#include <ArborX_AccessTraits.hpp>

namespace ArborX
{

namespace Details
{

template <typename Data, typename Permute, bool AttachIndices = false>
struct PermutedData
{
  using memory_space = typename Data::memory_space;
  using value_type = typename Data::value_type;

  Data _data;
  Permute _permute;

  KOKKOS_FUNCTION decltype(auto) operator()(int i) const
  {
    return _data(_permute(i));
  }
  KOKKOS_FUNCTION auto size() const { return _data.size(); }
};

} // namespace Details

template <typename Predicates, typename Permute, bool AttachIndices>
struct AccessTraits<Details::PermutedData<Predicates, Permute, AttachIndices>,
                    PredicatesTag>
{
  using PermutedPredicates =
      Details::PermutedData<Predicates, Permute, AttachIndices>;

  using memory_space = typename Predicates::memory_space;

  KOKKOS_FUNCTION static std::size_t
  size(PermutedPredicates const &permuted_predicates)
  {
    return permuted_predicates._data.size();
  }

  template <bool _Attach = AttachIndices>
  KOKKOS_FUNCTION static auto get(PermutedPredicates const &permuted_predicates,
                                  std::enable_if_t<_Attach, std::size_t> index)
  {
    auto const permuted_index = permuted_predicates._permute(index);
    return attach(permuted_predicates._data(permuted_index), (int)index);
  }

  template <bool _Attach = AttachIndices>
  KOKKOS_FUNCTION static decltype(auto)
  get(PermutedPredicates const &permuted_predicates,
      std::enable_if_t<!_Attach, std::size_t> index)
  {
    auto const permuted_index = permuted_predicates._permute(index);
    return permuted_predicates._data(permuted_index);
  }
};

} // namespace ArborX

#endif
