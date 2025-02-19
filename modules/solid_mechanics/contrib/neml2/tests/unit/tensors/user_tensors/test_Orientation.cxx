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
#include "neml2/tensors/user_tensors/Orientation.h"

using namespace neml2;

TEST_CASE("Orientation", "[tensors/user_tensors]")
{
  load_model("unit/tensors/user_tensors/test_Orientation.i");

  SECTION("Kocks, radians")
  {
    const auto & auto_1 = Factory::get_object<Rot>("Tensors", "kocks_rad");
    auto correct_1 = Rot(torch::tensor({{-0.08900237, -0.01804167, -0.90057498},
                                        {-0.06969849, -0.00174283, -0.9252516},
                                        {0.0, 0.0, 0.0}},
                                       default_tensor_options()));
    REQUIRE(torch::allclose(auto_1, correct_1));
  }
  SECTION("Kocks, degrees")
  {
    const auto & auto_1 = Factory::get_object<Rot>("Tensors", "kocks_deg");
    auto correct_1 = Rot(torch::tensor({{-0.17445754, -0.30216947, -0.52337294},
                                        {-0.17386297, 0.01368329, -0.89308248},
                                        {0.000000, 0.000000, 0.00000}},
                                       default_tensor_options()));
    REQUIRE(torch::allclose(auto_1, correct_1));
  }

  SECTION("Bunge, radians")
  {
    const auto & auto_1 = Factory::get_object<Rot>("Tensors", "bunge_rad");
    auto correct_1 = Rot(torch::tensor({{-0.05029174, 0.00504576, -0.10008088},
                                        {-0.03741789, 0.0028116, -0.01248295},
                                        {-0.000000, 0.000000, 0.000000}},
                                       default_tensor_options()));
    REQUIRE(torch::allclose(auto_1, correct_1, 1e-3, 1e-4));
  }
  SECTION("Bunge, degrees")
  {
    const auto & auto_1 = Factory::get_object<Rot>("Tensors", "bunge_deg");
    auto correct_1 = Rot(torch::tensor({{-0.30216947, 0.17445754, -0.52337294},
                                        {-0.09599247, 0.00924294, 0.03892541},
                                        {-0.000000, 0.000000, -0.000000}},
                                       default_tensor_options()));
    REQUIRE(torch::allclose(auto_1, correct_1, 1e-3, 1e-4));
  }

  SECTION("Roe, radians")
  {
    const auto & auto_1 = Factory::get_object<Rot>("Tensors", "roe_rad");
    auto correct_1 = Rot(torch::tensor({{-0.00504576, -0.05029174, -0.10008088},
                                        {-0.0028116, -0.03741789, -0.01248295},
                                        {0.000000, 0.000000, 0.000000}},
                                       default_tensor_options()));
    REQUIRE(torch::allclose(auto_1, correct_1, 1e-3, 1e-4));
  }
  SECTION("Roe, degrees")
  {
    const auto & auto_1 = Factory::get_object<Rot>("Tensors", "roe_deg");
    auto correct_1 = Rot(torch::tensor({{-0.17445754, -0.30216947, -0.52337294},
                                        {-0.00924294, -0.09599247, 0.03892541},
                                        {0.000000, -0.000000, -0.000000}},
                                       default_tensor_options()));
    REQUIRE(torch::allclose(auto_1, correct_1, 1e-3, 1e-4));
  }
}
