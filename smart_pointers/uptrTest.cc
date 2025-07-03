#include "uptr.hpp"
#include <iostream>
#include <assert.h>
#include <vector>

struct A
{
    int a=0;
    int b=0;
    A(int x, int y):a(x),b(y){}
};

struct Mover {
    Mover() = default;
    Mover(const Mover&) = delete;
    Mover& operator=(const Mover&) = delete;
    Mover(Mover&&) = default;
    Mover& operator=(Mover&&) = default;
};

std::ostream& operator<<(std::ostream& out, A var)
{
    out<<var.a<<" | "<<var.b<<'\n';

    return out;
}


int main()
{
    //normal
    uptr<A> a(new A{100,9090});
    std::cout<< *(a.get());
    
    //test: 'move only' stuff
    uptr<Mover> m1(new Mover());
    uptr<Mover> m2 = std::move(m1);

    //test: resetting to nullptr
    uptr<int> p(new int(5));
    p.reset(); 
    assert(!p);

    //test: inside nested vector
    uptr<std::vector<uptr<int>>> vec(new std::vector<uptr<int>>());

    for (int i = 0; i < 1000; ++i) 
    {
        vec->emplace_back(new int(i));
    }

    //test: protection from self assignment
    uptr<int> p2(new int(5));
    p2.reset(p2.get());
    
    //test: swaping the ptrs
    uptr<int> x(new int(42)); //42 is the answer of everything :3
    uptr<int> y(new int(123));
    
    std::cout<<"Before swap "<<*(x.get()) <<" | "<< *(y.get())<<'\n';
    
    x.swap(y);
    
    std::cout<<"After swap "<<*(x.get()) <<" | "<< *(y.get())<<'\n';
}
