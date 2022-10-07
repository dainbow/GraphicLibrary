#pragma once

#include <iostream>

#include <cstdint>
#include <cassert>

namespace Data {
    template <class T>
    void Destroy(T& elem) {
        elem.~T();
    }

    template <int>
    void Destroy(int& elem) {}

    template <class T, uint64_t tSize>
    class Array {
        T data_[tSize];
        uint64_t size_ = tSize;

        public:
            uint64_t GetSize() const {
                return size_;
            }

            Array() {
                for (uint64_t curIdx = 0; curIdx < tSize; curIdx++) {
                    data_[curIdx] = 0;
                }
            }

            Array(const T& fill) {
                for (uint64_t curIdx = 0; curIdx < tSize; curIdx++) {
                    data_[curIdx] = fill;
                }
            }

            Array(const Array& array) {
                *this = array;
            }

            T& operator[](uint64_t idx) {
                return data_[idx];
            }

            Array& operator=(const Array& array) {
                for (uint64_t curIdx = 0; curIdx < tSize; curIdx++) {
                    data_[curIdx] = array.data_[curIdx];
                }

                return *this;
            }

            void* operator new(uint64_t size) {
                std::cout << "Operator new for Array class" << std::endl;

                void* newPtr = calloc(size, 1);
                assert(newPtr && "CAN'T ALLOCATE MEMORY FOR ARRAY CLASS");

                return newPtr;
            }

            void* operator new[](uint64_t size) {
                std::cout << "Operator new[] for Array class" << std::endl;

                void* newPtr = calloc(size, 1);
                assert(newPtr && "CAN'T ALLOCATE MEMORY FOR ARRAY CLASS");

                return newPtr;
            }

            void operator delete(void* ptr) {
                std::cout << "Operator delete for array class" << std::endl;

                free(ptr);
            }

            void operator delete[](void* ptr) {
                std::cout << "Operator delete[] for array class" << std::endl;

                free(ptr);
            }

            ~Array() {
                for (uint64_t curIdx = 0; curIdx < tSize; curIdx++) {
                    Destroy(data_[curIdx]);
                }
            }
    };

    const uint64_t DefaultVectorCapacity     = 10;
    const uint64_t DefaultCapacityMultiplier = 2;

    template <class T>
    class Vector {
        char* data_;

        uint64_t size_;
        uint64_t capacity_;

        public:
            Vector(const uint64_t size) :
                data_(nullptr),
                size_(size), 
                capacity_(DefaultCapacityMultiplier)
                {
                while (capacity_ < size_) {
                    capacity_ *= DefaultCapacityMultiplier;
                }

                try {
                    data_     = new char[capacity_ * sizeof(T)];
                }
                catch (std::bad_alloc& badAlloc) {
                    std::cerr << "Can't alloc memory for vector" << std::endl;

                    size_     = 0;
                    capacity_ = 0;

                    throw std::bad_alloc();
                }
                
                for (uint64_t curIdx = 0; curIdx < size_; curIdx++) {
                    uint64_t curTranslatedIdx = curIdx * sizeof(T);

                    new (data_ + curTranslatedIdx) T();
                }   
            }

            Vector() {
                Vector(0);
            }

            uint64_t GetSize() const {
                return size_;
            }

            uint64_t GetCapacity() const {
                return capacity_;
            }

            T& operator[](const uint64_t idx) {
                if (idx >= size_) {
                    throw std::range_error("Invalid index for vector");
                }

                uint64_t curTranslatedIdx = idx * sizeof(T);
                return *((T*)(data_ + curTranslatedIdx));
            }


