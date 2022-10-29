/*
 * 작성자: 윤정도
 */

#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>
#include <utility>
#include <functional>
#include <cassert>
#include "ready_thread_statistics.h"

class ready_thread_collection_abstract
{

  // ===========================================================================
  //             P U B L I C
  // ===========================================================================
public:
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
  //             P R O T E C T E D
  // ===========================================================================
protected:
  ready_thread_collection_abstract() {}
  virtual ~ready_thread_collection_abstract() {}

  void update_statistics(double elpased_time)
  {
    std::unique_lock ul(m_statistics_lock);
    m_statistics.update(elpased_time);
  }

  void notify_ready(std::mutex& child_lock)
  {
    child_lock.unlock();
    m_thread_ready_signal_lock.lock();
    m_thread_ready_signal.notify_one();
    m_thread_ready_signal_lock.unlock();
    child_lock.lock();
  }

  void notify_end(std::mutex& child_lock)
  {
    child_lock.unlock();
    m_thread_end_signal_lock.lock();
    m_thread_end_signal.notify_one();
    m_thread_end_signal_lock.unlock();
    child_lock.lock();
  }

  // ===========================================================================
  //             P R O T E C T E D
  // ===========================================================================
protected:
  std::condition_variable m_thread_ready_signal;    
  std::mutex m_thread_ready_signal_lock;

  std::condition_variable m_thread_end_signal;      
  std::mutex m_thread_end_signal_lock;

  ready_thread_statistics m_statistics;
  std::mutex m_statistics_lock;

  friend class ready_thread;
};