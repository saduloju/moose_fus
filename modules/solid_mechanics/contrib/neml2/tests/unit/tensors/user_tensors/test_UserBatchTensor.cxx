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
#include "neml2/tensors/user_tensors/UserBatchTensor.h"

using namespace neml2;

TEST_CASE("UserBatchTensor", "[tensors/user_tensors]")
{
  SECTION("load and reshape correctly")
  {
    load_model("unit/tensors/user_tensors/test_UserBatchTensor.i");

    const auto & a = Factory::get_object<BatchTensor>("Tensors", "a");
    const auto a_correct = torch::tensor({{{1, 2, 3}, {4, 5, 6}}, {{7, 8, 9}, {10, 11, 12}}},
                                         default_tensor_options());
    REQUIRE(torch::allclose(a, a_correct));
    REQUIRE(a.batch_sizes() == TorchShape{2});
    REQUIRE(a.base_sizes() == TorchShape{2, 3});

    const auto & b = Factory::get_object<BatchTensor>("Tensors", "b");
    const auto b_correct = torch::tensor({{{1, 2, 3}, {4, 5, 6}}, {{7, 8, 9}, {10, 11, 12}}},
                                         default_tensor_options());
    REQUIRE(torch::allclose(b, b_correct));
    REQUIRE(b.batch_sizes() == TorchShape{});
    REQUIRE(b.base_sizes() == TorchShape{2, 2, 3});

    const auto & c = Factory::get_object<BatchTensor>("Tensors", "c");
    const auto c_correct = torch::tensor({{{1, 2, 3}, {4, 5, 6}}, {{7, 8, 9}, {10, 11, 12}}},
                                         default_tensor_options());
    REQUIRE(torch::allclose(c, c_correct));
    REQUIRE(c.batch_sizes() == TorchShape{2, 2, 3});
    REQUIRE(c.base_sizes() == TorchShape{});

    const auto & d = Factory::get_object<BatchTensor>("Tensors", "d");
    const auto d_correct =
        torch::tensor({{{1, 2, 3}, {4, 5, 6}}, {{1, 2, 3}, {4, 5, 6}}}, default_tensor_options());
    REQUIRE(torch::allclose(d, d_correct));
    REQUIRE(d.batch_sizes() == TorchShape{2});
    REQUIRE(d.base_sizes() == TorchShape{2, 3});
  }

  SECTION("error on invalid shape")
  {
    load_model("unit/tensors/user_tensors/test_UserBatchTensor_error.i");

    REQUIRE_THROWS_WITH(
        Factory::get_object_ptr<BatchTensor>("Tensors", "a"),
        Catch::Matchers::Contains("Number of values 1 must equal to either the "
                                  "base storage size 6 or the total storage size 12"));
  }
}
