#ifndef BEMAN_COPYABLE_FUNCTION_BUFFER
#define BEMAN_COPYABLE_FUNCTION_BUFFER



template<std::size_t BufferSize, std::size_t Alignment> 
class Buffer
{
    private:
        alignas(Alignment) std::byte buffer[BufferSize];
    public: 

    template<class Func> 
    Func* __get() 
    {
        using Stored = std::decay_t<Func>;
        return *std::launder(reinterpret_cast<Func**>(buffer));
    }

    template<class Func, class... Args> 
    void construct(Args&&... args)
    {
        using Stored = std::decay_t<Func>;
        Stored* ptr = new Stored(std::forward<Args>(args)...);
        std::construct_at(reinterpret_cast<Stored**>(buffer), ptr);
    }
        
        
};

template<class BufferType, class R, class... Args> 
struct VTable
{
    R (*call)(BufferType&, Args&&...);
    void (*destroy)(BufferType&);
};


#endif