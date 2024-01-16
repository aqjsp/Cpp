# C++虚函数

讲虚函数之前先讲讲面向对象的三大特性：封装、继承、多态。

**1、封装**

封装是指将数据（属性）和操作数据的方法（函数）封装在一个单元中，这个单元就是类。封装的主要目的是隐藏类的内部实现细节，只暴露必要的接口给外部使用者。

优点：

- 信息隐藏： 封装可以将类的内部细节隐藏起来，不暴露给外部，提高了安全性和防止误用。
- 简化接口： 封装通过提供清晰的接口简化了类的使用，使用者只需关注如何使用接口而不需要了解内部实现。
- 提高可维护性： 内部实现的修改不会影响外部使用者，从而提高了代码的可维护性。

示例：

```C
#include <iostream>
#include <string>

class Student {
private:
    std::string name;
    int age;

public:
    // 构造函数
    Student(const std::string& n, int a) : name(n), age(a) {}

    // 获取姓名
    std::string getName() const {
        return name;
    }

    // 设置年龄
    void setAge(int a) {
        if (a >= 0) {
            age = a;
        }
    }

    // 显示学生信息
    void displayInfo() const {
        std::cout << "Name: " << name << ", Age: " << age << std::endl;
    }
};

int main() {
    Student student("Alice", 20);
    
    // 使用公有接口获取和设置信息
    student.setAge(21);
    std::cout << "Student Name: " << student.getName() << std::endl;
    student.displayInfo();

    return 0;
}
```

**2、继承**

继承允许一个类（子类或派生类）继承另一个类（父类或基类）的属性和方法。通过继承，子类可以获得父类的特征，并可以添加新的特征或修改继承的特征。

优点：

- 代码重用： 继承允许在不重复编写代码的情况下扩展和修改现有类，提高了代码的重用性。
- 层次结构： 继承可以创建类的层次结构，使得代码更有组织性和可扩展性。
- 多态性支持： 继承是多态性的基础，通过基类指针或引用调用派生类的方法实现多态行为。

示例：

```C
#include <iostream>
#include <string>

// 基类
class Animal {
protected:
    std::string name;

public:
    Animal(const std::string& n) : name(n) {}

    void eat() {
        std::cout << name << " is eating." << std::endl;
    }
};

// 派生类
class Dog : public Animal {
public:
    Dog(const std::string& n) : Animal(n) {}

    void bark() {
        std::cout << name << " is barking." << std::endl;
    }
};

int main() {
    Dog myDog("Buddy");

    myDog.eat();  // 继承自基类
    myDog.bark(); // 派生类自己的方法

    return 0;
}
```

**3、多态**

多态性是指同一个操作可以作用于不同类型的对象，并且可以根据对象的类型执行不同的行为。多态性通过虚函数和函数重载实现。

- 编译时多态性（静态多态性）： 通过函数重载实现，编译器在编译时根据函数参数的类型和数量来选择调用合适的函数。这种多态性是在编译时解析的。
- 运行时多态性（动态多态性）： 通过虚函数和继承实现，允许在运行时根据对象的实际类型来调用适当的函数。这种多态性是在运行时解析的。

优点：

- 灵活性： 多态性允许在不同的情境下以通用的方式处理不同类型的对象，提高了代码的灵活性。
- 可扩展性： 可以轻松地添加新的派生类而不影响现有的代码，增加了系统的可扩展性。
- 简化接口： 多态性简化了代码的接口，允许使用者按统一的方式与不同类型的对象交互。

示例：

```C
#include <iostream>
#include <vector>

class Shape {
public:
    virtual void draw() {
        std::cout << "Drawing a shape." << std::endl;
    }
};

class Circle : public Shape {
public:
    void draw() override {
        std::cout << "Drawing a circle." << std::endl;
    }
};

class Square : public Shape {
public:
    void draw() override {
        std::cout << "Drawing a square." << std::endl;
    }
};

int main() {
    std::vector<Shape*> shapes;
    shapes.push_back(new Circle());
    shapes.push_back(new Square());

    for (Shape* shape : shapes) {
        shape->draw(); // 多态性：根据对象的实际类型调用适当的方法
    }

    // 释放内存
    for (Shape* shape : shapes) {
        delete shape;
    }

    return 0;
}
```

讲了这么多，进入今天主题吧，C++实现多态的虚函数。

在C++中，函数继承的方法可以让我们快速开发，为了满足多态和泛型编程，C++允许用户使用虚函数来完成运行时解析，与一般的编译时解析也有着本质区别。

**4、虚函数在内存中的分布**

对于C++了解的人都应该知道虚函数是通过一个虚函数表来实现的。在这个表中，主要是一个类的虚函数的地址表，这个表解决了继承、覆盖的问题，保证其真实反应实际的函数。这样的话，在有虚函数的类的实例中，这个表被分配在这个实例的内存中。当我们用父类的指针操作其子类的时候，这个虚表就非常重要了，它指明了实际所应该调用的函数。

```C
class A {
public:
    virtual void v_a(){}
    virtual ~A(){}
    int64_t _m_a;
};
int main()
{
    A* a = new A();
    return 0;
}
```

 定义一个类A，那么它在内存中分布的情况是什么样的呢？接下来一起看看

