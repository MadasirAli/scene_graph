#pragma once

#include <string>

#include "game_object.h"

namespace base {
  namespace scene_graph {

    class scene
    {
    public:
      scene(const std::string& name);
      ~scene() noexcept = default;

      scene(const scene&) = delete;
      scene& operator=(const scene) = delete;

      std::weak_ptr<game_object> create_game_object();
      void tick();

      const std::string& get_name() const {
        return _name;
      }

    private:
      std::string _name;
      std::shared_ptr<game_object> _pRoot;
    };
  }
}

