#ifndef BEMAN_COPYABLE_FUNCTION_HELPER
#define BEMAN_COPYABLE_FUNCTION_HELPER

#include <new>

template <std::size_t BufferSize, std::size_t Alignment>
class Buffer {
  private:
    alignas(Alignment) std::byte buffer[BufferSize];

  public:
    Buffer()                         = default;
    Buffer(const Buffer&)            = default;
    Buffer& operator=(const Buffer&) = default;
    ~Buffer()                        = default;

    Buffer(Buffer&&)            = default;
    Buffer& operator=(Buffer&&) = default;

    template <class DecayType>
    DecayType* get_ptr() {
        if (sizeof(DecayType) <= BufferSize) {
            return std::launder(reinterpret_cast<DecayType*>(buffer));
        } else {
            return std::launder(reinterpret_cast<DecayType**>(buffer))[0];
        }
    }

    template <class DecayType, class... Args>
    void construct(Args&&... args) {
        if (sizeof(DecayType) <= BufferSize) {
            std::construct_at(std::launder(reinterpret_cast<DecayType*>(buffer)), std::forward<Args>(args)...);
        } else {
            std::byte* allocated_ptr =
                static_cast<std::byte*>(::operator new(sizeof(DecayType), std::align_val_t{alignof(DecayType)}));
            std::construct_at(reinterpret_cast<std::byte**>(buffer), allocated_ptr);
            std::construct_at(std::launder(reinterpret_cast<DecayType*>(allocated_ptr)), std::forward<Args>(args)...);
        }
    }
};

template <class BufferType, class R, class... Args>
struct VTable {
    R (*call)(BufferType&, Args&&...);
    void (*destroy)(BufferType&);
    void (*clone)(BufferType& from, BufferType& to);
};

#endif
