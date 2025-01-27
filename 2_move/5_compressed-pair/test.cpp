#include "compressed_pair.h"

#include <catch.hpp>
#include <iostream>
#include <type_traits>
#include <memory>
#include <utility>
#include <mutex>


// empty class
struct Empty {};


// non-empty class
struct NonEmpty {
    int field = 0;
};

// move-only empty class
// non-copyable
struct MoveOnly {
    MoveOnly() = default;
    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly&& other) = default;
};

// non-empty move-only class
struct NonEmptyMoveOnly : public MoveOnly, public NonEmpty {};


// empty, copy-only class (+ copy operator= is deleted)
// non-movable
struct CopyOnly {
    CopyOnly() = default;
    CopyOnly(const CopyOnly&) = default;
    CopyOnly& operator=(const CopyOnly&) = default;
    CopyOnly(CopyOnly&&) = delete;
    CopyOnly& operator=(CopyOnly&&) = delete;
};

// nonempty, copy-only class (+ copy operator= is deleted)
struct NonEmptyCopyOnly : public CopyOnly, NonEmpty {};

// empty, but final (can't be derived from)
struct EmptyButFinal final {};


TEST_CASE("NonCompressedSimple") {
    using TestType = CompressedPair<int, std::string>;
    // creating TestType from 1 (rvalue) and "hello" (lvalue)
    TestType p{1, "hello"};

    // check that GetFirst and GetSecond are working
    REQUIRE(p.GetFirst() == 1);
    REQUIRE(p.GetSecond() == "hello");
}

TEST_CASE("NonCompressedModificationWorks") {

    // creating TestType from 1 (rvalue) and "hello" (lvalue)
    CompressedPair<int, std::string> p{1, "hello"};

    // modification works
    p.GetFirst() = 5;
    p.GetSecond()[3] = 'k';
    REQUIRE(p.GetFirst() == 5);
    REQUIRE(p.GetSecond() == "helko");
}


TEST_CASE("SimpleOperationsWithCompressedPair") {


    // U is not compressed, V is compressed
    // creation of compressed pair from lvalue and rvalues
    CompressedPair<std::string, std::allocator<std::string>> p = {"hello",
                                                                  std::allocator<std::string>{}};

    // allocating a new string in the heap with allocation, created before, with the string "hello" as value
    std::string* ptr = new (p.GetSecond().allocate(1)) std::string{p.GetFirst()};

    // checking, that the string is actually hello
    REQUIRE(*ptr == "hello");

    // calling destructor on the string
    ptr->~basic_string<char>();

    // deallocating the memory of the string
    p.GetSecond().deallocate(ptr, 1);
}

// structure with no default constructor
struct NoDefaultConstructor {

    // default constructor is deleted
    NoDefaultConstructor() = delete;

    // constructor with int argument
    NoDefaultConstructor(int f) : field(f) {
    }

    int field;
};


TEST_CASE("Constructors") {
    // Default.

    // there should be default constructor
    CompressedPair<int, std::string> p0;

    // int and std::string are constructed as default value
    REQUIRE(p0.GetFirst() == 0);
    REQUIRE(p0.GetSecond().empty());

    // Copy and move for empty classes.
    CopyOnly copy1;
    MoveOnly move1;
    
    // constructor from lvalue and rvalue for empty classes
    CompressedPair<CopyOnly, MoveOnly> p1{copy1, std::move(move1)};

    // Copy and move for non-empty classes.

    // nonempty and copyable and non-movable object
    NonEmptyCopyOnly copy2;

    // nonempty and movable and non-copyable object
    NonEmptyMoveOnly move2;


    // creating an object from lvalue and rvalue for non-empty classes
    CompressedPair<NonEmptyCopyOnly, NonEmptyMoveOnly> p3{copy2, std::move(move2)};

    // creating an object from 2 rvalues (one with nodefault constructor, the second is rvalue value of type int)
    CompressedPair<NoDefaultConstructor, int> p4(NoDefaultConstructor{3}, 0);

    // check the correctness of the field of the first value in compressed pair
    REQUIRE(p4.GetFirst().field == 3);
}


