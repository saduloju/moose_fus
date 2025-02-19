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

#include "neml2/tensors/LabeledAxis.h"

namespace neml2
{
int LabeledAxis::level = 0;

LabeledAxis::LabeledAxis()
  : _offset(0)
{
}

LabeledAxis::LabeledAxis(const LabeledAxis & other)
  : _variables(other._variables),
    _subaxes(other._subaxes),
    _layout(other._layout),
    _offset(other._offset)
{
}

LabeledAxis &
LabeledAxis::add(const LabeledAxisAccessor & accessor, TorchSize sz)
{
  add(*this, sz, accessor.vec().begin(), accessor.vec().end());
  return *this;
}

void
LabeledAxis::add(LabeledAxis & axis,
                 TorchSize sz,
                 const std::vector<std::string>::const_iterator & cur,
                 const std::vector<std::string>::const_iterator & end) const
{
  if (cur == end - 1)
  {
    if (!axis.has_variable(*cur))
      axis._variables.emplace(*cur, sz);
  }
  else
  {
    axis.add<LabeledAxis>(*cur);
    add(axis.subaxis(*cur), sz, cur + 1, end);
  }
}

LabeledAxis &
LabeledAxis::rename(const std::string & original, const std::string & rename)
{
  // This could be a variable name
  auto var = _variables.find(original);
  if (var != _variables.end())
  {
    auto sz = var->second;
    _variables.erase(var);
    _variables.emplace(rename, sz);
    return *this;
  }

  // or a sub-axis name
  auto subaxis = _subaxes.find(original);
  if (subaxis != _subaxes.end())
  {
    auto axis = subaxis->second;
    _subaxes.erase(subaxis);
    _subaxes.emplace(rename, axis);
    return *this;
  }

  return *this;
}

LabeledAxis &
LabeledAxis::remove(const std::string & name)
{
  // This could be a variable name
  auto count = _variables.erase(name);
  if (count)
    return *this;

  // or a sub-axis name
  count += _subaxes.erase(name);

  // If nothing has been removed, we should probably notify the user.
  neml_assert_dbg(count, "Nothing removed in LabeledAxis::remove, did you mispell the name?");

  return *this;
}

LabeledAxis &
LabeledAxis::clear()
{
  _variables.clear();
  _subaxes.clear();
  _layout.clear();
  _offset = 0;

  return *this;
}

std::vector<LabeledAxisAccessor>
LabeledAxis::merge(LabeledAxis & other)
{
  std::vector<LabeledAxisAccessor> merged_vars;
  merge(other, {}, merged_vars);
  return merged_vars;
}

void
LabeledAxis::merge(LabeledAxis & other,
                   std::vector<std::string> subaxes,
                   std::vector<LabeledAxisAccessor> & merged_vars)
{
  // First merge the variables
  for (const auto & [name, sz] : other._variables)
    if (!has_variable(name))
    {
      _variables.emplace(name, sz);
      auto new_var = subaxes;
      new_var.push_back(name);
      merged_vars.push_back({new_var});
    }

  // Then merge the subaxes
  for (auto & [name, subaxis] : other._subaxes)
  {
    auto found = _subaxes.find(name);
    if (found == _subaxes.end())
      _subaxes.emplace(name, std::make_shared<LabeledAxis>());

    auto new_subaxes = subaxes;
    new_subaxes.push_back(name);
    _subaxes[name]->merge(*subaxis, new_subaxes, merged_vars);
  }
}

void
LabeledAxis::setup_layout()
{
  _offset = 0;
  _layout.clear();

  // First emplace all the variables
  for (auto & [name, sz] : _variables)
  {
    std::pair<TorchSize, TorchSize> range = {_offset, _offset + sz};
    _layout.emplace(name, range);
    _offset += sz;
  }

  // Then subaxes
  for (auto & [name, axis] : _subaxes)
  {
    // Setup the sub-axis if necessary
    axis->setup_layout();
    std::pair<TorchSize, TorchSize> range = {_offset, _offset + axis->storage_size()};
    _layout.emplace(name, range);
    _offset += axis->storage_size();
  }
}

bool
LabeledAxis::has_variable(const LabeledAxisAccessor & var) const
{
  if (var.empty())
    return false;

  if (var.vec().size() > 1)
  {
    if (has_subaxis(var.vec()[0]))
      return subaxis(var.vec()[0]).has_variable(var.slice(1));
    else
      return false;
  }
  else
    return _variables.count(var.vec()[0]);
}

bool
LabeledAxis::has_subaxis(const LabeledAxisAccessor & s) const
{
  if (s.empty())
    return false;

  if (s.vec().size() > 1)
  {
    if (has_subaxis(s.vec()[0]))
      return subaxis(s.vec()[0]).has_subaxis(s.slice(1));
    else
      return false;
  }
  else
    return _subaxes.count(s.vec()[0]);
}

TorchSize
LabeledAxis::storage_size(const LabeledAxisAccessor & accessor) const
{
  return storage_size(accessor.vec().begin(), accessor.vec().end());
}

TorchSize
LabeledAxis::storage_size(const std::vector<std::string>::const_iterator & cur,
                          const std::vector<std::string>::const_iterator & end) const
{
  if (cur == end - 1)
  {
    if (_variables.count(*cur))
      return _variables.at(*cur);
    else if (_subaxes.count(*cur))
      return _subaxes.at(*cur)->storage_size();

    neml_assert_dbg(false, "Trying to find the storage size of a non-existent item named ", *cur);
  }

  return subaxis(*cur).storage_size(cur + 1, end);
}

TorchIndex
LabeledAxis::indices(const LabeledAxisAccessor & accessor) const
{
  if (accessor.empty())
    return torch::indexing::Slice();

  return indices(0, accessor.vec().begin(), accessor.vec().end());
}

TorchIndex
LabeledAxis::indices(TorchSize offset,
                     const std::vector<std::string>::const_iterator & cur,
                     const std::vector<std::string>::const_iterator & end) const
{
  neml_assert_dbg(_layout.count(*cur), "Axis/variable named ", *cur, " does not exist.");
  const auto & [rbegin, rend] = _layout.at(*cur);
  if (cur == end - 1)
    return torch::indexing::Slice(offset + rbegin, offset + rend);

  return subaxis(*cur).indices(offset + rbegin, cur + 1, end);
}

std::vector<std::pair<TorchIndex, TorchIndex>>
LabeledAxis::common_indices(const LabeledAxis & other, bool recursive) const
{
  using namespace torch::indexing;

  std::vector<std::pair<TorchIndex, TorchIndex>> indices;
  std::vector<TorchSize> idxa;
  std::vector<TorchSize> idxb;
  common_indices(other, recursive, idxa, idxb, 0, 0);

  if (idxa.empty())
    return indices;

  // We could be smart and merge contiguous indices
  size_t i = 0;
  size_t j = 1;
  while (j < idxa.size() - 1)
  {
    if (idxa[j] == idxa[j + 1] && idxb[j] == idxb[j + 1])
      j += 2;
    else
    {
      indices.push_back({Slice(idxa[i], idxa[j]), Slice(idxb[i], idxb[j])});
      i = j + 1;
      j = i + 1;
    }
  }
  indices.push_back({Slice(idxa[i], idxa[j]), Slice(idxb[i], idxb[j])});

  return indices;
}

void
LabeledAxis::common_indices(const LabeledAxis & other,
                            bool recursive,
                            std::vector<TorchSize> & idxa,
                            std::vector<TorchSize> & idxb,
                            TorchSize offseta,
                            TorchSize offsetb) const
{
  for (const auto & [name, sz] : _variables)
    if (other.has_variable(name))
    {
      auto && [begina, enda] = _layout.at(name);
      idxa.push_back(offseta + begina);
      idxa.push_back(offseta + enda);
      auto && [beginb, endb] = other._layout.at(name);
      idxb.push_back(offsetb + beginb);
      idxb.push_back(offsetb + endb);
    }

  if (recursive)
    for (const auto & [name, axis] : _subaxes)
      if (other.has_subaxis(name))
        axis->common_indices(other.subaxis(name),
                             true,
                             idxa,
                             idxb,
                             offseta + _layout.at(name).first,
                             offsetb + other._layout.at(name).first);
}

std::vector<std::string>
LabeledAxis::item_names() const
{
  std::vector<std::string> names;
  for (const auto & item : _layout)
    names.push_back(item.first);
  return names;
}

std::set<LabeledAxisAccessor>
LabeledAxis::variable_accessors(bool recursive, const LabeledAxisAccessor & subaxis) const
{
  std::set<LabeledAxisAccessor> accessors;
  variable_accessors(accessors, {}, recursive, subaxis);
  return accessors;
}

void
LabeledAxis::variable_accessors(std::set<LabeledAxisAccessor> & accessors,
                                LabeledAxisAccessor cur,
                                bool recursive,
                                const LabeledAxisAccessor & subaxis) const
{
  for (auto & var : _variables)
  {
    LabeledAxisAccessor var_accessor{{var.first}};
    var_accessor = var_accessor.on(cur);
    if (subaxis.empty())
      accessors.insert(var_accessor);
    else if (var_accessor.slice(0, subaxis.size()) == subaxis)
      accessors.insert(var_accessor);
  }

  if (recursive)
    for (auto & [name, axis] : _subaxes)
    {
      auto next = cur.append(name);
      axis->variable_accessors(accessors, next, recursive, subaxis);
    }
}

const LabeledAxis &
LabeledAxis::subaxis(const std::string & name) const
{
  neml_assert_dbg(
      _subaxes.count(name), "In LabeledAxis::subaxis, no subaxis matches given name ", name);

  return *_subaxes.at(name);
}

LabeledAxis &
LabeledAxis::subaxis(const std::string & name)
{
  neml_assert_dbg(
      _subaxes.count(name), "In LabeledAxis::subaxis, no subaxis matches given name ", name);

  return *_subaxes.at(name);
}

bool
LabeledAxis::equals(const LabeledAxis & other) const
{
  // They must have the same size
  if (_offset != other._offset)
    return false;

  // Comparing unordered maps is easy, two maps are equal if they have the same number of
  // elements and the elements in one container are a permutation of the elements in the other
  // container
  if (_variables != other._variables)
    return false;

  // For subaxes, it's a little bit tricky as we need to compare the dereferenced axes.
  for (auto & [name, axis] : _subaxes)
    if (other._subaxes.count(name) == 0)
      return false;
    else if (*other._subaxes.at(name) != *axis)
      return false;

  return true;
}

std::ostream &
operator<<(std::ostream & os, const LabeledAxis & info)
{
  os << utils::indentation(LabeledAxis::level);
  os << "LabeledAxis [" << info.storage_size() << "] {";
  if (info.nitem() == 0)
  {
    os << "}";
    return os;
  }

  LabeledAxis::level += 1;

  for (const auto & [name, sz] : info._variables)
    os << std::endl << utils::indentation(LabeledAxis::level) << name << ": [" << sz << "]";

  for (const auto & [name, axis] : info._subaxes)
  {
    os << std::endl;
    os << utils::indentation(LabeledAxis::level);
    os << name << ": " << std::endl;
    LabeledAxis::level += 1;
    os << *axis;
    LabeledAxis::level -= 1;
  }
  os << std::endl;
  os << utils::indentation(--LabeledAxis::level) << "}";
  return os;
}

void
LabeledAxis::to_dot(
    std::ostream & os, int & id, std::string axis_name, bool subgraph, bool node_handle) const
{
  // Preemble
  os << (subgraph ? "subgraph " : "graph ");
  os << "cluster_" << id++ << " ";
  os << "{\n";
  os << "label = \"" << axis_name << "\"\n";
  os << "bgcolor = lightgrey\n";

  // The axis should have an invisible node so that I can draw arrows
  if (node_handle)
    os << "\"" << axis_name << "\" [label = \"\", style = invis]\n";

  // Write all the variables
  for (const auto & [name, sz] : _variables)
  {
    os << "\"" << axis_name + " " + name << "\" ";
    os << "[style = filled, color = white, shape = Square, ";
    os << "label = \"" << name + " [" << sz << "]\"]\n";
  }

  // Write all the subaxes
  for (const auto & [name, subaxis] : _subaxes)
    subaxis->to_dot(os, id, axis_name + " " + name, true);

  os << "}\n";
}

bool
operator==(const LabeledAxis & a, const LabeledAxis & b)
{
  return a.equals(b);
}

bool
operator!=(const LabeledAxis & a, const LabeledAxis & b)
{
  return !a.equals(b);
}
} // namespace neml2
