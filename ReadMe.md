## <b> 목표 </b>
여러 쓰레드를 준비선상에 세우고 동시에 시작하도록 만들기 위함

<br>
<br>

## <b> 사용 방법 </b>
헤더파일만으로 이뤄진 라이브러리이므로 git clone 후 include 내부의 헤더파일들만 복사해서 쓰면 됨.

run()함수는 이전에 실행한  
run()함수가 완전히 끝나야 시작됨.

<br>
<br>

## <b> 예시 코드 (main.cpp) </b>
```cpp
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
```

<br>
<br>
<br>

## <b> 실행 결과</b>

[Debug 빌드시] => _DEBUG 디파인 포함된 경우
```
[12848] started
[19448] started
g_val = 13
last_execution_time : 0.0055ms
average_execution_time : 0.00939966ms
total_execution_time : 93.9966ms
launch_count : 10000
[12500] start terminate
[12848] terminated
[12500] start terminate
[19448] terminated
```

[Release 빌드시]
```
g_val = 13
last_execution_time : 0.0055ms
average_execution_time : 0.00939966ms
total_execution_time : 93.9966ms
launch_count : 10000
```