// function to check the size of the compressed pair
template <class V>
inline constexpr bool is_compressed_v = std::is_empty_v<V> && !std::is_final_v<V>;  // NOLINT
template <class U, class V>
void CheckSize() {
    // if U
    //  - is non union
    //  - has no non_static fields (except 0 size bit-fields)
    //  - has no virtual methods 
    //  - has no virtual Base classes
    //  - has no non-empty Base classes
    //  - can be derived from
    //
    // if V 
    //  - is not a union
    //  - has no non-static data (except 0 size bit fields)
    //  - has no virtual functions
    //  - has no virtual base classes
    //  - has no non-empty Base classes
    //  - can be derived from
    if constexpr (is_compressed_v<U> && is_compressed_v<V>) {
        // if V is base of U or U is base of V
        // U is derived from V or V is derived from U or V = U
        if constexpr (std::is_base_of_v<U, V> || std::is_base_of_v<V, U>) {
            // the size should be 2
            static_assert(sizeof(CompressedPair<U, V>) == 2);

        // if
        // U is not derived from V
        // V is not derived from U
        // V != U or V is union or U is union
        } else {
            // the size should be 1
            static_assert(sizeof(CompressedPair<U, V>) == 1);
        }

    // V
    // - is not a union
    // - has no virtual base classes
    // - has no virtual methods
    // - has no non-static members (except bit-fields of size 0)
    // - has no non-empty base
    // - can be derived from
    // - U is not derived from V
    // - V != U or U is union

    // U
    // is union or has non-empty base or has virtual base classes or has virtual methods or has non-static members or is final
    } else if constexpr (is_compressed_v<V> && !std::is_base_of_v<V, U>) {

        // the size should be the same as U
        static_assert(sizeof(CompressedPair<U, V>) == sizeof(U));

    // U
    // - is not a union
    // - has no virtual base classes
    // - has no virtual methods
    // - has no non-static members (except bit-fields of size 0)
    // - has no non-empty base
    // - can be derived from
    // - V is not derived from U
    // - U != V or V is union
    
    // V
    // is union or has non-empty base or has virtual base classes or has virtual methods or has non-static members or is final
    } else if constexpr (is_compressed_v<U> && !std::is_base_of_v<U, V>) {

        // the size should be the same as V
        static_assert(sizeof(CompressedPair<U, V>) == sizeof(V));



    // V is not compressed (is union or has non-empty base or has virtual base classes or has virtual methods or has non-static members or is final) 
    // and U is not compressed (is union or has non-empty base or has virtual base classes or has virtual methods or has non-static members or is final)
    // OR
    
    // V is compressed (
    // - is not a union
    // - has no virtual base classes
    // - has no virtual methods
    // - has no non-static members (except bit-fields of size 0)
    // - has no non-empty base
    // - can be derived from
    // )
    // AND
    // U is not compressed (is union or has non-empty base or has virtual base classes or has virtual methods or has non-static members or is final)
    // AND
    // U is derived from V or (V != U and V is not union and U is not union)
    
    
    // OR
    
    // U is compressed  (
    // - is not a union
    // - has no virtual base classes
    // - has no virtual methods
    // - has no non-static members (except bit-fields of size 0)
    // - has no non-empty base
    // - can be derived from
    // )
    // AND
    // V is not compressed  (is union or has non-empty base or has virtual base classes or has virtual methods or has non-static members or is final)
    // AND
    // V is derived from U or (U != V and U is not union and V is not union)
    } else {


        
        // the size should be the same as the size of std::pair of U and V
        // sizeof(pair<U, V>)
        static_assert(sizeof(CompressedPair<U, V>) == sizeof(std::pair<U, V>));
    }
};



// structure with overloaded operator()
struct MyStatelessFunctor {

    // const operator() that returns std::string
    std::string operator()() const {
        return "I am so tired.";
    }
};

// another empty structure
struct Empty2 {};

// empty structure, which is derived from another empty structure
struct Empty3 : Empty2 {};

// non empty structure, which is derived from another empty structure
struct NonEmptyDescendant : Empty {
    int field;
};

// empty union
union EmptyUnion {};

// empty structures
struct Empty4 {};
struct Empty5 {};
struct Empty6 {};

// another empty structure, that can be a base
struct EmptyBase {};

// empty structure that is derived from some empty base
struct EmptyDerived1 : EmptyBase {};

