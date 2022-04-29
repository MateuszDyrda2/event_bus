#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

namespace md {
/** Base class for all events */
struct event
{
    event(const std::string& name):
        name(name) { }
    std::string name;
};
/** Base class for event listeners */
class listener
{
  public:
    virtual void on_event(event*) = 0;
};
class event_bus
{
  public:
    using listeners_t = std::unordered_map<std::string, std::vector<listener*>>;
    using lock_t      = std::mutex;
    class event_sink
    {
      public:
        void add_listener(listener* l);
        void remove_listener(listener* l);
        event_sink(const event_sink&) = delete;
        event_sink(event_sink&&)      = delete;
        event_sink& operator=(const event_sink&) noexcept = delete;
        event_sink& operator=(event_sink&&) noexcept = delete;
        inline void operator+=(listener* l)
        {
            add_listener(l);
        }
        inline void operator-=(listener* l)
        {
            remove_listener(l);
        }
        ~event_sink();

      private:
        event_sink(event_bus& b, std::vector<listener*>& l):
            listeners(l), ebus(b) { }

      private:
        friend event_bus;
        std::vector<listener*>& listeners;
        event_bus& ebus;
    };

  public:
    event_bus()  = default;
    ~event_bus() = default;
    void fire(event* e);
    void fire_immediate(event* e);
    void flush();
    event_sink sink(const std::string& event_id);

  private:
    std::vector<event*> events;
    lock_t eventLock;
    listeners_t listeners;
    lock_t listenerLock;
    friend event_sink;
};
event_bus::event_sink event_bus::sink(const std::string& event_id)
{
    listenerLock.lock();
    return event_sink(*this, listeners[event_id]);
}
inline void event_bus::event_sink::add_listener(listener* l)
{
    listeners.push_back(l);
}
inline void event_bus::event_sink::remove_listener(listener* l)
{
    listeners.erase(std::find(listeners.begin(), listeners.end(), l));
}
event_bus::event_sink::~event_sink()
{
    ebus.listenerLock.unlock();
}
inline void event_bus::fire(event* e)
{
    std::lock_guard lck(eventLock);
    events.push_back(e);
}
inline void event_bus::fire_immediate(event* e)
{
    std::lock_guard lck(listenerLock);
    if(auto&& iter = listeners.find(e->name); iter != listeners.end())
    {
        for(auto&& l : iter->second)
        {
            l->on_event(e);
        }
    }
}
inline void event_bus::flush()
{
    for(auto&& e : events)
    {
        if(auto&& iter = listeners.find(e->name); iter != listeners.end())
        {
            for(auto&& l : iter->second)
            {
                l->on_event(e);
            }
        }
        delete e;
    }
    events.clear();
}
} // namespace lemon
