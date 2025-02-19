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

#include "neml2/models/crystallography/crystallography.h"

#include "neml2/tensors/Transformable.h"
#include "neml2/tensors/tensors.h"

using namespace torch::indexing;

namespace neml2
{
namespace crystallography
{
namespace crystal_symmetry_operators
{
const torch::Tensor
tetragonal(const torch::TensorOptions & options)
{
  return torch::tensor({{o, z, z, z},
                        {z, z, o, z},
                        {z, o, z, z},
                        {z, z, z, o},
                        {a, z, z, -a},
                        {a, z, z, a},
                        {z, a, a, z},
                        {z, -a, a, z}},
                       options);
}

const torch::Tensor
hexagonal(const torch::TensorOptions & options)
{
  return torch::tensor({{o, z, z, z},
                        {-h, z, z, b},
                        {h, z, z, b},
                        {b, z, z, -h},
                        {z, z, z, o},
                        {b, z, z, h},
                        {z, -h, b, z},
                        {z, o, z, z},
                        {z, h, b, z},
                        {z, b, h, z},
                        {z, z, o, z},
                        {z, b, -h, z}},
                       options);
}

const torch::Tensor
cubic(const torch::TensorOptions & options)
{
  return torch::tensor({{o, z, z, z},   {h, h, h, h},    {-h, h, h, h},  {h, -h, h, h},
                        {h, h, -h, h},  {-h, -h, -h, h}, {h, -h, -h, h}, {-h, -h, h, h},
                        {-h, h, -h, h}, {z, z, o, z},    {z, z, z, o},   {z, o, z, z},
                        {z, -a, z, a},  {z, a, z, a},    {a, z, a, z},   {a, z, -a, z},
                        {z, z, -a, a},  {a, a, z, z},    {a, -a, z, z},  {z, z, a, a},
                        {z, -a, a, z},  {a, z, z, -a},   {z, a, a, z},   {a, z, z, a}},
                       options);
}
} // namespace crystal_symmetry_operators

R2
symmetry_operations_from_orbifold(std::string orbifold, const torch::TensorOptions & options)
{
  if (orbifold == "432")
  {
    return transform_from_quaternion(Quaternion(crystal_symmetry_operators::cubic(options)));
  }
  else if (orbifold == "23")
  {
    return transform_from_quaternion(
        Quaternion(crystal_symmetry_operators::cubic(options).index({Slice(0, 12)})));
  }
  else if (orbifold == "622")
  {
    return transform_from_quaternion(Quaternion(crystal_symmetry_operators::hexagonal(options)));
  }
  else if (orbifold == "32")
  {
    return transform_from_quaternion(Quaternion(
        torch::cat({crystal_symmetry_operators::hexagonal(options).index({Slice(0, 3)}),
                    crystal_symmetry_operators::hexagonal(options).index({Slice(9, 12)})})));
  }
  else if (orbifold == "6")
  {
    return transform_from_quaternion(
        Quaternion(crystal_symmetry_operators::hexagonal(options).index({Slice(0, 6)})));
  }
  else if (orbifold == "3")
  {
    return transform_from_quaternion(
        Quaternion(crystal_symmetry_operators::hexagonal(options).index({Slice(0, 3)})));
  }
  else if (orbifold == "42")
  {
    return transform_from_quaternion(Quaternion(crystal_symmetry_operators::tetragonal(options)));
  }
  else if (orbifold == "4")
  {
    return transform_from_quaternion(Quaternion(
        torch::cat({crystal_symmetry_operators::tetragonal(options).index({Slice(0, 1)}),
                    crystal_symmetry_operators::tetragonal(options).index({Slice(3, 6)})})));
  }
  else if (orbifold == "222")
  {
    return transform_from_quaternion(
        Quaternion(crystal_symmetry_operators::tetragonal(options).index({Slice(0, 4)})));
  }
  else if (orbifold == "2")
  {
    return transform_from_quaternion(
        Quaternion(crystal_symmetry_operators::tetragonal(options).index({Slice(0, 2)})));
  }
  else if (orbifold == "1")
  {
    return transform_from_quaternion(
        Quaternion(crystal_symmetry_operators::tetragonal(options).index({Slice(0, 1)})));
  }
  else
  {
    throw NEMLException("Unknown crystal class " + orbifold);
  }
}

Vec
unique_bidirectional(const R2 & ops, const Vec & inp)
{
  neml_assert_dbg(inp.batch_dim() == 0);
  // Batched tensor with all possible answers
  auto options = ops * inp;
  // I think we have to go one by one...
  // Slightly annoying that while Vec and torch::Tensor are convertible a
  // list of Vecs aren't convertable into a TensorList
  std::vector<torch::Tensor> unique{torch::Tensor(options.batch_index({0}))};
  Vec unique_vecs = Vec(torch::stack(unique));
  for (TorchSize i = 1; i < options.batch_sizes()[0]; i++)
  {
    auto vi = options.batch_index({i});
    // Compares list of vectors to vector to figure out if any are the same
    auto same = [](const torch::Tensor & a, const torch::Tensor & b)
    { return torch::any(torch::all(torch::isclose(a, b), 1)); };
    if (!(same(unique_vecs, vi).item<bool>() || same(unique_vecs, -vi).item<bool>()))
    {
      unique.push_back(torch::Tensor(vi));
      unique_vecs = Vec(torch::stack(unique));
    }
  }
  // Get the batch of all possible answers
  return unique_vecs;
}

} // namespace crystallography
} // namespace neml2
