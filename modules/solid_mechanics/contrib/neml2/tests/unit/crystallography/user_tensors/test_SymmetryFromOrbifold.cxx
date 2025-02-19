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
#include "neml2/models/crystallography/user_tensors/SymmetryFromOrbifold.h"
#include "neml2/tensors/tensors.h"

using namespace neml2;
using namespace neml2::crystallography;
namespace fs = std::filesystem;

TEST_CASE("SymmetryFromOrbifold", "[tensors]")
{
  torch::manual_seed(42);
  const auto & DTO = default_tensor_options();

  Real a = std::sqrt(3.0) / 2.0;
  Real h = 0.5;

  // All the correct matrices from Kocks, Tome, and Wenk
  // (well after you fix the typos in my edition)
  auto ktw_tetra = torch::tensor({{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
                                  {{-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}},
                                  {{1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 0.0, -1.0}},
                                  {{-1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}},
                                  {{0.0, 1.0, 0.0}, {-1.0, 0.0, 0.0}, {0.0, 0.0, 1.0}},
                                  {{0.0, -1.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 1.0}},
                                  {{0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, -1.0}},
                                  {{0.0, -1.0, 0.0}, {-1.0, 0.0, 0.0}, {0.0, 0.0, -1.0}}},
                                 DTO);

  auto ktw_hexagonal = torch::tensor({{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
                                      {{-h, a, 0.0}, {-a, -h, 0.0}, {0.0, 0.0, 1.0}},
                                      {{-h, -a, 0.0}, {a, -h, 0.0}, {0.0, 0.0, 1.0}},
                                      {{h, a, 0.0}, {-a, h, 0.0}, {0.0, 0.0, 1.0}},
                                      {{-1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}},
                                      {{h, -a, 0.0}, {a, h, 0.0}, {0.0, 0.0, 1.0}},
                                      {{-h, -a, 0.0}, {-a, h, 0.0}, {0.0, 0.0, -1.0}},
                                      {{1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 0.0, -1.0}},
                                      {{-h, a, 0.0}, {a, h, 0.0}, {0.0, 0.0, -1.0}},
                                      {{h, a, 0.0}, {a, -h, 0.0}, {0.0, 0.0, -1.0}},
                                      {{-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}},
                                      {{h, -a, 0.0}, {-a, -h, 0.0}, {0.0, 0.0, -1.0}}},
                                     DTO);

  auto ktw_cubic = torch::tensor({{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
                                  {{0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}},
                                  {{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}},
                                  {{0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {-1.0, 0.0, 0.0}},
                                  {{0.0, -1.0, 0.0}, {0.0, 0.0, -1.0}, {1.0, 0.0, 0.0}},
                                  {{0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}, {-1.0, 0.0, 0.0}},
                                  {{0.0, 0.0, -1.0}, {1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}},
                                  {{0.0, 0.0, -1.0}, {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}},
                                  {{0.0, 0.0, 1.0}, {-1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}},
                                  {{-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}},
                                  {{-1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}},
                                  {{1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 0.0, -1.0}},
                                  {{0.0, 0.0, -1.0}, {0.0, -1.0, 0.0}, {-1.0, 0.0, 0.0}},
                                  {{0.0, 0.0, 1.0}, {0.0, -1.0, 0.0}, {1.0, 0.0, 0.0}},
                                  {{0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}, {-1.0, 0.0, 0.0}},
                                  {{0.0, 0.0, -1.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}},
                                  {{-1.0, 0.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, -1.0, 0.0}},
                                  {{1.0, 0.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, 1.0, 0.0}},
                                  {{1.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, -1.0, 0.0}},
                                  {{-1.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}},
                                  {{0.0, -1.0, 0.0}, {-1.0, 0.0, 0.0}, {0.0, 0.0, -1.0}},
                                  {{0.0, 1.0, 0.0}, {-1.0, 0.0, 0.0}, {0.0, 0.0, 1.0}},
                                  {{0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, -1.0}},
                                  {{0.0, -1.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 1.0}}},
                                 DTO);

  using namespace torch::indexing;

  // Relates each crystal class to the relevant operators
  std::map<std::string, std::tuple<torch::Tensor, std::vector<TorchSize>>> ops{
      {"1", {ktw_tetra, {0}}},
      {"2", {ktw_tetra, {0, 1}}},
      {"222", {ktw_tetra, {0, 1, 2, 3}}},
      {"42", {ktw_tetra, {0, 1, 2, 3, 4, 5, 6, 7}}},
      {"4", {ktw_tetra, {0, 3, 4, 5}}},
      {"3", {ktw_hexagonal, {0, 1, 2}}},
      {"6", {ktw_hexagonal, {0, 1, 2, 3, 4, 5}}},
      {"32", {ktw_hexagonal, {0, 1, 2, 9, 10, 11}}},
      {"622", {ktw_hexagonal, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}}},
      {"23", {ktw_cubic, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}}},
      {"432", {ktw_cubic, {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                           12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}}}};

  // A fun lambda that provides the R2 version of the correct symmetry operations
  auto matrix_operations = [ops](std::string a)
  { return std::get<0>(ops.at(a)).index({torch::tensor(std::get<1>(ops.at(a)))}); };

  // Load all the models in
  Factory::clear();
  load_model(fs::absolute("unit/crystallography/user_tensors/test_SymmetryFromOrbifold.i"));

  auto fmt_name = [](std::string cls) { return "class_" + cls; };

  SECTION("check matrix operations")
  {
    auto cls = GENERATE("1", "2", "222", "42", "4", "3", "6", "32", "622", "23", "432");
    auto & grp = Factory::get_object<R2>("Tensors", fmt_name(cls));

    REQUIRE(torch::allclose(grp, matrix_operations(cls)));
  }

  SECTION("check the group includes the identity")
  {
    auto cls = GENERATE("1", "2", "222", "42", "4", "3", "6", "32", "622", "23", "432");
    auto & grp = Factory::get_object<R2>("Tensors", fmt_name(cls));
    REQUIRE(torch::any(torch::all(torch::isclose(grp, R2::identity(DTO)).flatten(1), 1))
                .item()
                .toBool());
  }

  SECTION("check the group is closed")
  {
    auto cls = GENERATE("1", "2", "222", "42", "4", "3", "6", "32", "622", "23", "432");
    auto & grp = Factory::get_object<R2>("Tensors", fmt_name(cls));
    // A loop is forgivable here I hope...
    for (TorchSize i = 0; i < grp.batch_sizes()[0]; i++)
    {
      R2 op_i = grp.batch_index({i});
      for (TorchSize j = 0; j < grp.batch_sizes()[0]; j++)
      {
        R2 op_j = grp.batch_index({j});
        R2 prod = op_i * op_j;
        REQUIRE(torch::any(torch::all(torch::isclose(grp, prod).flatten(1), 1)).item().toBool());
      }
    }
  }

  SECTION("check the group has each inverse operation")
  {
    auto cls = GENERATE("1", "2", "222", "42", "4", "3", "6", "32", "622", "23", "432");
    auto & grp = Factory::get_object<R2>("Tensors", fmt_name(cls));
    // A loop is forgivable here I hope...
    for (TorchSize i = 0; i < grp.batch_sizes()[0]; i++)
    {
      R2 inv = grp.batch_index({i}).inverse();
      REQUIRE(torch::any(torch::all(torch::isclose(grp, inv).flatten(1), 1)).item().toBool());
    }
  }
}
