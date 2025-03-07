#include "component.h"

#include "id.h"
#include "game_object.h"

using namespace base::scene_graph;

component::component() :
  _id(get_id())
{}

void component::enable()
{
  _state_change = component::state_change::enable;
}

void component::disable()
{
  _state_change = component::state_change::disable;
}

void component::clear_pending_changes()
{
  _call_record_state_change = state_change::none;

  bool activeInHierarchy = p_pGameObject.lock()->is_active();

  if (state_change::disable == _state_change && true == _active) {
    _active = false;

    if (activeInHierarchy) {
      _call_record_state_change = state_change::disable;
    }
  }
  else if (state_change::enable == _state_change && false == _active) {
    _active = true;

    if (activeInHierarchy) {
      _call_record_state_change = state_change::enable;
    }
  }
  _state_change = state_change::none;
}
