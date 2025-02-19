// Copyright 2023, UChicago Argonne, LLC
// All Rights Reserved
// Software Name: NEML2 -- the New Engineering material Model Library, version 2
// By: Argonne National Laboratory
// OPEN SOURCE LICENSE (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include "neml2/misc/types.h"
#include "neml2/misc/error.h"

namespace neml2
{
/**
 * Two tensors are said to be broadcastable if
 * 1. Base shapes are the same
 * 2. Batch shapes are broadcastable (see sizes_broadcastable)
 */
template <class... T>
bool broadcastable(T &&... tensors);

/**
 * @brief The batch dimension after broadcasting
 *
 * This should be as simple as the maximum batch_dim() among all arguments.
 */
template <class... T>
TorchSize broadcast_batch_dim(T &&...);

/**
 * @brief A helper function to assert that all tensors are broadcastable
 *
 * In most cases, this assertion is necessary as libTorch will raise runtime_errors if things go
 * wrong. Therefore, this function is just so that we can detect errors before libTorch does and
 * emit some more mearningful error messages within the NEML2 context.
 */
template <class... T>
void neml_assert_broadcastable(T &&...);

/**
 * @brief A helper function to assert (in Debug mode) that all tensors are broadcastable
 *
 * In most cases, this assertion is necessary as libTorch will raise runtime_errors if things go
 * wrong. Therefore, this function is just so that we can detect errors before libTorch does and
 * emit some more mearningful error messages within the NEML2 context.
 */
template <class... T>
void neml_assert_broadcastable_dbg(T &&...);

/**
 * @brief A helper function to assert that all tensors are batch-broadcastable
 *
 * In most cases, this assertion is necessary as libTorch will raise runtime_errors if things go
 * wrong. Therefore, this function is just so that we can detect errors before libTorch does and
 * emit some more mearningful error messages within the NEML2 context.
 */
template <class... T>
void neml_assert_batch_broadcastable(T &&...);

/**
 * @brief A helper function to assert that (in Debug mode) all tensors are batch-broadcastable
 *
 * In most cases, this assertion is necessary as libTorch will raise runtime_errors if things go
 * wrong. Therefore, this function is just so that we can detect errors before libTorch does and
 * emit some more mearningful error messages within the NEML2 context.
 */
template <class... T>
void neml_assert_batch_broadcastable_dbg(T &&...);

namespace utils
{
/// Check if all shapes are the *same*.
template <class... T>
bool sizes_same(T &&... shapes);

/**
 * @brief Check if the shapes are broadcastable.
 *
 * Shapes are said to be broadcastable if, starting from the trailing dimension and
 * iterating backward, the dimension sizes either are equal, one of them is 1, or one of them does
 * not exist.
 */
template <class... T>
bool sizes_broadcastable(T &&... shapes);

/**
 * @brief Return the broadcast shape of all the shapes.
 */
template <class... T>
TorchShape broadcast_sizes(T &&... shapes);

/**
 * @brief The flattened storage size of a tensor with given shape
 *
 * For example,
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
 * storage_size({}) == 1;
 * storage_size({0}) == 0;
 * storage_size({1}) == 1;
 * storage_size({1, 2, 3}) == 6;
 * storage_size({5, 1, 1}) == 5;
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
TorchSize storage_size(TorchShapeRef shape);

template <typename... S>
TorchShape add_shapes(S &&... shape);

/**
 * @brief Pad shape \p s to dimension \p dim by prepending sizes of \p pad.
 *
 * @param s The original shape to pad
 * @param dim The resulting dimension
 * @param pad The values used to pad the shape, default to 1
 * @return TorchShape The padded shape with dimension \p dim
 */
TorchShape pad_prepend(TorchShapeRef s, TorchSize dim, TorchSize pad = 1);

/**
 * @brief Pad shape \p s to dimension \p dim by appending sizes of \p pad.
 *
 * @param s The original shape to pad
 * @param dim The resulting dimension
 * @param pad The values used to pad the shape, default to 1
 * @return TorchShape The padded shape with dimension \p dim
 */
TorchShape pad_append(TorchShapeRef s, TorchSize dim, TorchSize pad = 1);

std::string indentation(int level, int indent = 2);

template <typename T>
std::string stringify(const T & t);

namespace details
{
template <typename... S>
TorchShape add_shapes_impl(TorchShape &, TorchShapeRef, S &&...);

TorchShape add_shapes_impl(TorchShape &);
} // namespace details
} // namespace utils
} // namespace neml2

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

