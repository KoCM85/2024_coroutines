#include <iostream>
#include <string>
#include <coroutine>
#include <print>
#include <future>
#include <thread>

#include "coroutine.h"
#include "VERSION.h"


// coroutine
namespace my {

	template<class t_task = my::task<void>>
	t_task coroutine() {

		co_return;
		// falling off end of the function or co_return; => promise.return_void();
		// co_return value; => promise.return_value(value);
	}


	template<class type, class t_task = my::task<type>>
	t_task coroutine(type data) {
		co_yield data + data;
		// co_yield value; => promise.yield_value(value);

		co_yield data + data + data;

		co_return data;
	}

	template<class type, class t_task = my::async_task<type>>
	t_task async_coroutine(type&& value_1, type&& value_2) {
		auto res = co_await my::make_awaiter(std::move(value_1));
		std::cout << "in coroutine: " << "1: " << res << '\n';

		res += co_await my::make_awaiter(std::move(value_2));
		std::cout << "in coroutine: " << "2: " << res << '\n';

		co_return res;
	}
}


int main() {
	using namespace std::string_literals;

	using namespace my;

	std::cout << "v " << coroutine_VERSION_MAJOR << '.' << coroutine_VERSION_MINOR << '.' << coroutine_VERSION_PATCH << '\n';

	const std::string dashes("---------------");
	size_t counter = 1;

	{ // 1   task<void> sequent
		std::println(std::cout, "{1} {0} {1}", counter, dashes);

		auto res = coroutine();
	}

	++counter;

	{ // 2   task<std::string> sequent
		std::println(std::cout, "\n{1} {0} {1}", counter, dashes);

		auto res = coroutine("hello there"s);
		auto& corout_handl = res.corout_handl;
		auto& promise = corout_handl.promise();
		auto& res_val = promise.value;

		while (!corout_handl.done()) {
			std::cout << res_val << '\n';

			corout_handl.resume(); // corout_handl.resume() == corout_handl()
		}

		std::cout << res_val << '\n';

		corout_handl.destroy();
	}

	++counter;

	{ // 3   task<void> async
		std::println(std::cout, "\n{1} {0} {1}", counter, dashes);

		auto res = coroutine<async_task<void>>();
	}

	++counter;

	{ // 4   task<std::string> async
		std::println(std::cout, "\n{1} {0} {1}", counter, dashes);

		auto res = coroutine<std::string, async_task<std::string>>("hello there"s);
		auto& corout_handl = res.corout_handl;
		auto& promise_type = corout_handl.promise();
		auto& promise = promise_type.promise;
		std::future<std::string> future;

		while (!corout_handl.done()) {
			future = promise.get_future();

			try {
				std::cout << future.get() << '\n';
			}
			catch (const std::exception& except) {
				std::cerr << "exception " << except.what() << '\n';
			}
			catch (...) {
				std::cerr << "exception ... \n";
			}

			corout_handl.resume(); // corout_handl.resume() == corout_handl()
		}
		future = promise.get_future();

		std::cout << future.get() << '\n';

		corout_handl.destroy();
	}

	++counter;

	{ // 5 
		std::println(std::cout, "\n{0} {1} {0}", dashes, counter);

		std::cout << "main thread id: " << std::this_thread::get_id() << '\n';

		auto res_cour = async_coroutine("first example"s, "second example"s);
		auto& corout_handler = res_cour.corout_handl;
		auto& corout_promise = corout_handler.promise();
		auto& promise = corout_promise.promise;
		auto future = promise.get_future();
		
		future.wait();
		std::cout << "in main: " << future.get() << '\n';

		while (!corout_handler.done()) {
		}
		corout_handler.destroy();
	}

	return 0;
}