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

#include "neml2/solvers/Solver.h"
#include "neml2/solvers/NonlinearSystem.h"

namespace neml2
{
/**
 * @brief The nonlinear solver solves a nonlinear system of equations.
 *
 */
class NonlinearSolver : public Solver
{
public:
  static OptionSet expected_options();

  /**
   * @brief Construct a new NonlinearSolver object
   *
   * @param options The options extracted from the input file
   */
  NonlinearSolver(const OptionSet & options);

  /**
   * @brief Solve the given nonlinear system.
   *
   * @param system The nonlinear system of equations.
   * @param sol The initial solution which will be iteratively updated during the solve. It will be
   * the solution to the system upon convergence
   * @return A boolean indicating whether the solve has succeeded and the number of iterations
   * before convergence.
   */
  virtual std::tuple<bool, size_t> solve(NonlinearSystem & system, BatchTensor & sol) = 0;

  /// Absolute tolerance
  Real atol;
  /// Relative tolerance
  Real rtol;
  /// Maximum number of iterations
  unsigned int miters;
};
} // namespace neml2
