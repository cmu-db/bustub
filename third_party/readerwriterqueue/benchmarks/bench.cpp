// ©2013-2015 Cameron Desrochers.
// Distributed under the simplified BSD license (see the LICENSE file that
// should have come with this file).

// Benchmarks for moodycamel::ReaderWriterQueue.

#if defined(_MSC_VER) && _MSC_VER < 1700
#define NO_FOLLY_SUPPORT
#endif

#if defined(_MSC_VER) && _MSC_VER < 1700
#define NO_CIRCULAR_BUFFER_SUPPORT
#endif

#if !defined(__amd64__) && !defined(_M_X64) && !defined(__x86_64__) && !defined(_M_IX86) && !defined(__i386__)
#define NO_SPSC_SUPPORT  // SPSC implementation is for x86 only
#endif

#include "ext/1024cores/spscqueue.h"            // Dmitry's (on Intel site)
#ifndef NO_FOLLY_SUPPORT
#include "ext/folly/ProducerConsumerQueue.h"    // Facebook's folly (GitHub)
#endif
#include "../readerwriterqueue.h"               // Mine
#ifndef NO_CIRCULAR_BUFFER_SUPPORT
#include "../readerwritercircularbuffer.h"      // Mine
template<typename T>
class BlockingReaderWriterCircularBufferAdapter : public moodycamel::BlockingReaderWriterCircularBuffer<T> {
public:
	BlockingReaderWriterCircularBufferAdapter(std::size_t capacity) : moodycamel::BlockingReaderWriterCircularBuffer<T>(capacity) { }
	void enqueue(T const& x) { this->wait_enqueue(x); }
};
#endif
#include "systemtime.h"
#include "../tests/common/simplethread.h"

#include <iostream>
#include <iomanip>
#include <numeric>		// For std::accumulate
#include <algorithm>
#include <random>
#include <ctime>

#ifndef UNUSED
#define UNUSED(x) ((void)x);
#endif

using namespace moodycamel;
#ifndef NO_FOLLY_SUPPORT
using namespace folly;
#endif


typedef std::minstd_rand RNG_t;


enum BenchmarkType {
	bench_raw_add,
	bench_raw_remove,
	bench_empty_remove,
	bench_single_threaded,
	bench_mostly_add,
	bench_mostly_remove,
	bench_heavy_concurrent,
	bench_random_concurrent,

	BENCHMARK_COUNT
};


// Returns the number of seconds elapsed (high-precision), and the number of enqueue/dequeue
// operations performed (in the out_Ops parameter)
template<typename TQueue>
double runBenchmark(BenchmarkType benchmark, unsigned int randomSeed, double& out_Ops);

const int BENCHMARK_NAME_MAX = 17;		// Not including null terminator
const char* benchmarkName(BenchmarkType benchmark);


