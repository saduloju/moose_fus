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

#include "neml2/solvers/NonlinearSolver.h"
#include "neml2/misc/math.h"

namespace neml2
{
/**
 * @copydoc neml2::NonlinearSolver
 *
 * The Newton-Raphson method is used to iteratively update the initial guess until the residual
 * becomes zero within specified tolerances.
 */
class Newton : public NonlinearSolver
{
public:
  static OptionSet expected_options();

  Newton(const OptionSet & options);

  virtual std::tuple<bool, size_t> solve(NonlinearSystem & system, BatchTensor & x) override;

protected:
  /// Prepare solver internal data before the iterative update
  virtual void prepare(const NonlinearSystem & /*system*/, const BatchTensor & /*x*/) {}

  /**
   * @brief Check for convergence. The current iteration is said to be converged if the residual
   * norm is below the absolute tolerance or or the ratio between the residual norm and the initial
   * residual norm is below the relative tolerance.
   *
   * @param itr The current iteration number
   * @param nR The current residual norm
   * @param nR0 The initial residual norm
   * @return true Converged
   * @return false Not converged
   */
  virtual bool converged(size_t itr, const torch::Tensor & nR, const torch::Tensor & nR0) const;

  /// Update trial solution
  virtual void update(NonlinearSystem & system, BatchTensor & x);

  /// Do a final update to track AD function graph
  virtual void final_update(NonlinearSystem & system, BatchTensor & x);

  /// Find the current update direction
  virtual BatchTensor solve_direction(const NonlinearSystem & system);
};
} // namespace neml2
