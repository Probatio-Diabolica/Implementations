#ifndef SHRPTR_HPP
#define SHRPTR_HPP

#include "controlBlock.hpp"
#include <utility>

template<typename T>
class weakPtr;

template <typename T>
class shrPtr
{
public:
    shrPtr() = default; 
   
    //I scaled this constructor so that it can work for lock() 
    //tho, this allows optional ref increment
    explicit shrPtr(controlBlock<T>* cb, bool addRef)
        : m_ctrlBlk(cb)
    {
        if(m_ctrlBlk and m_ctrlBlk->useCount() > 0)
        {
            if(addRef) m_ctrlBlk->increment();
            m_resource = m_ctrlBlk->get();
        }
        else 
        {
            m_ctrlBlk  = nullptr;
            m_resource = nullptr;
        }
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

    ~shrPtr()
    {
        if(m_ctrlBlk) m_ctrlBlk->decrement();
    }

    explicit operator bool() const { return m_resource != nullptr; }

    //swap
    void swap(shrPtr& other)
    {
        std::swap(other.m_resource , m_resource);
        std::swap(other.m_ctrlBlk,m_ctrlBlk);
    }

    T* get() const { return m_resource; }

    T& operator *() const  { return *m_resource; }

    T* operator ->() const { return get(); }

    std::uint64_t useCount() const 
    {
        return (m_ctrlBlk) ? m_ctrlBlk->useCount() : 0;
    }

    template<typename N> 
    friend class weakPtr;

private:
    T* m_resource = nullptr;
    controlBlock<T>* m_ctrlBlk = nullptr;

    explicit shrPtr(T* ptr) = delete;
};



template<typename N, typename... Args>
shrPtr<N> makeShr(Args&&... args)
{
    auto cv = new controlBlock<N>(std::forward<Args>(args)...);

    return shrPtr<N> {cv,false};
}

#endif
