#include "igi/retail/dynobj/hash_lookup.hpp"

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

// 0x004CF300
void test_lookup_returns_the_node_at_the_index()
{
    igi::retail::dynobj::HashItem first;
    igi::retail::dynobj::HashItem second;
    igi::retail::dynobj::HashItem third;
    first.next = &second;
    second.next = &third;
    third.next = nullptr;

    igi::retail::dynobj::HashItem* root = &first;
    require(igi::retail::dynobj::DynObj_LookupHashItem_14(&root, 0) == &first, "index 0");
    require(igi::retail::dynobj::DynObj_LookupHashItem_14(&root, 1) == &second, "index 1");
    require(igi::retail::dynobj::DynObj_LookupHashItem_14(&root, 2) == nullptr,
            "the last node is not reachable: the loop ends on a null successor");
}

void test_single_element_list_returns_null()
{
    igi::retail::dynobj::HashItem only;
    igi::retail::dynobj::HashItem* root = &only;
    require(igi::retail::dynobj::DynObj_LookupHashItem_14(&root, 0) == nullptr,
            "retail quirk: a one-element list returns null");
}

} // namespace

int main()
{
    test_lookup_returns_the_node_at_the_index();
    test_single_element_list_returns_null();

    std::cout << "All retail DynObj hash-lookup tests passed.\n";
    return EXIT_SUCCESS;
}
