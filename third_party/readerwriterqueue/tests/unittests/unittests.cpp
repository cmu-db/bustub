// ©2013-2015 Cameron Desrochers
// Unit tests for moodycamel::ReaderWriterQueue

#include <cstdio>
#include <cstdio>
#include <cstring>
#include <string>
#include <memory>

#include "minitest.h"
#include "../common/simplethread.h"
#include "../../readerwriterqueue.h"
#include "../../readerwritercircularbuffer.h"

using namespace moodycamel;


// *NOT* thread-safe
struct Foo
{
	Foo() : copied(false) { id = _id()++; }
	Foo(Foo const& other) : id(other.id), copied(true) { }
	Foo(Foo&& other) : id(other.id), copied(other.copied) { other.copied = true; }
	Foo& operator=(Foo&& other)
	{
		verify();
		id = other.id, copied = other.copied;
		other.copied = true;
		return *this;
	}
	~Foo() { verify(); }

private:
	void verify()
	{
		if (copied) return;
		if (id != _last_destroyed_id() + 1) {
			_destroyed_in_order() = false;
		}
		_last_destroyed_id() = id;
		++_destroy_count();
	}

public:
	static void reset() { _destroy_count() = 0; _id() = 0; _destroyed_in_order() = true; _last_destroyed_id() = -1; }
	static int destroy_count() { return _destroy_count(); }
	static bool destroyed_in_order() { return _destroyed_in_order(); }
	
private:
	static int& _destroy_count() { static int c = 0; return c; }
	static int& _id() { static int i = 0; return i; }
	static bool& _destroyed_in_order() { static bool d = true; return d; }
	static int& _last_destroyed_id() { static int i = -1; return i; }
	
	int id;
	bool copied;
};


#if MOODYCAMEL_HAS_EMPLACE
class UniquePtrWrapper
{
public:
	UniquePtrWrapper() = default;
	UniquePtrWrapper(std::unique_ptr<int> p) : m_p(std::move(p)) {}
	int get_value() const { return *m_p; }
	std::unique_ptr<int>& get_ptr() { return m_p; }
private:
	std::unique_ptr<int> m_p;
};
#endif

/// Extracted from private static method of ReaderWriterQueue
static size_t ceilToPow2(size_t x)
{
	// From http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	for (size_t i = 1; i < sizeof(size_t); i <<= 1) {
		x |= x >> (i << 3);
	}
	++x;
	return x;
}


class ReaderWriterQueueTests : public TestClass<ReaderWriterQueueTests>
{
public:
	ReaderWriterQueueTests()
	{
		REGISTER_TEST(create_empty_queue);
		REGISTER_TEST(enqueue_one);
		REGISTER_TEST(enqueue_many);
		REGISTER_TEST(nonempty_destroy);
		REGISTER_TEST(try_enqueue);
		REGISTER_TEST(try_dequeue);
		REGISTER_TEST(peek);
		REGISTER_TEST(pop);
		REGISTER_TEST(size_approx);
		REGISTER_TEST(max_capacity);
		REGISTER_TEST(threaded);
		REGISTER_TEST(blocking);
		REGISTER_TEST(vector);
#if MOODYCAMEL_HAS_EMPLACE
		REGISTER_TEST(emplace);
		REGISTER_TEST(try_enqueue_fail_workaround);
		REGISTER_TEST(try_emplace_fail);
#endif
		REGISTER_TEST(blocking_circular_buffer);
	}
	
	bool create_empty_queue()
	{
		{
			ReaderWriterQueue<int> q;
		}
		
		{
			ReaderWriterQueue<int> q(1234);
		}
		
		return true;
	}
	
	bool enqueue_one()
	{
		int item;
		
		{
			item = 0;
			ReaderWriterQueue<int> q(1);
			q.enqueue(12345);
			ASSERT_OR_FAIL(q.try_dequeue(item));
			ASSERT_OR_FAIL(item == 12345);
		}
		
		{
			item = 0;
			ReaderWriterQueue<int> q(1);
			ASSERT_OR_FAIL(q.try_enqueue(12345));
			ASSERT_OR_FAIL(q.try_dequeue(item));
			ASSERT_OR_FAIL(item == 12345);
		}
		
		return true;
	}
	
