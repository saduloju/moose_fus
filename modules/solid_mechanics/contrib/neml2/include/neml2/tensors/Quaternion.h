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

#include "neml2/tensors/FixedDimTensor.h"

namespace neml2
{
class R2;
class Scalar;
/**
 * @brief A batched quaternion
 *
 * Our convention is s q1 q2 q3, i.e. the scalar part first
 *
 * The logical storage space is (4,).
 *
 * Currently we only use this for data storage, so the only
 * method needed is to convert it to an R2
 */
class Quaternion : public FixedDimTensor<Quaternion, 4>
{
public:
  using FixedDimTensor<Quaternion, 4>::FixedDimTensor;

  /// fill with four scalars
  static Quaternion fill(const Scalar & s, const Scalar & q1, const Scalar & q2, const Scalar & q3);

  /// fill with four reals
  static Quaternion fill(const Real & s,
                         const Real & q1,
                         const Real & q2,
                         const Real & q3,
                         const torch::TensorOptions & options = default_tensor_options());

  /// Accessor
  Scalar operator()(TorchSize i) const;

  /// Convert to R2
  R2 to_R2() const;
};
} // namespace neml2
