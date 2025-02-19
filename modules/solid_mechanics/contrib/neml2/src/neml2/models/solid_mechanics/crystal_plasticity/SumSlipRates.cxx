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

#include "neml2/models/solid_mechanics/crystal_plasticity/SumSlipRates.h"

#include "neml2/models/crystallography/CrystalGeometry.h"

#include "neml2/tensors/tensors.h"
#include "neml2/tensors/list_tensors.h"

namespace neml2
{
register_NEML2_object(SumSlipRates);

OptionSet
SumSlipRates::expected_options()
{
  OptionSet options = Model::expected_options();

  options.set<VariableName>("slip_rates") = VariableName("state", "internal", "slip_rates");
  options.set<VariableName>("sum_slip_rates") = VariableName("state", "internal", "sum_slip_rates");

  options.set<std::string>("crystal_geometry_name") = "crystal_geometry";

  return options;
}

SumSlipRates::SumSlipRates(const OptionSet & options)
  : Model(options),
    _crystal_geometry(register_data<crystallography::CrystalGeometry>(
        options.get<std::string>("crystal_geometry_name"))),
    _sg(declare_output_variable<Scalar>("sum_slip_rates")),
    _g(declare_input_variable_list<Scalar>(_crystal_geometry.nslip(), "slip_rates"))
{
}

void
SumSlipRates::set_value(bool out, bool dout_din, bool d2out_din2)
{
  neml_assert_dbg(!d2out_din2, "Second derivative not implemented.");

  // Grab the input
  const auto g = Scalar(_g, batch_dim() + 1);

  if (out)
    _sg = math::abs(g).batch_sum(-1);

  if (dout_din)
    _sg.d(_g) = BatchTensor(math::sign(g), batch_dim()).base_unsqueeze(0);
}

} // namespace neml2
