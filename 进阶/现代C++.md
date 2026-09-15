# 现代C++

C++11 之后人们爱列特性：右值引用、lambda、`auto`、`constexpr`、范围 for。清单会过时，所有权语义不会。移动把「拷一份」变成「把缓冲偷走，源掏空」；`unique_ptr` 把独占焊进类型；lambda 能捕获、能当函数对象；`optional` 把「可能没有」从指针和魔数里拆出来。入门篇的 `auto` 是推导规则，指针篇的移动是值类别，对象模型的五法则是特殊成员，内存篇的 RAII 是析构。本篇把这些收成一件事：**谁拥有、何时结束、编译期能钉死多少**。C++17 是默认；C++20 的 concepts / ranges / coroutine 点到为止——它们仍是同一条所有权轴上的刻度，不是新语言。

Python 没有移动：名字绑定，对象在堆上，RC 到 0 再死。Go 没有移动语义关键字，赋值拷切片头或整个值，逃逸到堆交给 GC。C++ 把「将亡」写进类型系统，于是容器扩容、函数返回、工厂交棒，都可以不分配。新标准的第一课不是「有哪些新关键字」，是「值类别开始驱动资源转手」。

```cpp
#include <string>
#include <utility>
#include <vector>

std::vector<std::string> make() {
    std::vector<std::string> v;
    v.emplace_back("hello");
    return v;                           // 移动或 NRVO，不是拷 那块缓冲
}

int main() {
    auto v = make();
    auto w = std::move(v);              // v 合法但空；缓冲在 w
    // v.size() 通常 0；不要再用 v 里的元素
}
```

`std::move` 不移动。它是把左值转成将亡值，让重载选中移动构造。真的转手发生在 `vector` 的移动构造里：三个指针拷过去，源置空。指针篇写过动机；这里往下挖：什么时候必须手写移动、什么时候 `return std::move(x)` 是错的、lambda 怎么把所有权关进闭包、`optional` / `variant` 怎么把联合所有权表达成值。

---

## 一、移动：所有权转手，不是更快的拷贝

### 1、将亡值选中移动重载

```cpp
struct Buf {
    int* p;
    std::size_t n;
    Buf() : p(nullptr), n(0) {}
    explicit Buf(std::size_t n) : p(new int[n]{}), n(n) {}
    ~Buf() { delete[] p; }
    Buf(const Buf& o) : Buf(o.n) {
        std::copy(o.p, o.p + n, p);
    }
    Buf(Buf&& o) noexcept : p(o.p), n(o.n) {
        o.p = nullptr;
        o.n = 0;
    }
    Buf& operator=(Buf o) noexcept {    // 按值：拷或移进 o，再 swap
        std::swap(p, o.p);
        std::swap(n, o.n);
        return *this;
    }
};
```

拷贝：新缓冲，逐元素拷。移动：指针转手，源可析构（`delete[] nullptr` 合法）。`noexcept` 让 `vector<Buf>` 扩容走移动；没有 `noexcept`，扩容可能拷，对象模型篇写过。按值赋值 `operator=(Buf o)` 一份代码吃拷贝和移动：左值进来拷进 `o`，右值移进 `o`，再 swap。强异常保证来自「先造出 `o` 再碰 `this`」。

`std::move(x)` 之后 `x` 进入有效但未指定状态。`vector` 通常空，`string` 通常空，内置类型原值还在——不要依赖。只保证：可析构、可赋值。`x = something;` 让它重新有值。循环里 `sink(std::move(x)); use(x);` 是在用掏空的对象。

### 2、return 不要 move 局部，要 move 成员和形参

```cpp
Buf make() {
    Buf b(100);
    return b;                           // NRVO 或隐式当将亡值
}

Buf forward_on(Buf b) {                 // 形参不是返回槽
    return b;                           // C++11 起这里走移动；C++17 仍建议直接 return b
}

struct Holder {
    Buf b;
    Buf take() { return std::move(b); } // 成员：必须 move，否则拷
};
```

`return std::move(b);` 对**局部对象**常阻止 NRVO：编译器看到的是「把左值转成右值再构造返回槽」，不再把 `b` 和返回槽当成同一块。局部直接 `return b;`。形参已经不可能是返回槽，`return b;` 仍会移（C++11 对返回的局部和形参允许当将亡值）。成员、`*this` 的子对象、`std::move` 才必要。

