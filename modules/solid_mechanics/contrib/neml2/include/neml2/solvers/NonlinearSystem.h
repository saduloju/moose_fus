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

#include "neml2/tensors/BatchTensor.h"
#include "neml2/base/OptionSet.h"

namespace neml2
{
/**
 * @brief Definition of a nonlinear system of equations.
 *
 */
class NonlinearSystem
{
public:
  static OptionSet expected_options();

  NonlinearSystem(const OptionSet & options);

  /**
   * @brief Compute algebraic Jacobian-based automatic scaling following
   * https://cs.stanford.edu/people/paulliu/files/cs517-project.pdf
   *
   * @param verbose Print automatic scaling convergence information
   */
  virtual void init_scaling(const bool verbose = false);

  /// Apply scaling to the residual
  BatchTensor scale_residual(const BatchTensor & r) const;
  /// Apply scaling to the Jacobian
  BatchTensor scale_Jacobian(const BatchTensor & J) const;
  /// Remove scaling from the search direction, i.e. \f$ J^{-1} r \f$
  BatchTensor scale_direction(const BatchTensor & p) const;

  /// Set the solution vector
  virtual void set_solution(const BatchTensor & x);

  /// Get the solution vector
  virtual BatchTensor solution() const { return _solution; }

  /// Convenient shortcut to set the current solution, assemble and return the system residual
  BatchTensor residual(const BatchTensor & x);
  /// Convenient shortcut to assemble and return the system residual
  void residual();

  /// Convenient shortcut to set the current solution, assemble and return the system Jacobian
  BatchTensor Jacobian(const BatchTensor & x);
  /// Convenient shortcut to assemble and return the system Jacobian
  void Jacobian();

  /// Convenient shortcut to set the current solution, assemble and return the system residual and Jacobian
  std::tuple<BatchTensor, BatchTensor> residual_and_Jacobian(const BatchTensor & x);
  /// Convenient shortcut to assemble and return the system residual and Jacobian
  void residual_and_Jacobian();

  const BatchTensor & residual_view() const { return _autoscale ? _scaled_residual : _residual; }
  const BatchTensor & Jacobian_view() const { return _autoscale ? _scaled_Jacobian : _Jacobian; }

  /// The residual norm
  BatchTensor residual_norm() const;

protected:
  /**
   * @brief Compute the residual and Jacobian
   *
   * @param residual Whether residual is requested
   * @param Jacobian Whether Jacobian is requested
   */
  virtual void assemble(bool, bool) = 0;

  /// Number of degrees of freedom
  TorchSize _ndof;

  /// View for the solution of this nonlinear system
  BatchTensor _solution;

  /// View for the residual of this nonlinear system
  BatchTensor _residual;

  /// View for the Jacobian of this nonlinear system
  BatchTensor _Jacobian;

  BatchTensor _scaled_residual;

  BatchTensor _scaled_Jacobian;

  /// If true, do automatic scaling
  const bool _autoscale;

  /// Tolerance for convergence check of the iterative automatic scaling algorithm
  const Real _autoscale_tol;

  /// Maximum number of iterations allowed for the iterative automatic scaling algorithm
  const unsigned int _autoscale_miter;

  /// Flag to indicate whether scaling matrices have been computed
  bool _scaling_matrices_initialized;

  /// Row scaling "matrix" -- since it's a batched diagonal matrix, we are only storing its diagonals
  BatchTensor _row_scaling;

  /// Column scaling "matrix" -- since it's a batched diagonal matrix, we are only storing its diagonals
  BatchTensor _col_scaling;
};
} // namespace neml2
