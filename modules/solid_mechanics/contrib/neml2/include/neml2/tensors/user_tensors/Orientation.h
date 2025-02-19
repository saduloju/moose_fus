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

#include "neml2/base/Registry.h"
#include "neml2/base/NEML2Object.h"

#include "neml2/tensors/Rot.h"

namespace neml2
{
/**
 * @brief Create batch of rotations, with various methods
 */
class Orientation : public Rot, public NEML2Object
{
public:
  static OptionSet expected_options();

  /**
   * @brief Construct a new Orientation object
   *
   * @param "input_type" -- the method used to define the angles, "euler_angles" or "random"
   * @param "angle_convention" -- Euler angle convention, "Kocks", "Roe", or "Bunge"
   * @param "angle_type" -- type of angles, either "degrees" or "radians"
   * @param "values" -- input Euler angles, as a flattened nx3 matrix
   * @param "normalize" -- if true do a "shadow parameter" replacement of the underlying MRP
   * representation to move the inputs farther away from the singularity
   * @param "random_seed" -- random seed for random angle generation
   * @param "quantity" -- number (batch size) of random orientations
   */
  Orientation(const OptionSet & options);

private:
  /// A helper method to dispatch to the correct fill method based on the selected options
  Rot fill(const OptionSet & options) const;

  /// Fill from an array of Euler angles
  Rot fill_euler_angles(const torch::Tensor & vals,
                        std::string angle_convention,
                        std::string angle_type) const;

  /// Fill from rotation matrices
  Rot fill_matrix(const R2 & vals) const;

  /// Fill some number of random orientations
  Rot fill_random(unsigned int n, TorchSize random_seed) const;

  /// Fill from standard Rodrigues parameters
  Rot fill_rodrigues(const Scalar & rx, const Scalar & ry, const Scalar & rz) const;

  /// Expand to fill a batch dimension if required
  Rot expand_as_needed(const Rot & input, unsigned int inp_size) const;
};
} // namespace neml2
