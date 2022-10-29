/*
 * 작성자: 윤정도
 */


#pragma once

#include "dbg.h"
#include "ready_thread_collection_abstract.h"

class ready_thread {
public:
  // ===========================================================================
  //             P U B L I C
  // ===========================================================================

  template <typename t_action>
  void set_action(t_action&& action) {
    using namespace std::chrono;
    std::unique_lock ul(m_lock);
    m_action = [this, &action]() {
      const auto start = high_resolution_clock::now();
      std::forward<t_action>(action)();
      const auto end = high_resolution_clock::now();
      const duration<double, std::milli> fp_ms = end - start;
      update_statistics(fp_ms.count());
    };
  }

  double total_execution_time(const time_precision precision)
  {
    std::unique_lock ul(m_statistics_lock);
    return m_statistics.get_total_execution_time(precision);
  }

  double average_execution_time(const time_precision precision)
  {
    std::unique_lock ul(m_statistics_lock);
    return m_statistics.get_average_execution_time(precision);
  }

  double last_execution_time(const time_precision precision)
  {
    std::unique_lock ul(m_statistics_lock);
    return m_statistics.get_last_execution_time(precision);
  }

  int launch_count() const
  {
    return m_statistics.get_launch_count();
  }

  void reset_statistics()
  {
    std::unique_lock ul(m_statistics_lock);
    m_statistics.reset();
  }

  // ===========================================================================
  //             P R I V A T E
  // ===========================================================================
private:
  explicit ready_thread(ready_thread_collection_abstract* parent) :
    m_running(true),
    m_run_wait(false),
    m_parent(parent),
    m_thread([this] { worker(); })
  {
  }
  ~ready_thread() { finalize(); }

  void run() {
    std::unique_lock ul(m_lock);
    if (!m_action) {
      assert(false);
      return;
    }
    m_run_wait = true;
    m_cond.notify_one();
  }

  void finalize() {
    {
      std::unique_lock ul(m_lock);
      if (!m_running)
        return;

      dbg::write_line("start terminate");
      m_running = false;
      m_action = {};
    }

    m_cond.notify_one();
    m_thread.join();
  }

  bool can_running() {
    std::unique_lock ul(m_lock);
    return m_run_ready;
  }

  bool is_action_ended() {
    std::unique_lock ul(m_lock);
    return m_run_wait == false;
  }


  void worker() {
    dbg::write_line("started");
    m_run_ready = true;

    while (true) {
      std::unique_lock ul(m_lock);
      m_parent->notify_ready(m_lock);
      m_cond.wait(ul, [this] { return !this->m_running || this->m_run_wait; });
      if (!m_running) break;
      if (m_action) {
        m_action();
        m_run_wait = false;
        m_parent->notify_end(m_lock);
      }
    }

    dbg::write_line("terminated");
  }

  void update_statistics(double elpased_time)
  {
    std::unique_lock ul(m_statistics_lock);
    m_statistics.update(elpased_time);
  }

  // ===========================================================================
  //             P R I V A T E
  // ===========================================================================
private:
  bool m_running;
  bool m_run_wait;
  bool m_run_ready{};

  ready_thread_collection_abstract* m_parent;
  ready_thread_statistics m_statistics;
  std::mutex m_statistics_lock;

  std::thread m_thread;
  std::condition_variable m_cond;
  std::mutex m_lock;
  std::function<void()> m_action;

  friend class ready_thread_collection;
};


