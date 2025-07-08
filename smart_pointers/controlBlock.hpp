#ifndef CONTROLBLOCK_HPP
#define CONTROLBLOCK_HPP
#include<cstdint>
#include<utility>   

template <typename T>
class controlBlock
{
public:
    explicit controlBlock(T* ptr)
    : m_resource(ptr),m_shrCount(1) {}

    template<typename... Args>
    controlBlock(Args&&... args) 
        : m_resource(new T(std::forward<Args>(args)...)),m_shrCount(1),m_weakCount(0) {}

    std::uint64_t useCount() const { return m_shrCount; }

    T* get() const { return m_resource;}

    void increment() { ++m_shrCount; }

    void decrement()
    {
        if(--m_shrCount == 0)
        {
            delete m_resource;
            m_resource = nullptr;
            if(m_weakCount == 0) delete this;
        }
    }

    void incrementWeak() { ++m_weakCount; }
    
    void decrementWeak()
    {
        if(--m_weakCount == 0 and m_shrCount == 0) delete this;
    }
    
    ~controlBlock()
    {
        if(--m_weakCount==0 and m_shrCount==0) delete this;
    }

private:
    T* m_resource;
    std::uint64_t m_shrCount = 1;
    std::uint64_t m_weakCount = 0;
};

#endif
