//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/containers/intrusive_list.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127)  // constant conditional expression
#endif                           // _MSC_VER

#include <list>

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#include <algorithm>
#include <functional>

#include "gtest/gtest.h"
#include "test.base/gtest_helper.h"

// using-declaration of IntrusiveList type and its options
using vox::containers::IntrusiveList;
using vox::containers::Option;

// Test whether assertion compliance tests can be ran on the container specified
// as template argument.
// The default implementation allows to run all tests.
template <typename Ty>
struct TestAssertCompliance {
    enum { kValue = 1 };
};

// Check compiler settings for std/crt debug level availability.
#if defined(_MSC_VER)
#define HAS_STD_ASSERTION 1  // Win32 std lib has debug option.
#elif defined(__GNUC__) && defined(_GLIBCXX_DEBUG)
#define HAS_STD_ASSERTION 1  // _GLIBCXX_DEBUG enables GCC Libc debug option.
#else
#define HAS_STD_ASSERTION 0  // Disabled by default.
#endif

// Specializes for std::list,  according to compilation settings.
template <typename Ty>
struct TestAssertCompliance<std::list<Ty>> {
    enum { kValue = HAS_STD_ASSERTION };
};

// Defines a test object that inherits from IntrusiveList::Hook in order to be
// listed by an IntrusiveList.
// Every instance is assigned a value (obtained for a global instance counter)
// used for instance sorting and comparison.
template <typename Options0 = Option<>>
class TestObj1 : public IntrusiveList<TestObj1<Options0>, Options0>::Hook {
public:
    // Constructs a TestObj1 and increments global TestObj1 counter.
    TestObj1() : instance_(s_instance_counter_++) {}

    // Does not copy the Node itself, just maintains the assigned instance number.
    TestObj1(TestObj1 const& _r)  // NOLINT cannot be explicit as used by std::list
        : IntrusiveList<TestObj1<Options0>, Options0>::Hook(), instance_(_r.instance_) {}

    // Implements comparison operators.
    bool operator==(TestObj1 const& _r) const { return instance_ == _r.instance_; }
    bool operator<(TestObj1 const& _r) const { return instance_ < _r.instance_; }
    bool operator>(TestObj1 const& _r) const { return instance_ > _r.instance_; }

private:
    // Disallows assignment operator.
    void operator=(TestObj1 const& _r);

    // Assigned instance value.
    const int instance_;

    // TestObj1 instance counter.
    static int s_instance_counter_;
};

template <typename Options0>
int TestObj1<Options0>::s_instance_counter_ = 0;

// Defines a test object that inherits from TestObj1 in order to be listed by
// two IntrusiveList at a time.
template <typename Options1, typename Options2>
class TestObj2 : public TestObj1<Options1>, public IntrusiveList<TestObj2<Options1, Options2>, Options2>::Hook {
public:
    // Constructs a default TestObj2.
    TestObj2() = default;

    // Does not copy the Node itself, just maintains the assigned instance number.
    TestObj2(TestObj2 const& _r)
        : TestObj1<Options1>(_r), IntrusiveList<TestObj2<Options1, Options2>, Options2>::Hook() {}
};

// Applies the _Test function to some std::list<> and Intrusive<> types.
template <template <typename> class Test>
void BindTypes() {
    // std::list
    Test<std::list<TestObj1<>>>()();

    // kAuto link mode
    typedef Option<vox::containers::LinkMode::kAuto, 0> OptionsAuto0;
    typedef TestObj1<OptionsAuto0> AutoTestObj0;
    Test<IntrusiveList<AutoTestObj0, OptionsAuto0>>()();

    // kSafe link mode
    typedef Option<vox::containers::LinkMode::kSafe, 0> OptionsSafe0;
    typedef TestObj1<OptionsSafe0> SafeTestObj0;
    Test<IntrusiveList<SafeTestObj0, OptionsSafe0>>()();

    // kUnsafe link mode
    typedef Option<vox::containers::LinkMode::kUnsafe, 0> OptionsUnsafe0;
    typedef TestObj1<OptionsUnsafe0> UnsafeTestObj0;
    Test<IntrusiveList<UnsafeTestObj0, OptionsUnsafe0>>()();

    // Auto link mode and safe link mode of a single object in two different
    // lists.
    typedef Option<vox::containers::LinkMode::kSafe, 1> OptionsSafe1;
    typedef TestObj2<OptionsAuto0, OptionsSafe1> LocalTestObj01;
    Test<IntrusiveList<LocalTestObj01, OptionsSafe1>>()();
}

// Tests compliance with "front" push/pop function specifications.
template <typename List>
struct CompliancePushPopFront {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;

        List l;
        const List& r_const_l = l;

        if (void(0), TestAssertCompliance<List>::kValue) {
            EXPECT_ASSERTION(l.front(), "");
            EXPECT_ASSERTION(r_const_l.front(), "");
            EXPECT_ASSERTION(l.back(), "");
            EXPECT_ASSERTION(r_const_l.back(), "");
        }
        l.push_front(first);
        EXPECT_TRUE(l.size() == 1 && l.front() == first && l.back() == first);
        EXPECT_TRUE(r_const_l.size() == 1 && r_const_l.front() == first && r_const_l.back() == first);
        l.push_front(second);
        EXPECT_TRUE(l.size() == 2 && l.front() == second && l.back() == first);
        l.pop_front();
        EXPECT_TRUE(l.size() == 1 && l.front() == first && l.back() == first);

        l.clear();
    }
};

TEST(CompliancePushPopFront, IntrusiveList) { BindTypes<CompliancePushPopFront>(); }

// Tests compliance with "back" push/pop function specifications.
template <typename List>
struct CompliancePushPopBack {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;

