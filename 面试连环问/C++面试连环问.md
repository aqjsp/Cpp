# C++面试连环问

面试官丢一句「指针和引用有什么区别」，要的不是清单，是你能不能被往下追：空不空、能不能换绑、形参合同、实现是不是指针。下面每题按能说出口的骨架写，追问带代码，对照基础 / 进阶篇。C++17。

---

### 1、指针和引用有什么区别？

能说出口：引用是别名，必须绑活对象，不能空、不能换绑；指针是独立对象，装着地址，可以空、可以改指向、可以再取地址。`sizeof(引用)` 等于所指类型，`sizeof(指针)` 是指针宽度。形参 `T&` 合同是「进来就是活对象」，`T*` 合同是「可能空」。

往下追：引用是不是就是指针？实现常常是，语言不给你这个指针去空、去算术。`int& r = n; r = 2;` 写的是 `n`，不是换绑。`&r == &n`，没有「引用自己的地址」。不存在引用的指针这种类型；存在指针的引用 `int*&`。

```cpp
int n = 1;
int& r = n;                 // 必须立刻绑
int* p = &n;
r = 2;                      // n == 2
p = nullptr;                // 合法
// int& e;                  // 非法：没绑
// int& z = nullptr;        // 非法
```

再往下：`const T&` 能绑右值，`T&` 不能——所以只读大对象用 `const T&`，要改用 `T&`，可选才 `T*`。`int*& rp = p;` 是指针这只盒子的别名，赋值改的是指向。数组传函数退化成指针，`sizeof` 变成指针宽度，这是另一道题的入口。

```cpp
void bump(int& x) { x += 1; }
void bump_p(int* x) { if (x) *x += 1; }

int n = 1;
bump(n);
bump_p(&n);
bump_p(nullptr);            // 合法
// bump(1);                 // 非法：非 const 左值引用绑不上右值
```

对照：指针篇「引用是别名，指针是对象」。可选、延迟绑定、C API 用指针；必须存在用引用。小标量按值，别 `const int&`。

---

### 2、new 和 malloc 有什么区别？

能说出口：`malloc` 买字节，返回 `void*`，失败返回空，不构造。`new T` 先分配再构造，返回 `T*`，失败抛 `bad_alloc`（除非 `nothrow`）。配对：`new`/`delete`、`new[]`/`delete[]`、`malloc`/`free`，混用 UB。`delete nullptr` 合法。

往下追：构造抛了会不会漏那块裸内存？不会——语言先释放再让异常走。泄漏发生在 `new` **已经返回**之后你把指针放进局部，再 `throw`，栈展开不 `delete` 裸指针。定位 new `new (ptr) T` 不分配，配对是 `ptr->~T()`，不是 `delete ptr`。

```cpp
int* a = static_cast<int*>(std::malloc(sizeof(int)));
if (!a) return;
*a = 1;
std::free(a);

int* b = new int{1};        // 分配 + 构造
delete b;

alignas(int) unsigned char buf[sizeof(int)];
int* c = new (buf) int{2};  // 不分配
c->~int();                  // 不是 delete c
```

再往下：`new T[n]` 配 `delete[]`。数组 new 会在块前头记个数（实现细节），用错释放函数会把个数当数据、或漏析构。`malloc(0)` 实现定义。过对齐类型 `malloc` 不够，要用 `aligned_alloc` 或重载 `new`。`nothrow` 版本失败返回空，之后仍要自己查，合同接近 malloc，但构造还是会跑。

```cpp
int* a = new int[3]{};
delete[] a;
// delete a;                // UB

int* z = new int{};         // 0
int* g = new int;           // 垃圾，别读
delete z;
delete g;
```

对照：内存篇「new 不只是分配」。`new int` 默认初始化是垃圾，`new int{}` 才是 0。对象池那篇的 `make_unique<T>` 走的仍是这条 `new`，只是第一次。

---

### 3、虚函数怎么实现？虚析构为什么必须？

能说出口：对象头上一个 vptr，指向类的 vtable。调用 `p->f()` 若 `f` 是 virtual：取 vptr → 按槽取函数指针 → 间接调用。槽位编译期定，运行时一次间接。基类指针 `delete` 派生堆对象，基类析构必须 virtual，否则只调基类析构，派生资源漏，且是 UB。

往下追：构造 / 析构里调虚函数走的是**当前正在构造的那一层**，不是最终动态类型——vptr 此时指向当前类的表。没有虚构造函数。`override` 拦签名写错。按值传基类会切片，虚调用没了。

```cpp
struct Base {
    virtual void who() { std::cout << "Base\n"; }
    virtual ~Base() = default;
};
struct Derived : Base {
    void who() override { std::cout << "Derived\n"; }
    ~Derived() override = default;
};

Base* p = new Derived;
p->who();                   // Derived
delete p;                   // ~Derived 然后 ~Base
```

再往下：派生类写同名非虚是隐藏，不是覆盖——基类重载一并被挡。`using Base::f;` 拉回来。虚函数覆盖要求签名匹配（含 `const`、引用限定），返回类型允许协变。`final` 拦再覆盖。没有虚函数就没有 RTTI 可用的 `dynamic_cast` 下行（实现通常靠 vtable 旁的 typeinfo）。虚调用贵在一次间接 + 无法内联；热路径能静态绑就别虚。

