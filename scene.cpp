#include "scene.h"

using namespace base::scene_graph;

scene::scene(const std::string& name) :
  _name(name),
  _pRoot(game_object::create())
{}

std::weak_ptr<game_object> scene::create_game_object()
{
  return game_object::create(_pRoot);
}

void scene::tick()
{
  _pRoot->tick(component::call_type::prepare);

  _pRoot->tick(component::call_type::awake);
  _pRoot->tick(component::call_type::enable);
  _pRoot->tick(component::call_type::start);
  _pRoot->tick(component::call_type::update);
  _pRoot->tick(component::call_type::pre_render);
  _pRoot->tick(component::call_type::render);
  _pRoot->tick(component::call_type::late_update);
  _pRoot->tick(component::call_type::disable);
  _pRoot->tick(component::call_type::destory);

  _pRoot->tick(component::call_type::clear);
}