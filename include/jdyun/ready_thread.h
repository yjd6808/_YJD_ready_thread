/*
 * 작성자: 윤정도
 */


#pragma once

#include "dbg.h"
#include "ready_thread_collection_abstract.h"

class ready_thread {
public:
  enum class running_state : int
  {
    start_wait,     // 액션 실행 후부터 액션 실행전까지의 상태
    pre_action      // 액션 실행 요청이 들어온 이후 액션 수행동안의 상태
  };

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

  bool has_action()
  {
    std::unique_lock ul(m_lock);
    if (m_action)
      return true;

    return false;
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
    m_terminated(false),
    m_wait_sem(1, 0),
    m_running_state(running_state::start_wait),
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
    m_running_state = running_state::pre_action;
    m_wait_sem.signal();
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

    m_wait_sem.signal();
    m_thread.join();

    // 흠.. condition_variable이 현재 wait 중인지를 알 수가 없네..
    // 종료될때까지 깨우도록 하자.
   /* while (!m_terminated)
      m_wait_sem.signal();

    m_thread.join();*/
  }

  void worker() {
    dbg::write_line("started");
    while (true) {
      m_parent->notify_ready();

      std::unique_lock ul(m_lock);
      while (true)
      {
        ul.unlock();
        m_wait_sem.wait();  // 무조건 선대기, 대기하는 동안 락 풀어놔야 외부에서 시그널을 줄 수 있음
        ul.lock();

        if (!m_running)
          goto end;

        if (m_running_state == running_state::pre_action)
          break;
      } 

      if (m_action) {
        m_action();
        m_running_state = running_state::start_wait;
        m_parent->notify_end();
      }
    }

  end:
    dbg::write_line("terminated");
    m_terminated = true;
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
  bool m_terminated;
  running_state m_running_state;

  ready_thread_collection_abstract* m_parent;
  ready_thread_statistics m_statistics;
  std::mutex m_statistics_lock;

  std::thread m_thread;
  std::mutex m_lock;
  semaphore m_wait_sem;
  std::function<void()> m_action;

  friend class ready_thread_collection;
};