C++17 强制性复制省略：纯右值初始化，构造可以完全不跑，连移动都不。`Buf b = make();` 若 `make` 里 NRVO 失败，17 仍可能直接在 `b` 里造。所以「我写了移动构造，返回一定会被调用」在 17 下更不真。移动构造仍必须正确：省略失败、放进容器、`swap` 路径都会走它。

### 3、移动只转手资源，切片和多态

`unique_ptr`、`vector`、`string`、`optional` 都会移动。多态对象按值移仍切片：移的是基类那一段，vptr 按目标类型设。要转手派生对象，移 `unique_ptr<Base>`。`auto_ptr` 在 C++17 **已移除**——它拷贝即移，容器里是炸弹。只认 `unique_ptr`。

被移走的 `unique_ptr` 是空，`get() == nullptr`。被移走的 `shared_ptr` 空，计数不碰（移动不加减）。被移走的 `vector` 空，capacity 0（通常）。不要 `move` 之后还 `push_back` 到源上当它仍是原来那个缓冲——源是另一个合法空容器。

`const T` 不能移：`const` 对象选不中 `T&&` 重载，只能拷。`const unique_ptr<T>` 废了独占转手。成员 `const std::string` 让整个类的移动变成拷贝字符串。要 const 接口，用 const 成员函数，别把成员本身钉 const，除非你真的永远不转手。

返回 `const T` 同样挡移动（C++17 前更疼）。新代码不要返回 `const` 值。只读用 `const T&` 或 `T` 由调用方自己 const。

---

## 二、lambda：函数对象，捕获是成员

### 1、闭包类型是独一无二的 class

```cpp
int k = 1;
auto f = [k](int x) { return x + k; };  // 按值捕获，f 里有一份 k
auto g = [&k](int x) { return x + k; }; // 按引用捕获，g 里是 k 的引用
```

`f` 的类型只有编译器知道，每次写一个 lambda 一个类型。`operator()` 默认 const——不能改按值捕获的副本，除非 `mutable`。按值捕获的是**当时**的 `k`；按引用捕获的是对象本身，lambda 活过 `k` 就是悬空。内存篇的 UAF，闭包版。

```cpp
auto make_fn() {
    int n = 1;
    return [&n] { return n; };          // 炸弹：n 已死
}
```

`[&]` 全引用、`[=]` 全按值、C++14 `[=, &x]` 混合、C++14 初始化捕获 `[u = std::move(ptr)]`。默认写明捕获列表，不要 `[=]` 把整个作用域按值拷一份还以为便宜——`[=]` 拷的是用到的自动变量，成员仍通过 `this` 指针进来（C++17 的 `[=]` 捕获 `this` 是指针，不是对象）。`[*this]` C++17：按值拷当前对象进闭包。异步、存 lambda 超过当前帧，用 `[*this]` 或把需要的成员按值拆出来，不要 `[&]` 或隐含 `this`。

```cpp
struct Svc {
    std::string name;
    auto bound() {
        return [*this] { return name; };    // 拷 Svc；svc 死了仍安全
    }
    auto dangling() {
        return [&] { return name; };        // 绑 *this，svc 死则悬
    }
};
```

### 2、所有权进闭包：move 捕获

```cpp
auto p = std::make_unique<int>(1);
auto f = [p = std::move(p)] { return *p; };  // C++14 初始化捕获
// p 已空；f 独占那个 int
```

`std::function<int()>` 要求闭包可拷贝。`unique_ptr` 不可拷，装不进 `function`。要类型擦除又独占：自己写 move-only 包装，或 C++23 `move_only_function`（本篇点到）。C++17 路径：能不用 `function` 就模板形参 `F&&`，或 `unique_ptr` 不进 `function`。`function` 还可能堆分配，虚调用一次。热路径、短 lambda 当算法谓词，直接模板。

```cpp
template<class F>
void for_each_item(F f) {
    for (auto& x : items_) f(x);
}

// std::function<void(Item&)> 作为成员存回调：所有权清楚，税也清楚
```

### 3、泛型 lambda、constexpr、作为比较器

