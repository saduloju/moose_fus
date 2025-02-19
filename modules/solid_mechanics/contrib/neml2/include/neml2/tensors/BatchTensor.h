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

#include "neml2/tensors/BatchTensorBase.h"

namespace neml2
{
class BatchTensor : public BatchTensorBase<BatchTensor>
{
public:
  using BatchTensorBase<BatchTensor>::BatchTensorBase;
};

namespace math
{
/**
 * @brief Batched matrix-matrix product
 *
 * The input matrices \p a and \p b must have exactly 2 base dimensions. The batch shapes must
 * broadcast.
 */
BatchTensor bmm(const BatchTensor & a, const BatchTensor & b);

/**
 * @brief Batched matrix-vector product
 *
 * The input tensor \p a must have exactly 2 base dimensions.
 * The input tensor \p v must have exactly 1 base dimension.
 * The batch shapes must broadcast.
 */
BatchTensor bmv(const BatchTensor & a, const BatchTensor & v);

/**
 * @brief Batched vector-vector (dot) product
 *
 * The input tensor \p a must have exactly 1 base dimension.
 * The input tensor \p vbmust have exactly 1 base dimension.
 * The batch shapes must broadcast.
 */
BatchTensor bvv(const BatchTensor & a, const BatchTensor & b);
}

BatchTensor operator*(const BatchTensor & a, const BatchTensor & b);
}
