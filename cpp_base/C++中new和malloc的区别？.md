# C++面试高频题：new和malloc的区别？

大家好，我是Q。

相信大部分C++选手在面试过程中，都会被问到`malloc`和`new`的区别。

你是怎么回答的呢？

不知道先说什么后说什么？或者是勉强答出来了，让面试官觉得你逻辑不够清晰，没有得到他想要的答案。

虽然说是一道非常简单非常基础的题目，但是面试官就能从你的回答中观察到你对技术的追求，你愿不愿意对某项技术做更深的思考？

那今天我就冒充一下面试官吧哈哈，给大家讲讲到底该怎么回答？

### 1、基本区别

#### malloc

`malloc` 是 C 和 C++ 标准库中的一个函数，用于从堆中分配一块未初始化的连续内存区域。

函数原型：

```c
void *malloc (size_t __size)
```

其中 `size` 参数指定了所需分配的字节数。

`malloc` 成功时返回一个 `void*` 类型的指针，指向已分配的空间；如果分配失败，则返回 `NULL`。

由于返回的是 `void*`，在 C++ 中通常需要将其强制转换为目标类型的指针。

`malloc` 不会自动初始化分配的内存，也不会调用任何构造函数或析构函数，这意味着必须手动初始化这块内存才能安全地使用它。

#### new

`new` 是 C++ 中的一个关键字兼操作符，用于动态地从自由存储区（free store）上为对象分配内存空间，并自动调用该对象的构造函数进行初始化。

函数原型：

```c
void* operator new(std::size_t)
void* operator new[](std::size_t)
```

它不仅负责分配内存，还确保对象被正确地构造。

`new` 返回的是一个指向所创建对象的具体类型指针，因此不需要显式的类型转换。

此外，当内存分配失败时，`new` 会抛出 `std::bad_alloc` 异常而不是返回 `NULL`。

对于数组，C++ 提供了 `new[]` 操作符，它可以分配一个指定类型的对象数组，并为每个元素调用构造函数。相应的，释放由 `new[]` 分配的内存应当使用 `delete[]`，以确保每个对象的析构函数都被调用。

其实上面这些已经足矣，在面试过程中清晰的回答出来就没有什么问题。

不过以下这些问题点还是要注意的。

### 2、内存分配失败的行为

#### malloc

当 `malloc` 无法分配足够的内存时，它返回 `nullptr`（在 C++ 中为 `NULL`）。

需要显式地检查返回值是否为 `nullptr` 来判断内存是否分配成功。

#### new

默认情况下，`new` 如果内存分配失败，会抛出 `std::bad_alloc` 异常。不需要显式检查返回值。

如果希望 `new` 在分配失败时返回 `nullptr`，可以使用 `new(std::nothrow)` 版本。

```c
int* p = new(std::nothrow) int;
if (!p) {
    // 内存分配失败的处理
}
```

### 3、内存释放

`malloc` 使用 `free` 来释放内存。

`new` 使用 `delete` 来释放内存。

**注意：** 一定要匹配使用 `malloc` 和 `free`，以及 `new` 和 `delete`。不能混用 `malloc/free` 和 `new/delete`，否则会引发内存错误。

```c
int* p = (int*)malloc(sizeof(int)); // malloc 分配
free(p);                             // 使用 free 释放

int* p2 = new int;                  // new 分配
delete p2;                          // 使用 delete 释放
```

错误示范：

```c
int* p = (int*)malloc(sizeof(int)); 
delete p;  // 错误！应该使用 free(p)
```

### 4、构造函数的调用

**`malloc`** 只分配内存，但不会调用对象的构造函数。

**`new`** 在分配内存的同时会调用对象的构造函数。这是 C++ 的一大特点，因为 C++ 是面向对象的语言，对象的构造函数和析构函数非常重要。

```c
class MyClass {
public:
    MyClass() { std::cout << "Constructed\n"; }
};

MyClass* p = new MyClass; // 调用构造函数
```

### 5、使用场景

`malloc` 是 C 风格的内存分配方式，通常在 C++ 中不推荐使用，因为它不调用构造函数，不能直接初始化对象。它更多的用于 C 风格的代码，或者一些 C 和 C++ 混合开发的场景中。

`new` 是 C++ 专用的动态内存分配方式，广泛用于 C++ 中，特别是在需要创建对象的动态实例时（即堆上的对象）。C++ 中的对象管理通常依赖 `new` 和 `delete` 来确保构造函数和析构函数的正确调用。

### 6、内存对齐与类型安全

**`malloc`** 返回的是 `void*` 类型，需要显式地转换为目标类型指针，这可能会导致类型不安全的操作。

**`new`** 返回的是目标类型的指针（例如 `int*`），它会自动进行类型安全的转换，避免了类型不匹配的问题。
