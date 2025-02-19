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

#include "neml2/tensors/user_tensors/FillR2.h"

namespace neml2
{
register_NEML2_object(FillR2);

OptionSet
FillR2::expected_options()
{
  OptionSet options = NEML2Object::expected_options();
  options.set<std::vector<CrossRef<Scalar>>>("values");
  return options;
}

FillR2::FillR2(const OptionSet & options)
  : R2(fill(options.get<std::vector<CrossRef<Scalar>>>("values"))),
    NEML2Object(options)
{
}

R2
FillR2::fill(const std::vector<CrossRef<Scalar>> & values) const
{
  if (values.size() == 1)
    return R2::fill(values[0]);
  else if (values.size() == 3)
    return R2::fill(values[0], values[1], values[2]);
  else if (values.size() == 6)
    return R2::fill(values[0], values[1], values[2], values[3], values[4], values[5]);
  else if (values.size() == 9)
    return R2::fill(values[0],
                    values[1],
                    values[2],
                    values[3],
                    values[4],
                    values[5],
                    values[6],
                    values[7],
                    values[8]);
  else
    neml_assert(false,
                "Number of values must be 1, 3, 6, or 9, but ",
                values.size(),
                " values are provided.");

  return R2();
}
} // namespace neml2
