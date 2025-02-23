//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#ifdef NEML2_ENABLED

#include "neml2/models/Model.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"
#include "SymmetricRankTwoTensor.h"
#include "SymmetricRankFourTensor.h"
#include "MaterialProperty.h"

#endif

#include "InputParameters.h"

class MooseObject;
class Action;

#ifdef NEML2_ENABLED

/**
 * Pretty print the summary of a NEML2 model
 */
namespace neml2
{
std::ostream & operator<<(std::ostream &, const Model &);
}

#endif

namespace NEML2Utils
{

#ifdef NEML2_ENABLED

/**
 * Convert a MOOSE data structure to its NEML2 counterpart
 */
template <typename T>
neml2::BatchTensor toNEML2(const T &);

/**
 * Convert a wrapped (batched) MOOSE data structure to its NEML2 counterpart
 * The wrapper should implement size()
 */
template <typename T>
neml2::BatchTensor toNEML2Batched(const T & data);

// @{ Template specializations
template <>
neml2::BatchTensor toNEML2(const Real & v);
template <>
neml2::BatchTensor toNEML2(const RankTwoTensor & r2t);
template <>
neml2::BatchTensor toNEML2(const std::vector<Real> & v);
// @}

/**
 * Decompose a `std::vector<std::tuple<Args...>>` into a `std::tuple<std::vector<Args>...>`
 * The original data structure (the batched tuple) is what BatchMaterial gathered for us, but it
 * allows for inhomogeneous batches, i.e., the batch data structure could change from batch to
 * batch. The batched tuple is efficient in terms of data gathering, but it is disadvantageous when
 * we want to convert and set all batches at once. This method essentially "homogenizes" the batched
 * tuple to a tuple of batched data structures.
 */
template <typename... Args>
std::tuple<std::vector<Args>...>
homogenizeBatchedTuple(const std::vector<std::tuple<Args...>> & from);

template <size_t I = 0, typename... Args>
void homogenizeBatchedTupleInner(const std::vector<std::tuple<Args...>> & from,
                                 std::tuple<std::vector<Args>...> & to);

/**
 * Convert a NEML2 data structure to its MOOSE counterpart
 */
template <typename T>
T toMOOSE(const neml2::BatchTensor &);

// @{ Template specializations
template <>
SymmetricRankTwoTensor toMOOSE(const neml2::BatchTensor & t);
template <>
std::vector<Real> toMOOSE(const neml2::BatchTensor & t);
template <>
SymmetricRankFourTensor toMOOSE(const neml2::BatchTensor & t);
// @}

/**
 * Convert a MOOSE data structure to its NEML2 counterpart and copy the values into a NEML2
 * LabeledVector
 */
template <size_t I = 0, typename T, typename... Ts>
void set(neml2::LabeledVector & v,
         const std::vector<neml2::VariableName> & indices,
         const T * t0,
         const Ts *... t);

/**
 * Convert a wrapped (batched) MOOSE data structure to its NEML2 counterpart and copy the values
 * into a NEML2 LabeledVector
 */
template <size_t I = 0, typename T, typename... Ts>
void setBatched(neml2::LabeledVector & v,
                const std::vector<neml2::VariableName> & indices,
                const T * t0,
                const Ts *... t);

static std::string NEML2_help_message = R""""(
==============================================================================
To debug NEML2 related issues:
1. Build and run MOOSE in dbg mode.
2. Re-run the simulation using the dbg executable, and often times
   NEML2 will provide a more helpful error message.
3. If the error message is not helpful, or if there is still no error message,
   run the simulation through a debugger: See
   https://mooseframework.inl.gov/application_development/debugging.html
4. If the issue is due to a NEML2 bug, feel free to report it at
   https://github.com/reverendbedford/neml2/issues
==============================================================================
)"""";

////////////////////////////////////////////////////////////////////////////////
// Implementations
////////////////////////////////////////////////////////////////////////////////
template <typename T>
neml2::BatchTensor
toNEML2Batched(const T & data)
{
  std::vector<torch::Tensor> res(data.size());
  for (const auto i : index_range(data))
    res[i] = toNEML2<typename T::value_type>(data[i]);
  return neml2::BatchTensor(torch::stack(res, 0), 1);
}

template <typename... Args>
std::tuple<std::vector<Args>...>
homogenizeBatchedTuple(const std::vector<std::tuple<Args...>> & from)
{
  std::tuple<std::vector<Args>...> to;
  homogenizeBatchedTupleInner(from, to);
  return to;
}

template <size_t I, typename... Args>
void
homogenizeBatchedTupleInner(const std::vector<std::tuple<Args...>> & from,
                            std::tuple<std::vector<Args>...> & to)
{
  typedef typename std::tuple_element<I, std::tuple<Args...>>::type Arg;
  std::vector<Arg> toi;
  std::transform(from.cbegin(),
                 from.cend(),
                 std::back_inserter(toi),
                 [](const auto & b) { return std::get<I>(b); });
  std::get<I>(to) = toi;

  // recursively act on the rest of the args
  if constexpr ((I + 1) < sizeof...(Args))
    homogenizeBatchedTupleInner<I + 1>(from, to);
}

template <size_t I, typename T, typename... Ts>
void
set(neml2::LabeledVector & v,
    const std::vector<neml2::VariableName> & indices,
    const T * t0,
    const Ts *... t)
{
  if (t0)
    v.set(toNEML2(*t0), indices[I]);

  // Recursively act on the rest of the data
  // The compiler should be able to easily deduce the rest of the template parameters...
  if constexpr (sizeof...(Ts) > 1)
    set<I + 1>(v, indices, t...);
}

template <size_t I, typename T, typename... Ts>
void
setBatched(neml2::LabeledVector & v,
           const std::vector<neml2::VariableName> & indices,
           const T * t0,
           const Ts *... t)
{
  if (t0)
    v.set(toNEML2Batched(*t0), indices[I]);

  // recursively act on the rest of the data
  // The compiler should be able to easily deduce the rest of the template parameters...
  if constexpr (sizeof...(Ts) > 0)
    setBatched<I + 1>(v, indices, t...);
}

#endif // NEML2_ENABLED

/**
 * Augment class description if NEML2 is not enabled
 */
void addClassDescription(InputParameters & params, const std::string & desc);

/**
 * Error message if NEML2 is not enabled
 */
void libraryNotEnabledError(const InputParameters & params);

} // namespace NEML2Utils
