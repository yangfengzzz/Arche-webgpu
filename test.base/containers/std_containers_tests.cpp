//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "vox.base/containers/array.h"
#include "vox.base/containers/deque.h"
#include "vox.base/containers/list.h"
#include "vox.base/containers/map.h"
#include "vox.base/containers/queue.h"
#include "vox.base/containers/set.h"
#include "vox.base/containers/stack.h"
#include "vox.base/containers/string.h"
#include "vox.base/containers/unordered_map.h"
#include "vox.base/containers/unordered_set.h"
#include "vox.base/containers/vector.h"
#include "vox.base/span.h"

TEST(Allocator, Containers) {
    vox::StdAllocator<int> int_allocator;
    int_allocator.deallocate(int_allocator.allocate(46), 46);
    vox::StdAllocator<float> other_allocator(int_allocator);

    class Object {
    public:
        explicit Object(int& _counter) : counter_(_counter) { ++counter_; }
        ~Object() { --counter_; }

    private:
        int& counter_;
    };

    int counter = 0;
    vox::StdAllocator<Object> ObjectAllocator;
    Object* pointer = ObjectAllocator.allocate(1);
    int_allocator.construct(pointer, counter);
    EXPECT_EQ(counter, 1);

    int_allocator.destroy(pointer);
    EXPECT_EQ(counter, 0);

    ObjectAllocator.deallocate(pointer, 1);

    EXPECT_TRUE(int_allocator == other_allocator);
    EXPECT_FALSE(int_allocator != other_allocator);
}

TEST(Vector, Containers) {
    typedef vox::vector<int> Container;
    Container container;
    container.push_back(1);
    container.insert(container.begin(), 0);
    container.push_back(2);
    container.push_back(3);
    EXPECT_EQ(container[0], 0);
    EXPECT_EQ(container[1], 1);
    EXPECT_EQ(container[2], 2);
    EXPECT_EQ(container[3], 3);

    Container container2 = std::move(container);
}

TEST(Array, Containers) {
    typedef vox::array<int, 4> Container;
    Container container{{0, 1, 2, 3}};
    EXPECT_EQ(container[0], 0);
    EXPECT_EQ(container[1], 1);
    EXPECT_EQ(container[2], 2);
    EXPECT_EQ(container[3], 3);

    Container container2 = container;
    (void)container2;
}

TEST(VectorExtensions, Containers) {
    typedef vox::vector<int> Container;
    Container container;
    int* null = nullptr;

    // Non-mutable access.
    EXPECT_EQ(array_begin(container), null);
    EXPECT_EQ(array_end(container), null);
    EXPECT_EQ(array_end(container), array_begin(container));
    EXPECT_TRUE(make_span(container).empty());

    container.push_back(1);
    container.push_back(2);
    EXPECT_EQ(*array_begin(container), 1);
    EXPECT_EQ(*(array_begin(container) + 1), 2);
    EXPECT_EQ(array_begin(container) + 2, array_end(container));
    EXPECT_NE(array_end(container), null);
    EXPECT_EQ(*(array_end(container) - 2), 1);
    EXPECT_EQ(array_end(container) - array_begin(container), 2);
    EXPECT_EQ(make_span(container).begin(), array_begin(container));
    EXPECT_EQ(make_span(container).end(), array_end(container));

    const Container const_container(container);
    EXPECT_EQ(*array_begin(const_container), 1);
    EXPECT_EQ(*(array_begin(const_container) + 1), 2);
    EXPECT_EQ(array_begin(const_container) + 2, array_end(const_container));
    EXPECT_NE(array_end(const_container), null);
    EXPECT_EQ(*(array_end(const_container) - 2), 1);
    EXPECT_EQ(array_end(const_container) - array_begin(const_container), 2);
    EXPECT_EQ(make_span(const_container).begin(), array_begin(const_container));
    EXPECT_EQ(make_span(const_container).end(), array_end(const_container));

    // Mutable access.
    *array_begin(container) = 0;
    EXPECT_EQ(*array_begin(container), 0);
    EXPECT_EQ(*(array_begin(container) + 1), 2);
}

TEST(Deque, Containers) {
    typedef vox::deque<int> Container;
    Container container;
    container.push_back(1);
    container.push_front(0);
    container.push_back(2);
    EXPECT_EQ(container[0], 0);
    EXPECT_EQ(container[1], 1);
    EXPECT_EQ(container[2], 2);
    container.clear();

    Container container2 = std::move(container);
}

TEST(List, Containers) {
    typedef vox::list<int> Container;
    Container container;
    container.push_back(1);
    container.push_front(0);
    container.push_back(2);
    EXPECT_EQ(container.front(), 0);
    EXPECT_EQ(container.back(), 2);
    container.clear();

    Container container2 = std::move(container);
}

TEST(Stack, Containers) {
    typedef vox::stack<int> Container;
    Container container;
    container.push(1);
    container.push(2);
    EXPECT_EQ(container.top(), 2);
    container.pop();
    EXPECT_EQ(container.top(), 1);
    container.pop();

    Container container2 = std::move(container);
}

TEST(Queue, Containers) {
    {
        typedef vox::queue<int> Container;
        Container container;
        container.push(1);
        container.push(2);
        EXPECT_EQ(container.back(), 2);
        EXPECT_EQ(container.front(), 1);
        container.pop();
        EXPECT_EQ(container.back(), 2);
        container.pop();

        Container container2 = std::move(container);
    }
    {
        typedef vox::priority_queue<int> Container;
        Container container;
        container.push(1);
        container.push(2);
        container.push(0);
        EXPECT_EQ(container.top(), 2);
        container.pop();
        EXPECT_EQ(container.top(), 1);
        container.pop();
        EXPECT_EQ(container.top(), 0);
        container.pop();
    }
}

