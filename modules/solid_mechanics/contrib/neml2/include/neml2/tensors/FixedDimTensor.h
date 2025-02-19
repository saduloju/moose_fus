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

#include "neml2/tensors/BatchTensor.h"

namespace neml2
{
/**
 * @brief FixedDimTensor inherits from BatchTensorBase and additionally templates on the base shape.
 *
 * @tparam S Base shape
 */
template <class Derived, TorchSize... S>
class FixedDimTensor : public BatchTensorBase<Derived>
{
public:
  /// The base shape
  static inline const TorchShape const_base_sizes = {S...};

  /// The base dim
  static constexpr TorchSize const_base_dim = sizeof...(S);

  /// The base storage
  static inline const TorchSize const_base_storage = utils::storage_size({S...});

  /// Default constructor
  FixedDimTensor() = default;

  /// Construct from another torch::Tensor given batch dimension
  explicit FixedDimTensor(const torch::Tensor & tensor, TorchSize batch_dim);

  /// Construct from another torch::Tensor and infer batch dimension
  FixedDimTensor(const torch::Tensor & tensor);

  /// Implicit conversion to a BatchTensor and loses information on the fixed base shape
  operator BatchTensor() const;

  /// Unbatched empty tensor
  [[nodiscard]] static Derived
  empty(const torch::TensorOptions & options = default_tensor_options());
  /// Empty tensor given batch shape
  [[nodiscard]] static Derived
  empty(TorchShapeRef batch_shape, const torch::TensorOptions & options = default_tensor_options());
  /// Unbatched zero tensor
  [[nodiscard]] static Derived
  zeros(const torch::TensorOptions & options = default_tensor_options());
  /// Zero tensor given batch shape
  [[nodiscard]] static Derived
  zeros(TorchShapeRef batch_shape, const torch::TensorOptions & options = default_tensor_options());
  /// Unbatched unit tensor
  [[nodiscard]] static Derived
  ones(const torch::TensorOptions & options = default_tensor_options());
  /// Unit tensor given batch shape
  [[nodiscard]] static Derived
  ones(TorchShapeRef batch_shape, const torch::TensorOptions & options = default_tensor_options());
  /// Unbatched tensor filled with a given value given base shape
  [[nodiscard]] static Derived
  full(Real init, const torch::TensorOptions & options = default_tensor_options());
  /// Full tensor given batch shape
  [[nodiscard]] static Derived
  full(TorchShapeRef batch_shape,
       Real init,
       const torch::TensorOptions & options = default_tensor_options());
};
} // namespace neml2
