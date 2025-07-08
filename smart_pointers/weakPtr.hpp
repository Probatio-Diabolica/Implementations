#ifndef WEAK_PTR_HPP
#define WEAK_PTR_HPP

#include"controlBlock.hpp"
#include"shrPtr.hpp"
#include <utility>

template <typename T>
class weakPtr
{
public:
    weakPtr() = default;

    weakPtr(const shrPtr<T>& shared)
    : m_ctrlBlk(shared.m_ctrlBlk)
    {
        if(m_ctrlBlk) m_ctrlBlk->incrementWeak();
    }

    //cpy
    weakPtr(const weakPtr& other)
    :m_ctrlBlk(other.m_ctrlBlk)
    {
        if(m_ctrlBlk) m_ctrlBlk->incrementWeak();
    }

    //move
    weakPtr(weakPtr&& other) noexcept
        : m_ctrlBlk(std::exchange(other.m_ctrlBlk, nullptr))
    {}

    //cpy  assignment
    weakPtr& operator =(const weakPtr& other)
    {
        if(this != &other)
        {
            release();

            m_ctrlBlk = other.m_ctrlBlk;

            if(m_ctrlBlk) m_ctrlBlk->incrementWeak();
        }

        return *this;
    }

    //move assignment
    weakPtr& operator=(weakPtr&& other) noexcept
    {
        if(this != &other)
        {
            release();

            m_ctrlBlk = std::exchange(other.m_ctrlBlk, nullptr);
        }
        return *this;
    }

    shrPtr<T> lock() const
    {
        if(m_ctrlBlk and m_ctrlBlk->useCount() > 0) return shrPtr<T>(m_ctrlBlk,true);

        return shrPtr<T>{};
    }

    bool expired() const {return !m_ctrlBlk or m_ctrlBlk->useCount()==0;}

    void reset()
    {
        release();
        m_ctrlBlk = nullptr;
    }
    ~weakPtr()
    {
        release();
    }

private:
    controlBlock<T>* m_ctrlBlk = nullptr;

    void release()
    {
        if(m_ctrlBlk) m_ctrlBlk->decrementWeak();
    }
};

#endif
