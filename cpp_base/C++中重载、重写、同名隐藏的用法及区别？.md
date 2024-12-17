# C++中重载、重写、同名隐藏的用法及区别？

### 1. 重载（Overloading）

**重载**是指在同一作用域内，定义多个同名的函数，但它们的参数列表不同（即参数个数、类型、顺序不同）。通过不同的参数类型或数量，来区分重载的函数。

重载的规则：

- 必须在同一作用域内定义多个同名函数。
- 参数类型、参数个数或顺序必须不同，才能区分重载函数。
- 返回类型不能作为区分重载函数的标准。
- 编译器在调用时会根据传入参数的类型来选择合适的重载函数。

#### 用法

重载通常用于处理同一功能但输入类型不同的情况，例如打印不同类型的数据。

#### 示例

```c
#include <iostream>
using namespace std;

class Printer {
public:
    void print(int i) {
        cout << "Printing int: " << i << endl;
    }

    void print(double d) {
        cout << "Printing double: " << d << endl;
    }

    void print(const char* str) {
        cout << "Printing string: " << str << endl;
    }
};

int main() {
    Printer p;
    p.print(10);       // 调用 print(int)
    p.print(3.14);     // 调用 print(double)
    p.print("Hello");  // 调用 print(const char*)

    return 0;
}
```

输出：

```c
Printing int: 10
Printing double: 3.14
Printing string: Hello
```

解释：

- `print` 函数根据不同的参数类型（`int`、`double`、`const char*`）进行了重载。
- 每个函数做相似的事情，但输入的数据类型不同。

### 2. 重写（Overriding）

**重写**是指在派生类中重新定义一个在基类中已经声明过的虚函数（`virtual` 函数）。重写函数必须保持与基类虚函数的相同签名（即函数名、参数个数、类型等），且基类函数必须被标记为 `virtual`，这样才会触发多态。

重写的规则：

- 基类函数必须是虚函数（`virtual`）。
- 派生类中重写的函数必须与基类的虚函数保持相同的函数签名。
- 重写通常用来改变基类的行为，派生类通过重写实现自己的逻辑。
- 在 C++11 及以后，推荐使用 `override` 关键字来明确标识重写行为，这有助于编译器检查是否正确重写了基类的函数。

#### 用法

重写主要用于在继承关系中，当子类需要更改或扩展父类方法的实现时，使用重写来改变基类的行为。

#### 示例

```c
#include <iostream>
using namespace std;

class Animal {
public:
    virtual void sound() {  // 基类的虚函数
        cout << "Animal makes a sound" << endl;
    }
};

class Dog : public Animal {
public:
    void sound() override {  // 派生类重写基类的虚函数
        cout << "Dog barks" << endl;
    }
};

int main() {
    Animal* animal = new Dog();
    animal->sound();  // 调用 Dog 类的重写函数

    delete animal;
    return 0;
}
```

输出：

```c
Dog barks
```

解释：

- `sound` 是一个虚函数，在 `Animal` 类中定义。
- `Dog` 类重写了 `sound` 函数，提供了新的实现。
- 当通过基类指针调用 `sound` 函数时，实际调用的是 `Dog` 类中的实现，这是 **多态** 的体现。

### 3. 同名隐藏（Name Hiding）

**同名隐藏**发生在派生类中定义了与基类中同名的函数、变量或类型时，派生类中的定义会隐藏基类中的定义。当基类和派生类中的成员具有相同名称时，派生类中的成员会覆盖基类中的成员，造成基类成员的隐藏。

同名隐藏的规则：

- 在派生类中，如果定义了与基类相同名称的成员（函数、变量等），派生类中的成员会隐藏基类中的成员。
- 同名隐藏不仅可以发生在函数上，还可以发生在成员变量、类型等上。
- 同名隐藏不需要函数签名相同，可以不同（例如，参数个数、类型等）。
- 如果想访问基类的同名成员，可以通过 `Base::member` 来显式指定。

#### 用法

同名隐藏通常是不小心发生的，它可能导致基类中的成员被错误地隐藏，产生意料之外的行为。因此，避免同名隐藏是一种良好的编程实践，尤其是在继承中。

#### 示例

```c
#include <iostream>
using namespace std;

class Base {
public:
    void show() {
        cout << "Base show()" << endl;
    }

    void print() {
        cout << "Base print()" << endl;
    }
};

class Derived : public Base {
public:
    void show() {  // 同名隐藏：派生类中的 show 隐藏了基类中的 show
        cout << "Derived show()" << endl;
    }

    void print(int x) {  // 同名隐藏：派生类中的 print 隐藏了基类中的 print
        cout << "Derived print: " << x << endl;
    }
};

int main() {
    Derived obj;
    obj.show();   // 调用 Derived::show，隐藏了 Base::show
    obj.print(10); // 调用 Derived::print，隐藏了 Base::print

    Base* bptr = &obj;
    bptr->show();   // 调用 Base::show（基类指针指向派生类对象，调用基类中的 show）
    bptr->print();  // 调用 Base::print（基类指针指向派生类对象，调用基类中的 print）

    return 0;
}
```

输出：

```c
Derived show()
Derived print: 10
Base show()
Base print()
```

解释：

- 在 `Derived` 类中，定义了与 `Base` 类中同名的函数 `show` 和 `print`。
- 因为 `Derived` 类的 `show` 和 `print` 函数的参数不同，它们将覆盖基类中对应的函数。
- 然而，当通过基类指针调用这些函数时，仍然调用的是基类中的版本，因为它们没有被声明为 `virtual`。

### 对比

| 特性           | 重载（Overloading）                            | 重写（Overriding）                            | 同名隐藏（Name Hiding）                    |
| -------------- | ---------------------------------------------- | --------------------------------------------- | ------------------------------------------ |
| 定义范围       | 在同一作用域内，通过不同的参数列表定义同名函数 | 在派生类中，重新定义基类中的虚函数            | 在派生类中，定义与基类中同名的函数或变量   |
| 继承关系       | 不涉及继承                                     | 必须在继承关系中发生，基类需要虚函数          | 与继承有关，但没有虚函数的概念             |
| 参数签名       | 必须不同（参数个数、类型、顺序不同）           | 必须相同（包括函数名、参数、返回值类型等）    | 可以不同，派生类中的函数签名可以与基类不同 |
| 编译时检查     | 编译器根据参数列表自动选择调用的重载函数       | 编译器检查函数签名是否一致，确保多态性        | 编译器根据函数名选择调用隐藏的函数         |
| 是否影响多态性 | 不影响多态性                                   | 影响多态性，通过虚函数实现运行时多态          | 不会影响多态性，因为隐藏的是同名函数或变量 |
| 示例           | `void print(int); void print(double);`         | `virtual void show();` 在派生类中重写基类函数 | 在派生类中重新定义与基类同名的函数或变量   |

### 总结：

- **重载**用于在同一作用域中定义多个同名但参数不同的函数，常见于处理不同类型的数据。
- **重写**用于继承中，子类重写基类的虚函数，以实现不同的行为。
- **同名隐藏**发生在子类中定义了与基类同名的成员（函数、变量等），可能导致基类的成员被隐藏，产生意料外的行为。