C++14 `[](auto x) { return x; }` 是模板 `operator()`。C++17 允许 `constexpr` lambda（隐式，若函数体满足）。用来：`std::sort(v.begin(), v.end(), [](const auto& a, const auto& b) { return a.id < b.id; });`。不要写函数指针再配 `qsort`，类型和内联都差一截。

捕获 `this` 的 lambda 当虚函数里的回调丢给工作线程：`this` 指向的对象必须还活着。`enable_shared_from_this` + `[self = shared_from_this()]` 把寿命延到闭包死。智能指针篇的 `shared_from_this` 主场之一就是异步 lambda。不要 `[this]` 丢进 `std::thread` 然后函数返回。

lambda 没有默认的 `operator==`（C++20 无捕获的可以比）。当 map 的 key 更别扭。它是函数对象，不是闭包在 Python 意义上的「第一等对象带身份」——Python 的 lambda 是堆上函数对象，捕获是 cell；C++ 的 lambda 常在栈上，捕获是成员，拷贝闭包就是拷那些成员。

---

## 三、auto、decltype、结构化绑定：类型还在，名字变短

### 1、auto 丢掉顶层 const 和引用

入门篇写过推导。这里钉工程选择：

```cpp
std::vector<int> v{1, 2, 3};
auto a = v[0];                          // int，拷一份
auto& b = v[0];                         // int&
const auto& c = v[0];
auto&& d = v[0];                        // int&（左值折叠）
auto&& e = 1;                           // int&&
```

`auto` 不是 Python 的动态类型。循环里 `for (auto x : v)` 拷每个元素；`for (auto& x : v)` 改元素；`for (const auto& x : v)` 只读不拷。`vector<bool>` 的 `auto&` 会编译失败或绑到代理——又一条不用 `vector<bool>` 的理由。`for (auto&& x : v)` 万能，代理也能绑。

返回类型 `auto f();` 要能从 return 语句推导，多条 return 类型得一致。接口、头文件、ABI 边界把类型写出来：`auto` 满天飞的成员类型让人读不懂，改实现推导一变，调用方一起炸。局部、循环、迭代器 `auto it = v.begin()` 合适。

`decltype(auto)` 按 `decltype` 规则保留引用：`decltype(auto) f() { return x; }` 可能是引用，悬不悬看 `x`。比 `auto` 更容易把引用漏出函数。默认 `auto`，真要转发返回才 `decltype(auto)`。

### 2、结构化绑定是名字，不是独立对象的拷

```cpp
std::map<std::string, int> m{{"a", 1}};
for (const auto& [k, v] : m) {          // C++17
    // k 是 const string&，v 是 const int&
}

auto [x, y] = std::pair<int, int>{1, 2}; // x、y 是拷进匿名对象里的成员的别名
auto& [a, b] = *m.begin();              // 绑到 pair 的成员
```

`auto [x, y] = expr;` 先用 `auto` 规则造一个匿名对象（拷或移），再让 `x`、`y` 当它的成员别名。`auto& [x, y] = expr;` 绑到左值。`const auto&` 只读。元组、pair、数组、有 `tuple_size` 的聚合都能拆。

```cpp
struct Point { int x, y; };
Point p{1, 2};
auto [a, b] = p;                        // 拷 Point，a、b 是副本的成员
p.x = 9;
// a 仍是 1
```

不要以为结构化绑定是 Python 的解包再绑两个名字到原对象——默认是先拷（或移）再起别名。要别名加 `&`。`if (auto [it, ok] = m.insert({k, v}); ok)` C++17 if 初始化 + 绑定，`ok` 是 `bool`。

### 3、CTAD、using、别名

C++17 类模板实参推导：`std::pair p{1, 2.0};`、`std::vector v{1, 2, 3};`、`std::lock_guard lock(mu);`。`vector v{v2}` 可能推成 `vector<vector<int>>` 一个元素——用 `vector v(v2)` 或写明类型。`optional o{1}` 推 `optional<int>`。

`using Ptr = std::unique_ptr<Node>;` 比 `typedef` 干净，可模板：`template<class T> using Vec = std::vector<T>;`。现代代码 `using`，不 `typedef`。

---

## 四、optional、variant、any：值里的「没有」和「之一」

### 1、optional：可能有一个 T

```cpp
#include <optional>

std::optional<int> parse(const std::string& s) {
    if (s.empty()) return std::nullopt;
    return std::stoi(s);
}

if (auto n = parse(t)) {
    use(*n);
}
```

