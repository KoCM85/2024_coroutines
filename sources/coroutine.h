#include <iostream>
#include <string>
#include <exception>
#include <coroutine>
#include <concepts>
#include <future>
#include <thread>
#include <sstream>

/* info links
	https://www.pvsm.ru/c-3/390400?ysclid=m29776wg2k869090204
	https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html
	https://www.w3computing.com/articles/how-to-use-cpp-coroutines-for-asynchronous-tasks/
*/

// promise_type nested to the task
namespace my {

	template<class type>
	concept simple_data = std::integral<type> || std::floating_point<type> || std::same_as<type, void> || std::convertible_to<type, std::string>;

	template<class type>
		requires (simple_data<type>)
	struct task {
		struct promise_type { // promise
			~promise_type() {
				std::cout << "~promise_type() sequent \n";
			}

			task get_return_object() {
				return { .corout_handl = std::coroutine_handle<promise_type>::from_promise(*this) };
			}

			std::suspend_never initial_suspend() {
				return {};
			}

			std::suspend_always final_suspend() noexcept {
				return {};
			}

			void unhandled_exception() {
				except_ptr = std::current_exception();
			}

			std::suspend_always yield_value(type value_) {
				value = value_;

				return {};
			}

			void return_value(type value_) {
				value = value_;
			}

			type value;

			std::exception_ptr except_ptr;
		};

		~task() {
			std::cout << "~task() sequent \n";
		}

		std::coroutine_handle<promise_type> corout_handl;
	};

	template<> // specialization for task<void>
	struct task<void>::promise_type { // promise
		~promise_type() {
			std::cout << "~promise_type() sequent \n";
		}

		task<void> get_return_object() {
			return { .corout_handl = std::coroutine_handle<promise_type>::from_promise(*this) };
		}

		std::suspend_never initial_suspend() {
			return {};
		}

		std::suspend_never final_suspend() noexcept {
			return {};
		}

		void unhandled_exception() {
			except_ptr = std::current_exception();
		}

		void return_void() {}

		std::exception_ptr except_ptr;
	};


	template<class type>
		requires (simple_data<type>)
	struct async_task {
		struct promise_type { // promise
			~promise_type() {
				std::cout << "~promise_type() async \n";
			}

			async_task get_return_object() {
				return { .corout_handl = std::coroutine_handle<promise_type>::from_promise(*this) };
			}

			std::suspend_never initial_suspend() {
				return {};
			}

			std::suspend_always final_suspend() noexcept {
				return {};
			}

			void unhandled_exception() {
				promise.set_exception(std::current_exception());
			}

			std::suspend_always yield_value(type value_) {
				promise = std::promise<type>();
				promise.set_value(value_);

				return {};
			}

			void return_value(type value_) {
				promise = std::promise<type>();
				promise.set_value(value_);
			}

			std::promise<type> promise;
		};

		~async_task() {
			std::cout << "~async_task() \n";
		}

		std::coroutine_handle<promise_type> corout_handl;
	};


	template<> // specialization for task<void, std::launch::async>
	struct async_task<void>::promise_type { // promise
		~promise_type() {
			std::cout << "~promise_type() async \n";
		}

		async_task<void> get_return_object() {
			return { .corout_handl = std::coroutine_handle<promise_type>::from_promise(*this) };
		}

		std::suspend_never initial_suspend() {
			return {};
		}

		std::suspend_never final_suspend() noexcept {
			return {};
		}

		void unhandled_exception() {
			promise.set_exception(std::current_exception());
		}

		void return_void() {}

		std::promise<void> promise;
	};

}

// awaiter
namespace my {
	template<class type, class t_task = my::async_task<type>>
	auto make_awaiter(type&& value_) {
		struct awaiter {
			bool await_ready() {
				return false;
			}

			void await_suspend(std::coroutine_handle<typename t_task::promise_type> corout_handler_) {
				auto lambda = [](const type& value) -> type {
					std::thread::id thread_id = std::this_thread::get_id();
					std::stringstream s_stream;

					s_stream << " thread id: " << thread_id;


					return value + s_stream.str();
					};

				std::future<type> fut_res = std::async(std::launch::async, lambda, std::ref(val));

				try {
					val = fut_res.get();
				}
				catch (...) {
					std::cerr << "error \n";
				}

				corout_handler_.resume();
			}

			// await_resume can return void.
			// void await_resume() {}
			type await_resume() { return val; }

			type val;
		};

		return awaiter{ .val = value_ };
	}
}