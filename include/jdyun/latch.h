/*
 * 작성자 : 윤정도
 * 세마포어랑 좀 반대느낌으로 시그널이 일정갯수만큼 도달해야
 * 크리티컬 섹션에 진입하도록 함
 * latch라고 비슷한게 있네 이미 ㅋㅋ
 * https://en.cppreference.com/w/cpp/thread/latch
 */


#pragma once

#include <condition_variable>
#include <cassert>
#include <mutex>

class latch
{
public:
  latch(int max_count, int initial_done_count) :
    m_max_count(max_count),
    m_done_count(initial_done_count)
  {
    assert(m_max_count > 0);
  }


  void signal()
  {
    std::unique_lock ul(m_lock);
    m_done_count++;
    if (m_done_count >= m_max_count) {
      m_condvar.notify_one();
    }
  }

  void wait()
  {
    std::unique_lock ul(m_lock);
    while (m_done_count < m_max_count)
      m_condvar.wait(ul);
    m_done_count = 0;
  }

  bool try_wait()
  {
    std::unique_lock ul(m_lock);
    if (m_done_count < m_max_count) {
      m_done_count = 0;
      return true;
    }

    return false;
  }

  int max_count() const
  {
    return m_max_count;
  }

  int done_count()
  {
    std::unique_lock ul(m_lock);
    return m_done_count;
  }

private:
  std::mutex m_lock;
  std::condition_variable m_condvar;
  int m_max_count;
  int m_done_count;     // 도착한 시그널의 수
};