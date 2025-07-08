
#include "shrPtr.hpp"
#include "weakPtr.hpp"
#include <iostream>
#include <cassert>

struct Track {
    int value;
    Track(int v) : value(v) {
        std::cout << "[Track] Constructed with value " << value << '\n';
    }
    ~Track() {
        std::cout << "[Track] Destroyed with value " << value << '\n';
    }
};

int main() {
    std::cout << "=== TEST 1: Basic lock() success ===\n";
    {
        auto shared = makeShr<Track>(42);
        weakPtr<Track> weak(shared);
        assert(!weak.expired());

        {
            auto locked = weak.lock();
            assert(locked);
            assert(locked->value == 42);
        }

        std::cout << "useCount: " << shared.useCount() << '\n';
        assert(!weak.expired());
    }
    std::cout << "-> Exited scope, object should be destroyed.\n\n";

    std::cout << "=== TEST 2: Expired weakPtr ===\n";
    weakPtr<Track> dangling;
    {
        auto shared = makeShr<Track>(100);
        dangling = weakPtr<Track>(shared);
    }
    assert(dangling.expired());
    assert(!dangling.lock());
    std::cout << "-> lock() returned null as expected.\n\n";

    std::cout << "=== TEST 3: Multiple weak pointers ===\n";
    {
        auto shared = makeShr<Track>(77);
        weakPtr<Track> w1(shared), w2(shared);
        assert(!w1.expired() && !w2.expired());

        shared = shrPtr<Track>(); //Just drop the last shared here and make me happy!!
        assert(w1.expired());
        assert(w2.expired());
    }

    std::cout << "\nAll tests passed.\n";
    return 0;
}
