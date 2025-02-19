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

#include "neml2/models/crystallography/user_tensors/FillMillerIndex.h"

namespace neml2
{
namespace crystallography
{

register_NEML2_object(FillMillerIndex);

OptionSet
FillMillerIndex::expected_options()
{
  OptionSet options = NEML2Object::expected_options();
  options.set<std::vector<Integer>>("values");
  return options;
}

FillMillerIndex::FillMillerIndex(const OptionSet & options)
  : MillerIndex(fill(options.get<std::vector<Integer>>("values"))),
    NEML2Object(options)
{
}

MillerIndex
FillMillerIndex::fill(const std::vector<Integer> & values) const
{
  if ((values.size() % 3) != 0)
    neml_assert(false, "Number of provided values must be a multiple of three!");

  return MillerIndex(torch::tensor(values, default_integer_tensor_options()).reshape({-1, 3}));
}
} // namespace crystallography
} // namespace neml2
