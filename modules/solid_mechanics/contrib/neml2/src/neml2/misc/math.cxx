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

#include "neml2/misc/math.h"
#include "neml2/misc/error.h"
#include "neml2/tensors/tensors.h"

namespace neml2
{
namespace math
{
ConstantTensors::ConstantTensors()
{
  _full_to_mandel_map = torch::tensor({0, 4, 8, 5, 2, 1}, default_integer_tensor_options());

  _mandel_to_full_map =
      torch::tensor({0, 5, 4, 5, 1, 3, 4, 3, 2}, default_integer_tensor_options());

  _full_to_mandel_factor =
      torch::tensor({1.0, 1.0, 1.0, sqrt2, sqrt2, sqrt2}, default_tensor_options());

  _mandel_to_full_factor =
      torch::tensor({1.0, invsqrt2, invsqrt2, invsqrt2, 1.0, invsqrt2, invsqrt2, invsqrt2, 1.0},
                    default_tensor_options());

  _full_to_skew_map = torch::tensor({7, 2, 3}, default_integer_tensor_options());

  _skew_to_full_map = torch::tensor({0, 2, 1, 2, 0, 0, 1, 0, 0}, default_integer_tensor_options());

  _full_to_skew_factor = torch::tensor({1.0, 1.0, 1.0}, default_tensor_options());

  _skew_to_full_factor =
      torch::tensor({0.0, -1.0, 1.0, 1.0, 0.0, -1.0, -1.0, 1.0, 0.0}, default_tensor_options());
}

ConstantTensors &
ConstantTensors::get()
{
  static ConstantTensors cts;
  return cts;
}

const torch::Tensor &
ConstantTensors::full_to_mandel_map()
{
  return get()._full_to_mandel_map;
}

const torch::Tensor &
ConstantTensors::mandel_to_full_map()
{
  return get()._mandel_to_full_map;
}

const torch::Tensor &
ConstantTensors::full_to_mandel_factor()
{
  return get()._full_to_mandel_factor;
}

const torch::Tensor &
ConstantTensors::mandel_to_full_factor()
{
  return get()._mandel_to_full_factor;
}

const torch::Tensor &
ConstantTensors::full_to_skew_map()
{
  return get()._full_to_skew_map;
}

const torch::Tensor &
ConstantTensors::skew_to_full_map()
{
  return get()._skew_to_full_map;
}

const torch::Tensor &
ConstantTensors::full_to_skew_factor()
{
  return get()._full_to_skew_factor;
}

const torch::Tensor &
ConstantTensors::skew_to_full_factor()
{
  return get()._skew_to_full_factor;
}

BatchTensor
full_to_reduced(const BatchTensor & full,
                const torch::Tensor & rmap,
                const torch::Tensor & rfactors,
                TorchSize dim)
{
  using namespace torch::indexing;

  auto batch_dim = full.batch_dim();
  auto starting_dim = batch_dim + dim;
  auto trailing_dim = full.dim() - starting_dim - 2; // 2 comes from the reduced axes (3,3)
  auto starting_shape = full.sizes().slice(0, starting_dim);
  auto trailing_shape = full.sizes().slice(starting_dim + 2);

  TorchSlice net(starting_dim, None);
  net.push_back(Ellipsis);
  net.insert(net.end(), trailing_dim, None);
  auto map =
      rmap.index(net).expand(utils::add_shapes(starting_shape, rmap.sizes()[0], trailing_shape));
  auto factor = rfactors.to(full).index(net);

  return BatchTensor(
      factor * torch::gather(full.reshape(utils::add_shapes(starting_shape, 9, trailing_shape)),
                             starting_dim,
                             map),
      batch_dim);
}

BatchTensor
reduced_to_full(const BatchTensor & reduced,
                const torch::Tensor & rmap,
                const torch::Tensor & rfactors,
                TorchSize dim)
{
  using namespace torch::indexing;

  auto batch_dim = reduced.batch_dim();
  auto starting_dim = batch_dim + dim;
  auto trailing_dim = reduced.dim() - starting_dim - 1; // There's only 1 axis to unsqueeze
  auto starting_shape = reduced.sizes().slice(0, starting_dim);
  auto trailing_shape = reduced.sizes().slice(starting_dim + 1);

  TorchSlice net(starting_dim, None);
  net.push_back(Ellipsis);
  net.insert(net.end(), trailing_dim, None);
  auto map = rmap.index(net).expand(utils::add_shapes(starting_shape, 9, trailing_shape));
  auto factor = rfactors.to(reduced).index(net);

  return BatchTensor((factor * torch::gather(reduced, starting_dim, map))
                         .reshape(utils::add_shapes(starting_shape, 3, 3, trailing_shape)),
                     batch_dim);
}

BatchTensor
full_to_mandel(const BatchTensor & full, TorchSize dim)
{
  return full_to_reduced(
      full,
      ConstantTensors::full_to_mandel_map().to(full.options().dtype(TORCH_INT_DTYPE)),
      ConstantTensors::full_to_mandel_factor().to(full.options()),
      dim);
}

BatchTensor
mandel_to_full(const BatchTensor & mandel, TorchSize dim)
{
  return reduced_to_full(
      mandel,
      ConstantTensors::mandel_to_full_map().to(mandel.options().dtype(TORCH_INT_DTYPE)),
      ConstantTensors::mandel_to_full_factor().to(mandel.options()),
      dim);
}

BatchTensor
full_to_skew(const BatchTensor & full, TorchSize dim)
{
  return full_to_reduced(
      full,
      ConstantTensors::full_to_skew_map().to(full.options().dtype(TORCH_INT_DTYPE)),
      ConstantTensors::full_to_skew_factor().to(full.options()),
      dim);
}

BatchTensor
skew_to_full(const BatchTensor & skew, TorchSize dim)
{
  return reduced_to_full(
      skew,
      ConstantTensors::skew_to_full_map().to(skew.options().dtype(TORCH_INT_DTYPE)),
      ConstantTensors::skew_to_full_factor().to(skew.options()),
      dim);
}

BatchTensor
jacrev(const BatchTensor & y, const BatchTensor & p)
{
  neml_assert(p.batch_sizes() == y.batch_sizes(),
              "The batch shape of the parameter must be the same as the batch shape "
              "of the output. However, the batch shape of the parameter is ",
              p.batch_sizes(),
              ", and the batch shape of the output is ",
              y.batch_sizes());

  // flatten y to handle arbitrarily shaped output
  auto yf = BatchTensor(
      y.reshape(utils::add_shapes(y.batch_sizes(), utils::storage_size(y.base_sizes()))),
      y.batch_dim());

  neml_assert_dbg(yf.base_dim() == 1, "Flattened output must be flat.");

  auto dyf_dp = BatchTensor::empty(
      yf.batch_sizes(), utils::add_shapes(yf.base_sizes(), p.base_sizes()), yf.options());

  for (TorchSize i = 0; i < yf.base_sizes()[0]; i++)
  {
    auto v = BatchTensor::zeros_like(yf);
    v.index_put_({torch::indexing::Ellipsis, i}, 1.0);
    const auto dyfi_dp = torch::autograd::grad({yf},
                                               {p},
                                               {v},
                                               /*retain_graph=*/true,
                                               /*create_graph=*/false,
                                               /*allow_unused=*/false)[0];
    if (dyfi_dp.defined())
      dyf_dp.base_index_put({i, torch::indexing::Ellipsis}, dyfi_dp);
  }

  // Reshape the derivative back to the correct shape
  const auto dy_dp = BatchTensor(
      dyf_dp.reshape(utils::add_shapes(y.batch_sizes(), y.base_sizes(), p.base_sizes())),
      y.batch_dim());

  return dy_dp;
}

BatchTensor
base_diag_embed(const BatchTensor & a, TorchSize offset, TorchSize d1, TorchSize d2)
{
  return BatchTensor(
      torch::diag_embed(
          a, offset, d1 < 0 ? d1 : d1 + a.batch_dim() + 1, d2 < 0 ? d2 : d2 + a.batch_dim() + 1),
      a.batch_dim());
}

SR2
skew_and_sym_to_sym(const SR2 & e, const WR2 & w)
{
  // In NEML we used an unrolled form, I don't think I ever found
  // a nice direct notation for this one
  auto E = R2(e);
  auto W = R2(w);
  return SR2(W * E - E * W);
}

SSR4
d_skew_and_sym_to_sym_d_sym(const WR2 & w)
{
  auto I = R2::identity(w.options());
  auto W = R2(w);
  return SSR4(R4(torch::einsum("...ia,...jb->...ijab", {W, I}) -
                 torch::einsum("...ia,...bj->...ijab", {I, W})));
}

SWR4
d_skew_and_sym_to_sym_d_skew(const SR2 & e)
{
  auto I = R2::identity(e.options());
  auto E = R2(e);
  return SWR4(R4(torch::einsum("...ia,...bj->...ijab", {I, E}) -
                 torch::einsum("...ia,...jb->...ijab", {E, I})));
}

WR2
multiply_and_make_skew(const SR2 & a, const SR2 & b)
{
  auto A = R2(a);
  auto B = R2(b);

  return WR2(A * B - B * A);
}

WSR4
d_multiply_and_make_skew_d_first(const SR2 & b)
{
  auto I = R2::identity(b.options());
  auto B = R2(b);
  return WSR4(R4(torch::einsum("...ia,...bj->...ijab", {I, B}) -
                 torch::einsum("...ia,...jb->...ijab", {B, I})));
}

WSR4
d_multiply_and_make_skew_d_second(const SR2 & a)
{
  auto I = R2::identity(a.options());
  auto A = R2(a);
  return WSR4(R4(torch::einsum("...ia,...jb->...ijab", {A, I}) -
                 torch::einsum("...ia,...bj->...ijab", {I, A})));
}

namespace linalg
{
BatchTensor
vector_norm(const BatchTensor & v)
{
  neml_assert_dbg(v.base_dim() == 0 || v.base_dim() == 1,
                  "v in vector_norm has base dimension ",
                  v.base_dim(),
                  " instead of 0 or 1.");

  // If the vector is a logical scalar just return its absolute value
  if (v.base_dim() == 0)
    return math::abs(v);

  return BatchTensor(torch::linalg::vector_norm(
                         v, /*order=*/2, /*dim=*/-1, /*keepdim=*/false, /*dtype=*/c10::nullopt),
                     v.batch_dim());
}

BatchTensor
solve(const BatchTensor & A, const BatchTensor & B)
{
  return BatchTensor(torch::linalg::solve(A, B, /*left=*/true), A.batch_dim());
}

std::tuple<BatchTensor, BatchTensor>
lu_factor(const BatchTensor & A, bool pivot)
{
  auto [LU, pivots] = torch::linalg_lu_factor(A, pivot);
  return {BatchTensor(LU, A.batch_dim()), BatchTensor(pivots, A.batch_dim())};
}

BatchTensor
lu_solve(const BatchTensor & LU,
         const BatchTensor & pivots,
         const BatchTensor & B,
         bool left,
         bool adjoint)
{
  return BatchTensor(torch::linalg_lu_solve(LU, pivots, B, left, adjoint), B.batch_dim());
}
} // namespace linalg
} // namespace math
} // namespace neml2
