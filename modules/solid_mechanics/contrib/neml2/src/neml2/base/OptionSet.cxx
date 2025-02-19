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

#include "neml2/base/OptionSet.h"

namespace neml2
{
bool
OptionSet::contains(const std::string & name) const
{
  OptionSet::const_iterator it = _values.find(name);
  if (it != _values.end())
    return true;
  return false;
}

const OptionSet::OptionBase &
OptionSet::get(const std::string & name) const
{
  neml_assert(this->contains(name),
              "ERROR: no option named \"",
              name,
              "\" found.\n\nKnown options:\n",
              *this);

  return *_values.at(name);
}

OptionSet::OptionBase &
OptionSet::set(const std::string & name)
{
  neml_assert(this->contains(name),
              "ERROR: no option named \"",
              name,
              "\" found.\n\nKnown options:\n",
              *this);

  return *_values[name];
}

void
OptionSet::clear()
{
  _values.clear();
}

OptionSet &
OptionSet::operator=(const OptionSet & source)
{
  this->OptionSet::clear();
  *this += source;
  this->_metadata = source._metadata;
  return *this;
}

void
OptionSet::operator+=(const OptionSet & source)
{
  for (const auto & [key, value] : source._values)
    _values[key] = value->clone();
}

OptionSet::OptionSet(const OptionSet & p) { *this = p; }

// LCOV_EXCL_START
void
OptionSet::print(std::ostream & os) const
{
  OptionSet::const_iterator it = _values.begin();

  while (it != _values.end())
  {
    os << "    - " << it->first << ":\n";
    os << "        type: " << it->second->type() << '\n';
    os << "        value: ";
    it->second->print(os);
    if (++it != _values.end())
      os << '\n';
  }
}

std::ostream &
operator<<(std::ostream & os, const OptionSet & p)
{
  p.print(os);
  return os;
}
// LCOV_EXCL_STOP

OptionSet::iterator
OptionSet::begin()
{
  return _values.begin();
}

OptionSet::const_iterator
OptionSet::begin() const
{
  return _values.begin();
}

OptionSet::iterator
OptionSet::end()
{
  return _values.end();
}

OptionSet::const_iterator
OptionSet::end() const
{
  return _values.end();
}
} // namespace neml2
