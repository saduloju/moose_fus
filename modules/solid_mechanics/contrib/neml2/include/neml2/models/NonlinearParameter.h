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
#include "neml2/tensors/tensors.h"
#include "neml2/tensors/macros.h"

namespace neml2
{
/**
 * @brief The base class for *nonlinear* parameters
 *
 * The word "nonlinear" refers to the fact that the parameter can change as a function of state or
 * forces. In other words, in the context of updating an implicit model, the value of the parameter
 * can change fron nonlinear iteration to nonlinear iteration, as the guess of the solution keeps
 * updating.
 *
 * The output of a nonlinear parameter is not a "parameter" in our usual definition as one does not
 * calibrate or optimize the nonlinear parameter in a training loop. However, the definition of the
 * nonlinear parameter itself is oftentimes parameterized on a set of parameters (in the canonical
 * definition). Those parameters can be calibrated or optimized.
 *
 * @tparam T The class is templated on the output tensor type which can be any NEML2 primitive
 * tensor type.
 */
template <typename T>
class NonlinearParameter : public Model
{
public:
  static OptionSet expected_options();

  NonlinearParameter(const OptionSet & options);

  /// Get the nonlinear parameter
  const Variable<T> & param() const { return _p; }

protected:
  /// The nonlinear parameter
  Variable<T> & _p;
};
} // namespace neml2
