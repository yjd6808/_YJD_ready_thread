/*
 * 작성자: 윤정도
 * 쓰레드 세이프하지 않음.
 */

#pragma once

#include <vector>

#include "ready_thread.h"
#include "ready_thread_collection_abstract.h"

class ready_thread_collection : public ready_thread_collection_abstract {

  // ===========================================================================
  //             P U B L I C
  // ===========================================================================
public:
  ready_thread_collection(int size) :
    ready_thread_collection_abstract(size),
    m_finalized(false)
  {
    for (int i = 0; i < size; i++)
      m_threads.push_back(new ready_thread(this));
  }
  ~ready_thread_collection() override { finalize(); }




  // ===========================================================================
  //             P R I V A T E
  // ===========================================================================
private:

  void run_all_threads() 
  {
    for (size_t i = 0; i < m_threads.size(); i++)
      m_threads[i]->run();
  }

  // ===========================================================================
  //             P U B L I C
  // ===========================================================================
public:
  void run() {
    m_start_wait_fm.wait();
    using namespace std::chrono;

    const auto start = high_resolution_clock::now();
    run_all_threads();
    m_end_wait_fm.wait();
    const auto end = high_resolution_clock::now();
    const duration<double, std::micro> ms = end - start;
    update_statistics(ms.count());
  }


  void finalize() 
  {
    if (m_finalized) 
      return;

    m_finalized = true;

    for (size_t i = 0; i < m_threads.size(); i++) {
      m_threads[i]->finalize();
      delete m_threads[i];
    }
  }

  ready_thread* operator[](const int& idx) const
  {
    if (idx < 0 || idx >= static_cast<int>(m_threads.size())) return nullptr;
    return m_threads[idx];
  }


  template <typename t_call>
  void set_action(const size_t idx, t_call&& action)
  {
    m_threads[idx]->set_action(std::forward<t_call>(action));
  }

  template <typename t_call>
  void set_other_action(t_call&& action)
  {
    for (size_t i = 0; i < m_threads.size(); i++)
      if (!m_threads[i]->has_action())
        m_threads[i]->set_action(std::forward<t_call>(action));
  }

  template <typename t_call>
  void set_action(t_call&& action)
  {
    for (size_t i = 0; i < m_threads.size(); i++)
        m_threads[i]->set_action(std::forward<t_call>(action));
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

  void reset_statistics(bool init_children = true)
  {
    if (init_children)
      for (const auto child : m_threads)
        child->reset_statistics();

    std::unique_lock ul(m_statistics_lock);
    m_statistics.reset();
  }

  int size() const
  {
    return int(m_threads.size());
  }

  // ===========================================================================
  //             P R I V A T E
  // ===========================================================================
private:
  bool m_finalized;
  std::vector<ready_thread*> m_threads;
};



