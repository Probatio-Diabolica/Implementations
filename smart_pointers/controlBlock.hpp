#ifndef CONTROLBLOCK_HPP
#define CONTROLBLOCK_HPP
#include<cstdint>
#include<utility>   

template<typename T>
class shrPtr;

template<typename T>
class weakPtr;

template <typename T>
class controlBlock
{
public:

    template<typename... Args>
    explicit controlBlock(Args&&... args)
        : m_resource(new T(std::forward<Args>(args)...)) 
    {}

    std::uint64_t useCount() const { return m_shrCount; }

    T* get() const { return m_resource;}

    void increment() { ++m_shrCount; }

    void decrement()
    {
        if(--m_shrCount == 0)
        {
            delete m_resource;
            m_resource = nullptr;
            delSelf();
        }
    }

    void incrementWeak() { ++m_weakCount; }
    
    void decrementWeak()
    {
        --m_weakCount;
        delSelf();
    }
    
    ~controlBlock() = default;
    
private:
    T* m_resource;
    std::uint64_t m_shrCount = 1;
    std::uint64_t m_weakCount = 0;

    void delSelf()
    {
        if(m_shrCount == 0 and m_weakCount == 0) delete this;
    }

    template<typename>
    friend class shrPtr;

    template<typename>
    friend class weakPtr;
};

#endif
