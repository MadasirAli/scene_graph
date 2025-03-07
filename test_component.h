#pragma once

#include "component.h"

namespace base {
  namespace scene_graph {
    class test_component : public component
    {
    private:
      void awake() override;
      void on_enable() override;
      void start() override;
      void update() override;
      void pre_render() override;
      void on_render() override;
      void late_update() override;
      void on_disable() override;
      void on_destroy() override;
    };
  }
}