```cpp
void by_value(Base b) { b.who(); }      // 切片，永远 Base
void by_ref(Base& b) { b.who(); }       // 虚
Base x;
Derived d;
by_value(d);                            // Base
by_ref(d);                              // Derived
x.who();                                // 静态类型就是 Base，虚不虚一样
```

对照：虚函数篇开头那个非虚 `who` 打印 Base。值传 `void f(Base b)` 永远 Base。对象模型篇：加虚函数，布局多一个 vptr。

---

### 4、unique_ptr / shared_ptr / weak_ptr 怎么选？

能说出口：独占用 `unique_ptr`，不可拷可移，默认宽度一个指针。共享所有权用 `shared_ptr`，控制块上原子计数。`weak_ptr` 观察、不延长寿命，破环、缓存。能独占就不要共享。

往下追：两个线程各拷一份 `shared_ptr`，计数安全；同时写 `*p` 是 data race。共享**同一个** `shared_ptr` 变量（一个赋值一个拷贝）连计数那层都不是线程安全的。`make_shared` 一次分配对象+控制块，对象死后控制块还在，大对象+长期 `weak_ptr` 会把内存钉住。从 `this` 造 `shared_ptr` 会另起控制块，double free；用 `enable_shared_from_this`。

```cpp
auto u = std::make_unique<int>(1);
std::shared_ptr<int> s = std::move(u);  // unique 升 shared
std::weak_ptr<int> w = s;
s.reset();
assert(w.expired());
```

再往下：删除器是 `unique_ptr` 类型的一部分，无状态空 class 走 EBO，宽度仍一个指针；函数指针当删除器就两只指针宽。`unique_ptr<T[]>` 用 `delete[]`。`release()` 交裸指针并置空，之后必须有人 `delete`，对象池场景调用它几乎必炸。别名 `shared_ptr`：控制块跟对象 A，指针跟成员，寿命绑 A。`weak_ptr::lock` 失败返回空，不要先 `expired` 再 `lock` 当互斥——中间最后一份 `shared_ptr` 可以死。

```cpp
struct Node : std::enable_shared_from_this<Node> {
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;
    std::shared_ptr<Node> self() { return shared_from_this(); }
};
// 构造期间不要 shared_from_this：控制块还没挂上
```

对照：智能指针篇「拷贝安全 ≠ 解引用安全」。形参不要默认 `shared_ptr<T>`，那是在入股所有权。对象池用 `unique_ptr` + 自定义删除器还池，不是 `shared_ptr`。

---

### 5、什么是 RAII？和智能指针什么关系？

能说出口：资源获取即初始化——分配焊在构造，释放焊在析构。栈展开必析构已经构造完的自动对象，所以异常路径也释放。智能指针是 RAII 的一种应用，管的是堆上 `T`；文件、锁、socket 同一套。

往下追：`lock_guard` 也是 RAII，管的是 mutex。析构禁止抛，否则 `terminate`。成员按声明反序析构：先拿到的资源后放，正好对上「后拿到的依赖先拿到的」。手写 `new` 立刻交给 RAII 对象；能 `vector` / `string` / `unique_ptr` 就不要出现在五法则清单上。

```cpp
void leak() {
    int* p = new int{1};
    throw std::runtime_error("boom");
    delete p;               // 走不到
}
void ok() {
    auto p = std::make_unique<int>(1);
    throw std::runtime_error("boom");   // ~unique_ptr 仍 delete
}
```

再往下：异常安全三档——不抛（析构、`swap`、`move` 最好）、基本保证（抛了对象仍有效）、强保证（抛了状态回滚）。拷贝赋值先造新再放旧是强保证的常见拼法。工厂返回 `unique_ptr`，所有权在类型里读。C API 的 `FILE*` 用带 `fclose` 删除器的 `unique_ptr`，不要裸着传到第三个函数。

```cpp
struct FileCloser {
    void operator()(std::FILE* f) const { if (f) std::fclose(f); }
};
using UniqueFile = std::unique_ptr<std::FILE, FileCloser>;
UniqueFile fp(std::fopen("a.txt", "r"));
```

对照：内存篇第一段。对象池那篇用自定义 deleter 把「还池」焊进 `unique_ptr` 析构，还是 RAII，只是释放变成还仓库。

---

### 6、三法则、五法则、零法则？

能说出口：析构、拷贝构造、拷贝赋值三条要一起管（三法则）。C++11 加上移动构造、移动赋值（五法则）。定义了其中一条，其它必须显式写或 `= delete` / `= default`。零法则：资源交给成员（`vector`、`unique_ptr`），自己不写这五条。

往下追：写了析构，编译器仍可能生成拷贝——浅拷导致 double free。写了拷贝，移动不会隐式生成。`vector<T>` 扩容优先走 `noexcept` 移动；移动会抛就可能改拷，N 次堆分配。按值赋值 `operator=(T o)` 一份代码吃拷贝和移动，强保证来自「先造 `o` 再碰 `this`」。

