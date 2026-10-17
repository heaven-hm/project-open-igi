#include "igi/retail/mem/heap_list.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

// 0x004AF8F0: header offsets are +0x04 (head link) and +0x08 (tail pointer).
[[maybe_unused]] void test_insert_splices_at_the_tail()
{

    std::uint8_t header[0x10] = {};
    void** const head_link = reinterpret_cast<void**>(header + 0x04);
    void** const tail_slot = reinterpret_cast<void**>(header + 0x08);
    *head_link = nullptr;
    *tail_slot = head_link; // empty list: tail points at the head link

    void* node[2] = {nullptr, nullptr};
    void* const result = Camera_SetFieldOfView_33(header, node);

    require(result == header, "returns the header");
    require(node[1] == head_link, "the node's next is the previous tail");
    require(node[0] == header + 0x04, "the node's prev is the head anchor");
    require(*head_link == node, "the previous tail is updated to the node");
    require(*tail_slot == node, "the tail pointer now points at the node");
}

void test_unlink_removes_node()
{
    void* prev[2] = {nullptr, nullptr};
    void* node[2] = {nullptr, nullptr};
    void* next[2] = {nullptr, nullptr};

    prev[0] = node;
    node[1] = prev;
    node[0] = next;
    next[1] = node;

    void** const result = Camera_ApplyHandiCam_33(node);
    require(result == node, "Camera_ApplyHandiCam_33 returns node");
    require(prev[0] == next, "prev->link updated to next");
    require(next[1] == prev, "next->back updated to prev");
    require(node[0] == nullptr && node[1] == nullptr, "node links cleared");
}

} // namespace

int main()
{
    test_unlink_removes_node();
#if defined(_M_IX86) || defined(__i386__)
    test_insert_splices_at_the_tail();
    std::cout << "All retail heap-list tests passed.\n";
#else
    // The retail header stores links four bytes apart, which only holds with 32-bit pointers.
    // On a 64-bit host the transcription cannot be exercised without changing the offsets.
    std::cout << "Retail heap-list unlink passed; tail-insert skipped on 64-bit ABI.\n";
#endif
    return EXIT_SUCCESS;
}
