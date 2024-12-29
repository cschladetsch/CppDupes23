#pragma once

#include <string>
#include <vector>
#include <memory>
#include <coroutine>
#include <future>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <filesystem>
#include <array>
#include <iostream>

namespace FileComparator {

struct FileInfo {
    std::string path;
    std::string name;
    std::size_t size;
    std::string hash;
};

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency())
        : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock lock(queueMutex);
                        condition.wait(lock, [this] {
                            return stop || !tasks.empty();
                        });
                        if (stop && tasks.empty()) {
                            return;
                        }
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template<class F>
    std::future<std::invoke_result_t<F>> enqueue(F&& f) {
        using return_type = std::invoke_result_t<F>;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::forward<F>(f)
        );
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock lock(queueMutex);
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    ~ThreadPool() {
        {
            std::unique_lock lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

template<typename T>
class Generator {
public:
    struct promise_type {
        T value;
        Generator get_return_object() { return Generator{handle::from_promise(*this)}; }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        std::suspend_always yield_value(T v) {
            value = std::move(v);
            return {};
        }
        void return_void() {}
    };

    using handle = std::coroutine_handle<promise_type>;

    Generator(handle h) : coro(h) {}
    ~Generator() { if (coro) coro.destroy(); }

    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& other) noexcept : coro(other.coro) { other.coro = nullptr; }
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (coro) coro.destroy();
            coro = other.coro;
            other.coro = nullptr;
        }
        return *this;
    }

    class iterator {
    public:
        iterator(handle h = nullptr) : coro(h) {
            if (coro) coro.resume();
        }
        void operator++() { if (coro) coro.resume(); }
        T& operator*() { return coro.promise().value; }
        bool operator==(std::default_sentinel_t) const { return !coro || coro.done(); }

    private:
        handle coro;
    };

    iterator begin() { return iterator{coro}; }
    std::default_sentinel_t end() { return {}; }

private:
    handle coro;
};

// Forward declarations
std::vector<FileInfo> scanDirectory(const std::string& directory);
bool compareFiles(const FileInfo& file1, const FileInfo& file2);
Generator<FileInfo> scanDirectoryAsync(const std::string& directory);
std::future<std::string> computeHashAsync(const std::string& path);

} // namespace FileComparator
