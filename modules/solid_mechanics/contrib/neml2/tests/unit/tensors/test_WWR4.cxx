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
#include "neml2/tensors/tensors.h"

using namespace torch::indexing;

using namespace neml2;

TEST_CASE("WWR4", "[tensors]")
{
  torch::manual_seed(42);
  const auto & DTO = default_tensor_options();

  TorchShape B = {5, 3, 1, 2}; // batch shape

  SECTION("class WWR4")
  {
    SECTION("WWR4")
    {
      SECTION("from R4")
      {
        auto u = R4(torch::rand(utils::add_shapes(B, 3, 3, 3, 3), DTO));
        // Symmetrize it
        auto s = (u - u.transpose(0, 1) - u.transpose(2, 3) + u.transpose_minor()) / 4.0;

        // Converting to WWR4 should be equivalent to symmetrization
        REQUIRE(torch::allclose(WWR4(s), WWR4(u)));
      }
    }

    SECTION("identity")
    {
      auto a = WWR4::identity(DTO);
      REQUIRE(torch::allclose(a, torch::eye(3, DTO)));
    }
  }
}
