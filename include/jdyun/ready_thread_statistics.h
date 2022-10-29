/*
 * 작성자: 윤정도
 */

#pragma once

enum class time_precision
{
  seconds,
  miliseconds,
  microseconds
};

struct ready_thread_statistics
{
  std::atomic<int> launch_count = 0;

  double last_execution_time = 0;
  double total_execution_time = 0;
  double average_execution_time = 0;

  void reset()
  {
    launch_count = 0;

    total_execution_time = 0;
    last_execution_time = 0;
    average_execution_time = 0;
  }

  void update(double elapsed)
  {
    ++launch_count;

    total_execution_time += elapsed;
    last_execution_time = elapsed;
    average_execution_time = total_execution_time / launch_count;
  }

  int get_launch_count() const 
  {
    return launch_count.load(std::memory_order_relaxed);
  }

  double get_average_execution_time(const time_precision precision) const
  {
    switch (precision)
    {
    case time_precision::miliseconds: return average_execution_time / 1000;
    case time_precision::seconds: return average_execution_time / 1000000;
    case time_precision::microseconds: return average_execution_time;
    }
    return -1;
  }

  double get_total_execution_time(const time_precision precision) const
  {
    switch (precision)
    {
    case time_precision::miliseconds: return total_execution_time / 1'000;
    case time_precision::seconds: return total_execution_time / 1'000'000;
    case time_precision::microseconds: return total_execution_time;
    }
    return -1;
  }

  double get_last_execution_time(const time_precision precision) const
  {
    switch (precision)
    {
    case time_precision::miliseconds: return last_execution_time / 1'000;
    case time_precision::seconds: return last_execution_time / 1'000'000;
    case time_precision::microseconds: return last_execution_time;
    }
    return -1;
  }
};