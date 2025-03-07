#pragma once

#include <string>
#include <memory>
#include <vector>
#include <assert.h>
#include <utility>

#include "component.h"

namespace base {
  namespace scene_graph {

    class game_object : public std::enable_shared_from_this<game_object>
    {
      friend class scene;

    private:
      enum class state {
        alive,
        removed
      };

      class call_record {
      public:
        void clear()
        {
          added_component_entries.clear();
          removed_component_entries.clear();

          state_changed = component::state_change::none;
          get_destroyed = false;
        }

      public:
        std::vector<std::pair<component::call_type, std::shared_ptr<component>>> added_component_entries;
        std::vector<std::pair<component::call_type, std::shared_ptr<component>>> removed_component_entries;

        component::state_change state_changed = component::state_change::none;
        bool get_destroyed = false;
      };

    public:
      ~game_object() noexcept = default;

      game_object(const game_object&) = delete;
      game_object& operator=(const game_object&) = delete;

      static std::weak_ptr<game_object> create(std::weak_ptr<game_object> pParent) {
        auto ptr = std::shared_ptr<game_object>(new game_object());
        ptr->set_parent(pParent);

        return ptr;
      }

      template<typename T>
      std::weak_ptr<T> add_component()
      {
        using namespace std;
        static_assert(is_base_of<component, T>::value, "T must derive from component");

        auto pTComponent = make_shared<T>();
        auto pComponent = dynamic_pointer_cast<component>(pTComponent);

        pComponent->p_pGameObject = weak_from_this();
        pComponent->awake();
        _pComponents_to_add.emplace_back(move(pComponent));

        return pTComponent;
      }

      template<typename T>
      void remove_component()
      {
        using namespace std;

        static_assert(is_base_of<component, T>::value, "T must derive from component");
        for (auto& pComponentPair : _pComponents)
        {
          if (dynamic_pointer_cast<T>(pComponentPair.first))
          {
            pComponentPair.second = state::removed;
            return;
          }
        }

        assert(false);
      }

      template<typename T>
      std::weak_ptr<T> get_component()
      {
        using namespace std;

        static_assert(is_base_of<component, T>::value, "T must derive from component");
        for (const auto& pComponentPair : _pComponents)
        {
          auto pComponent = dynamic_pointer_cast<T>(pComponentPair.first);
          if (pComponent)
          {
            return pComponent;
          }
        }

        return weak_ptr<T>();
      }

      template<typename T>
      std::vector<std::weak_ptr<T>> get_components() {

        using namespace std;
        static_assert(is_base_of<component, T>::value, "T must derive from component");

        vector<weak_ptr<T>> result;

        for (const auto& pComponentPair : _pComponents)
        {
          auto pComponent = dynamic_pointer_cast<T>(pComponentPair.first);
          if (pComponent)
          {
            result.emplace_back(move(pComponent));
          }
        }

        return result;
      }

      void set_parent(std::weak_ptr<game_object> pParent);
      std::weak_ptr<game_object> get_child(size_t index);

      void enable();
      void disable();
      bool is_active() const {
        return _active;
      }
      size_t get_child_count() {
        return _pChildren.size();
      }

      void destroy();

    private:
      game_object();

      static std::shared_ptr<game_object> create() {
        return std::shared_ptr<game_object>(new game_object());
      }

      void clear_pending_changes();
      void tick(component::call_type type);

      void remove_child(uint64_t id);
      void add_child(std::shared_ptr<game_object> pObject);

    private:
      uint64_t _id;

      call_record _call_record;

      bool _active_in_hierarchy = true;
      bool _active = true;
      component::state_change _state_change = component::state_change::none;

      std::weak_ptr<game_object> _pParent;

      std::vector<std::pair
        <std::shared_ptr<game_object>, state>> _pChildren;

      std::vector<std::pair
        <std::shared_ptr<component>, state>> _pComponents;

      std::vector<std::shared_ptr<component>> _pComponents_to_add;
      std::vector<std::shared_ptr<game_object>> _pChildren_to_add;

      bool _marked_for_destroy = false;

    public:
      std::string name;
      std::string tag;
      uint32_t layer;
    };
  }
}