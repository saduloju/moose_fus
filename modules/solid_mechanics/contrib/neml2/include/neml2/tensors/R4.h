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
class Scalar;
class SSR4;
class R5;
class Rot;
class WWR4;

/**
 * @brief The (logical) full fourth order tensor.
 *
 * The logical storage space is (3, 3, 3, 3).
 */
class R4 : public FixedDimTensor<R4, 3, 3, 3, 3>
{
public:
  using FixedDimTensor<R4, 3, 3, 3, 3>::FixedDimTensor;

  R4(const SSR4 & T);

  R4(const WWR4 & T);

  /// Rotate
  R4 rotate(const Rot & r) const;

  /// Derivative of the rotated tensor w.r.t. the Rodrigues vector
  R5 drotate(const Rot & r) const;

  /// Accessor
  Scalar operator()(TorchSize i, TorchSize j, TorchSize k, TorchSize l) const;

  /// Arbitrary transpose two dimensions
  R4 transpose(TorchSize d1, TorchSize d2) const;

  /// Transpose minor axes
  R4 transpose_minor() const;

  /// Transpose major axes
  R4 transpose_major() const;
};
} // namespace neml2
