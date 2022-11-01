/*
 * 작성자: 윤정도
 */

#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>
#include <semaphore>
#include <utility>
#include <functional>
#include <cassert>

#include "latch.h"
#include "semaphore.h"
#include "ready_thread_statistics.h"

class ready_thread_collection_abstract
{
  // ===========================================================================
  //             P R O T E C T E D
  // ===========================================================================
protected:
  ready_thread_collection_abstract(int size) :
    m_start_wait_fm(size, 0),
    m_end_wait_fm(size, 0)
  {
  }
  virtual ~ready_thread_collection_abstract() {};

  void update_statistics(double elpased_time)
  {
    std::unique_lock ul(m_statistics_lock);
    m_statistics.update(elpased_time);
  }

  void notify_ready()
  {
    m_start_wait_fm.signal();
  }

  void notify_end()
  {
    m_end_wait_fm.signal();
  }

  // ===========================================================================
  //             P R O T E C T E D
  // ===========================================================================
protected:
  latch m_start_wait_fm;
  latch m_end_wait_fm;

  ready_thread_statistics m_statistics;
  std::mutex m_statistics_lock;

  friend class ready_thread;
};