	bool enqueue_many()
	{
		int item = -1;
		
		{
			ReaderWriterQueue<int> q(100);
			for (int i = 0; i != 100; ++i) {
				q.enqueue(i);
			}
			
			for (int i = 0; i != 100; ++i) {
				ASSERT_OR_FAIL(q.try_dequeue(item));
				ASSERT_OR_FAIL(item == i);
			}
		}
		
		{
			ReaderWriterQueue<int> q(100);
			for (int i = 0; i != 1200; ++i) {
				q.enqueue(i);
			}
			
			for (int i = 0; i != 1200; ++i) {
				ASSERT_OR_FAIL(q.try_dequeue(item));
				ASSERT_OR_FAIL(item == i);
			}
		}
		
		return true;
	}
	
	bool nonempty_destroy()
	{
		// Some elements at beginning
		Foo::reset();
		{
			ReaderWriterQueue<Foo> q(31);
			for (int i = 0; i != 10; ++i) {
				q.enqueue(Foo());
			}
			ASSERT_OR_FAIL(Foo::destroy_count() == 0);
		}
		ASSERT_OR_FAIL(Foo::destroy_count() == 10);
		ASSERT_OR_FAIL(Foo::destroyed_in_order());
		
		// Entire block
		Foo::reset();
		{
			ReaderWriterQueue<Foo> q(31);
			for (int i = 0; i != 31; ++i) {
				q.enqueue(Foo());
			}
			ASSERT_OR_FAIL(Foo::destroy_count() == 0);
		}
		ASSERT_OR_FAIL(Foo::destroy_count() == 31);
		ASSERT_OR_FAIL(Foo::destroyed_in_order());
		
		// Multiple blocks
		Foo::reset();
		{
			ReaderWriterQueue<Foo> q(31);
			for (int i = 0; i != 94; ++i) {
				q.enqueue(Foo());
			}
			ASSERT_OR_FAIL(Foo::destroy_count() == 0);
		}
		ASSERT_OR_FAIL(Foo::destroy_count() == 94);
		ASSERT_OR_FAIL(Foo::destroyed_in_order());
		
		// Some elements in another block
		Foo::reset();
		{
			ReaderWriterQueue<Foo> q(31);
			Foo item;
			for (int i = 0; i != 42; ++i) {
				q.enqueue(Foo());
			}
			ASSERT_OR_FAIL(Foo::destroy_count() == 0);
			for (int i = 0; i != 31; ++i) {
				ASSERT_OR_FAIL(q.try_dequeue(item));
			}
			ASSERT_OR_FAIL(Foo::destroy_count() == 31);
		}
		ASSERT_OR_FAIL(Foo::destroy_count() == 43);
		ASSERT_OR_FAIL(Foo::destroyed_in_order());
		
		// Some elements in multiple blocks
		Foo::reset();
		{
			ReaderWriterQueue<Foo> q(31);
			Foo item;
			for (int i = 0; i != 123; ++i) {
				q.enqueue(Foo());
			}
			for (int i = 0; i != 25; ++i) {
				ASSERT_OR_FAIL(q.try_dequeue(item));
			}
			for (int i = 0; i != 47; ++i) {
				q.enqueue(Foo());
			}
			for (int i = 0; i != 140; ++i) {
				ASSERT_OR_FAIL(q.try_dequeue(item));
			}
			for (int i = 0; i != 230; ++i) {
				q.enqueue(Foo());
			}
			for (int i = 0; i != 130; ++i) {
				ASSERT_OR_FAIL(q.try_dequeue(item));
			}
			for (int i = 0; i != 100; ++i) {
				q.enqueue(Foo());
			}
		}
		ASSERT_OR_FAIL(Foo::destroy_count() == 501);
		ASSERT_OR_FAIL(Foo::destroyed_in_order());
		
		return true;
	}
	
