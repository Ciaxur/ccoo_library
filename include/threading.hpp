#pragma once

#include <fmt/base.h>

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace ccoo::threading {

  class ThreadPool {
    public:
      using callback_fn = std::function<void()>;

    private:
      struct callback_state {
        public:
          callback_fn fn;
      };

      std::queue<callback_state> tasks;
      bool is_running;
      std::condition_variable cv;
      std::mutex mtx;

      std::vector<std::thread> workers;

      void worker_entrypoint() {
        for (;;) {
          std::unique_lock<std::mutex> lock(mtx);
          cv.wait(lock, [&] { return !tasks.empty() || !is_running; });

          if (tasks.empty() && !is_running) { return; }

          callback_state task = std::move(tasks.front());
          tasks.pop();
          lock.unlock();

          task.fn();
        }
      }

    public:
      /* Not copy-able */
      ThreadPool(const ThreadPool&)            = delete;
      ThreadPool& operator=(const ThreadPool&) = delete;

      ThreadPool(size_t n_workers) : is_running(true) {
        workers.reserve(n_workers);
        for (size_t i = 0; i < n_workers; i++) { workers.emplace_back(&ThreadPool::worker_entrypoint, this); }
      }

      ~ThreadPool() {
        {
          std::lock_guard<std::mutex> lock(mtx);
          is_running = false;
          cv.notify_all();
        }

        for (std::thread& worker : workers) { worker.join(); }
      }

      void add_task(callback_fn&& task) {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push(callback_state{ .fn = std::move(task) });
        cv.notify_one();
      }

      size_t size() { return workers.size(); }
  };
}