int main(int argc, char** argv)
{
#ifdef NDEBUG
	const int TEST_COUNT = 25;
#else
	const int TEST_COUNT = 2;
#endif
	assert(TEST_COUNT >= 2);

	const double FASTEST_PERCENT_CONSIDERED = 20;		// Consider only the fastest runs in the top 20%

	double rwqResults[BENCHMARK_COUNT][TEST_COUNT];
	double brwcbResults[BENCHMARK_COUNT][TEST_COUNT];
	double spscResults[BENCHMARK_COUNT][TEST_COUNT];
	double follyResults[BENCHMARK_COUNT][TEST_COUNT];
	
	// Also calculate a rough heuristic of "ops/s" (across all runs, not just fastest)
	double rwqOps[BENCHMARK_COUNT][TEST_COUNT];
	double brwcbOps[BENCHMARK_COUNT][TEST_COUNT];
	double spscOps[BENCHMARK_COUNT][TEST_COUNT];
	double follyOps[BENCHMARK_COUNT][TEST_COUNT];

	// Make sure the randomness of each benchmark run is identical
	unsigned int randSeeds[BENCHMARK_COUNT];
	for (unsigned int i = 0; i != BENCHMARK_COUNT; ++i) {
		randSeeds[i] = ((unsigned int)time(NULL)) * i;
	}

	// Run benchmarks
	for (int benchmark = 0; benchmark < BENCHMARK_COUNT; ++benchmark) {
		for (int i = 0; i < TEST_COUNT; ++i) {
			rwqResults[benchmark][i] = runBenchmark<ReaderWriterQueue<int>>((BenchmarkType)benchmark, randSeeds[benchmark], rwqOps[benchmark][i]);
		}
#ifndef NO_CIRCULAR_BUFFER_SUPPORT
		for (int i = 0; i < TEST_COUNT; ++i) {
			brwcbResults[benchmark][i] = runBenchmark<BlockingReaderWriterCircularBufferAdapter<int>>((BenchmarkType)benchmark, randSeeds[benchmark], brwcbOps[benchmark][i]);
		}
#else
		for (int i = 0; i < TEST_COUNT; ++i) {
			brwcbResults[benchmark][i] = 0;
			brwcbOps[benchmark][i] = 0;
		}
#endif
#ifndef NO_SPSC_SUPPORT
		for (int i = 0; i < TEST_COUNT; ++i) {
			spscResults[benchmark][i] = runBenchmark<spsc_queue<int>>((BenchmarkType)benchmark, randSeeds[benchmark], spscOps[benchmark][i]);
		}
#else
		for (int i = 0; i < TEST_COUNT; ++i) {
			spscResults[benchmark][i] = 0;
			spscOps[benchmark][i] = 0;
		}
#endif
#ifndef NO_FOLLY_SUPPORT
		for (int i = 0; i < TEST_COUNT; ++i) {
			follyResults[benchmark][i] = runBenchmark<ProducerConsumerQueue<int>>((BenchmarkType)benchmark, randSeeds[benchmark], follyOps[benchmark][i]);
		}
#else
		for (int i = 0; i < TEST_COUNT; ++i) {
			follyResults[benchmark][i] = 0;
			follyOps[benchmark][i] = 0;
		}
#endif
	}

	// Sort results
	for (int benchmark = 0; benchmark < BENCHMARK_COUNT; ++benchmark) {
		std::sort(&rwqResults[benchmark][0], &rwqResults[benchmark][0] + TEST_COUNT);
		std::sort(&brwcbResults[benchmark][0], &brwcbResults[benchmark][0] + TEST_COUNT);
		std::sort(&spscResults[benchmark][0], &spscResults[benchmark][0] + TEST_COUNT);
		std::sort(&follyResults[benchmark][0], &follyResults[benchmark][0] + TEST_COUNT);
	}
	
	// Display results
	int max = std::max(2, (int)(TEST_COUNT * FASTEST_PERCENT_CONSIDERED / 100));
	assert(max > 0);
#ifdef NO_CIRCULAR_BUFFER_SUPPORT
	std::cout << "Note: BRWCB queue not supported on this platform, discount its timings" << std::endl;
#endif
#ifdef NO_SPSC_SUPPORT
	std::cout << "Note: SPSC queue not supported on this platform, discount its timings" << std::endl;
#endif
#ifdef NO_FOLLY_SUPPORT
	std::cout << "Note: Folly queue not supported by this compiler, discount its timings" << std::endl;
#endif
	std::cout              << std::setw(BENCHMARK_NAME_MAX) << "         " << " |----------------  Min -----------------|----------------- Max -----------------|----------------- Avg -----------------|\n";
	std::cout << std::left << std::setw(BENCHMARK_NAME_MAX) << "Benchmark" << " |   RWQ   |  BRWCB  |  SPSC   |  Folly  |   RWQ   |  BRWCB  |  SPSC   |  Folly  |   RWQ   |  BRWCB  |  SPSC   |  Folly  | xSPSC | xFolly\n";
	std::cout.fill('-');
	std::cout              << std::setw(BENCHMARK_NAME_MAX) << "---------" << "-+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+-------+-------\n";
	std::cout.fill(' ');
	double rwqOpsPerSec = 0, brwcbOpsPerSec = 0, spscOpsPerSec = 0, follyOpsPerSec = 0;
	int opTimedBenchmarks = 0;
	for (int benchmark = 0; benchmark < BENCHMARK_COUNT; ++benchmark) {
		double rwqMin = rwqResults[benchmark][0], rwqMax = rwqResults[benchmark][max - 1];
		double brwcbMin = brwcbResults[benchmark][0], brwcbMax = brwcbResults[benchmark][max - 1];
		double spscMin = spscResults[benchmark][0], spscMax = spscResults[benchmark][max - 1];
		double follyMin = follyResults[benchmark][0], follyMax = follyResults[benchmark][max - 1];
		double rwqAvg = std::accumulate(&rwqResults[benchmark][0], &rwqResults[benchmark][0] + max, 0.0) / max;
		double brwcbAvg = std::accumulate(&brwcbResults[benchmark][0], &brwcbResults[benchmark][0] + max, 0.0) / max;
		double spscAvg = std::accumulate(&spscResults[benchmark][0], &spscResults[benchmark][0] + max, 0.0) / max;
		double follyAvg = std::accumulate(&follyResults[benchmark][0], &follyResults[benchmark][0] + max, 0.0) / max;
		double spscMult = rwqAvg < 0.00001 ? 0 : spscAvg / rwqAvg;
		double follyMult = follyAvg < 0.00001 ? 0 : follyAvg / rwqAvg;

		if (rwqResults[benchmark][0] != -1) {
			double rwqTotalAvg = std::accumulate(&rwqResults[benchmark][0], &rwqResults[benchmark][0] + TEST_COUNT, 0.0) / TEST_COUNT;
			double brwcbTotalAvg = std::accumulate(&brwcbResults[benchmark][0], &brwcbResults[benchmark][0] + TEST_COUNT, 0.0) / TEST_COUNT;
			double spscTotalAvg = std::accumulate(&spscResults[benchmark][0], &spscResults[benchmark][0] + TEST_COUNT, 0.0) / TEST_COUNT;
			double follyTotalAvg = std::accumulate(&follyResults[benchmark][0], &follyResults[benchmark][0] + TEST_COUNT, 0.0) / TEST_COUNT;
			rwqOpsPerSec += rwqTotalAvg == 0 ? 0 : std::accumulate(&rwqOps[benchmark][0], &rwqOps[benchmark][0] + TEST_COUNT, 0.0) / TEST_COUNT / rwqTotalAvg;
			brwcbOpsPerSec += brwcbTotalAvg == 0 ? 0 : std::accumulate(&brwcbOps[benchmark][0], &brwcbOps[benchmark][0] + TEST_COUNT, 0.0) / TEST_COUNT / brwcbTotalAvg;
			spscOpsPerSec += spscTotalAvg == 0 ? 0 : std::accumulate(&spscOps[benchmark][0], &spscOps[benchmark][0] + TEST_COUNT, 0.0) / TEST_COUNT / spscTotalAvg;
			follyOpsPerSec += follyTotalAvg == 0 ? 0 : std::accumulate(&follyOps[benchmark][0], &follyOps[benchmark][0] + TEST_COUNT, 0.0) / TEST_COUNT / follyTotalAvg;
			++opTimedBenchmarks;
		}

		std::cout
			<< std::left << std::setw(BENCHMARK_NAME_MAX) << benchmarkName((BenchmarkType)benchmark) << " | "
			<< std::fixed << std::setprecision(4) << rwqMin << "s | "
			<< std::fixed << std::setprecision(4) << brwcbMin << "s | "
			<< std::fixed << std::setprecision(4) << spscMin << "s | "
			<< std::fixed << std::setprecision(4) << follyMin << "s | "
			<< std::fixed << std::setprecision(4) << rwqMax << "s | "
			<< std::fixed << std::setprecision(4) << brwcbMax << "s | "
			<< std::fixed << std::setprecision(4) << spscMax << "s | "
			<< std::fixed << std::setprecision(4) << follyMax << "s | "
			<< std::fixed << std::setprecision(4) << rwqAvg << "s | "
			<< std::fixed << std::setprecision(4) << brwcbAvg << "s | "
			<< std::fixed << std::setprecision(4) << spscAvg << "s | "
			<< std::fixed << std::setprecision(4) << follyAvg << "s | "
			<< std::fixed << std::setprecision(2) << spscMult << "x | "
			<< std::fixed << std::setprecision(2) << follyMult << "x"
			<< "\n"
		;
	}

	rwqOpsPerSec /= opTimedBenchmarks;
	brwcbOpsPerSec /= opTimedBenchmarks;
	spscOpsPerSec /= opTimedBenchmarks;
	follyOpsPerSec /= opTimedBenchmarks;

	std::cout
		<< "\nAverage ops/s:\n"
		<< "    ReaderWriterQueue:                  " << std::fixed << std::setprecision(2) << rwqOpsPerSec / 1000000 << " million\n"
		<< "    BlockingReaderWriterCircularBuffer: " << std::fixed << std::setprecision(2) << brwcbOpsPerSec / 1000000 << " million\n"
		<< "    SPSC queue:                         " << std::fixed << std::setprecision(2) << spscOpsPerSec / 1000000 << " million\n"
		<< "    Folly queue:                        " << std::fixed << std::setprecision(2) << follyOpsPerSec / 1000000 << " million\n"
	;
	std::cout << std::endl;

	return 0;
}


