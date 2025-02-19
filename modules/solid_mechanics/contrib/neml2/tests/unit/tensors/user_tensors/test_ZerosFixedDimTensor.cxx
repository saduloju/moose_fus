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

#include <catch2/catch.hpp>

#include "utils.h"
#include "neml2/tensors/user_tensors/ZerosFixedDimTensor.h"
#include "neml2/tensors/tensors.h"

using namespace neml2;

#define test_ZerosFixedDimTensor(tensor_type, tensor_name, batch_shape)                            \
  SECTION("Zeros" #tensor_type)                                                                    \
  {                                                                                                \
    const auto & tensor_name = Factory::get_object<tensor_type>("Tensors", #tensor_name);          \
    REQUIRE(tensor_name.batch_sizes() == batch_shape);                                             \
    REQUIRE(tensor_name.base_sizes() == tensor_type::const_base_sizes);                            \
    REQUIRE(                                                                                       \
        torch::allclose(tensor_name, tensor_type::zeros(batch_shape, default_tensor_options())));  \
  }                                                                                                \
  static_assert(true)

TEST_CASE("ZerosFixedDimTensor", "[tensors/user_tensors]")
{
  load_model("unit/tensors/user_tensors/test_ZerosFixedDimTensor.i");

  TorchShape B{2, 1};

  test_ZerosFixedDimTensor(Scalar, a, B);
  test_ZerosFixedDimTensor(Vec, b, B);
  test_ZerosFixedDimTensor(Rot, c, B);
  test_ZerosFixedDimTensor(R2, d, B);
  test_ZerosFixedDimTensor(SR2, e, B);
  test_ZerosFixedDimTensor(R3, f, B);
  test_ZerosFixedDimTensor(SFR3, g, B);
  test_ZerosFixedDimTensor(R4, h, B);
  test_ZerosFixedDimTensor(SSR4, i, B);
  test_ZerosFixedDimTensor(R5, j, B);
  test_ZerosFixedDimTensor(SSFR5, k, B);
}