![image-20240112003025250](https://raw.githubusercontent.com/aqinzz/Pictures/main/202401120030746.png)

- 首先在主函数的栈帧上有一个 A 类型的指针指向堆里面分配好的对象 A 实例。
- 对象 A 实例的**头部**是一个 vtable 指针，紧接着是 **A 对象按照声明顺序排列的成员变量**。（当我们创建一个对象时，便可以通过实例对象的地址，得到该实例的虚函数表，从而获取其函数指针。）
- vtable 指针指向的是代码段中的 A 类型的**虚函数表中的第一个虚函数起始地址**。
- 虚函数表的结构其实是有一个头部的，叫做 vtable_prefix ，紧接着是按照声明顺序排列的虚函数。
- 注意到这里有两个虚析构函数，因为对象有两种构造方式，栈构造和堆构造，所以对应的，对象会有两种析构方式，其中堆上对象的析构和栈上对象的析构不同之处在于，栈内存的析构不需要执行 delete 函数，会自动被回收。
- typeinfo 存储着 A 的类基础信息，包括父类与类名称，C++关键字 typeid 返回的就是这个对象。
- typeinfo 也是一个类，对于没有父类的 A 来说，当前 tinfo 是 class_type_info 类型的，从虚函数指针指向的vtable 起始位置可以看出。

**5、虚函数表实现原理**

虚函数表是一个指向虚函数的指针数组，每个带有虚函数的类都有一个对应的虚函数表。

**虚函数指针**

其本质就是一个指向函数的指针，与普通的函数指针并没有什么大的区别。它指向程序员自己定义的虚函数，当子类调用虚函数的时候，实际上就是通过调用这个虚函数指针从而找到接口。

虚函数指针是一个真实存在的数据类型，在对象实例化的时候，放在这个对象地址的首位，目的就是为了保证运行的快速性。与对象的成员函数不一样的是，虚函数指针对外部是完全不可见的，除非直接访问地址或者是debug模式，否则它是不能被外部调用的。

只有拥有虚函数的类才能拥有虚函数指针，每个虚函数都会对应一个虚函数指针。那么，拥有虚函数的类都会产生额外的开销，并且也会在一定程度上影响程序的运行速度。

**虚函数表**

当一个类包含虚函数时，编译器会在该类的对象中添加一个指向虚函数表的指针。这个指针通常位于对象的内存布局的开头（虚指针），它们按照一定的顺序组织起来就会构成一个表状结构，叫做虚函数表。虚函数表本身是一个全局的、类特定的数组，其中包含了该类中所有虚函数的地址。

先来定义一个基类：

```C
class Panent
{
public:
    virtual void A(){cout<<"Panent::A"<<endl;}
    virtual void B(){cout<<"Panent::B"<<endl;}
    virtual void C(){cout<<"Panent::C"<<endl;}
};
```

对于基类Base的虚函数表记录的只有自己定义的虚函数。

![image-20240112003230605](https://raw.githubusercontent.com/aqinzz/Pictures/main/202401120032886.png)

下来再看看子类：

```C
class Children: public Panent
{
public:
    virtual void A(){cout<<"Children::f"<<endl;}
    virtual void B1(){cout<<"Children::B1"<<endl;}
    virtual void C1(){cout<<"Children::C1"<<endl;}
}
```

最常见的继承，就是子类对基类的虚函数进行覆盖继承。

![image-20240112003316918](https://raw.githubusercontent.com/aqinzz/Pictures/main/202401120033345.png)

此时的虚函数表：

![image-20240112003354781](https://raw.githubusercontent.com/aqinzz/Pictures/main/202401120033429.png)

基函数的表项仍然会保留，而得到正确继承的虚函数的指针将会被覆盖，而子类自己的虚函数将跟在表后。

当多继承的时候，表项将会增多，顺序将会体现为继承的顺序，那么子类的虚函数就跟在第一个表项后。

![image-20240112003413341](https://raw.githubusercontent.com/aqinzz/Pictures/main/202401120034323.png)

![image-20240112003431847](https://raw.githubusercontent.com/aqinzz/Pictures/main/202401120034367.png)

C++中一个类是公用一个虚函数表的，基类有基类的虚函数表，子类有子类的虚函数表，这样极大的节省了内存。

**虚表指针**

为了指定对象的虚表，对象内部包含一个虚表的指针，来指向自己所使用的虚表。为了让每个包含虚表的类的对象都拥有一个虚表指针，在编译阶段，编译器在类中添加了一个指针*__vptr，用来指向虚表。这样，当类的对象在创建时便拥有了这个指针，且这个指针的值会自动被设置为指向类的虚表，*__vptr一般在对象内存分布的最前面。

虚表指针的初始化确实发生在构造函数的调用过程中， 但是在执行构造函数体之前，即进入到构造函数的"{“和”}"之前。 为了更好的理解这一问题， 我们可以把构造函数的调用过程细分为两个阶段，即：

- 进入到构造函数体之前。在这个阶段如果存在虚函数的话，虚表指针被初始化。如果存在构造函数的初始化列表的话，初始化列表也会被执行。
- 进入到构造函数体内。这一阶段是我们通常意义上说的构造函数。

**带缺省参数的虚函数**

当缺省参数和虚函数一起出现的时候情况有点复杂，极易出错。我们知道，**虚函数是动态绑定的，但是为了执行效率，缺省参数是静态绑定的**。

参考：https://cloud.tencent.com/developer/article/2109996