# Breccia: A Modern Take on Systems Programming

Breccia is a new systems programming language with the aim to be 100% compatible
with your system's C99 ABI. This means that a C program can directly link a
Breccia program with NO CHANGES. In fact, even dynamic linking is supported.

Breccia will allow any random dll/so file to be rewritten with no ABI breaks.

# Goals

## Compatiblity

First and foremost, Breccia is designed for compatiblity with existing binaries.
This means that Breccia will target the stable C99 ABI for your system.

## Dynamic Linking

Breccia is also designed with dynamic linking as a first class citizen. This means
that every language feature is coherent when dynamic linking. This will be true
if you use `dlopen` or link at compile time. Breccia doesn't have headers, but the
compiler can generate C headers for ALL language features.

Additionally, Breccia may define its own API-only source format for ease of dynamic
linking.

## Ease of Use

Breccia is designed to be a modern language with human-readable syntax.

## Non-Prescriptive

Unlike lots of (great) modern languages, Breccia aims to be non-prescriptive. It
has a null type, double frees are possible, undefined behavior can happen. 

## Low Cost, Opt-in Polymorphism

Breccia supports Go-like interfaces which have no cost when not used. If you don't
need the feature, just don't use it! Performance will be the same as C. Implementing
an interface doesn't change the implementing data type at all. Use the inteface
representation in most places while using the raw struct in performance critical paths
for the best of all worlds!

Even when using interfaces, they don't cost much. An interface representation of
a struct is always a fat-pointer with two references. Dynamic dispatch functions
against interfaces require pointer 2 deferences.

Breccia Interfaces even work in C programs! 

### Is this better than C++ inherentence

Yes. 

In C++ (for most implementations), ineheriting from a class requires a "parent" pointer for _each instance_ of the child class. If you made a small
class like:

```c++
class Vec2 {
    public:
    float x;
    float y;
}
```

most 64-bit implementations would require 8 bytes of storage for a `Vec2`. This makes sense since `float` is a 4 byte type.

However, `Vec2` extended a parent class:

```c++
class Vec2: SomeUsefulParent {
    public:
    float x;
    float y;
}
```
Now, if you look at the result of `sizeof(Vec2)`, you might be suprised to find it now needs 16 bytes. That's twice the memory! If you were so bold to extend two parent classes, you'll find that `Vec2` now requires 24 bytes.

If you were representing a shape made from 1000 `Vec2`s, you would be wasting about 8 kilobytes of memory just by extending one class. 

This means that performance and/or memory sensitive code usually has to avoid inheritence altogether. 

In Breccia, `Vec2` can implement any number of interfaces without changing the in-memory representation of the struct. This means that there are no negative externalities of polymorphism! This makes it much safer to use in code where performance matters. 

Breccia interfaces only use memory when you actively have a reference to them.

## Performant

Breccia doesn't do anything for you. No resources will be used unless your code
specifically uses them. There is no magic.

## Wait, the C99 ABI is not well defined! What are you talking about?

The C99 ABI isn't formally defined, but on a given target platform (Arch/OS/libc)
the ABI is very stable. This is why Linux works! Breccia aims to be compatible
with existing C99 binaries on your system.