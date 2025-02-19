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

#include "neml2/tensors/VecBase.h"

namespace neml2
{
class Scalar;
class R2;
class Rot;
class SR2;
class WSR4;

/**
 * @brief A skew rank 2, represented as an axial vector
 *
 * The logical storage space is (3).
 */
class WR2 : public VecBase<WR2>
{
public:
  using VecBase<WR2>::VecBase;

  /// Skew-symmetrize a R2 then fill
  WR2(const R2 & T);

  /// Fill all entries
  [[nodiscard]] static WR2 fill(const Real & a21,
                                const Real & a02,
                                const Real & a10,
                                const torch::TensorOptions & options = default_tensor_options());
  [[nodiscard]] static WR2 fill(const Scalar & a21, const Scalar & a02, const Scalar & a10);

  /// Accessor
  Scalar operator()(TorchSize i, TorchSize j) const;

  /// Exponential map to make this into a rotation (Rot)
  Rot exp() const;

  /// Derivative of the exponential map
  R2 dexp() const;
};

} // namespace neml2
