/*
 * Tiny test framework by great glebushka98
 */
#include <random>
#include <mutex>
#include <atomic>
#include <queue>
#include <thread>
#include <iostream>
#include <functional>
#include <future>
#include <condition_variable>
#include <ctime>
#include <exception>
#include <cassert>
#include <fstream>

using std::cerr;
using std::cin;
using std::endl;
using std::mutex;
using std::queue;
using std::atomic;
using std::vector;
using std::string;
using std::function;
using std::pair;
using std::lock_guard;
using std::thread;
using std::packaged_task;
using std::make_shared;
using std::shared_ptr;
using std::unique_ptr;
using std::unique_lock;
using std::condition_variable;
using std::future;

mutex cout_lock;

namespace Tests {

    const char *Gray() {
        return "\033[90m";
    }

    const char *Red() {
        return "\033[31;1m";
    }

    const char *Green() {
        return "\033[32;1m";
    }

    const char *Blue() {
        return "\033[34;1m";
    }

    const char *ResetColor() {
        return "\033[0m";
    }

    const char *Purple() {
        return "\x1B[36m";
    }

    const char *Yellow() {
        return "\x1B[33m";
    }


    class ThreadPool {
    public:
        ThreadPool(int threadsCount, size_t max_size = 100) : max_size_(max_size) {
            isEnable = true;
            for (int i = 0; i < threadsCount; i++) {
                threads.emplace_back([this]() { Loop(); });
            }
        }

        template<typename CallBack, typename ...Args>
        auto AddTask(CallBack &&c, Args &&...args) {

            using Ret = decltype(c(args...));
            shared_ptr<packaged_task<Ret()> > task =
                    make_shared<packaged_task<Ret()> >(
                            std::bind(std::forward<CallBack>(c), std::forward<Args>(args)...)
                    );

            auto voidFunc = [task]() {
                (*task)();
            };

            {
                unique_lock<mutex> guard(queue_task_lock);
                cv.wait(guard, [this]() { return tasks.size() <= max_size_ || !isEnable; });
                tasks.push(std::move(voidFunc));
                cv.notify_one();
            }

            return task->get_future();
        }

        void Loop() {
            while (isEnable) {
                function<void()> func;
                {
                    unique_lock<mutex> guard(queue_task_lock);
                    if (!tasks.empty()) {
                        func = move(tasks.front());
                        tasks.pop();
                    } else {
                        cv.wait(guard, [this]() { return !tasks.empty() || !isEnable; });
                        continue;
                    }
                }
                if (!isEnable) {
                    return;
                }
                func();
                cv.notify_all();
            }
        }

        bool IsEnable() {
            return isEnable;
        }

        int Size() {
            return tasks.size();
        }

        void Stop() {
            if (!isEnable) {
                return;
            }
            isEnable = false;
            cv.notify_all();
        }

        virtual ~ThreadPool() {
            for (auto &t : threads) {
                t.join();
            }
        }

    private:
        queue<function<void()>> tasks;
        vector<thread> threads;
        mutex queue_task_lock;
        condition_variable cv;
        atomic<bool> isEnable;
        size_t max_size_;
    };

    class TestNotFoundException : public std::exception {
    public:
        TestNotFoundException(const std::string &why) : why_(why) {}

        virtual const char *what() const throw() {
            return why_.c_str();
        }

        std::string why_;
    };

    class Tester {
    public:
        Tester(int threads_count) : threads_count_(threads_count), thread_pool_(threads_count) {}

        template<bool ASYNC = true, typename Test, typename ...Args>
        void RegisterTest(const string &test_name, Test &&test, Args &&...args) {
            if (ASYNC) {
                assert(threads_count_ == 0);
            }

            if (ASYNC) {
                tests_.emplace_back(test_name,
                                    std::async(std::launch::deferred,
                                               std::bind(std::forward<Test>(test), std::forward<Args>(args)...)
                                    ));
            } else {
                unique_lock<mutex> guard(test_lock_);
                tests_.emplace_back(test_name,
                                    std::move(
                                            thread_pool_.AddTask(
                                                    std::bind(std::forward<Test>(test), std::forward<Args>(args)...)
                                            )
                                    )
                );
            }


        }