namespace neml2
{
template <class... T>
bool
broadcastable(T &&... tensors)
{
  if (!utils::sizes_same(tensors.base_sizes()...))
    return false;
  return utils::sizes_broadcastable(tensors.batch_sizes()...);
}

template <class... T>
TorchSize
broadcast_batch_dim(T &&... tensor)
{
  return std::max({tensor.batch_dim()...});
}

template <class... T>
void
neml_assert_broadcastable(T &&... tensors)
{
  neml_assert(broadcastable(tensors...),
              "The ",
              sizeof...(tensors),
              " operands are not broadcastable. The batch shapes are ",
              tensors.batch_sizes()...,
              ", and the base shapes are ",
              tensors.base_sizes()...);
}

template <class... T>
void
neml_assert_broadcastable_dbg([[maybe_unused]] T &&... tensors)
{
#ifndef NDEBUG
  neml_assert_dbg(broadcastable(tensors...),
                  "The ",
                  sizeof...(tensors),
                  " operands are not broadcastable. The batch shapes are ",
                  tensors.batch_sizes()...,
                  ", and the base shapes are ",
                  tensors.base_sizes()...);
#endif
}

template <class... T>
void
neml_assert_batch_broadcastable(T &&... tensors)
{
  neml_assert(utils::sizes_broadcastable(tensors.batch_sizes()...),
              "The ",
              sizeof...(tensors),
              " operands are not batch-broadcastable. The batch shapes are ",
              tensors.batch_sizes()...);
}

template <class... T>
void
neml_assert_batch_broadcastable_dbg([[maybe_unused]] T &&... tensors)
{
#ifndef NDEBUG
  neml_assert_dbg(utils::sizes_broadcastable(tensors.batch_sizes()...),
                  "The ",
                  sizeof...(tensors),
                  " operands are not batch-broadcastable. The batch shapes are ",
                  tensors.batch_sizes()...);
#endif
}
namespace utils
{
template <class... T>
bool
sizes_same(T &&... shapes)
{
  auto all_shapes = std::vector<TorchShapeRef>{shapes...};
  for (size_t i = 0; i < all_shapes.size() - 1; i++)
    if (all_shapes[i] != all_shapes[i + 1])
      return false;
  return true;
}

template <class... T>
bool
sizes_broadcastable(T &&... shapes)
{
  auto dim = std::max({shapes.size()...});
  auto all_shapes_padded = std::vector<TorchShape>{pad_prepend(shapes, dim)...};

  for (size_t i = 0; i < dim; i++)
  {
    TorchSize max_sz = 1;
    for (const auto & s : all_shapes_padded)
    {
      if (max_sz == 1)
      {
        neml_assert_dbg(s[i] > 0, "Found a size equal or less than 0.");
        if (s[i] > 1)
          max_sz = s[i];
      }
      else if (s[i] != 1 && s[i] != max_sz)
        return false;
    }
  }

  return true;
}

template <class... T>
TorchShape
broadcast_sizes(T &&... shapes)
{
  neml_assert_dbg(sizes_broadcastable(shapes...), "Shapes not broadcastable: ", shapes...);

  auto dim = std::max({shapes.size()...});
  auto all_shapes_padded = std::vector<TorchShape>{pad_prepend(shapes, dim)...};
  auto bshape = TorchShape(dim, 1);

  for (size_t i = 0; i < dim; i++)
    for (const auto & s : all_shapes_padded)
      if (s[i] > bshape[i])
        bshape[i] = s[i];

  return bshape;
}

template <typename... S>
TorchShape
add_shapes(S &&... shape)
{
  TorchShape net;
  return details::add_shapes_impl(net, std::forward<S>(shape)...);
}

template <typename T>
std::string
stringify(const T & t)
{
  std::ostringstream os;
  os << t;
  return os.str();
}

namespace details
{
template <typename... S>
TorchShape
add_shapes_impl(TorchShape & net, TorchShapeRef s, S &&... rest)
{
  net.insert(net.end(), s.begin(), s.end());
  return add_shapes_impl(net, std::forward<S>(rest)...);
}
} // namespace details
} // namespace utils
} // namespace neml2