// empty structure that is derived from some empty base
struct EmptyDerived2 : EmptyBase {};

// non-empty structure that is dervived from NonEmpty and EmptyBase
struct NonEmptyDerived : NonEmpty, EmptyBase {};

TEST_CASE("Sizes") {

    // checking the sizeof

    // U is not compressed, V is compressed, V is not a base for U => the sizeof should be sizeof(U) = 4
    CheckSize<int, Empty>();

    // U is compressed, V is not compressed, U is not a base for V => the sizeof should be sizeof(V) = 16
    CheckSize<Empty, long double>();

    // U is not compressed, V is not compressed => the sizeof should be sizeof(std::pair<std::string, EmptyButFInal>)
    CheckSize<std::string, EmptyButFinal>();

    // U is compressed, V is compressed, U = V => std::base_of<U, V> is true => sizeof should be 2
    CheckSize<Empty, Empty>();

    // U is not compressed, V is compressed, V is not a base for U => the sizeof should be sizeof(U) = 8
    CheckSize<std::unique_ptr<std::string>, Empty>();

    // U is compressed, V is not compressed, U is not a base for V => the sizeof should be sizeof(V) = 40
    CheckSize<Empty, std::mutex>();

    // U is not compressed, V is compressed, V is not a base for U => the sizeof should be sizeof(U) = 32
    CheckSize<std::basic_string<int>, MyStatelessFunctor>();

    // U is compressed, V is compressed, V is not a base for U, U is not a base for V, U != V => the sizeof should be 1
    CheckSize<Empty, Empty2>();

    // U is compressed, V is compressed, U is base for V => the sizeof should be 2
    CheckSize<Empty2, Empty3>();

    // U is compressed, V is not compressed, U is a base for V => the sizeof should be sizeof(std::pair<Empty, NonEmptyDescendant>) = 8
    CheckSize<Empty, NonEmptyDescendant>();

    // U is not compressed, V is not compressed, the sizeof should be sizeof(std::pair<EmptyUnion, int>) = 8
    CheckSize<EmptyUnion, int>();


    // that means, in particular that "std::is_empty_v<CompressedPair<Empty4, Empty5>>" should not have any non-static fields
    // ? : strange: is it possible, to create different number of fields, depending on the template type
    static_assert(std::is_empty_v<CompressedPair<Empty4, Empty5>>);

    // from previous, we know: CompressedPair<CompressedPair<Empty4, Empty5> is empty_v
    
    // If CompressedPair<Empty6, char>> is compressed, both of them are not the std::base_of each other => sizeof should be 1
    //          ifCompressedPair<Empty6, char>> is not compressed =>
    // U is compressed, V is not compressed, both of them are not the std::base_of each other => sizeof should be sizeof(CompressedPair<Empty6, char>>) =
    //          [Empty6 is compressed, char is not compressed, both of them are not the std::base_of each other => sizeof should be sizeof char = 1] = 1

    // in both cases sizeof should be 1 = sizeof(char)
    static_assert(
        sizeof(CompressedPair<CompressedPair<Empty4, Empty5>, CompressedPair<Empty6, char>>) ==
        sizeof(char));


    // EmptyDerived1, EmptyDerived2 are compressed, they are not base for each other
    // somehow, size should be 1, according to CheckSize
    // but actually, in my correct implementation, size was 2
    // this means that CheckSize doesn't cover all the cases ?
    static_assert(sizeof(CompressedPair<EmptyDerived1, EmptyDerived2>) == 2);

    // U is not compressed, V is compressed, both are not std::base of each other => sizeof should be sizeof(U) = 8
    static_assert(sizeof(CompressedPair<NonEmptyDerived, EmptyDerived1>) == 8);
}

TEST_CASE("ConstMethods") {
    // create const pair from lvalue and rvalue
    const CompressedPair<int, std::string> p1{1, "hello!"};

    // check two functions
    REQUIRE(p1.GetFirst() == 1);
    REQUIRE(p1.GetSecond() == "hello!");

    // create const pair from lvalue and rvalue
    // check two functions
    const CompressedPair<int, MyStatelessFunctor> p2{1, MyStatelessFunctor{}};
    REQUIRE(p2.GetFirst() == 1);
    REQUIRE(p2.GetSecond()() == "I am so tired.");
}