```cpp
struct Buf {
    int* p;
    explicit Buf(int n) : p(new int[n]{}) {}
    ~Buf() { delete[] p; }
    Buf(const Buf&) = delete;           // 有析构就必须表态
    Buf& operator=(const Buf&) = delete;
    Buf(Buf&& o) noexcept : p(o.p) { o.p = nullptr; }
    Buf& operator=(Buf&& o) noexcept {
        if (this != &o) { delete[] p; p = o.p; o.p = nullptr; }
        return *this;
    }
};
```

再往下：自己管资源就别只写析构——那是最常见的 double free 来源。成员已经是 `vector` / `string` 时再手写拷贝，多半是浅拷一份指针。`= default` 是「按成员做」，成员会移动则整体会移动。`= delete` 拷贝后，移动也不会隐式生成，要移动必须自己写。自赋值：`a = a` 先 `delete[] p` 再拷自己，崩。按值赋值躲开这条，因为 `o` 是另一只盒子。

```cpp
Buf& operator=(Buf o) noexcept {        // 按值：拷或移进 o
    std::swap(p, o.p);
    return *this;
}
```

对照：对象模型篇五法则；现代 C++ 篇 `Buf` 的按值赋值。对象池禁止移动，四条 `= delete`，同一条规则。

---

### 7、左值、右值、将亡值？std::move 做了什么？

能说出口：左值有身份、能取地址；纯右值是字面量、临时量；将亡值是「可以偷」的右值，`std::move(x)` 把左值转成将亡值，**不移动**。真转手发生在移动构造 / 移动赋值里。`move` 之后源处于有效但未指定状态，只保证可析构、可赋值。

往下追：`return std::move(local);` 常阻止 NRVO，局部直接 `return b;`。成员、`*this` 的子对象必须 `std::move`。形参 `T&&` 是右值引用；模板 `T&&` 是万能引用，要 `std::forward`。`const T&` 能绑右值，但绑上之后是 const 左值，偷不走。

```cpp
std::string a = "hello";
std::string b = std::move(a);           // a 合法但空
std::string make() {
    std::string s = "x";
    return s;                           // 不要 move
}
```

再往下：值类别驱动重载。`void f(T&); void f(T&&);` 左值走前者，临时量和 `move` 过的走后者。转发引用：`template<class T> void f(T&& x) { g(std::forward<T>(x)); }` —— `T` 推成左值引用时 `forward` 还是左值，推成非引用时还是右值。`std::move` 是 `static_cast<T&&>`。内置类型 `move` 之后值还在，不要依赖；`vector` 通常空。

```cpp
template <typename T>
void sink(T&& x) {
    box.push_back(std::forward<T>(x));  // 左值拷，右值移
}
```

对照：指针篇值类别；现代 C++ 篇「`std::move` 不移动」。Go / Python 没有这条将亡轴。

---

### 8、vector 扩容怎么做？迭代器何时失效？

能说出口：三指针 `begin` / `end` / `cap`。满了申请更大块（常见 2 倍，实现可不同），搬元素，释放旧缓冲。均摊 `push_back` O(1)，单次 O(n)。扩容后指向旧缓冲的迭代器、指针、引用全部失效，再用是 UAF。

往下追：`reserve(n)` 是合同：capacity ≥ n 之前，只在尾部插入不扩容，迭代器不因扩容失效（erase / insert 中间仍失效）。`size` 和 `capacity` 不是一回事。`operator[]` 越界 UB，`at` 抛。`vector<bool>` 不是 `vector`，不要用。移动最好 `noexcept`，否则扩容可能拷。

```cpp
std::vector<int> v{1, 2, 3};
auto it = v.begin();
int* p = &v[0];
v.reserve(100);             // 可能已经搬过；reserve 本身就可能失效旧 it
v.push_back(4);             // 若 capacity 够，it / p 仍有效
```

再往下：失效不全是扩容。`insert` / `erase` 中间：被删那个失效，其后的迭代器失效（要挪）。`end()` 在 `push_back` 后也会变。`shrink_to_fit` 可以重新分配，迭代器失效。空 `vector` 的 `data()` 不要解引用。`sizeof(vector<T>)` 与 `size()` 无关，常见 24。`vector<vector<int>>` 不是二维连续。

```cpp
std::vector<int> v;
v.reserve(3);
auto it = v.begin();
v.push_back(1);
v.push_back(2);
v.push_back(3);             // 未扩容，it 仍有效
v.push_back(4);             // 扩容，it 悬
```

对照：STL 篇开头那段循环 `push_back` 握着旧迭代器。`data()` 给 C API 连续缓冲。对象池的空闲栈是 `vector<T*>`，扩容的是指针数组，不是 `T` 本身。

---

### 9、map 和 unordered_map 怎么选？

能说出口：`map` 是有序树（通常红黑树），键要 `<`，查找 / 插入 O(log n)，迭代按键序。`unordered_map` 是哈希表，键要哈希和 `==`，均摊 O(1)，最坏 O(n)，无序。需要序、需要稳定最坏、键不好哈希：`map`。热路径查找、键哈希便宜：`unordered_map`。

往下追：`map` 插入不失效迭代器（除被删那个）。`unordered_map` 重哈希会让迭代器失效，引用到元素是否失效看标准：C++17 重哈希迭代器失效，指向元素的指针 / 引用仍有效（节点容器）。`reserve` / `max_load_factor` 能少重哈希。自定义类当键必须同时提供哈希和相等，且相等的哈希必须相等。