        List l;
        const List& r_const_l = l;
        l.push_back(first);
        EXPECT_TRUE(l.size() == 1 && l.front() == first && l.back() == first);
        EXPECT_TRUE(r_const_l.size() == 1 && r_const_l.front() == first && r_const_l.back() == first);
        l.push_back(second);
        EXPECT_TRUE(l.size() == 2 && l.front() == first && l.back() == second);
        l.pop_back();
        EXPECT_TRUE(l.size() == 1 && l.front() == first && l.back() == first);

        l.clear();
    }
};

TEST(CompliancePushPopBack, IntrusiveList) { BindTypes<CompliancePushPopBack>(); }

// Tests compliance of mixed "front" and "back" push/pop.
template <typename List>
struct CompliancePushPopMixed {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;

        List l;
        l.push_back(first);
        EXPECT_TRUE(l.size() == 1 && l.front() == first && l.back() == first);
        l.push_front(second);
        EXPECT_TRUE(l.size() == 2 && l.front() == second && l.back() == first);
        l.push_front(third);
        EXPECT_TRUE(l.size() == 3 && l.front() == third && l.back() == first);
        l.pop_back();
        EXPECT_TRUE(l.size() == 2 && l.front() == third && l.back() == second);
        l.pop_front();
        EXPECT_TRUE(l.size() == 1 && l.front() == second && l.back() == second);

        l.clear();
    }
};

TEST(CompliancePushPopMixed, IntrusiveList) { BindTypes<CompliancePushPopMixed>(); }

// Tests compliance with std::list begin iterator specifications.
template <typename List>
struct ComplianceBegin {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;

        List l;
        const List& r_const_l = l;
        EXPECT_TRUE(l.begin() == l.end());

        l.push_back(first);
        l.push_back(second);
        l.push_back(third);

        {
            auto it = l.begin();
            EXPECT_TRUE(*it == first);
        }

        {
            auto const_iter = r_const_l.begin();
            EXPECT_TRUE(*const_iter == first);
        }

        {
            typename List::const_iterator const_iter(l.begin());
            EXPECT_TRUE(*const_iter == first);
        }

        {
            auto it = l.begin();
            EXPECT_TRUE(*it == first);
            ++it;
            EXPECT_TRUE(*it == second);
            ++it;
            EXPECT_TRUE(*it == third);
            ++it;
            EXPECT_TRUE(it == l.end());
        }

        {
            auto it = r_const_l.begin();
            EXPECT_TRUE(*it == first);
            ++it;
            EXPECT_TRUE(*it == second);
            ++it;
            EXPECT_TRUE(*it == third);
            ++it;
            EXPECT_TRUE(it == r_const_l.end());
        }

        l.clear();
    }
};

TEST(ComplianceBegin, IntrusiveList) { BindTypes<ComplianceBegin>(); }

// Tests compliance of std::list end iterator specifications.
template <typename List>
struct ComplianceEnd {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;

        List l;
        const List& r_const_l = l;
        EXPECT_TRUE(l.begin() == l.end());
        EXPECT_TRUE(r_const_l.begin() == r_const_l.end());

        l.push_back(first);
        l.push_back(second);
        l.push_back(third);

        EXPECT_TRUE(l.begin() != l.end());
        EXPECT_TRUE(r_const_l.begin() != r_const_l.end());

        {
            auto it = l.end();
            EXPECT_TRUE(*--it == third);
        }

        {
            auto const_iter = r_const_l.end();
            EXPECT_TRUE(*--const_iter == third);
        }

        {
            auto const_iter = l.end();
            EXPECT_TRUE(*--const_iter == third);
        }

        {
            auto it = l.end();
            it--;
            EXPECT_TRUE(*it == third);
            it--;
            EXPECT_TRUE(*it == second);
            it--;
            EXPECT_TRUE(*it == first && it == l.begin());
        }

        {
            auto it = r_const_l.end();
            it--;
            EXPECT_TRUE(*it == third);
            it--;
            EXPECT_TRUE(*it == second);
            it--;
            EXPECT_TRUE(*it == first && it == r_const_l.begin());
        }

        l.clear();
    }
};

TEST(ComplianceEnd, IntrusiveList) { BindTypes<ComplianceEnd>(); }

// Tests compliance with std::list typedefs.
template <typename List>
struct ComplianceTypedef {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;

        List l;
        const List& r_const_l = l;
        l.push_front(first);
        l.push_back(second);

        auto const_iter = r_const_l.begin();
        EXPECT_TRUE(*const_iter == first);
        auto it = l.begin();
        EXPECT_TRUE(*it == first);

        auto const_rev_iter = r_const_l.rbegin();
        EXPECT_TRUE(*const_rev_iter == second);
        auto rev_iter = l.rbegin();
        EXPECT_TRUE(*rev_iter == second);

        typename List::const_pointer const_p = &r_const_l.front();
        EXPECT_TRUE(*const_p == first);
        typename List::pointer p = &l.front();
        EXPECT_TRUE(*p == first);
        typename List::const_reference const_r = r_const_l.front();
        EXPECT_TRUE(const_r == first);
        typename List::reference r = l.front();
        EXPECT_TRUE(r == first);

        typename List::difference_type diff = std::count(l.begin(), l.end(), first);
        EXPECT_EQ(diff, 1);

        typename List::size_type size = l.size();
        EXPECT_EQ(size, 2u);

        l.clear();
    }
};

TEST(ComplianceTypedef, IntrusiveList) { BindTypes<ComplianceTypedef>(); }

