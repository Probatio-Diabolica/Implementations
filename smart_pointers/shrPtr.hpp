#ifndef SHRPTR_HPP
#define SHRPTR_HPP

#include <cstdint>
#include <utility>

template<typename T>
struct controlBlock
{
private:
    T* m_resource;
    std::uint64_t m_refCount=0;
public:
    controlBlock(T* ptr): m_resource(ptr),m_refCount(1){}

    template <typename... Args>
    controlBlock(Args&&... args)
        : m_resource(new T(std::forward<Args>(args)...)), m_refCount(1)
    {}
        
    std::uint64_t useCount() const
    {
            return m_refCount;
    }

    T* get() const
    {
        return m_resource;
    }

    void increment() {++m_refCount;}
        
    void decrement() 
    {
        if(--m_refCount == 0)
        {
            delete m_resource;
            delete this;
        }
    }
};

template <typename T>
class shrPtr
{
public:
    shrPtr() = default; 
    explicit shrPtr(T* ptr)
        : m_resource(ptr), m_ctrlBlk(new controlBlock(ptr)){}
    
    explicit shrPtr(controlBlock<T>* cb)
    {
        m_ctrlBlk = cb;
        if(m_ctrlBlk) m_resource = m_ctrlBlk->get();
    }
   
    //cpy conc
    shrPtr(const shrPtr& other)
        :m_resource(other.m_resource), m_ctrlBlk(other.m_ctrlBlk)
    {
        if(m_ctrlBlk) m_ctrlBlk->increment();
    }
    
    //move conc
    shrPtr(shrPtr&& other) noexcept
        :m_resource(std::exchange(other.m_resource, nullptr)),
         m_ctrlBlk(std::exchange(other.m_ctrlBlk, nullptr))
    {}

    //copy assignment op
    shrPtr& operator =(const shrPtr& other)
    {
        if(this != &other)
        {
            if(m_ctrlBlk) m_ctrlBlk->decrement();

            m_resource = other.m_resource;
            m_ctrlBlk = other.m_ctrlBlk;

            if(m_ctrlBlk) m_ctrlBlk->increment();
        }

        return *this;
    }
   
    //move assignment op
    shrPtr& operator =(shrPtr&& other) noexcept
    {
        if(this != &other)
        {
            if(m_ctrlBlk) m_ctrlBlk->decrement();

             m_resource = std::exchange(other.m_resource,nullptr);
             m_ctrlBlk  = std::exchange(other.m_ctrlBlk,nullptr);
        }
        return *this;
    }

    explicit operator bool() const
    {
        return m_resource != nullptr;
    }

    //swap
    void swap(shrPtr& other)
    {
        std::swap(other.m_resource , m_resource);
        std::swap(other.m_ctrlBlk,m_ctrlBlk);
    }

    T* get() const {return m_resource;}

    T& operator *() const  {return *m_resource;}

    T* operator ->() const {return get();}

    std::uint64_t useCount() const {return m_ctrlBlk->useCount();}

    ~shrPtr()
    {
        if(m_ctrlBlk) m_ctrlBlk->decrement();
    }

    template<typename N, typename... Args>
    friend shrPtr<N> makeShr(Args&&... args);

private:
    
    
    T* m_resource = nullptr;
    controlBlock<T>* m_ctrlBlk = nullptr;

};



template<typename N, typename... Args>
shrPtr<N> makeShr(Args&&... args)
{
    auto cv = new controlBlock<N>(std::forward<Args>(args)...);

    return shrPtr<N> {cv};
}

#endif