`optional<T>` 是值：有或没有，有则原地（或实现允许的存储）放一个 `T`。不是堆上的 `T*`。`*o` / `o.value()` 在空时：`*` 是 UB，`value()` 抛 `bad_optional_access`。`value_or(0)` 空则返回 0。`bool(o)` 是否有值。

不要用 `optional<T&>`——C++17 标准 `optional` 不支持引用。观察用指针。`optional<T>` 当返回值：比魔数 `-1`、比「抛或返回 0」、比输出参数清楚。比 `unique_ptr<T>` 轻：没有堆，没有独占堆对象的语义，只是「可能没有这个值」。

`optional<string>` 空不构造 string；有值才构造。移动 `optional` 是移里面的 `T`（若有）。比较：空小于任何有值（有序）。热路径上 `optional` 多一个旗，别滥用成每个成员都 optional——那是在说「我的不变式不清楚」。

Python 的 `None`、Go 的 `, ok` 和指针零值，对应三套习惯。C++ 的 `optional` 把 ok 和值焊成一个对象，且不占用 `T` 的合法值域（不像用 `-1` 当空）。`T*` 可空但所有权含糊；`optional<T>` 所有权就是这个 optional 对象。

### 2、variant：闭集之和类型

```cpp
#include <variant>

using Node = std::variant<int, std::string>;

Node n = 1;
n = std::string{"hi"};

std::visit([](const auto& x) {
    // x 是 int 或 string，if constexpr 可分支
}, n);

if (auto* s = std::get_if<std::string>(&n)) {
    use(*s);
}
```

`variant<A, B, C>` 同一时刻一个，大小约 max(sizeof) + 判别器 + padding。没有堆（除非备选类型自己堆）。`get<I>(v)` 类型不对抛 `bad_variant_access`。`get_if` 返回指针，不对则空。`index()` 当前备选。`visit` 是闭集分发：加类型必须改 variant 和所有 visit——和虚函数开集相反。虚函数篇点过这根轴。

`variant` 不能有引用、不能有数组、不能有 void。空 `variant` 只有 `monostate` 当第一个类型表示「空」。赋值若抛，C++17 可能进入 `valueless_by_exception` 状态——要处理，或只放不抛的类型。移动、拷贝按当前备选走。

不要 `union` 手写判别器，除非和 C ABI 布局绑定。`variant` 就是带类型的 union + 析构对的那一个。Python 没有原生 sum type，常用 union 注解（运行时不挡）；Go 用接口或 `switch t := x.(type)`，开集。C++ `variant` 是编译期闭集。

### 3、any：开集擦除，有堆

`std::any` 可持任意可拷贝类型，类型擦除，常常堆分配。`any_cast<T>` 失败抛或返回指针。比 `void*` + 类型标签安全，比 `variant` 重，比虚接口更「什么都能装」。默认不要。配置、插件边界、真正不知道类型时才用。`any` 要求可拷贝，`unique_ptr` 装不进去。

`function` 是「可调用的 any」。同一类税。能模板就模板。

---

## 五、constexpr 进化：从常量表达式到编译期算法

### 1、C++11 / 14 / 17 能写什么

入门篇：`constexpr` 函数可以编译期求值。C++11 几乎只能一条 `return`。C++14 放开局部变量、循环、多语句。C++17：lambda 能 constexpr，`if constexpr` 编译期丢分支，`constexpr` 的 `static_assert` 更常见。

```cpp
constexpr int pow2(int n) {
    int r = 1;
    for (int i = 0; i < n; ++i) r *= 2;
    return r;
}

constexpr int k = pow2(4);              // 16，编译期
char buf[k];                            // 合法

template<class T>
void dump(T x) {
    if constexpr (std::is_pointer_v<T>) {
        std::cout << *x;
    } else {
        std::cout << x;
    }
}
```

`if constexpr` 假分支不实例化——模板里对「这个 T 没有的成员」不会去实例化那一侧。普通 `if` 两个分支都要合法。这是 17 写泛型代码的刀，不是运行时优化开关（运行时 `if` 编译器本来就会折常量）。

