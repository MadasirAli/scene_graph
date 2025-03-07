#pragma once

#include <memory>

namespace base
{
  namespace scene_graph {
    class component
    {
      friend class game_object;

    private:
      enum class state_change {
        none,
        enable,
        disable
      };
    public:
      enum class call_type {
        prepare,
        enable,
        awake,
        start,
        update,
        pre_render,
        render,
        late_update,
        disable,
        destory,
        clear
      };

    public:
      component(const component&) = delete;
      component& operator=(const component&) = delete;

    protected:
      component();

    protected:
      virtual void on_enable() {}
      virtual void awake() = 0;
      virtual void start() {}
      virtual void update() {}
      virtual void pre_render() {}
      virtual void on_render() {}
      virtual void late_update() {}
      virtual void on_disable() {}
      virtual void on_destroy() {}

    public:
      std::weak_ptr<game_object> get_game_object() const { return p_pGameObject; }
      void enable();
      void disable();

    private:
      void clear_pending_changes();

    private:
      uint64_t _id;

      std::weak_ptr<game_object> p_pGameObject;

      bool _active = true;
      state_change _state_change = state_change::none;
      state_change _call_record_state_change = state_change::none;

      bool _start_called = false;
    };
  }
}