```cpp
std::map<std::string, int> ordered;           // 按 key 排
std::unordered_map<std::string, int> fast;    // 均摊 O(1)
fast.reserve(1000);
```

再往下：`map::operator[]` 没有就插入默认值，只查用 `find` / `count` / C++17 `try_emplace`。`unordered_map` 的桶数是质数策略还是 2 的幂，实现不同，最坏退化成链表（C++11 要求拉链，不能开寻址丢迭代器稳定性）。自定义哈希质量差，碰撞会把 O(1) 打成 O(n)，这是线上事故不是理论。`multimap` / `unordered_multimap` 允许重复键。

```cpp
struct Key { int a, b; };
struct KeyHash {
    std::size_t operator()(const Key& k) const noexcept {
        return std::hash<int>{}(k.a) ^ (std::hash<int>{}(k.b) << 1);
    }
};
struct KeyEq {
    bool operator()(const Key& x, const Key& y) const noexcept {
        return x.a == y.a && x.b == y.b;
    }
};
std::unordered_map<Key, int, KeyHash, KeyEq> m;
```

对照：STL 篇容器换什么。不要在热路径上用 `map<string>` 当字典还假设 O(1)。

---

### 10、const 写在哪一侧？成员函数尾 const 是什么？

能说出口：`const T*` 不能改所指，指针能改指向；`T* const` 指针不能改，能改所指；`const T* const` 两边都不能。成员函数尾 `const`：`this` 是 `const T*`，只能调 const 成员、不能改非 `mutable` 成员。逻辑 const：物理上改缓存，用 `mutable`。

往下追：`const` 在 `*` 左边修饰所指，右边修饰指针本身——读声明从右往左。`const` 引用 `const T&` 能绑右值，延长临时量寿命。顶层 const 在传值时被拷掉；底层 const（所指 const）会保留。`const_cast` 去掉 const 去写原本就是 const 的对象是 UB。

```cpp
void f(const int* p);       // 不能 *p = 1，能 p = q
void g(int* const p);       // 能 *p = 1，不能 p = q

struct C {
    int n = 0;
    mutable int hits = 0;
    int get() const { ++hits; return n; }   // 逻辑 const
};
```

再往下：`const` 成员函数可以调，非 const 对象也可以调 const 成员——const 是只读合同，不是「只能 const 对象调」。返回 `this` 的 const 重载：`T& f(); const T& f() const;`。`const` 在成员位置：`const int n;` 必须在初始化列表设。编译期常量用 `constexpr` / `const` 整数，数组大小在 C++ 里 `const int` 可以，C 里不行——这是 C/C++ 差异常被追。

```cpp
const int k = 4;
int a[k];                   // C++ 合法
void show(const std::vector<int>& xs);  // 不拷、不改
```

对照：指针篇 const 位置；对象模型篇 `this` 的类型。入门篇 const 接口合同。

---

### 11、static 有哪些用法？

能说出口：五处。函数内 `static` 局部：第一次经过时初始化，寿命到程序结束，C++11 起初始化有锁。文件作用域 `static`：内部链接，别的 TU 看不见。`static` 成员变量：全类一份，类外定义。`static` 成员函数：没有 `this`，只能碰静态成员。`static` 全局函数：内部链接。

往下追：跨 TU 的动态初始化顺序未指定（static initialization order fiasco）。函数内静态比全局安全——第一次调用才初始化，顺序跟调用走。别在静态析构阶段再碰已经死掉的另一个静态对象。`static` 成员函数不能是 `const` / `virtual`——没有 `this`，也没有动态类型。

```cpp
int counter() {
    static int n = 0;       // 线程安全地初始化一次
    return ++n;             // ++n 本身不是线程安全，多线程要 atomic 或锁
}
struct S {
    static int k;
    static void f();        // 无 this
};
int S::k = 0;
```

再往下：`static` 局部在多线程第一次进入时有锁，递归重入同一函数要小心死锁（初始化里再调自己）。`thread_local` 是每线程一份静态，别把地址交给别的线程还假设活着。静态成员的定义必须且只能一份，模板静态成员更易踩 ODR。`static_cast` 的 static 和这里无关——面试里偶尔混。

```cpp
// TU1
static int g = 1;           // 内部链接，TU2 可以再有一份同名
// 类外
// int S::k = 0;            // 只能出现一次，通常放 .cpp
```

对照：内存篇静态存储；对象模型篇静态成员不占对象空间。`counter` 的初始化安全 ≠ `++n` 安全，和 `shared_ptr` 那条缝同构。

---

### 12、四种 cast 各干什么？

能说出口：`static_cast` 相关类型之间、显式调用可转换，编译期查。`const_cast` 只动 const / volatile。`reinterpret_cast` 比特级重新解释，几乎只用于和 C API、整数与指针。`dynamic_cast` 沿继承图，要虚函数（RTTI），指针失败返回空，引用失败抛 `bad_cast`。C 式 `(T)x` 什么都能干，禁止。

往下追：下行转换有虚函数时用 `dynamic_cast`，确定类型用 `static_cast`（错了 UB）。`void*` 转回来用 `static_cast` 回到原类型，不是 `reinterpret_cast`。`enum class` 和整数之间要 `static_cast`。不要 `const_cast` 去写字面量、去写真 const 对象。

