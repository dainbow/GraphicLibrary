#include "Primitives.hpp"

void Buffer::Nextdo(Image* image) {
    if (savedHead_ == UINT32_MAX) {
        return;
    }

    if (savedHead_ == bufferHead_) {
        savedHead_ = UINT32_MAX;

        return;
    }

    for (auto curChange : buffer_[bufferHead_]) {
        std::cout << (curChange.first >> 32) << " " << uint32_t(curChange.first) << std::endl;
        image->SetPixel(uint32_t(curChange.first >> 32), uint32_t(curChange.first), curChange.second.second);
    }

    bufferHead_ = ((bufferHead_ + 1) % MaxReturnAmount);
}

void Buffer::Undo(Image* image) {
    if (bufferHead_ == ((bufferTail_ + 1) % MaxReturnAmount)) {
        return;
    }

    if (savedHead_ == UINT32_MAX) {
        savedHead_ = bufferHead_;
    }

    if (bufferHead_ == 0) {
        bufferHead_ = MaxReturnAmount - 1;
    }
    else {
        bufferHead_--;
    }

    for (auto& curChange : buffer_[bufferHead_]) {
        // std::cout << (curChange.first >> 32) << " " << uint32_t(curChange.first) << std::hex << curChange.second.second << std::endl;
        image->SetPixel(uint32_t(curChange.first >> 32), uint32_t(curChange.first), curChange.second.first);
    }
}
