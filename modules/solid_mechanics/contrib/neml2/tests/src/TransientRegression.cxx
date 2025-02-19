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

#include "TransientRegression.h"
#include "neml2/drivers/TransientDriver.h"
#include <iomanip>
#include <torch/script.h>

namespace fs = std::filesystem;

namespace neml2
{
register_NEML2_object(TransientRegression);

OptionSet
TransientRegression::expected_options()
{
  OptionSet options = Driver::expected_options();
  options.set<std::string>("driver");
  options.set<std::string>("reference");
  options.set<Real>("rtol") = 1e-5;
  options.set<Real>("atol") = 1e-8;
  return options;
}

TransientRegression::TransientRegression(const OptionSet & options)
  : Driver(options),
    _driver(Factory::get_object<TransientDriver>("Drivers", options.get<std::string>("driver"))),
    _reference(options.get<std::string>("reference")),
    _rtol(options.get<Real>("rtol")),
    _atol(options.get<Real>("atol"))
{
  neml_assert(fs::exists(_reference), "Reference file '", _reference.string(), "' does not exist.");
}

bool
TransientRegression::run()
{
  _driver.run();

  // Verify the result
  auto res = torch::jit::load(_driver.save_as_path());
  auto res_ref = torch::jit::load(_reference);
  auto err_msg = diff(res.named_buffers(true), res_ref.named_buffers(true), _rtol, _atol);

  neml_assert(err_msg.empty(), err_msg);

  return true;
}

std::string
diff(const torch::jit::named_buffer_list & res,
     const torch::jit::named_buffer_list & ref,
     Real rtol,
     Real atol)
{
  std::map<std::string, torch::Tensor> res_map;
  for (auto item : res)
    res_map.emplace(item.name, item.value);

  std::map<std::string, torch::Tensor> ref_map;
  for (auto item : ref)
    ref_map.emplace(item.name, item.value);

  std::ostringstream err_msg;

  for (auto && [key, value] : res_map)
    if (ref_map.count(key) == 0)
      err_msg << "Result has extra variable " << key << ".\n";

  for (auto && [key, value] : ref_map)
  {
    if (res_map.count(key) == 0)
    {
      err_msg << "Result is missing variable " << key << ".\n";
      continue;
    }

    if (!torch::allclose(res_map[key], value, rtol, atol))
    {
      auto diff = torch::abs(res_map[key] - value) - rtol * torch::abs(value);
      err_msg << "Result has wrong value for variable " << key
              << ". Maximum mixed difference = " << std::scientific << diff.max().item<Real>()
              << " > atol = " << std::scientific << atol << "\n";
    }
  }

  return err_msg.str();
}
}
