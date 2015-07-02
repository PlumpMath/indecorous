#include "catch.hpp"

#include "coro/coro.hpp"
#include "coro/sched.hpp"
#include "rpc/handler.hpp"
#include "rpc/hub_data.hpp"
#include "rpc/target.hpp"

#include "bench.hpp"

using namespace indecorous;

size_t reps = 150;

void spawn_empty_lambda() {
    bench_timer_t timer("coro/spawn empty lambda", reps);
    for (size_t i = 0; i < reps; ++i) {
        coro_t::spawn([] { });
    }
}

void spawn_lambda_no_args() {
    int res = 0;
    bench_timer_t timer("coro/spawn lambda no args", reps);
    for (size_t i = 0; i < reps; ++i) {
        coro_t::spawn([&] { res += 1; });
    }
}

void no_args_fn() { }

void spawn_fn_ptr_no_args() {
    bench_timer_t timer("coro/spawn function pointer no args", reps);
    for (size_t i = 0; i < reps; ++i) {
        coro_t::spawn_now(&no_args_fn);
    }
}

class spawn_class_t {
public:
    spawn_class_t() : m_res(0) { }
    void no_args_fn() { m_res += 1; }
private:
    int m_res;
};

void spawn_class_fn_no_args() {
    bench_timer_t timer("coro/spawn class function no args", reps);
    spawn_class_t instance;
    for (size_t i = 0; i < reps; ++i) {
        coro_t::spawn_now(&spawn_class_t::no_args_fn, &instance);
    }
}

void spawn_std_function_no_args() {
    bench_timer_t timer("coro/spawn std::function no args", reps);
    for (size_t i = 0; i < reps; ++i) {
        coro_t::spawn_now(std::function<void()>(&no_args_fn));
    }
}

void spawn_std_bind_no_args() {
    bench_timer_t timer("coro/spawn std::bind no args", reps);
    for (size_t i = 0; i < reps; ++i) {
        coro_t::spawn_now(std::bind(&no_args_fn));
    }
}

struct spawn_bench_t : public handler_t<spawn_bench_t> {
    static void call() {
        spawn_empty_lambda();
        spawn_lambda_no_args();
        spawn_fn_ptr_no_args();
        spawn_class_fn_no_args();
        spawn_std_function_no_args();
        spawn_std_bind_no_args();
    }
};
IMPL_UNIQUE_HANDLER(spawn_bench_t);

TEST_CASE("coro/spawn", "[coro][spawn]") {
    scheduler_t sched(1, shutdown_policy_t::Eager);
    sched.broadcast_local<spawn_bench_t>();
    sched.run();
}