	bool try_enqueue()
	{
		ReaderWriterQueue<int> q(31);
		int item;
		int size = 0;
		
		for (int i = 0; i < 10000; ++i) {
			if ((rand() & 1) == 1) {
				bool result = q.try_enqueue(i);
				if (size == 31) {
					ASSERT_OR_FAIL(!result);
				}
				else {
					ASSERT_OR_FAIL(result);
					++size;
				}
			}
			else {
				bool result = q.try_dequeue(item);
				if (size == 0) {
					ASSERT_OR_FAIL(!result);
				}
				else {
					ASSERT_OR_FAIL(result);
					--size;
				}
			}
		}
		
		return true;
	}
	
	bool try_dequeue()
	{
		int item;
		
		{
			ReaderWriterQueue<int> q(1);
			ASSERT_OR_FAIL(!q.try_dequeue(item));
		}
		
		{
			ReaderWriterQueue<int, 2> q(10);
			ASSERT_OR_FAIL(!q.try_dequeue(item));
		}
		
		return true;
	}
	
	bool threaded()
	{
		weak_atomic<int> result;
		result = 1;
		
		ReaderWriterQueue<int> q(100);
		SimpleThread reader([&]() {
			int item;
			int prevItem = -1;
			for (int i = 0; i != 1000000; ++i) {
				if (q.try_dequeue(item)) {
					if (item <= prevItem) {
						result = 0;
					}
					prevItem = item;
				}
			}
		});
		SimpleThread writer([&]() {
			for (int i = 0; i != 1000000; ++i) {
				if (((i >> 7) & 1) == 0) {
					q.enqueue(i);
				}
				else {
					q.try_enqueue(i);
				}
			}
		});
		
		writer.join();
		reader.join();
		
		return result.load() == 1 ? true : false;
	}

	bool peek()
	{
		weak_atomic<int> result;
		result = 1;
		
		ReaderWriterQueue<int> q(100);
		SimpleThread reader([&]() {
			int item;
			int prevItem = -1;
			int* peeked;
			for (int i = 0; i != 100000; ++i) {
				peeked = q.peek();
				if (peeked != nullptr) {
					if (q.try_dequeue(item)) {
						if (item <= prevItem || item != *peeked) {
							result = 0;
						}
						prevItem = item;
					}
					else {
						result = 0;
					}
				}
			}
		});
		SimpleThread writer([&]() {
			for (int i = 0; i != 100000; ++i) {
				if (((i >> 7) & 1) == 0) {
					q.enqueue(i);
				}
				else {
					q.try_enqueue(i);
				}
			}
		});
		
		writer.join();
		reader.join();
		
		return result.load() == 1 ? true : false;
	}
	
	bool pop()
	{
		weak_atomic<int> result;
		result = 1;
		
		ReaderWriterQueue<int> q(100);
		SimpleThread reader([&]() {
			int item;
			int prevItem = -1;
			int* peeked;
			for (int i = 0; i != 100000; ++i) {
				peeked = q.peek();
				if (peeked != nullptr) {
					item = *peeked;
					if (q.pop()) {
						if (item <= prevItem) {
							result = 0;
						}
						prevItem = item;
					}
					else {
						result = 0;
					}
				}
			}
		});
		SimpleThread writer([&]() {
			for (int i = 0; i != 100000; ++i) {
				if (((i >> 7) & 1) == 0) {
					q.enqueue(i);
				}
				else {
					q.try_enqueue(i);
				}
			}
		});
		
		writer.join();
		reader.join();
		
		return result.load() == 1 ? true : false;
	}
	
