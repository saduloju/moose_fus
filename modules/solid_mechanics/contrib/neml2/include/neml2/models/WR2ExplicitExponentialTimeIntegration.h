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

#pragma once

#include "neml2/models/Model.h"

namespace neml2
{
/// Explicit exponential time integration for rotations
// This function takes as input a skew tensor giving the spin and does the rotational update
class WR2ExplicitExponentialTimeIntegration : public Model
{
public:
  static OptionSet expected_options();

  WR2ExplicitExponentialTimeIntegration(const OptionSet & options);

private:
  /// Variable name
  const VariableName _var_name;

  /// Variable rate name
  const VariableName _var_rate_name;

protected:
  /// Perform the time integration to update the variable value
  void set_value(bool out, bool dout_din, bool d2out_din2) override;

  /// Current value
  Variable<Rot> & _s;

  /// Current variable spin rate
  const Variable<WR2> & _s_dot;

  /// Previous value
  const Variable<Rot> & _sn;

  /// Current time
  const Variable<Scalar> & _t;

  /// Previous time
  const Variable<Scalar> & _tn;
};

} // namespace neml2