// Tests compliance of std::list iterator specifications.
template <typename List>
struct ComplianceIterator {
    void operator()() {
        typename List::value_type first;

        List l1, l2;
        l1.push_front(first);

        const List& rcosnt_l1 = l1;

        // Test operators
        EXPECT_TRUE(l1.begin() != l1.end());
        EXPECT_TRUE(l1.begin()++ == l1.begin());
        EXPECT_TRUE(++l1.begin() == l1.end());
        EXPECT_TRUE(rcosnt_l1.begin() != rcosnt_l1.end());
        EXPECT_TRUE(rcosnt_l1.begin()++ == rcosnt_l1.begin());
        EXPECT_TRUE(++rcosnt_l1.begin() == rcosnt_l1.end());
        EXPECT_TRUE(l1.begin() != rcosnt_l1.end());
        EXPECT_TRUE(++l1.begin() == l1.end());
        EXPECT_TRUE(l1.begin() != l1.end());
        EXPECT_TRUE(++l1.begin() == l1.end());

        // Dereference iterator
        {
            typename List::reference r11 = *l1.begin();
            EXPECT_TRUE(r11 == first);
            typename List::const_reference r12 = *rcosnt_l1.begin();
            EXPECT_TRUE(r12 == first);
        }

        // Test copy
        {
            auto it = l1.begin();
            auto assign_it = it;
            EXPECT_TRUE(assign_it == it);
            auto copy_it = it;
            EXPECT_TRUE(copy_it == it);
        }

        if (void(0), TestAssertCompliance<List>::kValue) {
            // Test comparing iterators of different lists
            EXPECT_ASSERTION(void(typename List::iterator() == l1.begin()), "");
            EXPECT_ASSERTION(void(typename List::const_iterator() == l1.begin()), "");
            EXPECT_ASSERTION(void(l1.begin() != static_cast<const List&>(l2).begin()), "");
            EXPECT_ASSERTION(void(l1.end() != static_cast<const List&>(l2).end()), "");
            EXPECT_ASSERTION(void(rcosnt_l1.begin() != l2.begin()), "");
            EXPECT_ASSERTION(void(rcosnt_l1.end() != l2.end()), "");

            // Test iterators bound cases
            EXPECT_ASSERTION(--l1.begin(), "");
            EXPECT_ASSERTION(l1.begin()--, "");
            EXPECT_ASSERTION(--rcosnt_l1.begin(), "");
            EXPECT_ASSERTION(rcosnt_l1.begin()--, "");

            EXPECT_ASSERTION(++rcosnt_l1.end(), "");
            EXPECT_ASSERTION(rcosnt_l1.end()++, "");
            EXPECT_ASSERTION(++rcosnt_l1.end(), "");
            EXPECT_ASSERTION(rcosnt_l1.end()++, "");

            // Dereferencing an invalid iterator
            EXPECT_ASSERTION(*typename List::iterator(), "");
            EXPECT_ASSERTION(*typename List::const_iterator(), "");
            EXPECT_ASSERTION(*typename List::reverse_iterator(), "");
            EXPECT_ASSERTION(*typename List::const_reverse_iterator(), "");
            EXPECT_ASSERTION(*l1.end(), "");
            EXPECT_ASSERTION(*rcosnt_l1.end(), "");
            EXPECT_ASSERTION(*l1.rend(), "");
            EXPECT_ASSERTION(*rcosnt_l1.rend(), "");
        }
        // Test iterator std functions
        {
            auto it = l1.begin();
            std::advance(it, 1);
            EXPECT_TRUE(it == l1.end());
            std::advance(it, -1);
            EXPECT_TRUE(it == l1.begin());
            if (void(0), TestAssertCompliance<List>::kValue) {
                EXPECT_ASSERTION(std::advance(it, 2), "");
            }
        }
        { EXPECT_EQ(std::distance(l1.begin(), l1.end()), 1); }

        l1.clear();
        l2.clear();
    }
};

TEST(ComplianceIterator, IntrusiveList) { BindTypes<ComplianceIterator>(); }

// Tests compliance with "rbegin" function specifications.
template <typename List>
struct ComplianceRBegin {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;

        List l;
        const List& rcosnt_l = l;
        EXPECT_TRUE(l.rbegin() == l.rend());

        l.push_back(first);
        l.push_back(second);
        l.push_back(third);
        EXPECT_TRUE(l.rbegin() != l.rend());

        // rbegin should be at the back of the list
        {
            auto rev_iter = l.rbegin();
            EXPECT_TRUE(*rev_iter == third);
        }

        // Iterate in reverse order
        {
            auto const_rev_iter = l.rbegin();
            EXPECT_TRUE(*const_rev_iter == third);
            ++const_rev_iter;
            EXPECT_TRUE(*const_rev_iter == second);
            ++const_rev_iter;
            EXPECT_TRUE(*const_rev_iter == first);
            ++const_rev_iter;
            EXPECT_TRUE(const_rev_iter == rcosnt_l.rend());
            if (void(0), TestAssertCompliance<List>::kValue) {
                EXPECT_ASSERTION(++const_rev_iter, "");  // Cannot increment beyond rend
            }
        }
        l.clear();
    }
};

TEST(ComplianceRBegin, IntrusiveList) { BindTypes<ComplianceRBegin>(); }

// Tests compliance with "rend" function specifications.
template <typename List>
struct ComplianceREnd {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;

        List l;
        const List& rcosnt_l = l;
        EXPECT_TRUE(l.rbegin() == l.rend());

        l.push_back(first);
        l.push_back(second);
        l.push_back(third);
        EXPECT_TRUE(l.rbegin() != l.rend());

        // rend should be at the front of the list
        {
            auto rev_iter = l.rend();
            if (void(0), TestAssertCompliance<List>::kValue) {
                EXPECT_ASSERTION(*rev_iter, "");
            }
            --rev_iter;
            EXPECT_TRUE(*rev_iter == first);
        }

        // Iterate in reverse order
        {
            auto const_rev_iter = l.rend();
            --const_rev_iter;
            EXPECT_TRUE(*const_rev_iter == first);
            --const_rev_iter;
            EXPECT_TRUE(*const_rev_iter == second);
            --const_rev_iter;
            EXPECT_TRUE(*const_rev_iter == third);
            EXPECT_TRUE(const_rev_iter == rcosnt_l.rbegin());
            // Cannot increment below rbegin
            if (void(0), TestAssertCompliance<List>::kValue) {
                EXPECT_ASSERTION(--const_rev_iter, "");
            }
        }
        l.clear();
    }
};

