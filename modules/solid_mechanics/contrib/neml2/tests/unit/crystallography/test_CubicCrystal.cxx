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

#include <filesystem>

#include "utils.h"
#include "neml2/base/Factory.h"
#include "neml2/models/crystallography/CubicCrystal.h"
#include "neml2/tensors/tensors.h"

using namespace neml2;
using namespace neml2::crystallography;
namespace fs = std::filesystem;

TEST_CASE("CubicCrystal", "[crystallography]")
{
  torch::manual_seed(42);
  const auto & DTO = default_tensor_options();

  // Load all the models in
  Factory::clear();
  load_model(fs::absolute("unit/crystallography/test_CubicCrystal.i"));

  SECTION("Simple FCC cubic model, defined with the simple specialized CubicCrystal class")
  {
    auto & model = Factory::get_object<CubicCrystal>("Data", "cube");

    SECTION("Lattice vectors are okay")
    {
      REQUIRE(torch::allclose(model.a1(), Vec::fill(1.2, 0.0, 0.0, DTO)));
      REQUIRE(torch::allclose(model.a2(), Vec::fill(0.0, 1.2, 0.0, DTO)));
      REQUIRE(torch::allclose(model.a3(), Vec::fill(0.0, 0.0, 1.2, DTO)));
    }

    SECTION("Reciprocol lattice vectors are okay")
    {
      REQUIRE(torch::allclose(model.b1(), Vec::fill(1.0 / 1.2, 0.0, 0.0, DTO)));
      REQUIRE(torch::allclose(model.b2(), Vec::fill(0.0, 1.0 / 1.2, 0.0, DTO)));
      REQUIRE(torch::allclose(model.b3(), Vec::fill(0.0, 0.0, 1.0 / 1.2, DTO)));
    }

    SECTION("Slip slicing")
    {
      // Come back to this later when we have examples with more than one slip system
      REQUIRE(torch::allclose(model.M(), model.slip_slice(model.M(), 0)));
    }

    SECTION("Slip crystallography")
    {
      SECTION("Number of systems correct")
      {
        REQUIRE(model.nslip() == 12);
        REQUIRE(model.nslip_groups() == 1);
        REQUIRE(model.nslip_in_group(0) == 12);
      }
      SECTION("Burgers vector lengths")
      {
        REQUIRE(torch::allclose(model.burgers(), torch::tensor(1.2 / std::sqrt(2) * 2, DTO)));
      }
      SECTION("Slip directions are unit vectors")
      {
        REQUIRE(torch::allclose(model.cartesian_slip_directions().norm(), torch::tensor(1.0, DTO)));
      }
      SECTION("Slip planes are unit vectors")
      {
        REQUIRE(torch::allclose(model.cartesian_slip_planes().norm(), torch::tensor(1.0, DTO)));
      }
      SECTION("Slip directions and planes are orthogonal")
      {
        REQUIRE(
            torch::allclose(model.cartesian_slip_directions().dot(model.cartesian_slip_planes()),
                            torch::tensor(0.0, DTO)));
      }
      SECTION("Schmid tensors")
      {
        R2 should = model.cartesian_slip_directions().outer(model.cartesian_slip_planes());
        SECTION("Full tensors") { REQUIRE(torch::allclose(should, model.A())); }
        SECTION("Symmetric tensors") { REQUIRE(torch::allclose(SR2(should), model.M())); }
        SECTION("Skew symmetric tensors") { REQUIRE(torch::allclose(WR2(should), model.W())); }
      }
    }
  }
}
