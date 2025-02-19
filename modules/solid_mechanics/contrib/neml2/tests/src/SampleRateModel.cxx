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

#include "SampleRateModel.h"
#include "neml2/tensors/SSR4.h"

using namespace neml2;

register_NEML2_object(SampleRateModel);

SampleRateModel::SampleRateModel(const OptionSet & options)
  : Model(options),
    foo(declare_input_variable<Scalar>("state", "foo")),
    bar(declare_input_variable<Scalar>("state", "bar")),
    baz(declare_input_variable<SR2>("state", "baz")),
    T(declare_input_variable<Scalar>("forces", "temperature")),
    foo_dot(declare_output_variable<Scalar>("state", "foo_rate")),
    bar_dot(declare_output_variable<Scalar>("state", "bar_rate")),
    baz_dot(declare_output_variable<SR2>("state", "baz_rate")),
    _a(declare_parameter("a", Scalar(-0.01, default_tensor_options()))),
    _b(declare_parameter("b", Scalar(-0.5, default_tensor_options()))),
    _c(declare_parameter("c", Scalar(-0.9, default_tensor_options())))
{
}

void
SampleRateModel::set_value(bool out, bool dout_din, bool d2out_din2)
{
  neml_assert_dbg(!d2out_din2, "Second derivative not implemented.");

  if (out)
  {
    foo_dot = (foo * foo + bar) * T + SR2(baz).tr();
    bar_dot = _a * bar + _b * foo + _c * T + SR2(baz).tr();
    baz_dot = (foo + bar) * baz * (T - 3);
  }

  if (dout_din)
  {
    auto I = SR2::identity(options());

    foo_dot.d(foo) = 2 * foo * T;
    foo_dot.d(bar) = T;
    foo_dot.d(baz) = I;
    foo_dot.d(T) = foo * foo + bar;

    bar_dot.d(foo) = _b;
    bar_dot.d(bar) = _a;
    bar_dot.d(baz) = I;
    bar_dot.d(T) = _c;

    baz_dot.d(foo) = baz * (T - 3);
    baz_dot.d(bar) = baz * (T - 3);
    baz_dot.d(baz) = (foo + bar) * (T - 3) * SR2::identity_map(options());
    baz_dot.d(T) = (foo + bar) * baz;
  }
}
