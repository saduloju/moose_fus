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

#include <catch2/catch.hpp>

#include "utils.h"
#include "neml2/tensors/tensors.h"
#include "neml2/misc/math.h"

using namespace neml2;

TEST_CASE("WR2", "[tensors]")
{
  torch::manual_seed(42);
  const auto & DTO = default_tensor_options();

  TorchShape B = {5, 3, 1, 2}; // batch shape

  SECTION("class WR2")
  {
    SECTION("WR2")
    {
      SECTION("from R2")
      {
        auto S = R2::fill(0.0, 0.1, 3.4, -0.1, 0.0, -1.2, -3.4, 1.2, 0.0);
        auto s = WR2::fill(1.2, 3.4, -0.1);
        REQUIRE(torch::allclose(WR2(S), s));
      }
    }

    SECTION("fill")
    {
      SECTION("fill from 3 values")
      {
        auto a1 = WR2::fill(1.2, 3.4, -0.1, DTO);
        auto a2 = WR2::fill(Scalar(1.2, DTO), Scalar(3.4, DTO), Scalar(-0.1, DTO));
        auto a3 = WR2::fill(
            Scalar::full(B, 1.2, DTO), Scalar::full(B, 3.4, DTO), Scalar::full(B, -0.1, DTO));
        auto b = R2(torch::tensor({{0.0, 0.1, 3.4}, {-0.1, 0.0, -1.2}, {-3.4, 1.2, 0.0}}, DTO));
        REQUIRE(torch::allclose(R2(a1), b));
        REQUIRE(torch::allclose(R2(a2), b));
        REQUIRE(torch::allclose(R2(a3), b.batch_expand(B)));
      }
    }

    SECTION("identity_map")
    {
      auto I = WR2::identity_map(DTO);
      auto a = WR2(torch::rand(utils::add_shapes(B, 3), DTO));

      auto apply = [](const BatchTensor & x) { return x; };
      auto da_da = finite_differencing_derivative(apply, a);

      REQUIRE(torch::allclose(I, da_da));
    }

    auto r = Rot::fill(0.13991834, 0.18234513, 0.85043991);
    auto w = WR2::fill(-0.2, 0.012, 0.15);
    auto W = R2(w);

    auto rb = r.batch_expand(B);
    auto wb = w.batch_expand(B);
    auto Wb = R2(wb);

    auto w0 = WR2::fill(0, 0, 0);
    auto w0b = w0.batch_expand(B);

    SECTION("rotate")
    {
      std::cout << w.rotate(r) << std::endl;
      std::cout << WR2(W.rotate(r)) << std::endl;

      REQUIRE(torch::allclose(w.rotate(r), WR2(W.rotate(r))));
      REQUIRE(torch::allclose(wb.rotate(rb), WR2(Wb.rotate(rb))));
    }

    SECTION("drotate")
    {
      // Rodrigues vector
      auto apply_r = [w](const BatchTensor & x) { return w.rotate(Rot(x)); };
      auto dwp_dr = finite_differencing_derivative(apply_r, r);
      auto dwp_drb = dwp_dr.batch_expand(B);

      REQUIRE(torch::allclose(w.drotate(r), dwp_dr, 1e-4));
      REQUIRE(torch::allclose(wb.drotate(rb), dwp_drb, 1e-4));

      // Rotation matrix
      auto R = R2(r);
      auto Rb = R2(rb);
      auto apply_R = [w](const BatchTensor & x) { return w.rotate(R2(x)); };
      auto dwp_dR = finite_differencing_derivative(apply_R, R);
      auto dwp_dRb = dwp_dR.batch_expand(B);

      REQUIRE(torch::allclose(w.drotate(R), dwp_dR));
      REQUIRE(torch::allclose(wb.drotate(Rb), dwp_dRb));
      REQUIRE(torch::allclose(w.drotate(Rb), dwp_dRb));
      REQUIRE(torch::allclose(wb.drotate(R), dwp_dRb));
    }

    SECTION("exp")
    {
      SECTION("correct definition")
      {
        auto correct = R2::fill(0.98873698,
                                -0.14963255,
                                -0.00304675,
                                0.14724505,
                                0.9689128,
                                0.19881371,
                                -0.02679696,
                                -0.19702309,
                                0.98003256);

        REQUIRE(torch::allclose(R2(w.exp()), correct, 1e-4, 1e-4));
        REQUIRE(torch::allclose(R2(wb.exp()), correct.batch_expand(B), 1e-4, 1e-4));
      }

      SECTION("zero maps to zero")
      {
        REQUIRE(torch::allclose(w0.exp(), Rot::identity(DTO)));
        REQUIRE(torch::allclose(w0b.exp(), Rot::identity(DTO).batch_expand(B)));
      }
    }

    SECTION("dexp")
    {
      auto apply = [](const WR2 & x) { return x.exp(); };
      SECTION("standard values")
      {
        auto dnum = finite_differencing_derivative(apply, w);

        REQUIRE(torch::allclose(dnum, w.dexp()));
        REQUIRE(torch::allclose(dnum.batch_expand(B), wb.dexp()));
      }

      SECTION("zero values")
      {
        auto dnum = finite_differencing_derivative(apply, w0);

        REQUIRE(torch::allclose(dnum, w0.dexp()));
        REQUIRE(torch::allclose(dnum.batch_expand(B), w0b.dexp()));
      }

      SECTION("small values")
      {
        auto ws = WR2::fill(1.0e-12, 0, 0);
        auto wsb = w0.batch_expand(B);

        auto dnum = finite_differencing_derivative(apply, ws);
        REQUIRE(torch::allclose(dnum, ws.dexp()));
        REQUIRE(torch::allclose(dnum.batch_expand(B), wsb.dexp()));
      }
    }

    SECTION("operator()")
    {
      using namespace torch::indexing;
      auto a = WR2(torch::rand(utils::add_shapes(B, 3), DTO));
      auto b = R2(a);
      for (TorchSize i = 0; i < 3; i++)
        for (TorchSize j = 0; j < 3; j++)
          REQUIRE(torch::allclose(a(i, j), b(i, j)));
    }
  }

  SECTION("abmba")
  {
    auto a = SR2(torch::rand({6}, DTO));
    auto b = SR2(torch::rand({6}, DTO));
    SECTION("product")
    {
      REQUIRE(
          torch::allclose(R2(math::multiply_and_make_skew(a, b)), R2(a) * R2(b) - R2(b) * R2(a)));
    }
    SECTION("derivatives")
    {
      SECTION("da")
      {
        auto apply1 = [b](const SR2 & x) { return math::multiply_and_make_skew(x, b); };
        auto d1 = finite_differencing_derivative(apply1, a);
        REQUIRE(torch::allclose(math::d_multiply_and_make_skew_d_first(b), d1));
      }

      SECTION("db")
      {
        auto apply2 = [a](const SR2 & x) { return math::multiply_and_make_skew(a, x); };
        auto d2 = finite_differencing_derivative(apply2, b);
        REQUIRE(torch::allclose(math::d_multiply_and_make_skew_d_second(a), d2));
      }
    }
  }
}