	bool size_approx()
	{
		weak_atomic<int> result;
		weak_atomic<int> front;
		weak_atomic<int> tail;
		
		result = 1;
		front = 0;
		tail = 0;
		
		ReaderWriterQueue<int> q(10);
		SimpleThread reader([&]() {
			int item;
			for (int i = 0; i != 100000; ++i) {
				if (q.try_dequeue(item)) {
					fence(memory_order_release);
					front = front.load() + 1;
				}
				int size = static_cast<int>(q.size_approx());
				fence(memory_order_acquire);
				int tail_ = tail.load();
				int front_ = front.load();
				if (size > tail_ - front_ || size < 0) {
					result = 0;
				}
			}
		});
		SimpleThread writer([&]() {
			for (int i = 0; i != 100000; ++i) {
				tail = tail.load() + 1;
				fence(memory_order_release);
				q.enqueue(i);
				int tail_ = tail.load();
				int front_ = front.load();
				fence(memory_order_acquire);
				int size = static_cast<int>(q.size_approx());
				if (size > tail_ - front_ || size < 0) {
					result = 0;
				}
			}
		});
		
		writer.join();
		reader.join();
		
		return result.load() == 1 ? true : false;
	}
	
	bool max_capacity()
	{
		{
			// this math for queue size estimation is only valid for q_size <= 256
			for (size_t q_size = 2; q_size < 256; ++q_size) {
				ReaderWriterQueue<size_t> q(q_size);
				ASSERT_OR_FAIL(q.max_capacity() == ceilToPow2(q_size+1)-1);

				const size_t start_cap = q.max_capacity();
				for (size_t i = 0; i < start_cap+1; ++i) // fill 1 past capacity to resize
					q.enqueue(i);
				ASSERT_OR_FAIL(q.max_capacity() == 3*start_cap+1);
			}
		}
		return true;
	}
	
	bool blocking()
	{
		{
			BlockingReaderWriterQueue<int> q;
			int item;
			
			q.enqueue(123);
			ASSERT_OR_FAIL(q.try_dequeue(item));
			ASSERT_OR_FAIL(item == 123);
			ASSERT_OR_FAIL(q.size_approx() == 0);
			
			q.enqueue(234);
			ASSERT_OR_FAIL(q.size_approx() == 1);
			ASSERT_OR_FAIL(*q.peek() == 234);
			ASSERT_OR_FAIL(*q.peek() == 234);
			ASSERT_OR_FAIL(q.pop());
			
			ASSERT_OR_FAIL(q.try_enqueue(345));
			q.wait_dequeue(item);
			ASSERT_OR_FAIL(item == 345);
			ASSERT_OR_FAIL(!q.peek());
			ASSERT_OR_FAIL(q.size_approx() == 0);
			ASSERT_OR_FAIL(!q.try_dequeue(item));
		}
		
		weak_atomic<int> result;
		result = 1;
		
		{
			BlockingReaderWriterQueue<int> q(100);
			SimpleThread reader([&]() {
				int item = -1;
				int prevItem = -1;
				for (int i = 0; i != 1000000; ++i) {
					q.wait_dequeue(item);
					if (item <= prevItem) {
						result = 0;
					}
					prevItem = item;
				}
			});
			SimpleThread writer([&]() {
				for (int i = 0; i != 1000000; ++i) {
					q.enqueue(i);
				}
			});
			
			writer.join();
			reader.join();
			
			ASSERT_OR_FAIL(q.size_approx() == 0);
			ASSERT_OR_FAIL(result.load());
		}

		{
			BlockingReaderWriterQueue<int> q(100);
			SimpleThread reader([&]() {
				int item = -1;
				int prevItem = -1;
				for (int i = 0; i != 1000000; ++i) {
					if (!q.wait_dequeue_timed(item, 1000)) {
						--i;
						continue;
					}
					if (item <= prevItem) {
						result = 0;
					}
					prevItem = item;
				}
			});
			SimpleThread writer([&]() {
				for (int i = 0; i != 1000000; ++i) {
					q.enqueue(i);
					for (volatile int x = 0; x != 100; ++x);
				}
			});
			
			writer.join();
			reader.join();

			int item;
			ASSERT_OR_FAIL(q.size_approx() == 0);
			ASSERT_OR_FAIL(!q.wait_dequeue_timed(item, 0));
			ASSERT_OR_FAIL(!q.wait_dequeue_timed(item, 1));
			ASSERT_OR_FAIL(result.load());
		}

#if MOODYCAMEL_HAS_EMPLACE
		{
			BlockingReaderWriterQueue<UniquePtrWrapper> q(100);
			std::unique_ptr<int> p { new int(123) };
			q.emplace(std::move(p));
			q.try_emplace(std::move(p));
			UniquePtrWrapper item;
			ASSERT_OR_FAIL(q.wait_dequeue_timed(item, 0));
			ASSERT_OR_FAIL(item.get_value() == 123);
			ASSERT_OR_FAIL(q.wait_dequeue_timed(item, 0));
			ASSERT_OR_FAIL(item.get_ptr() == nullptr);
			ASSERT_OR_FAIL(q.size_approx() == 0);
		}
#endif
		
		return true;
	}
	