TEST(ComplianceREnd, IntrusiveList) { BindTypes<ComplianceREnd>(); }

// Tests compliance with clear/empty specifications.
template <typename List>
struct ComplianceClearEmpty {
    void operator()() {
        typename List::value_type first;

        List l;
        EXPECT_TRUE(l.begin() == l.end());

        l.push_back(first);

        EXPECT_TRUE(l.begin() != l.end());
        EXPECT_TRUE(l.size() == 1 && !l.empty());

        l.clear();

        EXPECT_TRUE(l.size() == 0 && l.empty());
        EXPECT_TRUE(l.begin() == l.end());
    }
};

TEST(ComplianceClearEmpty, IntrusiveList) { BindTypes<ComplianceClearEmpty>(); }

// Tests compliance with "remove" function specifications.
template <typename List>
struct ComplianceRemove {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;
        typename List::value_type fourth;

        List l;
        l.push_back(first);
        l.push_back(second);
        l.push_back(third);
        l.push_back(fourth);

        l.remove(first);
        EXPECT_EQ(l.size(), 3u);
        EXPECT_TRUE(l.front() == second && l.back() == fourth);

        l.remove(third);
        EXPECT_EQ(l.size(), 2u);
        EXPECT_TRUE(l.front() == second && l.back() == fourth);

        l.clear();
    }
};

TEST(ComplianceRemove, IntrusiveList) { BindTypes<ComplianceRemove>(); }

// Implements a functor used to test "remove_if" function.
template <typename List>
class is_to_be_removed {
public:
    explicit is_to_be_removed(int _which) : which_(_which) {}
    bool operator()(typename List::const_reference) { return which_-- == 0; }

private:
    void operator=(const is_to_be_removed&);

    int which_;
};

// Tests compliance with "remove_if" function specifications.
template <typename List>
struct ComplianceRemoveIf {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;

        List l;
        l.push_back(first);
        l.push_back(second);
        l.push_back(third);

        l.remove_if(is_to_be_removed<List>(1));
        EXPECT_EQ(l.size(), 2u);
        EXPECT_TRUE(l.front() == first && l.back() == third);

        l.clear();
    }
};

TEST(ComplianceRemoveIf, IntrusiveList) { BindTypes<ComplianceRemoveIf>(); }

// Tests compliance with "erase" function specifications.
template <typename List>
struct ComplianceErase {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;
        typename List::value_type fourth;
        typename List::value_type fifth;
        typename List::value_type sixth;

        List l;
        l.push_back(first);
        l.push_back(second);
        l.push_back(third);
        l.push_back(fourth);
        l.push_back(fifth);
        l.push_back(sixth);

        // Bad range
        if (void(0), TestAssertCompliance<List>::kValue) {
            List l2;
            EXPECT_ASSERTION(l.erase(l2.begin(), l.begin()), "");
            EXPECT_ASSERTION(l.erase(++l.begin(), l.begin()), "");
        }

        // Erases first element
        {
            auto ret_it = l.erase(l.begin());
            EXPECT_TRUE(ret_it == l.begin() && l.front() == second);
        }

        // Erases all elements but the first
        {
            auto ret_it = l.erase(++l.begin(), l.end());
            EXPECT_TRUE(ret_it == l.end() && l.front() == second);
        }

        l.clear();
    }
};

TEST(ComplianceErase, IntrusiveList) { BindTypes<ComplianceErase>(); }

// Tests compliance with "insert" function specifications.
template <typename List>
struct ComplianceInsert {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;
        typename List::value_type fourth;
        typename List::value_type fifth;
        typename List::value_type sixth;

        List l;
        l.push_back(first);
        l.push_back(third);

        {
            l.insert(++l.begin(), second);
            EXPECT_TRUE(*++l.begin() == second);
        }

        {
            l.insert(l.end(), fourth);
            EXPECT_TRUE(*--l.end() == fourth);
        }

        l.clear();
    }
};

TEST(ComplianceInsert, IntrusiveList) { BindTypes<ComplianceInsert>(); }

// Tests compliance with "reverse" function specifications.
template <typename List>
struct ComplianceReverse {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;

        List l;
        l.reverse();  // Reverse empty list
        EXPECT_TRUE(l.empty());

        l.push_back(first);
        l.reverse();  // Reverse 1 element list
        EXPECT_EQ(l.size(), 1u);
        {
            auto const_iter = l.begin();
            EXPECT_TRUE(*const_iter == first);
        }

        l.push_back(second);
        l.reverse();  // Reverse list of 2 elements
        EXPECT_EQ(l.size(), 2u);
        {
            auto const_iter = l.begin();
            EXPECT_TRUE(*const_iter == second);
            ++const_iter;
            EXPECT_TRUE(*const_iter == first);
        }
        l.reverse();  // back to original order

        l.push_back(third);

        l.reverse();  // Reverse list of 3 elements
        EXPECT_EQ(l.size(), 3u);
        {
            auto const_iter = l.begin();
            EXPECT_TRUE(*const_iter == third);
            ++const_iter;
            EXPECT_TRUE(*const_iter == second);
            ++const_iter;
            EXPECT_TRUE(*const_iter == first);
            ++const_iter;
            EXPECT_TRUE(const_iter == l.end());
        }

        l.clear();
    }
};

TEST(ComplianceReverse, IntrusiveList) { BindTypes<ComplianceReverse>(); }

// Tests compliance with "splice" function specifications.
template <typename List>
struct ComplianceSplice {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;
        typename List::value_type fourth;
        typename List::value_type fifth;
        typename List::value_type sixth;
        typename List::value_type seventh;
        typename List::value_type eighth;
        typename List::value_type ninth;
        typename List::value_type tenth;
        typename List::value_type eleventh;
        typename List::value_type twelfth;
        typename List::value_type thirteenth;
        List l1;
        l1.push_back(first);
        l1.push_back(second);
        List l2;
        l2.push_back(third);
        l2.push_back(fourth);
        l2.push_back(fifth);
        List l3;
        l3.push_back(sixth);
        l3.push_back(seventh);
        List l4;
        l4.push_back(eighth);
        l4.push_back(ninth);
        l4.push_back(tenth);
        List l5;
        l5.push_back(eleventh);
        l5.push_back(twelfth);
        l5.push_back(thirteenth);
        List l_empty;

