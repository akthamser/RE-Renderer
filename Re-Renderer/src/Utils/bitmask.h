#pragma once
#include<iostream>

class Bitmask {
public:
    Bitmask() : m_Flags(0) {}

    void set(uint32_t flag) {
        m_Flags |= flag;
    }

    void clear(uint32_t flag) {
        m_Flags &= ~flag;
    }

    bool isSet(uint32_t flag) const {
        return (m_Flags & flag) != 0;
    }

    void reset() {
        m_Flags = 0;
    }

private:
    uint32_t m_Flags;
};
