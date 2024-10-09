#include <chrono>
#include <iostream>
#include "spdlog/spdlog.h"

using namespace std;

namespace timer {
    class Timer {

    private:
        vector<pair<string, long long>> timingEntries;

    public:
        void timeit(const string &name, chrono::time_point<chrono::high_resolution_clock> start) {
            auto end = chrono::high_resolution_clock::now();
            long long elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            timingEntries.emplace_back(name, elapsed);
        }

        void logTime() {
            for (auto &entry: timingEntries) {
                spdlog::info("'{}' executed in {} ms", entry.first, entry.second);
            }
            // Clear the entries after printing
            timingEntries.clear();
        }
    };
}