`constexpr` 构造让类型能当字面量类型：编译期对象、`std::array` 的编译期填、查找表。C++17 `std::optional`、`string_view` 部分 constexpr；`vector` 的 constexpr 是 C++20。不要指望 17 下在编译期 `push_back`。能 `constexpr` 的：整数计算、位运算、查表、`array`。

### 2、const vs constexpr vs consteval

`const int n = 1;` 运行时对象，只读。`constexpr int n = 1;` 要求常量初始化，能当模板实参。C++20 `consteval` 必须编译期（本篇点到）。`constinit` 保证静态初始化、不要求对象 const。入门篇钉过 const 合同；这里钉：能编译期算的表不要启动时算，省静态初始化顺序和启动时间。

`constexpr` 函数仍可运行时调用。参数不是常量表达式，就运行时跑。不是「这个函数永远零开销」，是「允许在常量上下文里求值」。过大的编译期递归会把编译打爆。编译期是另一台机器，账单是编译时间和错误信息。

### 3、string_view：观察，不拥有

```cpp
#include <string_view>

void f(std::string_view s);             // 不拥有，不拷贝缓冲

f("literal");                           // 好
f(std::string{"tmp"});                  // 临时 string 活过整个调用，好
std::string_view p = std::string{"tmp"}; // 悬：完整表达式末尾 string 死
```

C++17 `string_view` 是指针 + 长度，不保证以 `\0` 结尾。`data()` 给 C API 要小心。拥有仍用 `string`。函数只读文本：`string_view` 比 `const string&` 更能接字面量和子串，且不强制对方已经是 `string`。存成员、返回、跨调用：要么拥有 `string`，要么保证源活着。STL 篇的失效，view 版。

`string_view` 的切片 `sv.substr(1, 3)` 不分配。和 Go 的切片头类似——观察底层字节。Go 的 `string` 不可变且共享；C++ `string` 可变，`string_view` 不跟踪源的失效。Python `s[1:3]` 通常新对象（3.x 有 intern / 共享，但语义是新 str）。不要拿 Python 切片当 `string_view` 的寿命模型。

---

## 六、其它 C++17 日常：if 初始化、枚举、属性、文件系统点到

### 1、if / switch 初始化、结构化 if

```cpp
if (auto it = m.find(k); it != m.end()) {
    use(it->second);
}                                       // it 只在 if / else 里

if (auto o = parse(s); o) use(*o);
```

缩小作用域，避免 `it` 漏到后面被误用。`switch (int n = f(); n)` 同理。和结构化绑定叠：`if (auto [it, ok] = m.insert(e); ok)`。

### 2、枚举 class、using enum 没有（20 才有）

`enum class Color { Red, Blue };` C++11，不隐式转 int，作用域在 `Color::`。新代码不用无作用域 `enum`。底层类型 `enum class E : std::uint8_t`。这不是 17 新特性，但是现代默认。C++17 允许枚举直接列表初始化 `Color c{0};` 仍要小心。

### 3、[[nodiscard]]、[[maybe_unused]]、[[fallthrough]]

```cpp
[[nodiscard]] std::unique_ptr<T> make();
auto p = make();                        // 必须接；丢掉所有权的警告

switch (s) {
    case 0:
        f();
        [[fallthrough]];
    case 1:
        g();
        break;
}
```

`nodiscard` 焊在工厂、错误码上。C++17 属性是给人和编译器的合同，不是运行时。`maybe_unused` 挡未使用警告。不要用 `(void)x` 当现代写法，属性更准。

`inline` 变量：对象模型篇静态成员。C++17 `inline` 变量让头文件里的全局合并成一份。模板变量同理。

`std::byte`：既不是 `char` 也不是 `int`，用来表示字节，不开算术。缓冲 `vector<std::byte>`。文件系统 `std::filesystem` C++17 有，错误用 `error_code` 或抛 `filesystem_error`。点到：路径用 `path`，不要自己拼斜杠；存在性检查和 TOCTOU 仍是竞态，文件系统 API 不消灭它。

`std::apply`、`std::invoke`、折叠表达式：模板元编程日常。`(args && ...)` 把包折起来。写库才深挖；应用代码用 `make_index_sequence` 的次数应当低。

