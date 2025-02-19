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

#include "neml2/models/solid_mechanics/LinearIsotropicElasticity.h"
#include "neml2/tensors/SSR4.h"

namespace neml2
{
register_NEML2_object(LinearIsotropicElasticity);

OptionSet
LinearIsotropicElasticity::expected_options()
{
  OptionSet options = Elasticity::expected_options();
  options.set<CrossRef<Scalar>>("youngs_modulus");
  options.set<CrossRef<Scalar>>("poisson_ratio");
  return options;
}

LinearIsotropicElasticity::LinearIsotropicElasticity(const OptionSet & options)
  : Elasticity(options),
    _E(declare_parameter<Scalar>("E", "youngs_modulus")),
    _nu(declare_parameter<Scalar>("nu", "poisson_ratio"))
{
}

void
LinearIsotropicElasticity::set_value(bool out, bool dout_din, bool /*d2out_din2*/)
{
  // We need to work with the bulk modulus K and the shear modulus G so that the expression for
  // stiffness and compliance can be unified:
  const auto K = _E / 3 / (1 - 2 * _nu);
  const auto G = _E / 2 / (1 + _nu);
  const auto vf = _compliance ? 1 / (3 * K) : 3 * K;
  const auto df = _compliance ? 1 / (2 * G) : 2 * G;

  if (out)
    _to = vf * SR2(_from).vol() + df * SR2(_from).dev();

  if (dout_din)
  {
    const auto I = SSR4::identity_vol(options());
    const auto J = SSR4::identity_dev(options());
    _to.d(_from) = vf * I + df * J;
  }
}
} // namespace neml2
