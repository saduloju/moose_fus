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

#include "neml2/drivers/Driver.h"

namespace neml2
{
class TransientDriver;

class VTestVerification : public Driver
{
public:
  static OptionSet expected_options();

  VTestVerification(const OptionSet & options);

  bool run() override;

private:
  bool allclose(const std::string & var, torch::Tensor ref) const;

  /// The driver that will run the NEML2 model
  TransientDriver & _driver;

  /// The variables to be compared
  std::vector<std::string> _variables;

  /// The variables with the correct values (from the vtest file)
  std::vector<CrossRef<torch::Tensor>> _references;

  Real _rtol;
  Real _atol;

  /// Check the average of the model output instead...
  bool _taylor_average;
};
} // namespace neml2