        // Bad range
        if (void(0), TestAssertCompliance<List>::kValue) {
            EXPECT_ASSERTION(l4.splice(l4.begin(), l5, l4.begin()), "");
            EXPECT_ASSERTION(l4.splice(l4.begin(), l5, l4.begin(), l5.end()), "");
            EXPECT_ASSERTION(l4.splice(l4.begin(), l5, l5.end(), --l5.end()), "");
        }

        // Splices l1 just after the first element of l2
        l2.splice(++l2.begin(), l1);
        assert(l1.empty() && l2.size() == 5);
        {
            auto const_iter = l2.begin();
            EXPECT_TRUE(*const_iter == third);
            ++const_iter;
            EXPECT_TRUE(*const_iter == first);
            ++const_iter;
            EXPECT_TRUE(*const_iter == second);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fourth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fifth);
            ++const_iter;
            EXPECT_TRUE(const_iter == l2.end());
        }

        // Splices l2 with an empty list
        l2.splice(l2.begin(), l_empty);
        EXPECT_EQ(l2.size(), 5u);
        {
            auto const_iter = l2.begin();
            EXPECT_TRUE(*const_iter == third);
            ++const_iter;
            EXPECT_TRUE(*const_iter == first);
            ++const_iter;
            EXPECT_TRUE(*const_iter == second);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fourth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fifth);
            ++const_iter;
            EXPECT_TRUE(const_iter == l2.end());
        }

        // Splices l3 (starting at the second element) just after the first
        // element of l2
        l2.splice(++l2.begin(), l3, ++l3.begin(), l3.end());
        EXPECT_TRUE(l3.size() == 1 && l2.size() == 6);
        {
            auto const_iter = l3.begin();
            EXPECT_TRUE(*const_iter == sixth);
        }
        {
            auto const_iter = l2.begin();
            EXPECT_TRUE(*const_iter == third);
            ++const_iter;
            EXPECT_TRUE(*const_iter == seventh);
            ++const_iter;
            EXPECT_TRUE(*const_iter == first);
            ++const_iter;
            EXPECT_TRUE(*const_iter == second);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fourth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fifth);
            ++const_iter;
            EXPECT_TRUE(const_iter == l2.end());
        }

        // Splices all l4 except the last element at the beginning of l2
        l2.splice(l2.begin(), l4, l4.begin(), --l4.end());
        EXPECT_TRUE(l4.size() == 1 && l2.size() == 8);
        {
            auto const_iter = l4.begin();
            EXPECT_TRUE(*const_iter == tenth);
        }
        {
            auto const_iter = l2.begin();
            EXPECT_TRUE(*const_iter == eighth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == ninth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == third);
            ++const_iter;
            EXPECT_TRUE(*const_iter == seventh);
            ++const_iter;
            EXPECT_TRUE(*const_iter == first);
            ++const_iter;
            EXPECT_TRUE(*const_iter == second);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fourth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fifth);
            ++const_iter;
            EXPECT_TRUE(const_iter == l2.end());
        }

        // Splices no element from l5
        l4.splice(l4.begin(), l5, ++l5.begin(), ++l5.begin());
        EXPECT_TRUE(l4.size() == 1 && l5.size() == 3);
        {
            auto const_iter = l4.begin();
            EXPECT_TRUE(*const_iter == tenth);
            ++const_iter;
            EXPECT_TRUE(const_iter == l4.end());
        }
        {
            auto const_iter = l5.begin();
            EXPECT_TRUE(*const_iter == eleventh);
            ++const_iter;
            EXPECT_TRUE(*const_iter == twelfth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == thirteenth);
            ++const_iter;
            EXPECT_TRUE(const_iter == l5.end());
        }

        // Self splicing of a single element
        l5.splice(l5.begin(), l5, ++l5.begin());
        EXPECT_EQ(l5.size(), 3u);
        {
            auto const_iter = l5.begin();
            EXPECT_TRUE(*const_iter == twelfth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == eleventh);
            ++const_iter;
            EXPECT_TRUE(*const_iter == thirteenth);
            ++const_iter;
            EXPECT_TRUE(const_iter == l5.end());
        }

        // Self splicing of multiple elements
        l5.splice(l5.begin(), l5, ++l5.begin(), l5.end());
        EXPECT_EQ(l5.size(), 3u);
        {
            auto const_iter = l5.begin();
            EXPECT_TRUE(*const_iter == eleventh);
            ++const_iter;
            EXPECT_TRUE(*const_iter == thirteenth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == twelfth);
            ++const_iter;
            EXPECT_TRUE(const_iter == l5.end());
        }

        // Self splicing of multiple elements
        l5.splice(--l5.end(), l5, l5.begin(), --l5.end());
        EXPECT_EQ(l5.size(), 3u);
        {
            auto const_iter = l5.begin();
            EXPECT_TRUE(*const_iter == eleventh);
            ++const_iter;
            EXPECT_TRUE(*const_iter == thirteenth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == twelfth);
            ++const_iter;
            EXPECT_TRUE(const_iter == l5.end());
        }

        l1.clear();
        l2.clear();
        l3.clear();
        l4.clear();
        l5.clear();
    }
};

TEST(ComplianceSplice, IntrusiveList) { BindTypes<ComplianceSplice>(); }

// Tests compliance with "swap" function specifications.
template <typename List>
struct ComplianceSwap {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;
        typename List::value_type fourth;
        typename List::value_type fifth;
        typename List::value_type sixth;
        typename List::value_type seventh;

