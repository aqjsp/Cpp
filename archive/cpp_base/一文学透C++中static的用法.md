# 一文学透C++中static的用法

`static` 是 C++ 中一个多功能的关键字，用于控制变量的存储方式和可见性，以及类成员的行为。

### 1. `static` 用于局部变量

作用：当 `static` 用于局部变量时，变量的生命周期会扩展到整个程序的生命周期。它的作用范围仍然是局部的，但它不会在每次进入函数时重新分配内存，而是在函数首次调用时初始化一次，并且在后续的函数调用中保持上次修改的值。

用法：

```c
void func() {
    static int count = 0;  // 局部静态变量
    count++;
    std::cout << "调用次数: " << count << std::endl;
}

int main() {
    func();  // 输出: 调用次数: 1
    func();  // 输出: 调用次数: 2
    func();  // 输出: 调用次数: 3
    return 0;
}
```

- 解释：`count` 是一个局部静态变量，它只会在第一次调用 `func()` 时初始化。后续的调用不会重新初始化它，因此它保存并递增之前的值。

特点：

- 局部静态变量在函数调用间共享其值。
- 只会初始化一次，且在程序退出时销毁。

### 2. `static` 用于全局变量和函数

作用：当 `static` 用于全局变量或函数时，它使得该变量或函数的作用域仅限于声明它的文件。这意味着它不能被其他文件访问，起到了封装和避免命名冲突的作用。

用法：

```c
// file1.cpp
static int global_var = 10;  // 静态全局变量

static void func() {         // 静态函数
    std::cout << "Hello from func!" << std::endl;
}

// file2.cpp
// extern int global_var;   // 不能引用 file1.cpp 中的 global_var
// extern void func();       // 不能引用 file1.cpp 中的 func()
```

特点：

- `static` 限制了全局变量和函数的可见性，避免了与其他文件中同名的变量和函数产生冲突。
- 适用于内部实现细节，不希望外部访问的函数和变量。

### 3. `static` 用于类成员变量

作用：当 `static` 用于类成员变量时，意味着该变量是类的共享成员，而不是每个对象都有一份拷贝。所有该类的对象共享同一份静态成员变量。

用法：

```c
class MyClass {
public:
    static int count;  // 静态成员变量

    MyClass() {
        count++;
    }

    static void displayCount() {
        std::cout << "对象数量: " << count << std::endl;
    }
};

// 定义静态成员变量
int MyClass::count = 0;

int main() {
    MyClass obj1;
    MyClass obj2;
    MyClass obj3;
    MyClass::displayCount();  // 输出: 对象数量: 3
    return 0;
}
```

- 解释：`count` 是一个静态成员变量，属于 `MyClass` 类，而不是某个特定的对象。因此，所有对象共享该变量，而不是每个对象都有一份拷贝。
- 初始化：静态成员变量必须在类外进行初始化，如 `int MyClass::count = 0;`。

特点：

- 静态成员变量可以通过类名或对象名访问，但通常推荐使用类名。
- 静态成员变量在所有对象之间共享，且它的生命周期从程序开始到程序结束。

### 4. `static` 用于类成员函数

作用：当 `static` 用于类的成员函数时，该函数成为类的静态成员函数。静态成员函数只能访问静态成员变量或静态成员函数，不能直接访问类的非静态成员。

用法：

```c
class MyClass {
public:
    static int count;

    MyClass() {
        count++;
    }

    static void displayCount() {
        std::cout << "对象数量: " << count << std::endl;
    }
};

int MyClass::count = 0;

int main() {
    MyClass obj1;
    MyClass obj2;
    MyClass obj3;

    // 可以通过类名直接调用静态成员函数
    MyClass::displayCount();  // 输出: 对象数量: 3

    return 0;
}
```

解释：`displayCount` 是一个静态成员函数，它可以通过 `MyClass::displayCount()` 直接调用，而不需要创建 `MyClass` 的对象。静态成员函数不能访问类的实例数据成员，只能访问静态数据成员。

**特点**：

- 静态成员函数不需要对象实例即可调用。
- 只能访问类的静态成员。

### 总结：`static` 的各种用法

| 用法                  | 描述                                                 | 影响                                         |
| --------------------- | ---------------------------------------------------- | -------------------------------------------- |
| **局部静态变量**      | 局部变量只初始化一次，保持其值跨多个函数调用         | 生命周期为程序运行期间，作用域仅限于函数内部 |
| **静态全局变量/函数** | 限制全局变量或函数的作用域，仅限于定义它的文件       | 不能被外部访问，避免命名冲突                 |
| **类成员静态变量**    | 使得类成员变量在所有对象之间共享                     | 生命周期为程序运行期间，所有对象共享一份     |
| **类成员静态函数**    | 使得类成员函数不依赖于具体对象，可以直接通过类名调用 | 不能访问实例成员，只能访问静态成员           |

