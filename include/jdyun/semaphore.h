/*
 * 작성자 : 윤정도
 * 멤버 변수로 카운트 관리하는 세마포어 구현
 */


#pragma once

#include <condition_variable>
#include <cassert>
#include <mutex>

class semaphore
{
public:
  semaphore(int max_count, int initial_usable_count) :
    m_max_count(max_count),
    m_usable_count(initial_usable_count)
  {
    assert(m_max_count > 0);
  }


  void signal()
  {
    std::unique_lock ul(m_lock);
    if (m_usable_count < m_max_count) {
      m_usable_count++;
      m_condvar.notify_one();
    }
  }

  void wait()
  {
    std::unique_lock ul(m_lock);
    while (m_usable_count <= 0)
      m_condvar.wait(ul);
    m_usable_count--;
  }

  bool try_wait()
  {
    std::unique_lock ul(m_lock);
    if (m_usable_count > 0) {
      m_usable_count--;
      return true;
    }

    return false;
  }

  int max_count() const
  {
    return m_max_count;
  }

  int usable_count()
  {
    std::unique_lock ul(m_lock);
    return m_usable_count;
  }

private:
  std::mutex m_lock;
  std::condition_variable m_condvar;
  int m_max_count;
  int m_usable_count;     // 접근가능한 쓰레드 수
};