#pragma once

#include <cmath>
#include <stack>
#include <cstdio>

#include <string>
#include <unordered_set>

#define ELPIDIFOR_STANDART_EXTENDED
#define ELPIDIFOR_HIDDEN_LAYER

#include "../tools.hpp"

const std::string soPath        = "./Icons/dain/";

const std::string PatchTexture = soPath + "Patch.png";

struct CordsPair {
    int32_t x;
    int32_t y;

    bool operator<(const CordsPair& p2) {
        return x < p2.x;
    }

    operator uint64_t() const {
        return (uint64_t(x) << 32ull) + uint64_t(y);
    }
};

class AbstractTool : public booba::Tool {
    protected:
        std::string toolImage_;
        uint32_t toolIdx_;

    public:
        AbstractTool() :
        toolImage_(),
        toolIdx_(0)
        {}

        virtual ~AbstractTool() {

        }

        AbstractTool(const AbstractTool& tool) = default;
        AbstractTool& operator=(const AbstractTool& tool) = default;

        uint32_t GetIdx() {
            return toolIdx_;
        }

        virtual const char* getTexture() override {
            return toolImage_.data();
        }

        virtual void buildSetupWidget() override {}
};

class PatchTool : public AbstractTool {
    protected:
        std::unordered_set<uint64_t> colored_;

        CordsPair anchorPoint_;
        CordsPair virtualPoint_;

        bool isClicked_;

        uint64_t sizeScrollIdx_;
        int32_t brushSize_;

        uint64_t transScrollIdx_;
        double transparency_;
    public:
        PatchTool() :
        AbstractTool(),
        colored_(),
        anchorPoint_({-1, -1}), virtualPoint_({-1, -1}),
        isClicked_(0),
        sizeScrollIdx_(0),
        brushSize_(1),
        transScrollIdx_(0), transparency_(1)
        {
            toolImage_ = PatchTexture;
        }

        static void DrawCircle(booba::Image* layer, const CordsPair& center, const int32_t radius, const uint32_t color1, const uint32_t color2);

        virtual ~PatchTool() {}

        virtual void apply(booba::Image* image, const booba::Event* event) override;
        virtual void buildSetupWidget() override;
}; 
