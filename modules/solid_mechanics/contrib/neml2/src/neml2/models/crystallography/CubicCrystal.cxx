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

#include "neml2/models/crystallography/CubicCrystal.h"

#include "neml2/models/crystallography/crystallography.h"
#include "neml2/tensors/tensors.h"

namespace neml2
{
namespace crystallography
{

register_NEML2_object(CubicCrystal);

OptionSet
CubicCrystal::expected_options()
{
  OptionSet options = CrystalGeometry::expected_options();

  options.set("crystal_class").suppressed() = true;
  options.set("lattice_vectors").suppressed() = true;

  options.set<CrossRef<Scalar>>("lattice_parameter");

  return options;
}

CubicCrystal::CubicCrystal(const OptionSet & options)
  : CrystalGeometry(
        options,
        symmetry_operations_from_orbifold("432"),
        Vec(torch::Tensor(R2::fill(options.get<CrossRef<Scalar>>("lattice_parameter")))))
{
}

} // namespace crystallography
} // namespace neml2