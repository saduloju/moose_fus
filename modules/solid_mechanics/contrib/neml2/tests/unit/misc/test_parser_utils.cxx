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

#include "neml2/misc/parser_utils.h"

using namespace neml2;

TEST_CASE("parser_utils", "[misc]")
{
  SECTION("split")
  {
    REQUIRE(utils::split("a/b/c", "/") == std::vector<std::string>{"a", "b", "c"});
    REQUIRE(utils::split("/b/c", "/") == std::vector<std::string>{"b", "c"});
    REQUIRE(utils::split("a/b/", "/") == std::vector<std::string>{"a", "b"});
    REQUIRE(utils::split("/", "/") == std::vector<std::string>{});
  }

  SECTION("trim")
  {
    REQUIRE(utils::trim("a b cde   ") == "a b cde");
    REQUIRE(utils::trim("(123)", "()") == "123");
  }

  SECTION("start_with")
  {
    REQUIRE(utils::start_with("a b cde   ", "a b"));
    REQUIRE(!utils::start_with("abcde", "a b"));
  }

  SECTION("end_with")
  {
    REQUIRE(utils::end_with("a b cde   ", "e   "));
    REQUIRE(!utils::end_with("abcde", "e   "));
  }

  SECTION("parse")
  {
    SECTION("torch::Tensor")
    {
      REQUIRE_THROWS_WITH(utils::parse<torch::Tensor>("1"),
                          Catch::Matchers::Contains("Cannot parse torch::Tensor"));
    }

    SECTION("TorchShape")
    {
      REQUIRE(utils::parse<TorchShape>("(1,2,3,4,5,6)") == TorchShape{1, 2, 3, 4, 5, 6});
      REQUIRE(utils::parse<TorchShape>("(1,2,3)") == TorchShape{1, 2, 3});
      REQUIRE(utils::parse<TorchShape>("(1,2,3,)") == TorchShape{1, 2, 3});
      REQUIRE(utils::parse<TorchShape>("(,1,2,3)") == TorchShape{1, 2, 3});
      REQUIRE(utils::parse<TorchShape>("(,1,2,3,)") == TorchShape{1, 2, 3});
      REQUIRE(utils::parse<TorchShape>("( ,  1, 2, 3 , )") == TorchShape{1, 2, 3});
      REQUIRE(utils::parse<TorchShape>("()") == TorchShape{});
      REQUIRE_THROWS_WITH(
          utils::parse<TorchShape>("1"),
          Catch::Matchers::Contains("a shape must start with '(' and end with ')'"));
    }

    SECTION("bool")
    {
      REQUIRE(utils::parse<bool>("true"));
      REQUIRE(!utils::parse<bool>("false"));
      REQUIRE_THROWS_WITH(utils::parse<bool>("off"),
                          Catch::Matchers::Contains("Failed to parse boolean value"));
    }
  }
}