            bool Resize(uint64_t size) {
                if (size <= size_) {
                    for (uint64_t curIdx = size; curIdx < size_; curIdx++) {
                        Destroy((*this)[curIdx]);
                    }

                    size_ = size;
                    return 0;
                }

                if (size <= capacity_) {
                    for (uint64_t curIdx = size; curIdx < size_; curIdx++) {
                        uint64_t curTranslatedIdx = curIdx * sizeof(T);

                        new (data_ + curTranslatedIdx) T();
                    }

                    size_ = size;
                    return 0;
                }

                uint64_t newCapacity = capacity_;
                while (newCapacity < size) {
                    newCapacity *= DefaultCapacityMultiplier;
                }

                char* newPtr = nullptr;
                try {
                    newPtr = new char[newCapacity * sizeof(T)]();
                }
                catch (std::bad_alloc) {
                    std::cerr << "Can't alloc memory to increase vector" << std::endl;

                    throw std::bad_alloc();
                }
                
                for (uint64_t curIdx = 0; curIdx < size_; curIdx++) {
                    uint64_t curTranslatedIdx = curIdx * sizeof(T);

                    new (newPtr + curTranslatedIdx) T((*this)[curIdx]);
                }

                for (uint64_t curIdx = size_; curIdx < size; curIdx++) {
                    uint64_t curTranslatedIdx = curIdx * sizeof(T);

                    new (newPtr + curTranslatedIdx) T();
                }

                delete[] data_;
                data_ = newPtr;

                capacity_ = newCapacity;
                size_     = size;

                return 0;
            }

            bool PushBack(const T& elem) {
                try {
                    Resize(size_ + 1);
                }
                catch (std::bad_alloc& badAlloc) {
                    std::cerr << "Can't pushBack " << elem << " due to lack of memory" << std::endl;

                    return 1;
                }
                
                (*this)[size_ - 1] = elem;

                return 0;
            }

            Vector& operator=(const Vector& vector) {
                Resize(vector.GetSize());

                for (uint64_t curIdx = 0; curIdx < size_; curIdx++) {
                    uint64_t convertedIdx = curIdx * sizeof(T);

                    (*this)[curIdx] = vector[curIdx];
                }

                return *this;
            }

            Vector(const Vector& vector) :
                data_(new char[vector.GetCapacity() * sizeof(T)]),
                size_(vector.GetSize()), 
                capacity_(vector.GetCapacity())
            {
                for (uint64_t curIdx = 0; curIdx < size_; curIdx++) {
                    uint64_t convertedIdx = curIdx * sizeof(T); 

                    new (data_ + convertedIdx) T(vector[curIdx]);
                }
            }

            void* operator new(uint64_t size) {
                std::cout << "Operator new for Data::Vector class" << std::endl;

                void* newPtr = calloc(size, 1);
                if (!newPtr) {
                    throw std::bad_alloc();
                }

                return newPtr;
            }

            void* operator new[](uint64_t size) {
                std::cout << "Operator new[] for Data::Vector class" << std::endl;

                void* newPtr = calloc(size, 1);
                if (!newPtr) {
                    throw std::bad_alloc();
                }

                return newPtr;
            }

            void operator delete(void* ptr) {
                std::cout << "Operator delete for Data::Vector class" << std::endl;

                free(ptr);
            }

            void operator delete[](void* ptr) {
                std::cout << "Operator delete[] for Data::Vector class" << std::endl;

                free(ptr);
            }

            ~Vector() {
                for (uint64_t curIdx = 0; curIdx < GetSize(); curIdx++) {
                    Destroy((*this)[curIdx]);
                }
            }
    };

    template <class T>
    std::ostream& operator<<(std::ostream& stream, Vector<T> vector) {
        for (uint64_t curIdx = 0; curIdx < vector.GetSize(); curIdx++) {
            stream << vector[curIdx] << " ";
        }

        return stream;
    }

    template <class T, uint64_t tSize>
    std::ostream& operator<<(std::ostream& stream, Array<T, tSize> array) {
        for (uint64_t curIdx = 0; curIdx < tSize; curIdx++) {
            stream << array[curIdx] << " ";
        }

        return stream;
    }
};