TEST(Set, Containers) {
    {
        typedef vox::set<int> Container;
        Container container;
        EXPECT_TRUE(container.insert('c').second);
        EXPECT_TRUE(container.insert('a').second);
        EXPECT_TRUE(container.insert('b').second);
        EXPECT_FALSE(container.insert('b').second);
        EXPECT_TRUE(container.find('a') == container.begin());
        EXPECT_TRUE(container.find('c') == --container.end());
        EXPECT_EQ(container.erase('c'), 1u);

        EXPECT_TRUE(container.find('b') == --container.end());
        container.clear();

        Container container2 = std::move(container);
    }
    {
        typedef vox::multiset<int> Container;
        Container container;
        container.insert('c');
        container.insert('a');
        container.insert('b');
        container.insert('a');
        EXPECT_TRUE(container.find('a') == container.begin() || container.find('a') == ++container.begin());
        EXPECT_TRUE(container.find('c') == --container.end());
        EXPECT_EQ(container.erase('c'), 1u);
        EXPECT_TRUE(container.find('c') == container.end());
        EXPECT_EQ(container.erase('a'), 2u);
        EXPECT_TRUE(container.find('a') == container.end());
        container.clear();
    }
}

TEST(UnorderedSet, Containers) {
    {
        typedef vox::unordered_set<int> Container;
        Container container;
        EXPECT_TRUE(container.insert('c').second);
        EXPECT_TRUE(container.insert('a').second);
        EXPECT_TRUE(container.insert('b').second);
        EXPECT_FALSE(container.insert('a').second);
        EXPECT_TRUE(container.find('a') != container.end());
        EXPECT_TRUE(container.find('c') != container.end());
        EXPECT_EQ(container.erase('c'), 1u);
        EXPECT_TRUE(container.find('c') == container.end());
        container.clear();
    }
    {
        typedef vox::unordered_multiset<int> Container;
        Container container;
        container.insert('c');
        container.insert('a');
        container.insert('b');
        container.insert('a');
        EXPECT_TRUE(container.find('a') != container.end());
        EXPECT_TRUE(container.find('c') != container.end());
        EXPECT_EQ(container.erase('c'), 1u);
        EXPECT_TRUE(container.find('c') == container.end());
        EXPECT_EQ(container.erase('a'), 2u);
        EXPECT_TRUE(container.find('a') == container.end());
        container.clear();
    }
}

TEST(Map, Containers) {
    {
        typedef vox::map<char, int> Container;
        Container container;
        container['a'] = -3;
        container['c'] = -1;
        container['b'] = -2;
        container['d'] = 1;
        EXPECT_EQ(container['a'], -3);
        EXPECT_EQ(container['b'], -2);
        EXPECT_EQ(container['c'], -1);
        EXPECT_EQ(container['d'], 1);
        EXPECT_EQ(container.erase('d'), 1u);
        EXPECT_TRUE(container.find('d') == container.end());
        container.clear();

        Container container2 = std::move(container);
    }
    {
        typedef vox::multimap<char, int> Container;
        Container container;
        container.insert(std::pair<char, int>('a', -3));
        container.insert(std::pair<char, int>('c', -1));
        container.insert(std::pair<char, int>('b', -2));
        container.insert(std::pair<char, int>('d', 1));
        container.insert(std::pair<char, int>('d', 2));
        EXPECT_EQ(container.find('a')->second, -3);
        EXPECT_EQ(container.find('b')->second, -2);
        EXPECT_EQ(container.find('c')->second, -1);
        EXPECT_TRUE(container.find('d')->second == 1 || container.find('d')->second == 2);
        EXPECT_EQ(container.erase('d'), 2u);
        EXPECT_TRUE(container.find('d') == container.end());
        container.clear();
    }
}

TEST(UnorderedMap, Containers) {
    {
        typedef vox::unordered_map<char, int> Container;
        Container container;
        container['a'] = -3;
        container['c'] = -1;
        container['b'] = -2;
        container['d'] = 1;
        EXPECT_EQ(container['a'], -3);
        EXPECT_EQ(container['b'], -2);
        EXPECT_EQ(container['c'], -1);
        EXPECT_EQ(container['d'], 1);
        EXPECT_EQ(container.erase('d'), 1u);
        EXPECT_TRUE(container.find('d') == container.end());
        container.clear();
    }
    {
        typedef vox::unordered_multimap<char, int> Container;
        Container container;
        container.insert(std::pair<char, int>('a', -3));
        container.insert(std::pair<char, int>('c', -1));
        container.insert(std::pair<char, int>('b', -2));
        container.insert(std::pair<char, int>('d', 1));
        container.insert(std::pair<char, int>('d', 2));
        EXPECT_EQ(container.find('a')->second, -3);
        EXPECT_EQ(container.find('b')->second, -2);
        EXPECT_EQ(container.find('c')->second, -1);
        EXPECT_TRUE(container.find('d')->second == 1 || container.find('d')->second == 2);
        EXPECT_EQ(container.erase('d'), 2u);
        EXPECT_TRUE(container.find('d') == container.end());
        container.clear();
    }
}

TEST(string, Containers) {
    typedef vox::string string;
    string str;
    EXPECT_EQ(str.size(), 0u);
    str += "a string";
    EXPECT_STREQ(str.c_str(), "a string");

    string str2 = std::move(str);
    EXPECT_STREQ(str2.c_str(), "a string");

    str2.clear();
    EXPECT_EQ(str2.size(), 0u);
}