`std::as_const` 把左值转成 `const` 视图，避免 `const_cast` 反向用。`std::exchange(obj, val)` 把 `obj` 换成 `val` 并返回旧值，移动赋值里掏空源常用。`std::clamp`、`std::gcd` / `lcm`、`std::optional` 的比较，都是 17 的小函数，用上比手写少出错。

---

## 七、chrono、tuple、initializer_list：值语义的边角

### 1、chrono 是类型，不是整数时间戳

```cpp
#include <chrono>
using clock = std::chrono::steady_clock;
auto t0 = clock::now();
work();
auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - t0);
```

`system_clock` 可调、可倒退，墙钟用它。测间隔用 `steady_clock`，单调。`high_resolution_clock` 实现定义，可能是这两者之一。不要 `now().time_since_epoch().count()` 当可移植整数——单位和纪元都没钉死。睡眠 `this_thread::sleep_for(10ms);` 字面量 C++14。超时、TTL、缓存过期，类型里带着单位，比 `int timeout_ms` 难混。

时间点、时长都是值，可拷可移，没有所有权问题。和文件、锁无关。现代代码别再 `#define` 一个毫秒整数满天飞。

### 2、tuple 不是结构体的替代

`std::tuple<int, string, double>` 按位次访问 `get<0>`，结构化绑定拆开。返回三件套、临时打包可以；公共 API 用有名字的 struct。`tuple` 没有字段名，重构改顺序是静默的类型灾难。`pair` 是两元素的 tuple，`map` 的 value_type 是它，所以结构化绑定在 map 上自然。自己发明 `pair<pair<A, B>, C>` 该停。

`std::tie(a, b) = f();` 把已有变量绑成 tuple 再赋值。`std::ignore` 丢一位。C++17 结构化绑定多数时候更干净。`make_tuple` vs `forward_as_tuple`：后者存引用，寿命跟实参——又是 view 合同。

### 3、initializer_list 是引用数组，不是容器

`{1, 2, 3}` 传给 `void f(std::initializer_list<int>)`，背后是临时数组，`initializer_list` 只是指针 + 长度，**不拥有**。函数返回 `initializer_list` 指向临时数组，悬。成员存 `initializer_list` 同理。`vector v = {1, 2, 3}` 会把元素拷进 vector，那才拥有。构造函数同时有 `T(std::size_t)` 和 `T(initializer_list<U>)` 时，`T{5}` 可能走列表不是 5 个元素——`vector<int>{5}` 是一个元素 5，`vector<int>(5)` 是 5 个 0。入门篇点过列表初始化；容器上这是日常坑。

`auto x{1};` C++17 是 `int`。`auto x = {1};` 仍是 `initializer_list<int>`。等号差一个语义。

---

## 八、C++20 点到：concepts、ranges、coroutine、三比较

### 1、concepts 把模板错误提前

```cpp
// C++20 示意，本仓库默认 17 不能当可编译默认
// template<std::ranges::range R>
// void sort_all(R& r) { std::ranges::sort(r); }
```

C++17 用 SFINAE、`enable_if`、文档说「T 要有 begin」。错了就几十行 trait 爆炸。concepts 把约束写在接口上：不满足当场失败。语义仍是编译期多态，不是虚函数。虚函数篇的「编译期轴」在 20 有了名字。17 下继续模板 + 约定，别回退到宏。

### 2、ranges：视图不拥有

`views::filter | views::transform` 懒计算，底下是迭代器适配。视图常常绑到容器，容器扩容、析构，视图悬——`string_view` 同一类合同。`ranges::sort(v)` 比 `sort(v.begin(), v.end())` 少写一对迭代器。所有权没变：容器拥有，视图观察。17 用迭代器对；迁 20 时第一风险是把 view 存过源的寿命。

### 3、coroutine：co_await 不是绿色线程运行时

`co_await`、`co_yield`、`co_return` 把函数切成状态机，帧在堆上（或优化掉）。没有标准执行器、没有标准通道（23 才有 `generator` 等碎片）。要 asio / 自写 promise 类型才跑得起来。所有权：`coroutine_handle` 要人 `destroy`，RAII 包一层。不要把 coroutine 理解成 Go 的 goroutine——没有抢占调度器附送。点到：状态机 + 所有权仍是 RAII，只是帧从栈搬到堆。