	bool vector()
	{
		{
			std::vector<ReaderWriterQueue<int>> queues;
			queues.push_back(ReaderWriterQueue<int>());
			queues.emplace_back();

			queues[0].enqueue(1);
			queues[1].enqueue(2);
			std::swap(queues[0], queues[1]);

			int item;
			ASSERT_OR_FAIL(queues[0].try_dequeue(item));
			ASSERT_OR_FAIL(item == 2);

			ASSERT_OR_FAIL(queues[1].try_dequeue(item));
			ASSERT_OR_FAIL(item == 1);
		}
		
		{
			std::vector<BlockingReaderWriterQueue<int>> queues;
			queues.push_back(BlockingReaderWriterQueue<int>());
			queues.emplace_back();

			queues[0].enqueue(1);
			queues[1].enqueue(2);
			std::swap(queues[0], queues[1]);

			int item;
			ASSERT_OR_FAIL(queues[0].try_dequeue(item));
			ASSERT_OR_FAIL(item == 2);

			queues[1].wait_dequeue(item);
			ASSERT_OR_FAIL(item == 1);
		}
		return true;
	}

#if MOODYCAMEL_HAS_EMPLACE
	bool emplace()
	{
		ReaderWriterQueue<UniquePtrWrapper> q(100);
		std::unique_ptr<int> p { new int(123) };
		q.emplace(std::move(p));
		UniquePtrWrapper item;
		ASSERT_OR_FAIL(q.try_dequeue(item));
		ASSERT_OR_FAIL(item.get_value() == 123);
		ASSERT_OR_FAIL(q.size_approx() == 0);

		return true;
	}

	// This is what you have to do to try_enqueue() a movable type, and demonstrates why try_emplace() is useful
	bool try_enqueue_fail_workaround()
	{
		ReaderWriterQueue<UniquePtrWrapper> q(0);
		{
			// A failed try_enqueue() will still delete p
			std::unique_ptr<int> p { new int(123) };
			q.try_enqueue(std::move(p));
			ASSERT_OR_FAIL(q.size_approx() == 0);
			ASSERT_OR_FAIL(p == nullptr);
		}
		{
			// Workaround isn't pretty and potentially expensive - use try_emplace() instead
			std::unique_ptr<int> p { new int(123) };
			UniquePtrWrapper w(std::move(p));
			q.try_enqueue(std::move(w));
			p = std::move(w.get_ptr());
			ASSERT_OR_FAIL(q.size_approx() == 0);
			ASSERT_OR_FAIL(p != nullptr);
			ASSERT_OR_FAIL(*p == 123);
		}

		return true;
	}

	bool try_emplace_fail()
	{
		ReaderWriterQueue<UniquePtrWrapper> q(0);
		std::unique_ptr<int> p { new int(123) };
		q.try_emplace(std::move(p));
		ASSERT_OR_FAIL(q.size_approx() == 0);
		ASSERT_OR_FAIL(p != nullptr);
		ASSERT_OR_FAIL(*p == 123);

		return true;
	}
#endif

