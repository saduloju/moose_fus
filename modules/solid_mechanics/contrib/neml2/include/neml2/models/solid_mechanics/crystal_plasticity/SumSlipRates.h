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
namespace crystallography
{
class CrystalGeometry;
}

/// A helper model to calculate the sum of the absolute value of the slip rates
class SumSlipRates : public Model
{
public:
  static OptionSet expected_options();

  SumSlipRates(const OptionSet & options);

  /// Input: list of slip rates
  const VariableName slip_rates;

  /// Output: sum of the absolute values
  const VariableName sum_slip_rates;

protected:
  /// Calculate the sum and the derivatives
  void set_value(bool out, bool dout_din, bool d2out_din2) override;

  /// Crystal geometry class with slip geometry
  const crystallography::CrystalGeometry & _crystal_geometry;

  /// Sum of the absolute slip rates
  Variable<Scalar> & _sg;

  /// Slip rates
  const Variable<BatchTensor> & _g;
};
} // namespace neml2