        List l1;
        l1.push_back(first);
        l1.push_back(second);
        List l2;
        l2.push_back(third);
        l2.push_back(fourth);
        l2.push_back(fifth);
        List l3;
        l3.push_back(sixth);
        l3.push_back(seventh);

        // Swap with itself
        l1.swap(l1);
        EXPECT_EQ(l1.size(), 2u);
        {
            auto const_iter = l1.begin();
            EXPECT_TRUE(*const_iter == first);
            ++const_iter;
            EXPECT_TRUE(*const_iter == second);
            ++const_iter;
            EXPECT_TRUE(const_iter == l1.end());
        }

        // Uses list swap function
        l1.swap(l2);
        EXPECT_TRUE(l1.size() == 3 && l2.size() == 2);
        {
            auto const_iter = l1.begin();
            EXPECT_TRUE(*const_iter == third);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fourth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fifth);
            ++const_iter;
            EXPECT_TRUE(const_iter == l1.end());
        }
        {
            auto const_iter = l2.begin();
            EXPECT_TRUE(*const_iter == first);
            ++const_iter;
            EXPECT_TRUE(*const_iter == second);
            ++const_iter;
            EXPECT_TRUE(const_iter == l2.end());
        }

        // uses std::swap
        swap(l3, l2);
        {
            auto const_iter = l3.begin();
            EXPECT_TRUE(*const_iter == first);
            ++const_iter;
            EXPECT_TRUE(*const_iter == second);
            ++const_iter;
            EXPECT_TRUE(const_iter == l3.end());
        }
        {
            auto const_iter = l2.begin();
            EXPECT_TRUE(*const_iter == sixth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == seventh);
            ++const_iter;
            EXPECT_TRUE(const_iter == l2.end());
        }

        l1.clear();
        l2.clear();
        l3.clear();
    }
};

TEST(ComplianceSwap, IntrusiveList) { BindTypes<ComplianceSwap>(); }

// Tests compliance with "sort" function specifications.
template <typename List>
struct ComplianceSort {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;
        typename List::value_type fourth;
        typename List::value_type fifth;
        typename List::value_type sixth;

        List l;

        // Sort an empty list
        l.sort();

        l.push_back(sixth);

        // Sort list of 1 element
        l.sort();

        // Sort a list of 2 elements
        l.push_back(second);

        l.sort();

        l.push_back(third);
        l.push_back(fourth);
        l.push_back(fifth);
        l.push_back(first);

        for (int i = 0; i < 2; i++) {  // Sort twice the same list
            l.sort();
            {
                auto const_iter = l.begin();
                EXPECT_TRUE(*const_iter == first);
                ++const_iter;
                EXPECT_TRUE(*const_iter == second);
                ++const_iter;
                EXPECT_TRUE(*const_iter == third);
                ++const_iter;
                EXPECT_TRUE(*const_iter == fourth);
                ++const_iter;
                EXPECT_TRUE(*const_iter == fifth);
                ++const_iter;
                EXPECT_TRUE(*const_iter == sixth);
            }
        }

        l.sort(std::greater<typename List::value_type>());
        {
            auto const_iter = l.begin();
            EXPECT_TRUE(*const_iter == sixth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fifth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == fourth);
            ++const_iter;
            EXPECT_TRUE(*const_iter == third);
            ++const_iter;
            EXPECT_TRUE(*const_iter == second);
            ++const_iter;
            EXPECT_TRUE(*const_iter == first);
        }

        l.clear();
    }
};

TEST(ComplianceSort, IntrusiveList) { BindTypes<ComplianceSort>(); }

// Tests compliance with "merge" function specifications.
template <typename List>
struct ComplianceMerge {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;
        typename List::value_type fourth;
        typename List::value_type fifth;
        typename List::value_type sixth;

        {
            List l1;
            l1.push_back(second);
            l1.push_back(fourth);
            List l2;
            l2.push_back(first);
            l2.push_back(third);
            l1.merge(l2);
            EXPECT_TRUE(l1.size() == 4 && l2.empty());
            {
                auto const_iter = l1.begin();
                EXPECT_TRUE(*const_iter == first);
                ++const_iter;
                EXPECT_TRUE(*const_iter == second);
                ++const_iter;
                EXPECT_TRUE(*const_iter == third);
                ++const_iter;
                EXPECT_TRUE(*const_iter == fourth);
            }
            l1.clear();
            l2.clear();
        }

        {
            List l1;
            l1.push_back(second);
            l1.push_back(fourth);
            List l2;
            l2.push_back(third);
            l1.merge(l2);
            EXPECT_TRUE(l1.size() == 3 && l2.empty());
            {
                auto const_iter = l1.begin();
                EXPECT_TRUE(*const_iter == second);
                ++const_iter;
                EXPECT_TRUE(*const_iter == third);
                ++const_iter;
                EXPECT_TRUE(*const_iter == fourth);
            }
            l1.clear();
            l2.clear();
        }

        {
            List l1;
            l1.push_back(second);
            l1.push_back(third);
            List l2;
            l2.push_back(fourth);
            l1.merge(l2);
            EXPECT_TRUE(l1.size() == 3 && l2.empty());
            {
                auto const_iter = l1.begin();
                EXPECT_TRUE(*const_iter == second);
                ++const_iter;
                EXPECT_TRUE(*const_iter == third);
                ++const_iter;
                EXPECT_TRUE(*const_iter == fourth);
            }
            l1.clear();
            l2.clear();
        }

        {
            List l1;
            l1.push_back(second);
            List l2;
            l2.push_back(third);
            l2.push_back(fourth);
            l1.merge(l2);
            EXPECT_TRUE(l1.size() == 3 && l2.empty());
            {
                auto const_iter = l1.begin();
                EXPECT_TRUE(*const_iter == second);
                ++const_iter;
                EXPECT_TRUE(*const_iter == third);
                ++const_iter;
                EXPECT_TRUE(*const_iter == fourth);
            }
            l1.clear();
            l2.clear();
        }