```cpp
Base* b = new Derived;
auto* d = dynamic_cast<Derived*>(b);    // 失败则空
int n = static_cast<int>(3.7);          // 3
auto p = static_cast<int*>(std::malloc(4));
const int c = 1;
// int* w = const_cast<int*>(&c); *w = 2;  // UB
```

再往下：`dynamic_cast` 要 RTTI，可关（`-fno-rtti`），关了这条废。交叉转换（非单一继承路径）只能 `dynamic_cast`。整数转指针、函数指针转数据指针，条件实现，可移植代码别赌。C 式转换能把 const 去掉还能改类型，所以禁——搜 `(T)` 比搜四种 cast 难做 code review。

```cpp
void* vp = &n;
int* ip = static_cast<int*>(vp);        // 回到原类型
auto bits = reinterpret_cast<std::uintptr_t>(ip);
```

对照：指针篇四种 cast。`reinterpret_cast` 不是「万能 static」。

---

### 13、一个 class 对象在内存里怎么排？

能说出口：非静态数据成员按声明顺序排，中间 padding 对齐，尾部再补齐给数组。`sizeof` 不是成员之和。`this` 不占对象空间。有虚函数多一个 vptr（常见在开头）。空 class `sizeof` 至少 1，保证地址唯一。静态成员不在对象里。

往下追：标准布局、POD、平凡，是三套不同的条件，别混。空基类优化可以把空基类的 1 字节吃掉。菱形非虚继承会有两份基类子对象。虚继承再多 vbptr。热数据挨着放，少跨 cache line。协议结构体不要随便 `packed`，未对齐访问有的架构 SIGBUS。

```cpp
struct A { char c; int n; };            // 常见 sizeof 8，c 后垫 3
struct B { int n; char c; };            // 仍是 8，尾部垫 3
class Empty {};                         // sizeof >= 1
```

再往下：虚函数个数不影响对象大小（仍一只 vptr），影响 vtable 大小。多重继承可能多只 vptr，`this` 调整在 thunk 里。标准布局要求：没有虚函数、没有虚基类、访问控制一致、没有非标准布局基类等——满足才能和 C struct 互传。`offsetof` 只对标准布局有良好定义。

```cpp
struct WithV {
    virtual void f();
    int n;
};
// sizeof 常见 16：vptr 8 + n 4 + 尾部垫 4
```

对照：对象模型篇第一段空 class。Go 的空 struct 是 0 大小，C++ 不是。

---

### 14、什么叫线程安全？shared_ptr 线程安全吗？

能说出口：同一对象并发访问，要么不冲突（只读、或各写各的不相交），要么有 happens-before（mutex、atomic、join）。否则 data race，**未定义行为**，不是「结果可能偏」。`shared_ptr` 的计数增减是线程安全的；`operator*` 出去是普通 `T`，不保护。同一份 `shared_ptr` 变量的读写要自己同步。

往下追：`use_count() == 1` 不能当锁。`vector` 的 `operator[]` 无锁。const 方法不是线程安全——`const` 挡的是这个线程的写，不是别的线程。每个共享普通对象要能指出保护者：哪把 mutex 或哪个 atomic。

```cpp
std::shared_ptr<int> g;
void writer() { g = std::make_shared<int>(1); }     // 写 g 本身
void reader() { if (g) std::cout << *g; }           // 无同步读 g：UB
```

再往下：`const` 方法不是线程安全。`thread` 析构时若仍 joinable，`terminate`。`join` 是同步点：子线程写过的普通对象，join 之后主线程读不是 race。`detach` 后不能碰即将销毁的栈对象。TSan（`-fsanitize=thread`）抓 race，比「我看了反汇编是一条 inc」有用。

```cpp
int x = 0;
void f() { x = 1; }
std::thread t(f);
t.join();
std::cout << x << "\n";     // 1，定义良好
```

对照：智能指针篇第一节；并发篇 data race 定义。对象池：链表安全不是 `Packet` 字段安全。

---

### 15、memory_order 至少能说出哪几档？

能说出口：`seq_cst` 默认，全局全序，好懂，略贵。`acquire` 读：之后的读写不能排到这次读之前。`release` 写：之前的读写不能排到这次写之后。配对后，release 之前的写入对 acquire 之后的读取可见。`relaxed` 只保证这个原子对象的原子性，不发布别的内存。`acq_rel` 是 RMW 上同时两档。

往下追：计数、统计用 `relaxed`。发布指针 / flag 用 release，对端 acquire。不要单边 relaxed 当发布协议——x86 上小程序可能「看起来对」，ARM 上炸。`volatile` 不是其中任何一档。默认写 `atomic<int>` 不标序就是 seq_cst，先正确再抠。

```cpp
int data = 0;
std::atomic<bool> ready{false};
void pub() {
    data = 42;
    ready.store(true, std::memory_order_release);
}
void sub() {
    while (!ready.load(std::memory_order_acquire)) {}
    assert(data == 42);     // 定义良好
}
```

