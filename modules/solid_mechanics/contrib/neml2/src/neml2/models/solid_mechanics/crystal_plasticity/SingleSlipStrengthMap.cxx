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

#include "neml2/models/solid_mechanics/crystal_plasticity/SingleSlipStrengthMap.h"

namespace neml2
{
register_NEML2_object(SingleSlipStrengthMap);

OptionSet
SingleSlipStrengthMap::expected_options()
{
  OptionSet options = SlipStrengthMap::expected_options();

  options.set<VariableName>("slip_hardening") = VariableName("state", "internal", "slip_hardening");
  options.set<CrossRef<Scalar>>("constant_strength");

  return options;
}

SingleSlipStrengthMap::SingleSlipStrengthMap(const OptionSet & options)
  : SlipStrengthMap(options),
    _tau_bar(declare_input_variable<Scalar>("slip_hardening")),
    _tau_const(declare_parameter<Scalar>("constant_strength", "constant_strength"))
{
}

void
SingleSlipStrengthMap::set_value(bool out, bool dout_din, bool d2out_din2)
{
  neml_assert_dbg(!d2out_din2, "Second derivative not implemented.");

  if (out)
    _tau = (_tau_bar + _tau_const).base_unsqueeze(0).base_expand_as(_tau.value());

  if (dout_din)
    _tau.d(_tau_bar) = Scalar::ones(options()).base_expand_as(_tau.d(_tau_bar).value());
}
} // namespace neml2