        {
            List l1;
            l1.push_back(first);
            l1.push_back(fourth);
            List l2;
            l2.push_back(second);
            l2.push_back(third);
            l1.merge(l2);
            EXPECT_TRUE(l1.size() == 4 && l2.empty());
            {
                auto const_iter = l1.begin();
                EXPECT_TRUE(*const_iter == first);
                ++const_iter;
                EXPECT_TRUE(*const_iter == second);
                ++const_iter;
                EXPECT_TRUE(*const_iter == third);
                ++const_iter;
                EXPECT_TRUE(*const_iter == fourth);
            }
            l1.clear();
            l2.clear();
        }

        {
            List l1;
            l1.push_back(first);
            l1.push_back(fourth);
            List l2;
            l2.push_back(second);
            l2.push_back(third);
            l1.merge(l2, std::less<typename List::value_type>());
            EXPECT_TRUE(l1.size() == 4 && l2.empty());
            {
                auto const_iter = l1.begin();
                EXPECT_TRUE(*const_iter == first);
                ++const_iter;
                EXPECT_TRUE(*const_iter == second);
                ++const_iter;
                EXPECT_TRUE(*const_iter == third);
                ++const_iter;
                EXPECT_TRUE(*const_iter == fourth);
            }
            l1.clear();
            l2.clear();
        }

        {
            List l1;
            l1.push_back(third);
            l1.push_back(sixth);
            List l2;
            l2.push_back(second);
            l2.push_back(fourth);
            List l3;
            l3.push_back(first);
            l3.push_back(fifth);

            l1.merge(l1);  // Self merge
            EXPECT_EQ(l1.size(), 2u);

            // Merge l1 into l2 in (default) ascending order, l1 and l2 are sorted
            l2.merge(l1);

            EXPECT_TRUE(l1.empty() && l2.size() == 4u);
            {
                auto const_iter = l2.begin();
                EXPECT_TRUE(*const_iter == second);
                ++const_iter;
                EXPECT_TRUE(*const_iter == third);
                ++const_iter;
                EXPECT_TRUE(*const_iter == fourth);
                ++const_iter;
                EXPECT_TRUE(*const_iter == sixth);
            }

            // l2 and l3 are not sorted "greater"
            if (void(0), TestAssertCompliance<List>::kValue) {
                EXPECT_ASSERTION(l2.merge(l3, std::greater<typename List::value_type>()), "");
            }

            // So sort l2
            l2.sort(std::greater<typename List::value_type>());

            // l3 is still not sorted  "greater"
            if (void(0), TestAssertCompliance<List>::kValue) {
                EXPECT_ASSERTION(l2.merge(l3, std::greater<typename List::value_type>()), "");
            }

            // So sort l3
            l3.sort(std::greater<typename List::value_type>());

            // l2 and l3 are sorted now
            l2.merge(l3, std::greater<typename List::value_type>());
            EXPECT_TRUE(l3.empty() && l2.size() == 6);

            {
                auto const_iter = l2.begin();
                EXPECT_TRUE(*const_iter == sixth);
                ++const_iter;
                EXPECT_TRUE(*const_iter == fifth);
                ++const_iter;
                EXPECT_TRUE(*const_iter == fourth);
                ++const_iter;
                EXPECT_TRUE(*const_iter == third);
                ++const_iter;
                EXPECT_TRUE(*const_iter == second);
                ++const_iter;
                EXPECT_TRUE(*const_iter == first);
            }
            l1.clear();
            l2.clear();
            l3.clear();
        }
    }
};

TEST(ComplianceMerge, IntrusiveList) { BindTypes<ComplianceMerge>(); }

// Always return the boolean _b.
template <typename T, bool _b>
bool Always(T) {
    return _b;
}

// Counts the number of time a functor is called
template <typename List>
class Count {
public:
    Count() : num_(0) {}
    void operator()(typename List::const_reference) { ++num_; }
    int num_;
};

// Tests complicance with std::list comparison operators.
template <typename List>
struct ComplianceComparisonOperator {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;
        typename List::value_type fourth;
        typename List::value_type fifth;
        typename List::value_type sixth;

        typename List::value_type first_less(first);
        typename List::value_type second_less(second);
        typename List::value_type third_less(third);

        typename List::value_type first_copy(first);
        typename List::value_type second_copy(second);
        typename List::value_type third_copy(third);

        List l1;
        l1.push_back(first);
        l1.push_back(second);
        l1.push_back(third);

        List l1_less;
        l1_less.push_back(first_less);
        l1_less.push_back(second_less);

        List l1_copy;
        l1_copy.push_back(first_copy);
        l1_copy.push_back(second_copy);
        l1_copy.push_back(third_copy);

        List l2;
        l2.push_back(fourth);
        l2.push_back(fifth);

        EXPECT_TRUE(l1 == l1);
        EXPECT_TRUE(l1 == l1_copy);
        EXPECT_TRUE(l1_copy == l1);
        EXPECT_TRUE(l1 != l2);
        EXPECT_TRUE(l1 != l1_less);
        EXPECT_TRUE(l1_less != l1);

        EXPECT_TRUE(l1 <= l1);
        EXPECT_TRUE(l1 >= l1);

        EXPECT_TRUE(l1 < l2);
        EXPECT_FALSE(l2 < l1);
        EXPECT_FALSE(l1 < l1_copy);
        EXPECT_TRUE(l1_less < l1);
        EXPECT_TRUE(l1 <= l2);
        EXPECT_TRUE(l1 <= l2);
        EXPECT_TRUE(l1 <= l1_copy);
        EXPECT_TRUE(l1_less <= l1);

        EXPECT_TRUE(l1 > l1_less);
        EXPECT_FALSE(l1 > l1_copy);
        EXPECT_FALSE(l1 > l2);
        EXPECT_FALSE(l1 >= l2);
        EXPECT_TRUE(l1 >= l1_less);
        EXPECT_TRUE(l1 >= l1_copy);