再往下：`memory_order_consume` 几乎没人用，编译器当 acquire。false sharing 是性能不是 UB：两个 `atomic<int>` 挤一行缓存会慢，仍定义良好。读-改-写用 `fetch_add` / `compare_exchange_weak`，不要先 `load` 再 `store` 当 CAS。seq_cst 的全局全序让所有原子操作排成一条，跨变量推理简单，热路径再降。

```cpp
std::atomic<int> hits{0};
hits.fetch_add(1, std::memory_order_relaxed);   // 只计数
```

对照：并发篇四档。`fetch_add(..., relaxed)` 适合 hits 计数。对象池统计 acquire 次数可以 relaxed。

---

### 16、lambda 是什么？捕获按值还是按引用？

能说出口：lambda 是编译器生成的函数对象，`operator()` 里是函数体。`[]` 不捕获；`[=]` 按值；`[&]` 按引用；可以写 `[=, &x]` / `[a, &b]`。无捕获可转函数指针。`mutable` 才允许改按值捕获的副本。泛型 lambda：`[](auto x)`。

往下追：`[&]` 捕获局部，lambda 活过那个栈帧就是悬空。异步、存进 `std::function`、交给别的线程，用按值或 `shared_ptr`。`[=]` 捕获的是当时的值，不是后来改过的局部。`this` 按值捕获的是指针，对象死了同样悬——C++17 有 `*this` 按值捕获对象副本。

```cpp
int n = 1;
auto a = [n] { return n; };             // 副本
auto b = [&n] { return n; };            // 别名
auto c = [n]() mutable { return ++n; }; // 改副本
std::thread t([n] { /* n 的副本，安全 */ });
t.join();
```

再往下：`std::function` 能装 lambda，有类型擦除和可能的堆分配；小闭包可能 SBO。无捕获 lambda 是平凡函数对象，适合当删除器走 EBO。返回 lambda 时不要 `[&]` 捕获局部。泛型 lambda 本质是模板 `operator()`。C++17 起 lambda 可 `constexpr`（满足条件时）。

```cpp
auto add = [](auto a, auto b) { return a + b; };
auto p = std::make_unique<int>(1);
std::thread t([p = std::move(p)] { (void)*p; });    // 所有权进闭包
t.join();
```

对照：现代 C++ 篇 lambda 把所有权关进闭包。对象池测试里 `[&]` 捕获 `pool` 合法——`join` 前 `pool` 活着。

---

### 17、移动语义解决什么问题？和拷贝差在哪？

能说出口：拷贝是再造一份资源；移动是把资源指针转手，源置空。容器扩容、函数返回、`unique_ptr` 交棒，都可以不分配。类型要提供移动构造 / 移动赋值，且最好 `noexcept`。

往下追：不是「更快的拷贝」——有的类型移动和拷贝一样贵（`array<int, 10000>`）。`std::move` 只是转换。返回局部不要 `move`。强制移动一个还要用的对象是逻辑错误，不是性能技巧。`auto_ptr` 用拷贝语义做移动，已废；`unique_ptr` 才是焊在类型上的独占。

```cpp
std::vector<std::string> v;
v.emplace_back("hello");
auto w = std::move(v);      // v 通常空，缓冲在 w
```

再往下：NRVO / C++17 强制性复制省略让「我写了移动构造，返回一定会被调用」更不真。移动构造仍必须正确：省略失败、放进容器、`swap` 都会走。`vector` 扩容：移动 `noexcept` 才敢用，否则可能拷。`string` 短字符串优化下，短串移动可能仍拷字符，长串才转手堆缓冲。

```cpp
struct Holder {
    std::string s;
    std::string take() { return std::move(s); }  // 成员必须 move
};
```

对照：现代 C++ 篇第一节。五法则那题的 `Buf` 就是手写移动。

---

### 18、空 class 的 sizeof 为什么不是 0？

能说出口：完整对象要有独一无二的地址，`Empty a, b;` 必须 `&a != &b`。`Empty a[10]` 指针算术要能区分元素。编译器塞至少 1 字节占位。有了非静态成员，这 1 字节通常被成员吃掉。

往下追：空基类子对象可以是 0 大小（EBO），所以 `unique_ptr` 默认删除器是空 class，宽度仍是一个指针。Go 的空 struct 是 0，允许不同变量同地址。C++ 选了对象有身份。`new Empty[n]` 仍然按 `sizeof` 步长走。

```cpp
class Empty {};
static_assert(sizeof(Empty) >= 1);
Empty a, b;
assert(&a != &b);
```

再往下：空 class 作为成员仍占 1，两个空成员各 1。空基类才可能被 EBO 吃成 0。`unique_ptr<T, default_delete<T>>` 能和裸指针一样宽，靠的就是删除器当空基类。有状态删除器（对象池那只握着 `ObjectPool*`）吃不掉，宽度两只指针。

```cpp
struct E1 {};
struct E2 {};
struct Two { E1 a; E2 b; };             // sizeof 常见 2，不是 0
struct Inherit : E1, E2 {};             // 可能 1，EBO
```

对照：对象模型篇开篇。别把 `sizeof` 当「有效数据多少字节」。对象池 `Ptr` 有状态删除器，不指望 EBO。

---

### 19、菱形继承有什么问题？虚继承怎么解？