template<typename TQueue>
double runBenchmark(BenchmarkType benchmark, unsigned int randomSeed, double& out_Ops)
{
	typedef unsigned long long counter_t;

	SystemTime start;
	double result = 0;
	volatile int forceNoOptimizeDummy;

	switch (benchmark) {
	case bench_raw_add: {
		const counter_t MAX = 100 * 1000;
		out_Ops = MAX;
		TQueue q(MAX);
		int num = 0;
		start = getSystemTime();
		for (counter_t i = 0; i != MAX; ++i) {
			q.enqueue(num);
			++num;
		}
		result = getTimeDelta(start);
		
		int temp = -1;
		q.try_dequeue(temp);
		forceNoOptimizeDummy = temp;
	} break;
	case bench_raw_remove: {
		const counter_t MAX = 100 * 1000;
		out_Ops = MAX;
		TQueue q(MAX);
		int num = 0;
		for (counter_t i = 0; i != MAX; ++i) {
			q.enqueue(num);
			++num;
		}

		int element = -1;
		int total = 0;
		num = 0;
		start = getSystemTime();
		for (counter_t i = 0; i != MAX; ++i) {
			bool success = q.try_dequeue(element);
			assert(success && num++ == element);
			UNUSED(success);
			total += element;
		}
		result = getTimeDelta(start);
		assert(!q.try_dequeue(element));
		forceNoOptimizeDummy = total;
	} break;
	case bench_empty_remove: {
		const counter_t MAX = 2000 * 1000;
		out_Ops = MAX;
		TQueue q(MAX);
		int total = 0;
		start = getSystemTime();
		SimpleThread consumer([&]() {
			int element;
			for (counter_t i = 0; i != MAX; ++i) {
				if (q.try_dequeue(element)) {
					total += element;
				}
			}
		});
		SimpleThread producer([&]() {
			int num = 0;
			for (counter_t i = 0; i != MAX / 2; ++i) {
				if ((i & 32767) == 0) {		// Just to make sure the loops aren't optimized out entirely
					q.enqueue(num);
					++num;
				}
			}
		});
		producer.join();
		consumer.join();
		result = getTimeDelta(start);
		forceNoOptimizeDummy = total;
	} break;
	case bench_single_threaded: {
		const counter_t MAX = 200 * 1000;
		out_Ops = MAX;
		RNG_t rng(randomSeed);
		std::uniform_int_distribution<int> rand(0, 1);
		TQueue q(MAX);
		int num = 0;
		int element = -1;
		start = getSystemTime();
		for (counter_t i = 0; i != MAX; ++i) {
			if (rand(rng) == 1) {
				q.enqueue(num);
				++num;
			}
			else {
				q.try_dequeue(element);
			}
		}
		result = getTimeDelta(start);
		forceNoOptimizeDummy = (int)(q.try_dequeue(element));
	} break;
	case bench_mostly_add: {
		const counter_t MAX = 1200 * 1000;
		out_Ops = MAX;
		int readOps = 0;
		RNG_t rng(randomSeed);
		std::uniform_int_distribution<int> rand(0, 3);
		TQueue q(MAX);
		int element = -1;
		start = getSystemTime();
		SimpleThread consumer([&]() {
			for (counter_t i = 0; i != MAX / 10; ++i) {
				if (rand(rng) == 0) {
					q.try_dequeue(element);
					++readOps;
				}
			}
		});
		SimpleThread producer([&]() {
			int num = 0;
			for (counter_t i = 0; i != MAX; ++i) {
				q.enqueue(num);
				++num;
			}
		});
		producer.join();
		consumer.join();
		result = getTimeDelta(start);
		forceNoOptimizeDummy = (int)(q.try_dequeue(element));
		out_Ops += readOps;
	} break;
	case bench_mostly_remove: {
		const counter_t MAX = 1200 * 1000;
		out_Ops = MAX;
		int writeOps = 0;
		RNG_t rng(randomSeed);
		std::uniform_int_distribution<int> rand(0, 3);
		TQueue q(MAX);
		int element = -1;
		start = getSystemTime();
		SimpleThread consumer([&]() {
			for (counter_t i = 0; i != MAX; ++i) {
				q.try_dequeue(element);
			}
		});
		SimpleThread producer([&]() {
			int num = 0;
			for (counter_t i = 0; i != MAX / 10; ++i) {
				if (rand(rng) == 0) {
					q.enqueue(num);
					++num;
				}
			}
			writeOps = num;
		});
		producer.join();
		consumer.join();
		result = getTimeDelta(start);
		forceNoOptimizeDummy = (int)(q.try_dequeue(element));
		out_Ops += writeOps;
	} break;
	case bench_heavy_concurrent: {
		const counter_t MAX = 1000 * 1000;
		out_Ops = MAX * 2;
		TQueue q(MAX);
		int element = -1;
		start = getSystemTime();
		SimpleThread consumer([&]() {
			for (counter_t i = 0; i != MAX; ++i) {
				q.try_dequeue(element);
			}
		});
		SimpleThread producer([&]() {
			int num = 0;
			for (counter_t i = 0; i != MAX; ++i) {
				q.enqueue(num);
				++num;
			}
		});
		producer.join();
		consumer.join();
		result = getTimeDelta(start);
		forceNoOptimizeDummy = (int)(q.try_dequeue(element));
	} break;
	case bench_random_concurrent: {
		const counter_t MAX = 800 * 1000;
		int readOps = 0, writeOps = 0;
		TQueue q(MAX);
		int element = -1;
		start = getSystemTime();
		SimpleThread consumer([&]() {
			RNG_t rng(randomSeed);
			std::uniform_int_distribution<int> rand(0, 15);
			for (counter_t i = 0; i != MAX; ++i) {
				if (rand(rng) == 0) {
					q.try_dequeue(element);
					++readOps;
				}
			}
		});
		SimpleThread producer([&]() {
			RNG_t rng(randomSeed * 3 - 1);
			std::uniform_int_distribution<int> rand(0, 15);
			int num = 0;
			for (counter_t i = 0; i != MAX; ++i) {
				if (rand(rng) == 0) {
					q.enqueue(num);
					++num;
				}
			}
			writeOps = num;
		});
		producer.join();
		consumer.join();
		result = getTimeDelta(start);
		forceNoOptimizeDummy = (int)(q.try_dequeue(element));
		out_Ops = readOps + writeOps;
	} break;
	default:
		assert(false);
		out_Ops = 0;
		return 0;
	}

	UNUSED(forceNoOptimizeDummy);
	return result / 1000.0;
}

const char* benchmarkName(BenchmarkType benchmark)
{
	switch (benchmark) {
	case bench_raw_add: return "Raw add";
	case bench_raw_remove: return "Raw remove";
	case bench_empty_remove: return "Raw empty remove";
	case bench_single_threaded: return "Single-threaded";
	case bench_mostly_add: return "Mostly add";
	case bench_mostly_remove: return "Mostly remove";
	case bench_heavy_concurrent: return "Heavy concurrent";
	case bench_random_concurrent: return "Random concurrent";
	default: return "";
	}
}