        l1.clear();
        l2.clear();
        l1_less.clear();
        l1_copy.clear();
    }
};

TEST(ComplianceOperator, IntrusiveList) { BindTypes<ComplianceComparisonOperator>(); }

// Tests compliance with std::list algorithms.
template <typename List>
struct ComplianceAlgorithm {
    void operator()() {
        typename List::value_type first;
        typename List::value_type second;
        typename List::value_type third;
        typename List::value_type fourth;
        typename List::value_type fifth;
        typename List::value_type sixth;

        List l1;
        l1.push_back(first);
        l1.push_back(second);

        List l2;
        l2.push_back(third);
        l2.push_back(fourth);
        l2.push_back(fifth);
        l2.push_back(sixth);

        // Count algorithms
        EXPECT_EQ(std::count(l1.begin(), l1.end(), second), 1);
        EXPECT_EQ(std::count_if(l1.begin(), l1.end(), Always<typename List::const_reference, false>), 0);
        EXPECT_EQ(std::count_if(l1.begin(), l1.end(), Always<typename List::const_reference, true>), 2);

        // Iteration
        Count<List> res = std::for_each(l1.begin(), l1.end(), Count<List>());
        EXPECT_EQ(res.num_, 2);

        l1.clear();
        l2.clear();
    }
};

TEST(ComplianceAlgorithm, IntrusiveList) { BindTypes<ComplianceAlgorithm>(); }

// Tests IntrusiveList kSafe linkMode specific behavior.
TEST(SafeLink, IntrusiveList) {
    typedef Option<vox::containers::LinkMode::kSafe> LocalOptions;
    typedef TestObj1<LocalOptions> LocalTestObj;
    typedef IntrusiveList<LocalTestObj, LocalOptions> List;

    LocalTestObj obj;
    EXPECT_FALSE(obj.is_linked());

    {  // Test link state
        List l, other;

#ifndef NDEBUG
        EXPECT_FALSE(obj.debug_is_linked_in(l));
#endif  // NDEBUG

        l.push_front(obj);
        EXPECT_TRUE(obj.is_linked());

#ifndef NDEBUG
        EXPECT_TRUE(obj.debug_is_linked_in(l));
        EXPECT_FALSE(obj.debug_is_linked_in(other));
#endif  // NDEBUG

        // Cannot be pushed twice
        EXPECT_ASSERTION(l.push_front(obj), "");

        l.pop_front();
        EXPECT_TRUE(!obj.is_linked());

        // Cannot be unlinked while not linked
        EXPECT_ASSERTION(obj.unlink(), "");
    }

    // Destroy the list before the hook
    EXPECT_ASSERTION(List().push_front(obj), "");

    {  // Destroy the hook before the list
        List l;
        LocalTestObj obj2;
        l.push_front(obj2);
        EXPECT_ASSERTION(obj2.~LocalTestObj(), "");
        obj2.unlink();
    }
}

// Tests IntrusiveList kAuto linkMode specific behavior.
TEST(AutoLink, IntrusiveList) {
    typedef Option<vox::containers::LinkMode::kAuto> LocalOptions;
    typedef TestObj1<LocalOptions> LocalTestObj;
    typedef IntrusiveList<LocalTestObj, LocalOptions> List;

    LocalTestObj obj;
    EXPECT_TRUE(!obj.is_linked());

    {  // Test link state
        List l, other;

#ifndef NDEBUG
        EXPECT_TRUE(!obj.debug_is_linked_in(l));
#endif  // NDEBUG

        l.push_front(obj);
        EXPECT_TRUE(obj.is_linked());

#ifndef NDEBUG
        EXPECT_TRUE(obj.debug_is_linked_in(l));
        EXPECT_TRUE(!obj.debug_is_linked_in(other));
#endif  // NDEBUG

        // Cannot be pushed twice
        EXPECT_ASSERTION(l.push_front(obj), "");

        l.pop_front();
        EXPECT_TRUE(!obj.is_linked());

        // Cannot be unlinked while not linked
        EXPECT_ASSERTION(obj.unlink(), "");
    }

    {  // Destroy the list before the hook
        List l;
        l.push_front(obj);
    }
    EXPECT_TRUE(!obj.is_linked());

    {  // Destroy the hook before the list
        List l;
        {
            LocalTestObj obj2;
            l.push_front(obj2);
        }
        EXPECT_TRUE(l.empty());
    }
}

// Tests IntrusiveList kUnsafe linkMode specific behavior.
TEST(UnsafeLink, IntrusiveList) {
    typedef Option<vox::containers::LinkMode::kUnsafe> LocalOptions;
    typedef TestObj1<LocalOptions> LocalTestObj;
    typedef IntrusiveList<LocalTestObj, LocalOptions> List;

    LocalTestObj obj;
    EXPECT_TRUE(!obj.is_linked());

    {  // Test link state
        List l, other;

#ifndef NDEBUG
        EXPECT_TRUE(!obj.debug_is_linked_in(l));
#endif  // NDEBUG

        l.push_front(obj);
        EXPECT_TRUE(obj.is_linked());

#ifndef NDEBUG
        EXPECT_TRUE(obj.debug_is_linked_in(l));
        EXPECT_TRUE(!obj.debug_is_linked_in(other));
#endif  // NDEBUG

        // Cannot be pushed twice
        EXPECT_ASSERTION(l.push_front(obj), "");

        l.pop_front();
        EXPECT_TRUE(!obj.is_linked());

        // Cannot be unlinked while not linked
        EXPECT_ASSERTION(obj.unlink(), "");
    }

    {  // Destroy the list before the hook
        List l;
        l.push_front(obj);
    }  // obj is in an undefined state now

    {  // Destroy the hook before the list
        List l;
        {
            LocalTestObj obj2;
            l.push_front(obj2);
        }  // l is in an undefined state now
    }
}
