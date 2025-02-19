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

TEST_CASE("SR2", "[tensors]")
{
  torch::manual_seed(42);
  const auto & DTO = default_tensor_options();

  TorchShape B = {5, 3, 1, 2}; // batch shape

  SECTION("class SR2")
  {
    SECTION("SR2")
    {
      SECTION("from R2")
      {
        auto S = R2::fill(1.1, 1.2, 1.3, 1.2, 2.2, 2.3, 1.3, 2.3, 3.3);
        auto s = SR2::fill(1.1, 2.2, 3.3, 2.3, 1.3, 1.2);
        REQUIRE(torch::allclose(SR2(S), s));
      }
    }

    SECTION("fill")
    {
      SECTION("fill from 1 value")
      {
        auto a1 = SR2::fill(1.1, DTO);
        auto a2 = SR2::fill(Scalar(1.1, DTO));
        auto a3 = SR2::fill(Scalar::full(B, 1.1, DTO));
        auto b = R2(torch::tensor({{1.1, 0.0, 0.0}, {0.0, 1.1, 0.0}, {0.0, 0.0, 1.1}}, DTO));
        REQUIRE(torch::allclose(R2(a1), b));
        REQUIRE(torch::allclose(R2(a2), b));
        REQUIRE(torch::allclose(R2(a3), b.batch_expand(B)));
      }
      SECTION("fill from 3 values")
      {
        auto a1 = SR2::fill(1.1, 2.2, 3.3, DTO);
        auto a2 = SR2::fill(Scalar(1.1, DTO), Scalar(2.2, DTO), Scalar(3.3, DTO));
        auto a3 = SR2::fill(
            Scalar::full(B, 1.1, DTO), Scalar::full(B, 2.2, DTO), Scalar::full(B, 3.3, DTO));
        auto b = R2(torch::tensor({{1.1, 0.0, 0.0}, {0.0, 2.2, 0.0}, {0.0, 0.0, 3.3}}, DTO));
        REQUIRE(torch::allclose(R2(a1), b));
        REQUIRE(torch::allclose(R2(a2), b));
        REQUIRE(torch::allclose(R2(a3), b.batch_expand(B)));
      }
      SECTION("fill from 6 values")
      {
        auto a1 = SR2::fill(1.1, 2.2, 3.3, 2.3, 1.3, 1.2, DTO);
        auto a2 = SR2::fill(Scalar(1.1, DTO),
                            Scalar(2.2, DTO),
                            Scalar(3.3, DTO),
                            Scalar(2.3, DTO),
                            Scalar(1.3, DTO),
                            Scalar(1.2, DTO));
        auto a3 = SR2::fill(Scalar::full(B, 1.1, DTO),
                            Scalar::full(B, 2.2, DTO),
                            Scalar::full(B, 3.3, DTO),
                            Scalar::full(B, 2.3, DTO),
                            Scalar::full(B, 1.3, DTO),
                            Scalar::full(B, 1.2, DTO));
        auto b = R2(torch::tensor({{1.1, 1.2, 1.3}, {1.2, 2.2, 2.3}, {1.3, 2.3, 3.3}}, DTO));
        REQUIRE(torch::allclose(R2(a1), b));
        REQUIRE(torch::allclose(R2(a2), b));
        REQUIRE(torch::allclose(R2(a3), b.batch_expand(B)));
      }
    }

    SECTION("identity")
    {
      auto a = SR2::identity(DTO);
      auto b = torch::eye(3, DTO);
      REQUIRE(torch::allclose(R2(a), b));
    }

    SECTION("identity_map")
    {
      auto I = SR2::identity_map(DTO);
      auto a = SR2(torch::rand(utils::add_shapes(B, 6), DTO));

      auto apply = [](const BatchTensor & x) { return x; };
      auto da_da = finite_differencing_derivative(apply, a);

      REQUIRE(torch::allclose(I, da_da));
    }

    auto r = Rot::fill(0.13991834, 0.18234513, 0.85043991);
    auto T = SR2(R2::fill(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0));
    auto Tp = SR2::fill(-1.02332, 0.208734, 15.8146, -1.86545, -2.71806, 0.190785);

    auto rb = r.batch_expand(B);
    auto Tb = T.batch_expand(B);
    auto Tpb = Tp.batch_expand(B);

    SECTION("rotate")
    {
      REQUIRE(torch::allclose(T.rotate(r), Tp));
      REQUIRE(torch::allclose(Tb.rotate(rb), Tpb));
      REQUIRE(torch::allclose(T.rotate(rb), Tpb));
      REQUIRE(torch::allclose(Tb.rotate(r), Tpb));
    }

    SECTION("drotate")
    {
      // Rodrigues vector
      auto apply_r = [T](const BatchTensor & x) { return T.rotate(Rot(x)); };
      auto dTp_dr = finite_differencing_derivative(apply_r, r);
      auto dTp_drb = dTp_dr.batch_expand(B);

      REQUIRE(torch::allclose(T.drotate(r), dTp_dr, 1e-4));
      REQUIRE(torch::allclose(Tb.drotate(rb), dTp_drb, 1e-4));
      REQUIRE(torch::allclose(T.drotate(rb), dTp_drb, 1e-4));
      REQUIRE(torch::allclose(Tb.drotate(r), dTp_drb, 1e-4));

      // Rotation matrix
      auto R = R2(r);
      auto Rb = R2(rb);
      auto apply_R = [T](const BatchTensor & x) { return T.rotate(R2(x)); };
      auto dTp_dR = finite_differencing_derivative(apply_R, R);
      auto dTp_dRb = dTp_dR.batch_expand(B);

      REQUIRE(torch::allclose(T.drotate(R), dTp_dR));
      REQUIRE(torch::allclose(Tb.drotate(Rb), dTp_dRb));
      REQUIRE(torch::allclose(T.drotate(Rb), dTp_dRb));
      REQUIRE(torch::allclose(Tb.drotate(R), dTp_dRb));
    }

    SECTION("operator()")
    {
      using namespace torch::indexing;
      auto a = SR2(torch::rand(utils::add_shapes(B, 6), DTO));
      auto b = R2(a);
      for (TorchSize i = 0; i < 3; i++)
        for (TorchSize j = 0; j < 3; j++)
          REQUIRE(torch::allclose(a(i, j), b(i, j)));
    }

    SECTION("tr")
    {
      auto res = Scalar(15.0, DTO);
      REQUIRE(torch::allclose(T.tr(), res));
      REQUIRE(torch::allclose(T.batch_expand(B).tr(), res.batch_expand(B)));
    }

    SECTION("vol")
    {
      auto res = SR2::fill(5.0, DTO);
      REQUIRE(torch::allclose(T.vol(), res));
      REQUIRE(torch::allclose(T.batch_expand(B).vol(), res.batch_expand(B)));
    }

    SECTION("dev")
    {
      auto res = T - T.vol();
      REQUIRE(torch::allclose(T.dev(), res));
      REQUIRE(torch::allclose(T.batch_expand(B).dev(), res.batch_expand(B)));
    }

    SECTION("det")
    {
      auto res = Scalar(0, DTO);
      REQUIRE(torch::allclose(T.det(), res, /*rtol=*/0, /*atol=*/1e-5));
      REQUIRE(
          torch::allclose(T.batch_expand(B).det(), res.batch_expand(B), /*rtol=*/0, /*atol=*/1e-5));
    }

    SECTION("inner")
    {
      auto other = SR2(torch::rand({6}, DTO));
      auto res = Scalar(torch::Tensor(T).dot(other));
      REQUIRE(torch::allclose(T.inner(other), res));
      REQUIRE(torch::allclose(T.batch_expand(B).inner(other), res.batch_expand(B)));
      REQUIRE(torch::allclose(T.inner(other.batch_expand(B)), res.batch_expand(B)));
      REQUIRE(torch::allclose(T.batch_expand(B).inner(other.batch_expand(B)), res.batch_expand(B)));
    }

    SECTION("norm_sq")
    {
      auto res = Scalar(273.0, DTO);
      REQUIRE(torch::allclose(T.norm_sq(), res));
      REQUIRE(torch::allclose(T.batch_expand(B).norm_sq(), res.batch_expand(B)));
    }

    SECTION("norm")
    {
      auto res = Scalar(16.522711641858304, DTO);
      REQUIRE(torch::allclose(T.norm(), res));
      REQUIRE(torch::allclose(T.batch_expand(B).norm(), res.batch_expand(B)));
    }

    SECTION("outer")
    {
      auto other = SR2(torch::rand({6}, DTO));
      auto res = SSR4(torch::Tensor(T).outer(other));
      REQUIRE(torch::allclose(T.outer(other), res));
      REQUIRE(torch::allclose(T.batch_expand(B).outer(other), res.batch_expand(B)));
      REQUIRE(torch::allclose(T.outer(other.batch_expand(B)), res.batch_expand(B)));
      REQUIRE(torch::allclose(T.batch_expand(B).outer(other.batch_expand(B)), res.batch_expand(B)));
    }

    SECTION("inverse")
    {
      // We can use T as it's singular...
      // What's the chance of getting a random singular matrix? We'll see.
      auto a = SR2(torch::rand({6}, DTO));
      auto res = SR2(R2(torch::Tensor(R2(a)).inverse()));
      REQUIRE(torch::allclose(a.inverse(), res));
      REQUIRE(torch::allclose(a.batch_expand(B).inverse(), res.batch_expand(B)));
    }

    SECTION("transpose")
    {
      auto res = SR2(R2(T).transpose());
      REQUIRE(torch::allclose(T.transpose(), res));
      REQUIRE(torch::allclose(T.batch_expand(B).transpose(), res.batch_expand(B)));
    }

    SECTION("wemew")
    {
      auto w = WR2(torch::rand({3}, DTO));
      SECTION("product")
      {
        REQUIRE(
            torch::allclose(R2(math::skew_and_sym_to_sym(T, w)), R2(w) * R2(T) - R2(T) * R2(w)));
      }
      SECTION("derivatives")
      {
        SECTION("de")
        {
          auto apply1 = [w](const SR2 & x) { return math::skew_and_sym_to_sym(x, w); };
          auto d1 = finite_differencing_derivative(apply1, T);
          REQUIRE(torch::allclose(math::d_skew_and_sym_to_sym_d_sym(w), d1));
        }

        SECTION("dw")
        {
          auto apply2 = [T](const WR2 & x) { return math::skew_and_sym_to_sym(T, x); };
          auto d2 = finite_differencing_derivative(apply2, w);
          REQUIRE(torch::allclose(math::d_skew_and_sym_to_sym_d_skew(T), d2));
        }
      }
    }
  }
}
