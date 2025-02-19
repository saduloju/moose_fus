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
class R4;

/**
 * @brief The (logical) symmetric fourth order tensor, with skew symmetry in the first two
 * dimensions and last two dimensions
 *
 * Mandel notation/reduced skew is used, and so the logical storage space is (3, 3).
 */
class WWR4 : public FixedDimTensor<WWR4, 3, 3>
{
public:
  using FixedDimTensor<WWR4, 3, 3>::FixedDimTensor;
  WWR4(const R4 & F);

  /// Create the identity tensor \f$\delta_{ij}\delta_{kl}\f$
  static WWR4 identity(const torch::TensorOptions & options = default_tensor_options());
};

} // namespace neml2
