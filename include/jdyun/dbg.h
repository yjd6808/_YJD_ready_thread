﻿/*
 * 작성자: 윤정도
 */

#pragma once

#include <utility>
#include <iostream>
#include <thread>
#include <sstream>
#include <mutex>


class dbg {
  public:
    template <typename... t_args>
    static void write_line(const char* fmt, t_args&&... args) {
#if _DEBUG
      std::stringstream ss;
      ss << "[" << std::this_thread::get_id() << "] " << fmt << "\n";
      static std::mutex m; 
      m.lock();
      printf(ss.str().c_str(), std::forward<t_args>(args)...);
      m.unlock();
#endif
    }
};
