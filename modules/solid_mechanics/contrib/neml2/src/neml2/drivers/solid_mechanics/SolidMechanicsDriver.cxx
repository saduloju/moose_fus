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

#include "neml2/drivers/solid_mechanics/SolidMechanicsDriver.h"

namespace neml2
{
register_NEML2_object(SolidMechanicsDriver);

OptionSet
SolidMechanicsDriver::expected_options()
{
  OptionSet options = TransientDriver::expected_options();
  options.set<std::string>("control") = "STRAIN";
  options.set<VariableName>("total_strain") = VariableName("forces", "E");
  options.set<VariableName>("cauchy_stress") = VariableName("forces", "S");
  options.set<CrossRef<torch::Tensor>>("prescribed_strains");
  options.set<CrossRef<torch::Tensor>>("prescribed_stresses");
  return options;
}

SolidMechanicsDriver::SolidMechanicsDriver(const OptionSet & options)
  : TransientDriver(options),
    _control(options.get<std::string>("control"))
{
  if (_control == "STRAIN")
  {
    _driving_force = SR2(options.get<CrossRef<torch::Tensor>>("prescribed_strains"), 2);
    _driving_force_name = options.get<VariableName>("total_strain");
  }
  else if (_control == "STRESS")
  {
    _driving_force = SR2(options.get<CrossRef<torch::Tensor>>("prescribed_stresses"), 2);
    _driving_force_name = options.get<VariableName>("cauchy_stress");
  }
  else
    // LCOV_EXCL_START
    throw NEMLException("Unsupported control type.");
  // LCOV_EXCL_STOP

  _driving_force = _driving_force.to(_device);

  check_integrity();
}

void
SolidMechanicsDriver::check_integrity() const
{
  TransientDriver::check_integrity();
  neml_assert(_driving_force.dim() == 3,
              "Input strain/stress should have dimension 3 but instead has dimension",
              _driving_force.dim());
  neml_assert(_time.sizes()[0] == _driving_force.sizes()[0],
              "Input strain/stress and time should have the same number of time steps. The input "
              "time has ",
              _time.sizes()[0],
              " time steps, while the input strain/stress has ",
              _driving_force.sizes()[0],
              " time steps");
  neml_assert(_time.sizes()[1] == _driving_force.sizes()[1],
              "Input strain/stress and time should have the same batch size. The input time has a "
              "batch size of ",
              _time.sizes()[1],
              " while the input strain/stress has a batch size of ",
              _driving_force.sizes()[1]);
  neml_assert(_driving_force.sizes()[2] == 6,
              "Input strain/stress should have final dimension 6 but instead has final dimension ",
              _driving_force.sizes()[2]);
}

void
SolidMechanicsDriver::update_forces()
{
  TransientDriver::update_forces();
  auto current_driving_force = _driving_force.batch_index({_step_count});
  _in.set(current_driving_force, _driving_force_name);
}
}
