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

#include <sstream>
#include <iostream>

namespace neml2
{
class NEMLException : public std::exception
{
public:
  NEMLException(const std::string & msg)
    : _msg(msg)
  {
  }

  virtual const char * what() const noexcept;

private:
  std::string _msg;
};

template <typename... Args>
void neml_assert(bool assertion, Args &&... args);

template <typename... Args>
void neml_assert_dbg(bool assertion, Args &&... args);

namespace internal
{
template <typename T, typename... Args>
void stream_all(std::ostringstream & ss, T && val, Args &&... args);

void stream_all(std::ostringstream & ss);
} // namespace internal

template <typename... Args>
void
neml_assert(bool assertion, Args &&... args)
{
  if (!assertion)
  {
    std::ostringstream oss;
    internal::stream_all(oss, std::forward<Args>(args)...);
    throw NEMLException(oss.str().data());
  }
}

template <typename... Args>
void
neml_assert_dbg([[maybe_unused]] bool assertion, [[maybe_unused]] Args &&... args)
{
#ifndef NDEBUG
  neml_assert(assertion, args...);
#endif
}

namespace internal
{
template <typename T, typename... Args>
void
stream_all(std::ostringstream & ss, T && val, Args &&... args)
{
  ss << val;
  stream_all(ss, std::forward<Args>(args)...);
}
} // namespace internal
} // namespace neml2

///////////////////////////////////////////////////////////////////////////////
// Implementations
///////////////////////////////////////////////////////////////////////////////
