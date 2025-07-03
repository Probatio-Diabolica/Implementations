#include <iostream>
#include <cassert>
#include <vector>

#include "shrPtr.hpp"

struct Spy {
    int id;
    static inline int constructed = 0;
    static inline int destroyed = 0;

    Spy(int i) : id(i) 
    {
        ++constructed;
        std::cout << "Spy constructed: " << id << '\n';
    }

    ~Spy() 
    {
        ++destroyed;
        std::cout << "Spy destroyed: " << id << '\n';
    }
};

void resetSpy() 
{
    Spy::constructed = 0;
    Spy::destroyed = 0;
}

void nullTest() 
{
    std::cout << "\n[ NULL TEST ]\n";
    shrPtr<Spy> p; 
    assert(p.get() == nullptr);
    assert(!p);
}

void selfAssignTest() 
{
    std::cout << "\n[ SELF ASSIGNMENT TEST ]\n";
    auto s = makeShr<Spy>(100);
    s = s;
    assert(s.useCount() == 1);
    assert(s.get()->id == 100);
}

void copyTest() 
{
    std::cout << "\n[ COPY TEST ]\n";
    resetSpy();
    shrPtr<Spy> base = makeShr<Spy>(1);

    const int N = 1'000;
    std::vector<shrPtr<Spy>> army;
    army.reserve(N);

    for (int i = 0; i < N; ++i)
        army.push_back(base);

    assert(base.useCount() == N + 1);
    army.clear();
    assert(base.useCount() == 1);

    std::cout << "Constructions: " << Spy::constructed << "\nDestructions: " << Spy::destroyed << '\n';
    assert(Spy::constructed == 1);
}

void moveChainTest() 
{
    std::cout << "\n[ MOVE CHAIN TEST ]\n";
    resetSpy();
    shrPtr<Spy> a = makeShr<Spy>(9);
    for (int i = 0; i < 1000; ++i) 
    {
        shrPtr<Spy> temp = std::move(a);
        a = std::move(temp);
    }
    assert(a.get()->id == 9);
    assert(Spy::constructed == 1);
    assert(Spy::destroyed == 0);
}

void nestedLifetimeTest() 
{
    std::cout << "\n[ NESTED LIFETIME TEST ]\n";
    resetSpy();
    {
        shrPtr<Spy> outer = makeShr<Spy>(888);
        {
            shrPtr<Spy> inner1 = outer;
            shrPtr<Spy> inner2 = inner1;
            assert(inner1.useCount() == 3);
        }
        assert(outer.useCount() == 1);
    }
    assert(Spy::constructed == 1);
    assert(Spy::destroyed == 1);
}

void manualDtorTest() 
{
    std::cout << "\n[ MANUAL DESTRUCTOR TEST ]\n";
    resetSpy();
    shrPtr<Spy>* s = new shrPtr<Spy>(makeShr<Spy>(555));
    delete s;
    assert(Spy::destroyed == 1);
}

int main() 
{
    
    nullTest();
    
    selfAssignTest();
    
    copyTest();
    
    moveChainTest();
    
    nestedLifetimeTest();
    
    manualDtorTest();
}