	bool blocking_circular_buffer()
	{
		{
			// Basic enqueue
			BlockingReaderWriterCircularBuffer<int> q(65);
			for (int iteration = 0; iteration != 128; ++iteration) {  // check there's no problem with mismatch between nominal and allocated capacity
				ASSERT_OR_FAIL(q.max_capacity() == 65);
				ASSERT_OR_FAIL(q.size_approx() == 0);
				ASSERT_OR_FAIL(!q.try_pop());
				ASSERT_OR_FAIL(q.try_enqueue(0));
				ASSERT_OR_FAIL(q.max_capacity() == 65);
				ASSERT_OR_FAIL(q.size_approx() == 1);
				ASSERT_OR_FAIL(*q.peek() == 0);
				for (int i = 1; i != 65; ++i)
					q.wait_enqueue(i);
				ASSERT_OR_FAIL(q.size_approx() == 65);
				ASSERT_OR_FAIL(!q.try_enqueue(65));

				// Basic dequeue
				int item;
				ASSERT_OR_FAIL(q.try_dequeue(item));
				ASSERT_OR_FAIL(item == 0);
				for (int i = 1; i != 65; ++i) {
					q.wait_dequeue(item);
					ASSERT_OR_FAIL(item == i);
				}
				ASSERT_OR_FAIL(!q.try_dequeue(item));
				ASSERT_OR_FAIL(!q.wait_dequeue_timed(item, 1));
				ASSERT_OR_FAIL(item == 64);
			}
		}

		{
			// Zero capacity
			BlockingReaderWriterCircularBuffer<int> q(0);
			ASSERT_OR_FAIL(q.max_capacity() == 0);
			ASSERT_OR_FAIL(!q.try_enqueue(1));
			ASSERT_OR_FAIL(!q.wait_enqueue_timed(1, 0));
		}

		// Element lifetimes
		Foo::reset();
		{
			BlockingReaderWriterCircularBuffer<Foo> q(31);
			{
				Foo item;
				for (int i = 0; i != 23 + 32; ++i) {
					ASSERT_OR_FAIL(q.try_enqueue(Foo()));
					ASSERT_OR_FAIL(q.try_dequeue(item));
				}
				ASSERT_OR_FAIL(Foo::destroy_count() == 23 + 32);
				ASSERT_OR_FAIL(Foo::destroyed_in_order());
			}
			Foo::reset();

			{
				Foo item;
				for (int i = 0; i != 23 + 32; ++i) {
					ASSERT_OR_FAIL(q.try_enqueue(Foo()));
					item = std::move(*q.peek());
					ASSERT_OR_FAIL(q.try_pop());
				}
				ASSERT_OR_FAIL(!q.peek());
				ASSERT_OR_FAIL(!q.try_pop());
				ASSERT_OR_FAIL(Foo::destroy_count() == 23 + 32);
				ASSERT_OR_FAIL(Foo::destroyed_in_order());
			}
			Foo::reset();

			{
				Foo item;
				for (int i = 0; i != 10; ++i)
					ASSERT_OR_FAIL(q.try_enqueue(Foo()));
				ASSERT_OR_FAIL(q.size_approx() == 10);
				ASSERT_OR_FAIL(Foo::destroy_count() == 0);
				ASSERT_OR_FAIL(q.try_dequeue(item));
				ASSERT_OR_FAIL(q.size_approx() == 9);
				ASSERT_OR_FAIL(Foo::destroy_count() == 1);
			}
			ASSERT_OR_FAIL(Foo::destroy_count() == 2);
			ASSERT_OR_FAIL(Foo::destroyed_in_order());

			BlockingReaderWriterCircularBuffer<Foo> q2(std::move(q));
			ASSERT_OR_FAIL(q.size_approx() == 0);
			ASSERT_OR_FAIL(q2.size_approx() == 9);

			BlockingReaderWriterCircularBuffer<Foo> q3(2);
			q3 = std::move(q2);
			ASSERT_OR_FAIL(q.size_approx() == 0);
			ASSERT_OR_FAIL(q2.size_approx() == 0);
			ASSERT_OR_FAIL(q3.size_approx() == 9);

			q = std::move(q2);
			ASSERT_OR_FAIL(q.size_approx() == 0);
			ASSERT_OR_FAIL(q2.size_approx() == 0);
			ASSERT_OR_FAIL(q3.size_approx() == 9);
			ASSERT_OR_FAIL(Foo::destroy_count() == 2);
		}
		ASSERT_OR_FAIL(Foo::destroy_count() == 11);
		ASSERT_OR_FAIL(Foo::destroyed_in_order());

		weak_atomic<int> result;
		result = 1;

		{
			// Threaded
			BlockingReaderWriterCircularBuffer<int> q(8);
			SimpleThread reader([&]() {
				int item;
				for (int i = 0; i != 1000000; ++i) {
					int* peeked = q.peek();
					if (peeked) {
						item = *peeked;
						if (peeked != q.peek() || !q.try_pop())
							result = 0;
					}
					else {
						q.wait_dequeue(item);
					}
					if (item != i)
						result = 0;
				}
			});
			SimpleThread writer([&]() {
				for (int i = 0; i != 1000000; ++i)
					q.wait_enqueue(i);
			});

			writer.join();
			reader.join();

			ASSERT_OR_FAIL(q.size_approx() == 0);
			ASSERT_OR_FAIL(result.load());
		}

		return true;
	}
};



