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

#include "neml2/models/Model.h"

namespace neml2
{
Model::Stage Model::stage = UPDATING;

OptionSet
Model::expected_options()
{
  OptionSet options = Data::expected_options();
  options += NonlinearSystem::expected_options();
  options.set<bool>("use_AD_first_derivative") = false;
  options.set<bool>("use_AD_second_derivative") = false;
  options.set<int>("_extra_derivative_order") = 0;
  options.set<bool>("_nonlinear_system") = false;
  return options;
}

Model::Model(const OptionSet & options)
  : Data(options),
    ParameterStore(options, this),
    VariableStore(options, this),
    NonlinearSystem(options),
    _AD_1st_deriv(options.get<bool>("use_AD_first_derivative")),
    _AD_2nd_deriv(options.get<bool>("use_AD_second_derivative")),
    _options(default_tensor_options()),
    _deriv_order(-1),
    _extra_deriv_order(options.get<int>("_extra_derivative_order")),
    _nonlinear_system(options.get<bool>("_nonlinear_system"))
{
  check_AD_limitation();
}

void
Model::setup()
{
  // Declare nonlinear parameters as additional input variables
  for (const auto & [name, param] : nl_params())
    declare_input_variable(param->base_storage(), VariableName(param->name()));

  // Setup input and output axes
  setup_layout();

  // Setup functional dependence for each output variable
  for (auto && [y_name, y_var] : output_views())
  {
    y_var.clear_args();
    for (auto && [x_name, x_var] : input_views())
      y_var.add_arg(x_var);
  }

  // Setup variable views
  setup_input_views();
  setup_output_views();
}

void
Model::reinit(TorchShapeRef batch_shape,
              int deriv_order,
              const torch::Device & device,
              const torch::Dtype & dtype)
{
  neml_assert(host() == this, "This method should only be called on the host model.");

  // Re-cache batch shape if it has changed
  const auto batch_shape_promoted = batch_shape.empty() ? TorchShape{1} : batch_shape.vec();
  const bool batch_shape_changed = batch_shape_promoted != batch_sizes();
  if (batch_shape_changed)
    cache(batch_shape_promoted);

  // Re-cache tensor options if they have changed
  const bool device_changed = device != options().device();
  const bool dtype_changed = dtype != options().dtype();
  const bool options_changed = device_changed || dtype_changed;
  if (options_changed)
  {
    const auto options = default_tensor_options().device(device).dtype(dtype);
    cache(options);
    send_buffers_to(options);
    send_parameters_to(options);
  }

  // Reallocate variable storage when necessary
  allocate_variables(deriv_order, batch_shape_changed || options_changed);
}

void
Model::reinit(const BatchTensor & tensor, int deriv_order)
{
  reinit(tensor.batch_sizes(), deriv_order, tensor.device(), tensor.scalar_type());
}

void
Model::allocate_variables(int deriv_order, bool options_changed)
{
  const auto deriv_order_old = _deriv_order;
  const auto deriv_order_new = deriv_order + _extra_deriv_order;

  bool in = false;
  bool out = false;
  bool dout_din = false;
  bool d2out_din2 = false;

  if (options_changed)
  {
    in = true;
    out = deriv_order_new >= 0;
    dout_din = deriv_order_new >= 1;
    d2out_din2 = deriv_order_new >= 2;
  }
  else
  {
    out = deriv_order_new >= 0 && deriv_order_old < 0;
    dout_din = deriv_order_new >= 1 && deriv_order_old < 1;
    d2out_din2 = deriv_order_new >= 2 && deriv_order_old < 2;
  }

  _deriv_order = deriv_order_new;
  VariableStore::allocate_variables(batch_sizes(), options(), in, out, dout_din, d2out_din2);

  for (auto submodel : registered_models())
    submodel->allocate_variables(_deriv_order, options_changed);
}

void
Model::setup_input_views()
{
  VariableStore::setup_input_views();
  setup_submodel_input_views();
}

void
Model::setup_submodel_input_views()
{
  for (auto submodel : registered_models())
  {
    for (auto && [name, var] : submodel->input_views())
      var.setup_views(input_view(var.name()));

    submodel->setup_submodel_input_views();
  }
}

void
Model::setup_output_views()
{
  VariableStore::setup_output_views();
  setup_submodel_output_views();
}

void
Model::setup_submodel_output_views()
{
  for (auto submodel : registered_models())
  {
    for (auto && [name, var] : submodel->output_views())
      var.setup_views(&submodel->output_storage(),
                      &submodel->derivative_storage(),
                      &submodel->second_derivative_storage());

    submodel->setup_submodel_output_views();
  }
}

void
Model::reinit_input_views()
{
  VariableStore::reinit_input_views();

  if (is_nonlinear_system())
  {
    _ndof = host<Model>()->input_axis().storage_size("state");
    _solution = host<Model>()->input_storage()("state");
  }
}

void
Model::reinit_output_views(bool out, bool dout_din, bool d2out_din2)
{
  VariableStore::reinit_output_views(
      out, dout_din && requires_grad(), d2out_din2 && requires_2nd_grad());

  if (is_nonlinear_system())
  {
    if (out)
      _residual = output_storage()("residual");
    if (dout_din && requires_grad())
      _Jacobian = derivative_storage()("residual", "state");
  }
}

void
Model::detach_and_zero(bool out, bool dout_din, bool d2out_din2)
{
  VariableStore::detach_and_zero(
      out, dout_din && requires_grad(), d2out_din2 && requires_2nd_grad());

  for (auto submodel : registered_models())
    submodel->detach_and_zero(out, dout_din, d2out_din2);
}

void
Model::cache(TorchShapeRef batch_shape)
{
  _batch_sizes = batch_shape.vec();
  VariableStore::cache(batch_shape);
  for (auto submodel : registered_models())
    submodel->cache(batch_shape);
}

void
Model::cache(const torch::TensorOptions & options)
{
  _options = options;
  for (auto submodel : registered_models())
    submodel->cache(options);
}

void
Model::check_AD_limitation() const
{
  if (_AD_1st_deriv && !_AD_2nd_deriv)
    throw NEMLException("AD derivative is requested, but AD second derivative is not requested.");
}

void
Model::use_AD_derivatives(bool first, bool second)
{
  _AD_1st_deriv = first;
  _AD_2nd_deriv = second;
  check_AD_limitation();
}

void
Model::set_input(const LabeledVector & in)
{
  neml_assert_batch_broadcastable(in, input_storage());
  neml_assert_dbg(in.axis(0) == input_axis(),
                  "Incompatible input axis. The model has input axis: \n",
                  input_axis(),
                  "The input vector has axis: \n",
                  in.axis(0));

  input_storage().copy_(in.tensor().batch_expand(batch_sizes()));
}

LabeledVector
Model::get_output()
{
  return output_storage().clone();
}

LabeledMatrix
Model::get_doutput_dinput()
{
  return derivative_storage().clone();
}

LabeledTensor3D
Model::get_d2output_dinput2()
{
  return second_derivative_storage().clone();
}

LabeledVector
Model::value(const LabeledVector & in)
{
  set_input(in);
  value();
  return get_output();
}

std::tuple<LabeledVector, LabeledMatrix>
Model::value_and_dvalue(const LabeledVector & in)
{
  set_input(in);
  value_and_dvalue();
  return {get_output(), get_doutput_dinput()};
}

std::tuple<LabeledVector, LabeledMatrix, LabeledTensor3D>
Model::value_and_dvalue_and_d2value(const LabeledVector & in)
{
  set_input(in);
  value_and_dvalue_and_d2value();
  return {get_output(), get_doutput_dinput(), get_d2output_dinput2()};
}

void
Model::value()
{
  detach_and_zero(true, false, false);
  set_value(true, false, false);
}

void
Model::value_and_dvalue()
{
  neml_assert_dbg(requires_grad(),
                  "value_and_dvalue() is called but derivative storage hasn't been allocated.");

  detach_and_zero(true, true, false);

  if (!_AD_1st_deriv)
    set_value(true, true, false);
  else
  {
    input_storage().tensor().requires_grad_();
    set_value(true, false, false);
    extract_derivatives(/*retain_graph=*/true, /*create_graph=*/false, /*allow_unused=*/true);
    input_storage().tensor().requires_grad_(false);
  }
}

void
Model::value_and_dvalue_and_d2value()
{
  neml_assert_dbg(requires_2nd_grad(),
                  "value_and_dvalue_and_d2value() is called but second derivative storage hasn't "
                  "been allocated.");

  detach_and_zero(true, true, true);

  if (!_AD_2nd_deriv)
    set_value(true, true, true);
  else
  {
    input_storage().tensor().requires_grad_();

    if (!_AD_1st_deriv)
      set_value(true, true, false);
    else
    {
      set_value(true, false, false);
      extract_derivatives(/*retain_graph=*/true, /*create_graph=*/true, /*allow_unused=*/true);
    }

    extract_second_derivatives(
        /*retain_graph=*/true, /*create_graph=*/false, /*allow_unused=*/true);

    input_storage().tensor().requires_grad_(false);
  }
}

Model *
Model::registered_model(const std::string & name) const
{
  for (auto submodel : _registered_models)
    if (submodel->name() == name)
      return submodel;

  throw NEMLException("There is no registered model named '" + name + "' in '" + this->name() +
                      "'");
}

const std::set<VariableName>
Model::consumed_items() const
{
  return input_axis().variable_accessors(true);
}

const std::set<VariableName>
Model::provided_items() const
{
  return output_axis().variable_accessors(true);
}

void
Model::assemble(bool residual, bool Jacobian)
{
  if (residual && !Jacobian)
    value();
  else if (Jacobian)
    value_and_dvalue();
}

void
Model::extract_derivatives(bool retain_graph, bool create_graph, bool allow_unused)
{
  // Loop over rows to retrieve the derivatives
  if (output_storage().tensor().requires_grad())
    for (TorchSize i = 0; i < output_storage().base_sizes()[0]; i++)
    {
      auto grad_outputs = BatchTensor::zeros_like(output_storage());
      grad_outputs.index_put_({torch::indexing::Ellipsis, i}, 1.0);
      auto jac_row = torch::autograd::grad({output_storage()},
                                           {input_storage()},
                                           {grad_outputs},
                                           retain_graph,
                                           create_graph,
                                           allow_unused)[0];
      if (jac_row.defined())
        derivative_storage().base_index_put({i, torch::indexing::Slice()}, jac_row);
    }
}

void
Model::extract_second_derivatives(bool retain_graph, bool create_graph, bool allow_unused)
{
  // Loop over rows to retrieve the second derivatives
  if (derivative_storage().tensor().requires_grad())
    for (TorchSize i = 0; i < derivative_storage().base_sizes()[0]; i++)
      for (TorchSize j = 0; j < derivative_storage().base_sizes()[1]; j++)
      {
        auto grad_outputs = torch::zeros_like(derivative_storage());
        grad_outputs.index_put_({torch::indexing::Ellipsis, i, j}, 1.0);
        auto jac_row = torch::autograd::grad({derivative_storage()},
                                             {input_storage()},
                                             {grad_outputs},
                                             retain_graph,
                                             create_graph,
                                             allow_unused)[0];
        if (jac_row.defined())
          second_derivative_storage().base_index_put({i, j, torch::indexing::Slice()}, jac_row);
      }
}
} // namespace neml2