`<=>` 三路比较、`default operator==`：少写六套比较。`span<T>`：运行时长度的连续观察，`string_view` 的泛化。`jthread`：析构 join。`atomic<shared_ptr<T>>`：智能指针篇提过。这些都是 20。读代码能认；写 17 项目不要无故用。

模块（modules）改编译模型，不是所有权模型。本篇不展开。

`std::format`（20）、`expected`（23）把格式化和错误码做成值，仍是「不要靠注释交接」：`expected<T, E>` 是 `optional` 的错误版，所有权在返回值里，不在 `errno` 里。读到能认即可。17 项目错误继续异常或错误码，不要手搓一半 `expected`。

---

## 九、所有权语义怎么把特性串起来

### 1、签名即合同，现代写法

```cpp
std::unique_ptr<T> factory();           // 交出独占
void sink(std::unique_ptr<T>);          // 吃进独占
void view(const T&);                    // 不拥有
void view(std::string_view);            // 不拥有文本
void maybe_take(std::optional<T>);      // 有则移进一份值
void handle(std::variant<A, B>);        // 闭集之一，按值可移
```

`shared_ptr` 进签名是「入股」。`T&&` 是「我要移走」。`const T&` 是「我看看」。`optional<T>` 按值是「可选的一份」。lambda 捕获列表是闭包的成员列表，也是所有权列表。`auto` 不改变这些，只省略名字。

Python 签名没有所有权：全是共享引用，除非拷一份。Go 签名：值拷、指针共享、切片头共享。C++ 现代签名把转手、观察、可选、闭集写进类型，靠的就是 11–17 这些工具，不是靠注释。

### 2、零法则优先于新特性

能让成员自己管理：`string`、`vector`、`unique_ptr`，class 不写五法则。然后才能安心移动、放进 `vector`、放进 `optional`、被 lambda 按值捕获。手写析构却不写移动，容器扩容走拷贝，现代特性全给你绕开。对象模型的零法则是现代 C++ 的地基，不是复古。

`= default` 移动、`= delete` 拷贝：`unique_ptr` 当成员时类自动不可拷、可移。要半套（可拷要深拷），自己写。不要一边用 `unique_ptr` 一边手写浅拷。

### 3、不是年表

C++11：移动、lambda、`auto`、`unique_ptr`/`shared_ptr`、`unordered_map`、`thread`、`mutex`、`atomic`、`chrono`、范围 for、`constexpr` 初版、`enum class`。C++14：泛型 lambda、初始化捕获、`make_unique`、放宽 constexpr。C++17：结构化绑定、`optional`/`variant`/`any`/`string_view`、`if` 初始化、`scoped_lock`、`inline` 变量、强制复制省略、`filesystem`、`if constexpr`。C++20：concepts、ranges、coroutine、`span`、`jthread`、`<=>`。

年表的读法：每一档都在减少「所有权靠约定」。11 把转手和独占引进类型；14 让 lambda 能 move 捕获；17 把可选、闭集、观察做成值；20 把模板约束和视图做成语言。所以切入不是「新标准有哪些」，是「资源还要不要靠注释交接」。还靠注释的地方，就是下一刀该切的地方。

按值传 `T` 再内部移走，是 11 之后的默认「收下所有权」写法：左值进来拷一份（调用方保留），右值进来移（调用方放弃）。比 `const T&` + 内部再拷清楚——后者看不出要不要留下。比 `T&&` 只收右值更灵活。大对象、不可拷类型（`unique_ptr`）按值就是强制 `move` 进来。指针篇三种形参在现代代码里扩成：按值（收）、`const T&` / `string_view`（看）、`T*`（看且可空）、`T&`（改）、`T&&`（只收将亡）。`optional<T>` 按值是「可选地收一份」。签名写对，函数体里很少再问所有权。

---

## 十、对照、易错点和一份能跑的实验

对照：

| | 转手 | 可选 | 文本观察 | 闭包捕获 |
| --- | --- | --- | --- | --- |
| C++17 | 移动 / `unique_ptr` | `optional` | `string_view` | 按值 / 引用 / 初始化捕获 |
| Python | 绑名，无移动 | `None` | 切片常新对象 | cell，对象活在堆上 |
| Go | 赋值拷头或值 | 指针 / `, ok` | `string` 只读共享 | 闭包绑变量，逃逸分析上堆 |

易错点：