void printTests(ReaderWriterQueueTests const& tests)
{
	std::printf("   Supported tests are:\n");
	
	std::vector<std::string> names;
	tests.getAllTestNames(names);
	for (auto it = names.cbegin(); it != names.cend(); ++it) {
		std::printf("      %s\n", it->c_str());
	}
}


// Basic test harness
int main(int argc, char** argv)
{
	bool disablePrompt = false;
	std::vector<std::string> selectedTests;
	
	// Disable buffering (so that when run in, e.g., Sublime Text, the output appears as it is written)
	std::setvbuf(stdout, nullptr, _IONBF, 0);
	
	// Isolate the executable name
	std::string progName = argv[0];
	auto slash = progName.find_last_of("/\\");
	if (slash != std::string::npos) {
		progName = progName.substr(slash + 1);
	}
	
	ReaderWriterQueueTests tests;
	
	// Parse command line options
	if (argc == 1) {
		std::printf("Running all unit tests for moodycamel::ReaderWriterQueue.\n(Run %s --help for other options.)\n\n", progName.c_str());
	}
	else {
		bool printHelp = false;
		bool printedTests = false;
		bool error = false;
		for (int i = 1; i < argc; ++i) {
			if (std::strcmp(argv[i], "--help") == 0) {
				printHelp = true;
			}
			else if (std::strcmp(argv[i], "--disable-prompt") == 0) {
				disablePrompt = true;
			}
			else if (std::strcmp(argv[i], "--run") == 0) {
				if (i + 1 == argc || argv[i + 1][0] == '-') {
					std::printf("Expected test name argument for --run option.\n");
					if (!printedTests) {
						printTests(tests);
						printedTests = true;
					}
					error = true;
					continue;
				}
				
				if (!tests.validateTestName(argv[++i])) {
					std::printf("Unrecognized test '%s'.\n", argv[i]);
					if (!printedTests) {
						printTests(tests);
						printedTests = true;
					}
					error = true;
					continue;
				}
				
				selectedTests.push_back(argv[i]);
			}
			else {
				std::printf("Unrecognized option '%s'.\n", argv[i]);
				error = true;
			}
		}
		
		if (error || printHelp) {
			if (error) {
				std::printf("\n");
			}
			std::printf("%s\n    Description: Runs unit tests for moodycamel::ReaderWriterQueue\n", progName.c_str());
			std::printf("    --help            Prints this help blurb\n");
			std::printf("    --run test        Runs only the specified test(s)\n");
			std::printf("    --disable-prompt  Disables prompt before exit when the tests finish\n");
			return error ? -1 : 0;
		}
	}
	
	
	int exitCode = 0;
	
	bool result;
	if (selectedTests.size() > 0) {
		result = tests.run(selectedTests);
	}
	else {
		result = tests.run();
	}
	
	if (result) {
		std::printf("All %stests passed.\n", (selectedTests.size() > 0 ? "selected " : ""));
	}
	else {
		std::printf("Test(s) failed!\n");
		exitCode = 2;
	}
	
	if (!disablePrompt) {
		std::printf("Press ENTER to exit.\n");
		getchar();
	}
	return exitCode;
}