能说出口：`D` 继承 `B1`、`B2`，二者又都继承 `A`，`D` 里有两份 `A`：二义性、两份状态。虚继承让 `A` 在 `D` 里只有一份，由最派生类初始化这个虚基类。代价：多 vbptr、构造规则变复杂、访问多一次间接。

往下追：接口（无状态）菱形可以虚继承；有状态的菱形优先组合。虚析构仍然要。构造顺序：虚基类先于普通基类。不要为了「省一份」把所有继承改虚——布局和调用都变。

```cpp
struct A { int n; };
struct B1 : virtual A {};
struct B2 : virtual A {};
struct D : B1, B2 {};       // 一份 A，D 负责初始化 A
```

再往下：非虚菱形 `d.n` 二义，要 `d.B1::n`。两份 `A` 各有各的状态，改一份另一份不动——这常常是 bug 而不是特性。虚继承的 `A` 由 `D` 初始化，`B1` / `B2` 的构造函数里给 `A` 传的参数会被忽略。接口纯虚 + 虚继承是 COM / IUnknown 那条老路；新代码优先组合。

```cpp
struct A { int n = 0; };
struct B1 : A {};
struct B2 : A {};
struct D : B1, B2 {};
D d;
// d.n = 1;                 // 二义
d.B1::n = 1;
d.B2::n = 2;                // 两份
```

对照：虚函数篇菱形。Python 有 MRO；Go 没有继承菱形，接口组合。

---

### 20、volatile 是不是原子？能不能当锁？

能说出口：**不是。** `volatile` 阻止的是「对这个对象的访问被编译器优化掉」，给 MMIO、信号处理用。不保证原子、不防撕、不提供跨线程 happens-before，别的非 volatile 访问仍可重排过它。线程间共享用 `std::atomic` 或 mutex。

往下追：`volatile int x; ++x;` 两个线程仍是 data race。Java 的 `volatile` 有内存语义，C++ 没有对等物——对等物是 `atomic` 的 acquire/release。`const_cast` 去掉 volatile 去访问寄存器映射，也要知道自己在干什么。嵌入式读硬件寄存器用 `volatile`；网络包计数用 `atomic`。

```cpp
volatile int x = 0;
void f() { ++x; }           // 两个线程调 f：仍是 UB
std::atomic<int> y{0};
void g() { y.fetch_add(1); } // 定义良好
```

再往下：信号处理器里只碰 `volatile sig_atomic_t` 或无锁 atomic，仍不是随便什么 `volatile`。编译器对 `volatile` 的读写不做的是「合并 / 删掉」，做的仍是按抽象机访问那个对象——和别的对象之间没有栅栏。`atomic` 的 `volatile` 成员函数是另一回事（禁止某些优化），面试别往那拐。

```cpp
std::atomic<int> y{0};
void g() { y.fetch_add(1); }            // 定义良好
// volatile 的正确场合：
// volatile uint32_t* reg = ...;        // MMIO
// *reg = 1;                            // 必须真的写出
```

对照：并发篇「++ 为什么不是原子」。别把 Java 记忆搬进 C++。

---

### 21、编译和链接各做什么？模板为什么要写在头文件？

能说出口：预处理 → 编译成目标文件（每个 TU）→ 链接把符号收成可执行文件 / 库。声明让编译器知道名字和类型；定义提供实体。ODR：跨 TU 只能有一个定义（inline / template 除外）。未定义引用是链接错误；重复定义也是。

往下追：模板在调用点实例化，编译器必须看见定义，所以放头文件。`inline` 函数同样。`static` 内部链接，各 TU 一份，不会冲突也不会共享。声明在 `.h`、定义在 `.cpp` 是普通函数的默认。C++17 `inline` 变量让头文件里的全局有定义也不重复。

```cpp
// a.h
void f();                   // 声明
template <typename T>
T id(T x) { return x; }     // 模板：定义就在头里

// a.cpp
void f() {}                 // 唯一定义

// b.cpp
#include "a.h"
void g() { f(); id(1); }    // 链接到 f；id<int> 在本 TU 实例化
```

再往下：未定义引用（undefined reference）是链接期，常见原因：声明了没定义、模板定义没被看见、`.cpp` 没进工程、`inline` 忘了导致多定义或没定义。重复定义：头文件里写了非 inline 的非模板函数。名字修饰：C++ 把参数类型编进符号，C 不编，所以链 C 库要 `extern "C"`。编译错误看类型，链接错误看符号。

```cpp
extern "C" int c_api(int);              // 按 C 符号去链
// 头文件里：
// inline int add(int a, int b) { return a + b; }  // 可以
// int add(int a, int b) { return a + b; }         // 多 TU include 就重复定义
```

对照：入门篇翻译单元。链接错误看的是 mangled 名字，`extern "C"` 关掉修饰才能链 C。对象池模板全部在头文件，同一个原因。

---

### 22、内存泄漏怎么查？常见原因有哪些？

能说出口：泄漏是堆上对象没有释放路径。查：AddressSanitizer（`-fsanitize=address`，开发默认）、Valgrind `memcheck`、heap profiler。修：RAII，不要裸 `new`；环用 `weak_ptr`；容器清掉；静态缓存设上限。

