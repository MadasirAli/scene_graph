#include "game_object.h"

#include "id.h"

using namespace base::scene_graph;

void game_object::remove_child(uint64_t id)
{
  for (auto& pChildPair : _pChildren) {
    if (pChildPair.first->_id != id)
      continue;

    pChildPair.second = state::removed;
    return;
  }

  assert(false);
}

void game_object::add_child(std::shared_ptr<game_object> pObject)
{
  _pChildren_to_add.emplace_back(move(pObject));
}

void game_object::set_parent(std::weak_ptr<game_object> pParent)
{
  assert(false == pParent.expired());

  pParent.lock()->add_child(shared_from_this());

  if (_pParent.expired() == false) {
    _pParent.lock()->remove_child(_id);
  }
}

std::weak_ptr<game_object> game_object::get_child(size_t index)
{
  return _pChildren[index].first;
}

void game_object::enable()
{
  _state_change = component::state_change::enable;
}

void game_object::disable()
{
  _state_change = component::state_change::disable;
}

void game_object::clear_pending_changes()
{
  // finish children to add
  // finish components to add

  for (auto& pChild : _pChildren_to_add)
  {
    auto thisPtr = shared_from_this();

    // if same entries exists
    if (pChild->_pParent.expired() == false) {
      if (_id == pChild->_pParent.lock()->_id) {
        continue;
      }
    }

    pChild->_pParent = thisPtr;
    _pChildren.emplace_back(pChild, state::alive);
  }
  _pChildren_to_add.clear();

  for (auto& pComponent : _pComponents_to_add)
  {
    _pComponents.emplace_back(pComponent, state::alive);

    _call_record.added_component_entries.push_back(
      std::pair<component::call_type, std::shared_ptr<component>>(component::call_type::start, pComponent));
  }
  _pComponents_to_add.clear();

  // finish children to remove
  // finish components to remove

  for (auto it = _pChildren.begin(); it != _pChildren.end(); ) {
    if (it->second == state::removed) {
      it = _pChildren.erase(it);
    }
    else {
      ++it;
    }
  }

  for (auto it = _pComponents.cbegin(); it != _pComponents.cend();) {
    if (it->second == state::removed) {

      _call_record.removed_component_entries.push_back(
        std::pair<component::call_type, std::shared_ptr<component>>(component::call_type::destory, it->first));

      it = _pComponents.erase(it);
    }
    else {
      ++it;
    }
  }

  // finish state change

  if (false == _pParent.expired()) {
    _active_in_hierarchy = _pParent.lock()->_active;

    if (_marked_for_destroy == false) {
      _marked_for_destroy = _pParent.lock()->_marked_for_destroy;
    }
  }

  if (component::state_change::disable == _state_change && true == _active) {
    _active = false;

    if (_active_in_hierarchy) {
      _call_record.state_changed = component::state_change::disable;
    }
    // change active in hierarchy for childs
  }
  else if (component::state_change::enable == _state_change && false == _active) {
    _active = true;

    if (_active_in_hierarchy) {
      _call_record.state_changed = component::state_change::enable;
    }
    // change active in hierarchy for childs
  }

  if (_marked_for_destroy) {
    _call_record.get_destroyed = true;

    auto pParent = _pParent.lock();
    pParent->remove_child(_id);
  }

  for (auto& pComponentPair : _pComponents)
  {
    pComponentPair.first->clear_pending_changes();
  }


  _state_change = component::state_change::none;
}

void game_object::tick(component::call_type type)
{
  switch (type)
  {
  case component::call_type::prepare:
  {
    clear_pending_changes();
  }
  break;

  case component::call_type::awake:
  {
    if (component::state_change::enable == _call_record.state_changed) {
      for (auto& pComponentPair : _pComponents)
      {
        pComponentPair.first->awake();
      }
    }
  }
  break;

  case component::call_type::enable:
  {
    if (component::state_change::enable == _call_record.state_changed) {
      for (auto& pComponentPair : _pComponents)
      {
        pComponentPair.first->on_enable();
      }
    }
    else if (component::state_change::none == _call_record.state_changed) {
      for (auto& pComponentPair : _pComponents)
      {
        if (component::state_change::enable == pComponentPair.first->_call_record_state_change) {
          pComponentPair.first->on_enable();
        }
      }
    }
  }
  break;

  case component::call_type::start:
  {
    if (false == _active || false == _active_in_hierarchy)
      break;

    for (auto& pComponent : _call_record.added_component_entries)
    {
      pComponent.second->_start_called = true;
      pComponent.second->start();
    }

    for (auto& pComponentPair : _pComponents)
    {
      if (false == pComponentPair.first->_start_called) {
        pComponentPair.first->_start_called = true;
        pComponentPair.first->start();
      }
    }
  }
  break;

  case component::call_type::update:
  {
    if (false == _active || false == _active_in_hierarchy)
      break;

    for (auto& pComponentPair : _pComponents)
    {
      pComponentPair.first->update();
    }
  }
  break;

  case component::call_type::pre_render:
  {
    if (false == _active || false == _active_in_hierarchy)
      break;

    for (auto& pComponentPair : _pComponents)
    {
      pComponentPair.first->pre_render();
    }
  }
  break;

  case component::call_type::render:
  {
    if (false == _active || false == _active_in_hierarchy)
      break;

    for (auto& pComponentPair : _pComponents)
    {
      pComponentPair.first->on_render();
    }
  }
  break;

  case component::call_type::late_update:
  {
    if (false == _active || false == _active_in_hierarchy)
      break;

    for (auto& pComponentPair : _pComponents)
    {
      pComponentPair.first->late_update();
    }
  }
  break;

  case component::call_type::disable:
  {
    if (component::state_change::disable == _call_record.state_changed) {
      for (auto& pComponentPair : _pComponents)
      {
        pComponentPair.first->on_disable();
      }
    }
    else if (component::state_change::none == _call_record.state_changed) {
      for (auto& pComponentPair : _pComponents) {
        if (component::state_change::disable == pComponentPair.first->_call_record_state_change) {
          pComponentPair.first->disable();
        }
      }
    }
  }
  break;

  case component::call_type::destory:
  {
    if (_call_record.get_destroyed) {
      for (auto& pComponentPair : _pComponents) {
        pComponentPair.first->on_destroy();
      }
    }
    else {
      for (auto& pComponent : _call_record.removed_component_entries) {
        if (component::call_type::destory == pComponent.first) {
          pComponent.second->on_destroy();
        }
      }
    }
  }
  break;

  case component::call_type::clear:
  {
    _call_record.clear();
  }
  break;


  default:
    assert(false);
    break;
  }

  for (auto& pChildPair : _pChildren)
  {
    pChildPair.first->tick(type);
  }
}

void game_object::destroy()
{
  assert(_pParent.expired() == false);

  _marked_for_destroy = true;
}

game_object::game_object() :
  _id(get_id()),
  tag("Untagged"),
  layer(0)
{}