- `return std::move(local);` 挡 NRVO。
- `std::move` 之后继续用源。
- `const` 成员让移动退化成拷贝。
- lambda `[&]` 活过栈帧。
- `[=]` 以为拷了对象，其实拷了 `this` 指针。
- 不可拷 lambda 塞进 `std::function`。
- `for (auto x : big)` 无谓拷贝。
- 结构化绑定以为没拷。
- `*optional` 在空时。
- `string_view` 指临时 `string`。
- `variant` 当开集多态用，每加类型改全世界，却还要基类指针。
- `any` / `function` 当默认类型擦除。
- 手写析构不写移动，`vector` 扩容拷。
- `auto_ptr`、返回 `const` 值。
- C++17 项目里写 ranges / concepts / `co_await` 当能编译。
- `if constexpr` 写成普通 `if` 还对非法分支求值。
- `optional<T&>`。
- 捕获列表空着却用了成员，隐含 `this`，对象先死。
- `vector<int>{5}` 当 5 个 0。
- 返回 `initializer_list`。
- `system_clock` 测耗时。
- `tuple` 当公共 API。
- `forward_as_tuple` 存过实参寿命。
- `auto x = {1};` 当 int。
- 按值收大对象却写 `const T&` 再内部拷，签名撒谎。

`g++ -std=c++17 -O0 -Wall -Wextra -o modern modern.cpp && ./modern`：

```cpp
// modern.cpp — 移动、lambda 捕获、optional/variant、结构化绑定、if constexpr。C++17。
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

std::vector<std::string> make_vec() {
    std::vector<std::string> v;
    v.reserve(2);
    v.emplace_back("a");
    v.emplace_back("b");
    return v;
}

std::optional<int> parse_pos(int n) {
    if (n < 0) return std::nullopt;
    return n;
}

template<class T>
void show(const T& x) {
    if constexpr (std::is_same_v<T, std::string>) {
        std::cout << "str=" << x << "\n";
    } else {
        std::cout << "int=" << x << "\n";
    }
}

void take_view(std::string_view sv) {
    std::cout << "view=" << sv << "\n";
}

int main() {
    auto v = make_vec();
    auto w = std::move(v);
    std::cout << "w=" << w.size() << " v=" << v.size() << "\n";

    auto p = std::make_unique<int>(3);
    auto fn = [p = std::move(p)] { return *p; };
    std::cout << "lambda=" << fn() << " p_empty=" << !p << "\n";

    if (auto n = parse_pos(4)) {
        std::cout << "opt=" << *n << "\n";
    }
    std::cout << "null=" << parse_pos(-1).value_or(-99) << "\n";

    std::variant<int, std::string> u = 7;
    std::visit([](const auto& x) { show(x); }, u);
    u = std::string{"z"};
    std::visit([](const auto& x) { show(x); }, u);

    std::pair<std::string, int> kv{"k", 1};
    auto [key, val] = kv;
    kv.first = "changed";
    std::cout << "bind=" << key << " orig=" << kv.first << "\n";

    take_view("lit");
    std::string s = "owned";
    take_view(s);

    int k = 10;
    auto add = [k](int x) { return x + k; };
    k = 0;
    std::cout << "cap=" << add(1) << "\n";          // 11，按值捕获当时的 10
}
```

跑完对照：`v` 移走后 size 0，`w` 为 2；lambda 初始化捕获把 `unique_ptr` 关进去，外面空；`optional` 正数有值、负数 `value_or`；`visit` 先走 int 再走 string；结构化绑定默认拷，`key` 仍是 `"k"`；`string_view` 接字面量和活着的 `string`；按值捕获不跟后续的 `k = 0`。把 `take_view` 的实参改成 `std::string_view sv = std::string{"tmp"};` 再在后面用 `sv`，是悬空——加 ASAN 能打到。

检查清单：返回局部不写 `move`；转手用 `unique_ptr` / 按值；观察用引用 / `string_view`，不存过源；可选值用 `optional` 不用魔数；闭集用 `variant`，开集用虚函数；lambda 捕获列表当成员声明写；`auto` 局部、签名写类型；能零法则就零法则。C++20 的概念和视图是同一合同的语法糖和约束，不是新的所有权模型。新标准不是清单——清单上每一项，都在问资源现在由谁的析构释放、转手有没有经过类型系统。
