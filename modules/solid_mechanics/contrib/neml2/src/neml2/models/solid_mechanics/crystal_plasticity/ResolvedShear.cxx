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

#include "neml2/models/solid_mechanics/crystal_plasticity/ResolvedShear.h"
#include "neml2/models/crystallography/CrystalGeometry.h"

#include "neml2/tensors/tensors.h"
#include "neml2/tensors/list_tensors.h"

namespace neml2
{
register_NEML2_object(ResolvedShear);

OptionSet
ResolvedShear::expected_options()
{
  OptionSet options = Model::expected_options();
  options.set<VariableName>("resolved_shears") =
      VariableName("state", "internal", "resolved_shears");
  options.set<VariableName>("stress") = VariableName("state", "internal", "cauchy_stress");
  options.set<VariableName>("orientation") = VariableName("state", "orientation_matrix");
  options.set<std::string>("crystal_geometry_name") = "crystal_geometry";
  return options;
}

ResolvedShear::ResolvedShear(const OptionSet & options)
  : Model(options),
    _crystal_geometry(register_data<crystallography::CrystalGeometry>(
        options.get<std::string>("crystal_geometry_name"))),
    _rss(declare_output_variable_list<Scalar>(_crystal_geometry.nslip(), "resolved_shears")),
    _S(declare_input_variable<SR2>("stress")),
    _R(declare_input_variable<R2>("orientation"))
{
}

void
ResolvedShear::set_value(bool out, bool dout_din, bool d2out_din2)
{
  neml_assert_dbg(!d2out_din2, "Second derivative not implemented.");

  const auto S = SR2(_S).list_unsqueeze();
  const auto R = R2(_R).list_unsqueeze();

  if (out)
    _rss = BatchTensor(_crystal_geometry.M().rotate(R).inner(S), batch_dim());

  if (dout_din)
  {
    _rss.d(_S) = BatchTensor(_crystal_geometry.M().rotate(R), batch_dim());
    _rss.d(_R) =
        BatchTensor(SR2(_crystal_geometry.M().drotate(R).movedim(-3, -1))
                        .inner(SR2(_S).batch_unsqueeze(-1).batch_unsqueeze(-1).batch_unsqueeze(-1)),
                    batch_dim());
  }
}

} // namespace neml2
