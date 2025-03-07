#include "test_component.h"

#include <iostream>

using namespace base::scene_graph;
using namespace std;

void base::scene_graph::test_component::awake()
{
  cout << "Awake" << endl;
}

void base::scene_graph::test_component::on_enable()
{
  cout << "On Enable" << endl;
}

void base::scene_graph::test_component::start()
{
  cout << "Start" << endl;
}

void base::scene_graph::test_component::update()
{
  cout << "Update" << endl;
}

void base::scene_graph::test_component::pre_render()
{
  cout << "Pre Render" << endl;
}

void base::scene_graph::test_component::on_render()
{
  cout << "On Render" << endl;
}

void base::scene_graph::test_component::late_update()
{
  cout << "Late Update" << endl;
}

void base::scene_graph::test_component::on_disable()
{
  cout << "On Disable" << endl;
}

void base::scene_graph::test_component::on_destroy()
{
  cout << "On Destroy" << endl;
}
