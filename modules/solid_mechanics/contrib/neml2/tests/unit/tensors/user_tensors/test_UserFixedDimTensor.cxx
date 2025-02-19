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
#include "neml2/tensors/user_tensors/UserFixedDimTensor.h"

using namespace neml2;

TEST_CASE("UserFixedDimTensor", "[tensors/user_tensors]")
{
  SECTION("load and reshape correctly")
  {
    load_model("unit/tensors/user_tensors/test_UserFixedDimTensor.i");

    const auto & user_Scalar = Factory::get_object<Scalar>("Tensors", "Scalar");
    REQUIRE(user_Scalar.batch_sizes() == TorchShape{3, 2});

    const auto & user_Vec = Factory::get_object<Vec>("Tensors", "Vec");
    REQUIRE(user_Vec.batch_sizes() == TorchShape{3, 2});

    const auto & user_Rot = Factory::get_object<Rot>("Tensors", "Rot");
    REQUIRE(user_Rot.batch_sizes() == TorchShape{3, 2});

    const auto & user_R2 = Factory::get_object<R2>("Tensors", "R2");
    REQUIRE(user_R2.batch_sizes() == TorchShape{3, 2});

    const auto & user_SR2 = Factory::get_object<SR2>("Tensors", "SR2");
    REQUIRE(user_SR2.batch_sizes() == TorchShape{3, 2});

    const auto & user_R3 = Factory::get_object<R3>("Tensors", "R3");
    REQUIRE(user_R3.batch_sizes() == TorchShape{3, 2});

    const auto & user_SFR3 = Factory::get_object<SFR3>("Tensors", "SFR3");
    REQUIRE(user_SFR3.batch_sizes() == TorchShape{3, 2});

    const auto & user_R4 = Factory::get_object<R4>("Tensors", "R4");
    REQUIRE(user_R4.batch_sizes() == TorchShape{3, 2});

    const auto & user_SSR4 = Factory::get_object<SSR4>("Tensors", "SSR4");
    REQUIRE(user_SSR4.batch_sizes() == TorchShape{3, 2});

    const auto & user_R5 = Factory::get_object<R5>("Tensors", "R5");
    REQUIRE(user_R5.batch_sizes() == TorchShape{3, 2});

    const auto & user_SSFR5 = Factory::get_object<SSFR5>("Tensors", "SSFR5");
    REQUIRE(user_SSFR5.batch_sizes() == TorchShape{3, 2});
  }
}
