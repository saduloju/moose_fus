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

#include "neml2/tensors/tensors.h"
#include "neml2/tensors/Transformable.h"

using namespace neml2;

TEST_CASE("Symmetry operator definitions", "[tensors]")
{
  torch::manual_seed(42);
  const auto & DTO = default_tensor_options();

  SECTION("definitions")
  {
    SECTION("identity") { REQUIRE(torch::allclose(identity_transform(DTO), R2::identity(DTO))); }
    auto r = Rot::fill(0.1, -0.15, 0.05, DTO);
    SECTION("proper rotation")
    {
      REQUIRE(torch::allclose(proper_rotation_transform(r), r.euler_rodrigues()));
    }
    SECTION("improper rotation")
    {
      R2 ref = R2::identity(DTO) - 2 * (r / r.norm()).outer(r / r.norm());
      REQUIRE(torch::allclose(improper_rotation_transform(r), r.euler_rodrigues() * ref));
      REQUIRE(torch::allclose(improper_rotation_transform(r), ref * r.euler_rodrigues()));
    }
    SECTION("inversion")
    {
      REQUIRE(torch::allclose(inversion_transform(DTO), R2::fill(-1.0, DTO)));
    }
    SECTION("quaternion")
    {
      auto q = Quaternion::fill(-0.30411437, -0.15205718, 0.91234311, 0.22808578, DTO);
      REQUIRE(torch::allclose(transform_from_quaternion(q), q.to_R2()));
    }
  }
}
