#include <jdyun/ready_thread_collection.h>
#include <chrono>

using namespace std::chrono_literals;

int g_val = 0;

int main() {
  ready_thread_collection threads(2);
  threads[0]->set_action([]{ g_val++; });
  threads[1]->set_action([]{ g_val--; });

  for (int i = 0; i < 10000; i++)
    threads.run();

  std::cout << "g_val = " << g_val << "\n";

  // 작업 얼마나 했는지 출력
  std::cout << "last_execution_time : " << threads.last_execution_time(time_precision::miliseconds) << "ms\n";
  std::cout << "average_execution_time : " << threads.average_execution_time(time_precision::miliseconds) << "ms\n";
  std::cout << "total_execution_time : " << threads.total_execution_time(time_precision::miliseconds) << "ms\n";
  std::cout << "launch_count : " << threads.launch_count() << "\n";
  return 0;
}


