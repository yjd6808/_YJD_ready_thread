/*
 * 작성자: 윤정도
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
  ready_thread_collection(int size) {
    for (int i = 0; i < size; i++)
      m_threads.push_back(new ready_thread(this));
  }
  virtual ~ready_thread_collection() { finalize(); }


  // ===========================================================================
  //             P R I V A T E
  // ===========================================================================
private:
  void wait_for_running() {
    std::unique_lock ul(m_thread_ready_signal_lock);
    m_thread_ready_signal.wait(ul, [this] { return is_all_thread_ready(); });
  }

  void wait_for_end() {
    std::unique_lock ul(m_thread_end_signal_lock);
    m_thread_end_signal.wait(ul, [this] { return is_all_thread_end(); });
  }


  // 모든 쓰레드가 실행할 준비가 되었는지 
  bool is_all_thread_ready() const
  {
    for (const auto m_thread : m_threads)
      if (!m_thread->can_running())
        return false;
    return true;
  }

  bool is_all_thread_end() const
  {
    for (const auto m_thread : m_threads)
      if (!m_thread->is_action_ended())
        return false;
    return true;
  }

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
    wait_for_running();   // 1

    using namespace std::chrono;
    const auto start = high_resolution_clock::now();

    run_all_threads();    // 2
    wait_for_end();       // 3

    const auto end = high_resolution_clock::now();
    const duration<double, std::micro> ms = end - start;
    update_statistics(ms.count());
  }

  void finalize() const
  {
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


  // ===========================================================================
  //             P R I V A T E
  // ===========================================================================
private:
  std::vector<ready_thread*> m_threads;

  friend class ready_thread;
};



