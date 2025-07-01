#ifndef UPTR_HPP
#define UPTR_HPP

#include <utility>

template <typename T>
class uptr
{
public:
    uptr():m_ptr(nullptr){}
    explicit uptr(T *ptr = nullptr) : m_ptr(ptr){}

    uptr(const uptr& other) = delete;

    uptr(uptr&& ptr): m_ptr(ptr.release()){}

    template<class U>
    uptr(uptr<U>&& ptr) : m_ptr(ptr.release()){}

    ~uptr()
    {
        reset();
    }
    
    template<class U>
    
    void reset(uptr<U>&& ptr)
    {
        reset(ptr.release());
    }

    uptr& operator= (uptr&& ptr)
    {
        reset(ptr.release());
        return *this;                
    }

    template<class U>
    uptr& operator=(uptr<U>&& ptr)
    {
        reset(ptr.release());
        return *this;
    }

    uptr& operator= (const uptr& ptr) = delete;

    T* get() const {return m_ptr;}

    T* operator-> () const  {return get();}

    T& operator* () const {return *get();}

    T* release()
    {
        auto temp = m_ptr;
        m_ptr = nullptr;
        return temp;
    }

    void reset(T* newPtr = nullptr)
    {
        if(newPtr == m_ptr) return;
        
        delete m_ptr;
        m_ptr = newPtr;
    }

    void swap(uptr& other)
    {
        std::swap(this->m_ptr,other.m_ptr);
    }

    explicit operator bool() const 
    {
        return get() != nullptr;
    }

private:
    T *m_ptr = nullptr;

};

#endif