        template<bool ASYNC = true, typename Solution1, typename Solution2, class Gen, typename Checker>
        void RunStressTest(Solution1 &&first, Solution2 &&second, Gen &&generator,
                           Checker &&check_equal, std::ostream &out = std::cout) {
            uint64_t i = 0;
            for (int k = 0; k < 1000000;++k) {
                i++;
                if (!thread_pool_.IsEnable()) {
                    break;
                }
                auto stress_test = [this, &out, check_equal, first, second, generator, i]() mutable {
                    using namespace std::chrono;
                    auto ms = duration_cast<nanoseconds>(
                            system_clock::now().time_since_epoch()
                    );
                    std::mt19937 rd(ms.count());
                    for (uint64_t test_id = 0; test_id < 2; ++test_id) {
                        auto test = generator(rd);
                        if (!check_equal(test, first(test), second(test))) {
                            {
                                lock_guard<mutex> guard(cout_lock);
                                if (!thread_pool_.IsEnable()) {
                                    return;
                                }
                                std::cout << Green() << "TEST FOUND : " << ResetColor() << endl;
                                exit(0);
                                //out << test.ToString() << endl;
                                thread_pool_.Stop();
                            }
                        }
                    }
                    {
                        lock_guard<mutex> guard(cout_lock);
                        std::cout << Red() << "TEST NOT FOUND ON ITERATION : " << i << ResetColor() << endl;
                    }
                };
                if (ASYNC) {
                    RegisterTest<true>("std::to_string(i)", stress_test);
                } else {
                    RegisterTest<false>("std::to_string(i)", stress_test);
                }
            }
            for (auto& f : tests_) {
                f.second.get();
            }
        }

        void RunTests() {
            using std::cout;
            if (tests_.empty()) {
                cout << Red() << "WARN tests not found!" << ResetColor() << endl;
                return;
            }


            auto printer = [](int green, int red) {
                for (int i = 0; i < green + red; i++) {
                    cout << (i < green ? Green() : Red()) << "=" << ResetColor();
                }
                cout << endl;
            };

            size_t success = 0;
            for (auto &test : tests_) {
                try {
                    test.second.get();
                    success++;
                } catch (std::exception &e) {
                    cout << Gray() << "============================================================" << ResetColor()
                         << endl;
                    cout << test.first << " : " << Red() << "Failed: \n" << ResetColor()
                         << Purple() << e.what() << ResetColor() << endl;
                }
            }
            int green = ceil(60 * 1.0 * success / tests_.size());
            int red = 60 - green;
            printer(green, red);
            cout << Blue() << "Testing finished" << ResetColor() << endl;

            cout << Gray() << "===> " << ResetColor()
                 << Green() << "Success" << ResetColor() << ": " << success << ", "
                 << Red() << "Broken" << ResetColor() << ": " << tests_.size() - success << endl;
            if (success == tests_.size()) {
                cout << Green() << "All test passed!!!" << ResetColor() << endl;
            }
            thread_pool_.Stop();
        }
        mutex test_lock_;
        vector<pair<string, std::future<void>>> tests_;
        int threads_count_;
        ThreadPool thread_pool_;

    };


    template <typename T>
    bool VectorsEquals(const vector<T>& v1, const vector<T>& v2) {
        if (v1.size() != v2.size()) {
            return false;
        }
        for (size_t i = 0; i < v1.size(); ++i) {
            if (v1[i] != v2[i]) {
                return false;
            }
        }
        return true;
    }
}

#define REQUIRE(k) if (!(k)) throw std::runtime_error(string(__FILE__) + ":" + string(std::to_string(__LINE__)) + "\nREQUIRE(" + std::string(#k) + ")");