往下追：ASAN 抓 UAF、double free、部分泄漏；环导致的泄漏要看 `use_count` 出了作用域仍不是 0，或用泄漏检测。`unique_ptr` 自定义 deleter 还池不是泄漏——对象故意活在池里，池析构才 `delete`。忘记 `virtual` 析构是派生部分漏。`vector` 里 `T*` 是假 RAII，指针值被管，`T` 不管。

```cpp
// 环
struct Node {
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;           // 回边弱，不成环
};
// 查
// g++ -std=c++17 -fsanitize=address -O1 -g a.cpp && ./a.out
```

再往下：泄漏和 UAF 不是一类。泄漏是活对象没人放；UAF 是放了还碰。ASAN 对后者更响。`new`/`delete` 不配、`malloc`/`free` 混 `delete`，ASAN 也能打。Valgrind 慢但不必重编译。生产用 heap profiler 看增长曲线，比等 OOM 早。静态分析（clang-tidy `uninitialized`、所有权注解）补动态检测的窗。

```cpp
std::vector<std::unique_ptr<Node>> xs;  // 真 RAII
std::vector<Node*> ys;                  // 假 RAII：清 ys 不 ~Node
```

对照：内存篇检查清单；智能指针篇环。对象池 `owned_` 里的对象到池死才释放，ASAN 不应报——那是所有权设计，不是漏。

---

### 23、智能指针能当锁吗？mutex 怎么焊进 RAII？

能说出口：不能。智能指针管寿命，不管互斥。锁用 `std::lock_guard` / `unique_lock` / C++17 `scoped_lock`（多把锁，防死锁顺序）。`lock_guard` 构造加锁、析构解锁，异常也放。`unique_lock` 能 `unlock` / 交给条件变量。

往下追：同一把锁护住的数据要写清，不要一把进程大锁。`std::mutex` 不能拷贝、不能移动。条件变量 `wait` 必须带谓词，防止虚假唤醒和通知丢失。`lock()` / `unlock()` 手写配对，和手写 `new`/`delete` 同一类事故。

```cpp
std::mutex mu;
int n = 0;
void bump() {
    std::lock_guard<std::mutex> g(mu);
    ++n;
}
```

再往下：多把锁用 `std::scoped_lock(a, b)`，C++17 一次拿齐，内部 `std::lock` 防死锁。自己按地址排序也能，漏一次就循环等待。`recursive_mutex` 能同线程重入，掩盖「这把锁到底护谁」的设计问题，默认不用。条件变量：改条件、`notify`、`wait` 谓词，三件都在同一把锁的合同里。

```cpp
std::mutex m1, m2;
{
    std::scoped_lock g(m1, m2);         // C++17，一次拿两把
    // ...
}
std::condition_variable cv;
std::unique_lock<std::mutex> lk(mu);
cv.wait(lk, [&] { return ready; });     // 谓词，防虚假唤醒
```

对照：并发篇 mutex 焊进 RAII。对象池那把锁只罩空闲链表，不罩在外的 `T`。

---

### 24、为什么有时候要写虚析构，有时候写成 protected 非虚？

能说出口：堆上经基类指针删除 → 虚析构。不想让任何人经基类删除 → 基类析构 `protected` 且非虚，禁止 `delete` 基类指针，派生可以自己析构。值语义的基类（切片本来就会发生）不要虚析构凑热闹——虚函数会加 vptr，布局变了。

往下追：`unique_ptr<Base>` 删除时调 `Base` 的删除器，基类析构仍必须虚（或自定义删除器知道派生类型）。`shared_ptr<Base>` 从 `shared_ptr<Derived>` 转换时，控制块记下了 `Derived` 的删除器，**即使基类析构非虚也能正确销毁**——这是 `shared_ptr` 的类型擦除，不是语言规则，不要拿这个当「可以不写虚析构」的理由，换回 `unique_ptr` 就炸。

```cpp
struct Base { virtual ~Base() = default; };
std::unique_ptr<Base> p = std::make_unique<Derived>();
// ~unique_ptr → delete Base* → 必须虚析构
```

再往下：工厂返回 `unique_ptr<Base>`，类型已经决定删除走 `Base*`，虚析构是合同不是风格。值语义（`struct Color { int r,g,b; }`）不要继承体系，更不要虚。mixin / CRTP 静态多态没有 vptr，也不走虚析构这条。面试里「所有基类都要虚析构」是错的——先问有没有经基类指针 `delete`。

```cpp
struct Mixin {
protected:
    ~Mixin() = default;                 // 不能经 Mixin* 删除
};
struct Widget : Mixin {};               // ~Widget 可以，~Mixin 对用户不可达
```

对照：虚函数篇虚析构；智能指针篇删除器。接口当基类，默认虚析构。

---

检查清单：每题先一句合同（空不空、谁拥有、有没有 happens-before），再一句实现（vptr、控制块、三指针），再一句事故（UAF、double free、data race）。追问往「写了析构之后拷贝怎么办」「`move` 之后还能用吗」「这把锁护的是哪块内存」走，不往背诵关键字走。基础四篇钉盒子和所有权，进阶五篇钉容器、虚、并发、现代语法；本篇把同一根轴收成能说出口的句子。对象池是这根轴的一次应用：`unique_ptr` + 删除器 + mutex 边界，面试里被追「你写过对象池吗」就按那篇的非目标往下讲——不是 allocator。
