# C++设计模式全解析：构建健壮、可扩展的软件系统

在软件开发领域，设计模式是经过时间考验、被反复使用的解决方案，它们是软件设计中常见问题的通用、可重用的解决方案。

设计模式并非直接可用的代码，而是描述了在特定情境下如何解决特定问题的“蓝图”或“模板”。它们是软件工程领域中经验的结晶，是面向对象设计原则（如SOLID原则）在实践中的具体体现。

学习和掌握设计模式，对于C++开发者而言，具有极其重要的意义：

1.  提升代码质量：设计模式能够帮助我们编写出更具可读性、可维护性、可扩展性和复用性的代码。
2.  促进团队沟通：设计模式提供了一套通用的词汇和概念，使得开发者之间能够更高效地交流设计思想。
3.  加速开发进程：面对常见问题时，无需从零开始思考解决方案，可以直接应用成熟的设计模式。
4.  应对复杂性：设计模式能够将复杂系统分解为更小、更易于管理的组件，从而降低系统复杂性。
5.  提高面试竞争力：设计模式是衡量一个开发者设计能力和经验的重要指标，在技术面试中常被考察。

本文将全面深入地探讨C++中所有的经典设计模式，包括创建型、结构型和行为型三大类，共23种模式。

我将带领大家从每种模式的原理、解决的问题、核心思想、C++代码实现、使用场景以及优缺点等方面进行详细阐述，旨在为大家提供一份权威且实用的C++设计模式指南。

## 设计模式的分类

GoF（Gang of Four，四人帮）在其经典著作《设计模式：可复用面向对象软件的基础》中，将设计模式分为三大类：

#### 1、创建型模式

创建型模式关注对象的创建机制，旨在将对象的创建与使用分离，从而提高系统的灵活性和可维护性。它们提供了一种在创建对象时隐藏创建逻辑的方式，而不是直接使用`new`运算符。

*   单例模式
*   工厂方法模式
*   抽象工厂模式
*   建造者模式
*   原型模式

#### 2、结构型模式

结构型模式关注如何将类和对象组合成更大的结构，以实现新的功能。它们描述了如何通过继承和组合来组织对象，从而形成更灵活、更高效的结构。

*   适配器模式
*   桥接模式
*   组合模式
*   装饰器模式
*   外观模式
*   享元模式
*   代理模式

#### 3、行为型模式

行为型模式关注对象之间的职责分配和交互方式。它们描述了对象和类如何协同工作，以完成某个任务，从而提高系统的灵活性和可复用性。

*   责任链模式
*   命令模式
*   解释器模式
*   迭代器模式
*   中介者模式
*   备忘录模式
*   观察者模式
*   状态模式
*   策略模式
*   模板方法模式
*   访问者模式

接下来，我们将逐一深入探讨这些设计模式。

## 一、创建型模式

创建型模式是关于对象创建的模式，它们提供了一种在创建对象时隐藏创建逻辑的方式，而不是直接使用`new`运算符。这使得系统在创建对象时更加灵活，并且能够更好地控制对象的创建过程。

#### 1、单例模式

原理与意图：

单例模式确保一个类只有一个实例，并提供一个全局访问点来访问该实例。其核心思想是限制类的实例化次数，通常用于管理共享资源，如日志记录器、配置管理器、线程池或数据库连接池等。

解决的问题：

*   确保一个类只有一个实例。
*   为该实例提供一个全局访问点。

核心思想：

1.  私有化构造函数：阻止外部直接通过`new`关键字创建类的实例。
2.  静态成员变量：在类内部声明一个静态的私有实例指针或引用，用于存储唯一的实例。
3.  静态公共方法：提供一个静态的公共方法（通常命名为`getInstance()`或`instance()`），用于获取该唯一实例。该方法负责在首次调用时创建实例，并在后续调用时返回已存在的实例。

C++代码实现：

单例模式有多种实现方式，包括饿汉式、懒汉式（线程不安全、线程安全）、以及C++11后的局部静态变量方式（推荐）。

##### 饿汉式（Eager Initialization）

饿汉式单例在程序启动时就创建实例，无论是否需要。它的优点是线程安全，但缺点是如果实例创建开销大且不一定会被使用，会造成资源浪费。

```cpp
#include <iostream>
#include <string>

class SingletonEager {
private:
    static SingletonEager* instance; // 静态实例指针
    std::string name;

    // 私有构造函数
    SingletonEager(const std::string& n = "DefaultEager") : name(n) {
        std::cout << "SingletonEager instance created: " << name << std::endl;
    }

    // 禁止拷贝构造和赋值
    SingletonEager(const SingletonEager&) = delete;
    SingletonEager& operator=(const SingletonEager&) = delete;

public:
    // 获取实例的静态方法
    static SingletonEager* getInstance() {
        return instance;
    }

    void showMessage() {
        std::cout << "Hello from SingletonEager: " << name << std::endl;
    }
};

// 静态成员变量的定义和初始化
SingletonEager* SingletonEager::instance = new SingletonEager();

// int main() {
//     SingletonEager* s1 = SingletonEager::getInstance();
//     s1->showMessage();

//     SingletonEager* s2 = SingletonEager::getInstance();
//     s2->showMessage();

//     // 验证是同一个实例
//     if (s1 == s2) {
//         std::cout << "s1 and s2 are the same instance." << std::endl;
//     }

//     // 注意：饿汉式通常不需要手动delete，因为其生命周期与程序相同
//     // 但如果需要，可以提供一个静态的destroy方法
//     // delete SingletonEager::instance; // 不推荐直接delete

//     return 0;
// }
```

##### 懒汉式（Lazy Initialization） - 线程不安全

懒汉式单例在第一次使用时才创建实例。这种方式在单线程环境下可以节省资源，但在多线程环境下存在线程安全问题，可能创建多个实例。

```cpp
#include <iostream>
#include <string>

class SingletonLazyUnsafe {
private:
    static SingletonLazyUnsafe* instance;
    std::string name;

    SingletonLazyUnsafe(const std::string& n = "DefaultLazyUnsafe") : name(n) {
        std::cout << "SingletonLazyUnsafe instance created: " << name << std::endl;
    }

    SingletonLazyUnsafe(const SingletonLazyUnsafe&) = delete;
    SingletonLazyUnsafe& operator=(const SingletonLazyUnsafe&) = delete;

public:
    static SingletonLazyUnsafe* getInstance() {
        if (instance == nullptr) { // 第一次检查
            instance = new SingletonLazyUnsafe();
        }
        return instance;
    }

    void showMessage() {
        std::cout << "Hello from SingletonLazyUnsafe: " << name << std::endl;
    }

    // 提供一个静态方法用于清理资源
    static void destroyInstance() {
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
            std::cout << "SingletonLazyUnsafe instance destroyed." << std::endl;
        }
    }
};

// 静态成员变量的定义和初始化
SingletonLazyUnsafe* SingletonLazyUnsafe::instance = nullptr;

// int main() {
//     SingletonLazyUnsafe* s1 = SingletonLazyUnsafe::getInstance();
//     s1->showMessage();

//     SingletonLazyUnsafe* s2 = SingletonLazyUnsafe::getInstance();
//     s2->showMessage();

//     if (s1 == s2) {
//         std::cout << "s1 and s2 are the same instance." << std::endl;
//     }

//     SingletonLazyUnsafe::destroyInstance();

//     return 0;
// }
```

##### 懒汉式（Lazy Initialization） - 线程安全（双重检查锁定 DCLP）

为了解决懒汉式的线程安全问题，可以使用双重检查锁定（Double-Checked Locking Pattern, DCLP）。但在C++11之前，DCLP存在内存乱序问题，需要使用内存屏障。C++11及以后，`std::mutex`和`std::atomic`可以确保其正确性。

```cpp
#include <iostream>
#include <string>
#include <mutex> // For std::mutex

class SingletonLazySafeDCLP {
private:
    static SingletonLazySafeDCLP* instance;
    static std::mutex mtx; // 互斥锁
    std::string name;

    SingletonLazySafeDCLP(const std::string& n = "DefaultLazySafe") : name(n) {
        std::cout << "SingletonLazySafeDCLP instance created: " << name << std::endl;
    }

    SingletonLazySafeDCLP(const SingletonLazySafeDCLP&) = delete;
    SingletonLazySafeDCLP& operator=(const SingletonLazySafeDCLP&) = delete;

public:
    static SingletonLazySafeDCLP* getInstance() {
        if (instance == nullptr) { // 第一次检查：避免每次都加锁
            std::lock_guard<std::mutex> lock(mtx); // 加锁
            if (instance == nullptr) { // 第二次检查：确保只创建一次
                instance = new SingletonLazySafeDCLP();
            }
        }
        return instance;
    }

    void showMessage() {
        std::cout << "Hello from SingletonLazySafeDCLP: " << name << std::endl;
    }

    static void destroyInstance() {
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
            std::cout << "SingletonLazySafeDCLP instance destroyed." << std::endl;
        }
    }
};

// 静态成员变量的定义和初始化
SingletonLazySafeDCLP* SingletonLazySafeDCLP::instance = nullptr;
std::mutex SingletonLazySafeDCLP::mtx;

// int main() {
//     // 可以在多线程环境下测试其安全性
//     // std::thread t1([]{ SingletonLazySafeDCLP::getInstance()->showMessage(); });
//     // std::thread t2([]{ SingletonLazySafeDCLP::getInstance()->showMessage(); });
//     // t1.join();
//     // t2.join();

//     SingletonLazySafeDCLP* s1 = SingletonLazySafeDCLP::getInstance();
//     s1->showMessage();
//     SingletonLazySafeDCLP::destroyInstance();
//     return 0;
// }
```

##### C++11 局部静态变量（推荐）

C++11标准规定，局部静态变量的初始化是线程安全的。这意味着编译器会保证在多线程环境下，局部静态变量只会被初始化一次。这是实现线程安全懒汉式单例最简洁、最推荐的方式。

```cpp
#include <iostream>
#include <string>

class SingletonModern {
private:
    std::string name;

    SingletonModern(const std::string& n = "DefaultModern") : name(n) {
        std::cout << "SingletonModern instance created: " << name << std::endl;
    }

    SingletonModern(const SingletonModern&) = delete;
    SingletonModern& operator=(const SingletonModern&) = delete;

public:
    static SingletonModern& getInstance() {
        static SingletonModern instance; // 局部静态变量，C++11保证线程安全
        return instance;
    }

    void showMessage() {
        std::cout << "Hello from SingletonModern: " << name << std::endl;
    }

    // 析构函数可以省略，因为静态变量在程序结束时自动销毁
    ~SingletonModern() {
        std::cout << "SingletonModern instance destroyed: " << name << std::endl;
    }
};

// int main() {
//     SingletonModern::getInstance().showMessage();
//     SingletonModern::getInstance().showMessage();

//     // 验证是同一个实例
//     SingletonModern& s1 = SingletonModern::getInstance();
//     SingletonModern& s2 = SingletonModern::getInstance();
//     if (&s1 == &s2) {
//         std::cout << "s1 and s2 are the same instance." << std::endl;
//     }

//     return 0;
// }
```

使用场景：

*   日志记录器：整个应用程序只需要一个日志实例来记录日志。
*   配置管理器：应用程序的配置信息通常只需要一个实例来加载和管理。
*   线程池/数据库连接池：为了避免频繁创建和销毁资源，通常会使用单例模式来管理这些共享资源池。
*   硬件设备管理器：控制对唯一硬件设备的访问。

优缺点：

*   优点：
    *   控制实例数量：确保一个类只有一个实例，避免资源冲突。
    *   全局访问点：提供方便的全局访问方式。
    *   资源节约：懒汉式可以延迟实例创建，节省资源。
*   缺点：
    *   违反单一职责原则：单例类既负责创建自己的唯一实例，又负责其本身的业务逻辑。
    *   扩展性差：单例模式的类通常是最终类，难以扩展。
    *   测试困难：单例模式引入了全局状态，使得单元测试变得复杂，因为测试之间可能相互影响。
    *   多线程问题：懒汉式在多线程环境下需要额外处理线程安全，否则可能创建多个实例（C++11局部静态变量除外）。

#### 2、工厂方法模式

原理与意图：

工厂方法模式定义了一个用于创建对象的接口，但让子类决定实例化哪一个类。工厂方法让类的实例化延迟到子类。它将对象的创建过程封装在工厂方法中，从而将客户端代码与具体的产品类解耦。

解决的问题：

*   当一个类无法预知它将要创建的对象的类时。
*   当一个类希望由其子类来指定它所创建的对象时。
*   当类将创建对象的职责委托给多个帮助子类中的某一个，并且你希望将哪个帮助子类是委托者这一信息局部化时。

核心思想：

1.  抽象产品（Product）：定义产品对象的接口，是所有具体产品类的父类。
2.  具体产品（Concrete Product）：实现抽象产品接口的具体产品类。
3.  抽象工厂（Creator）：声明工厂方法，该方法返回一个抽象产品类型的对象。抽象工厂也可以定义一个默认的工厂方法实现，返回一个默认的具体产品对象。
4.  具体工厂（Concrete Creator）：实现抽象工厂接口，重写工厂方法以返回特定具体产品类型的对象。

C++代码实现：

```cpp
#include <iostream>
#include <string>

// 抽象产品接口
class Product {
public:
    virtual ~Product() = default;
    virtual std::string getName() const = 0;
};

// 具体产品A
class ConcreteProductA : public Product {
public:
    std::string getName() const override {
        return "ConcreteProductA";
    }
};

// 具体产品B
class ConcreteProductB : public Product {
public:
    std::string getName() const override {
        return "ConcreteProductB";
    }
};

// 抽象工厂接口
class Creator {
public:
    virtual ~Creator() = default;
    virtual Product* createProduct() const = 0;
    
    // 工厂方法也可以包含一些通用逻辑
    void someOperation() const {
        Product* product = createProduct();
        std::cout << "Creator: Doing some operation with " << product->getName() << std::endl;
        delete product; // 负责销毁产品
    }
};

// 具体工厂A
class ConcreteCreatorA : public Creator {
public:
    Product* createProduct() const override {
        return new ConcreteProductA();
    }
};

// 具体工厂B
class ConcreteCreatorB : public Creator {
public:
    Product* createProduct() const override {
        return new ConcreteProductB();
    }
};

// int main() {
//     Creator* creatorA = new ConcreteCreatorA();
//     creatorA->someOperation(); // 通过工厂方法创建并使用产品A
//     delete creatorA;

//     Creator* creatorB = new ConcreteCreatorB();
//     creatorB->someOperation(); // 通过工厂方法创建并使用产品B
//     delete creatorB;

//     return 0;
// }
```

使用场景：

*   框架设计：当一个类库或框架需要创建某些对象，但又不想在代码中硬编码具体类时，可以使用工厂方法。例如，GUI框架中的按钮、文本框等组件的创建。
*   可插拔组件：当系统需要支持多种类型的产品，并且这些产品可以动态替换时。例如，不同的日志记录器、不同的解析器等。
*   解耦：将对象的创建和使用分离，降低客户端与具体产品类之间的耦合度。

优缺点：

*   优点：
    *   开闭原则：增加新的产品类时，只需增加对应的具体工厂类，无需修改原有代码，符合“开闭原则”。
    *   解耦：客户端代码与具体产品类解耦，只依赖于抽象产品和抽象工厂。
    *   单一职责原则：创建产品的职责被封装在工厂方法中。
*   缺点：
    *   类数量增加：每增加一个产品，就需要增加一个具体产品类和一个具体工厂类，导致类的数量增多，系统复杂度增加。
    *   层次结构复杂：当产品族和工厂族变得庞大时，类的层次结构会变得复杂。

#### 3、抽象工厂模式

原理与意图：

抽象工厂模式提供一个接口，用于创建一系列相关或相互依赖对象的家族，而无需指定它们具体的类。它允许客户端使用抽象接口来创建一组相关的产品，而无需知道这些产品的具体实现。

解决的问题：

*   当系统需要创建一组相关或相互依赖的对象，并且这些对象应该一起使用时。
*   当系统需要独立于这些对象的创建方式时。
*   当系统需要提供一个产品族的多个变体时。

核心思想：

1.  抽象产品（Abstract Product）：为每种类型的产品定义一个抽象接口。
2.  具体产品（Concrete Product）：实现抽象产品接口的具体产品类。
3.  抽象工厂（Abstract Factory）：声明一组用于创建抽象产品的方法，每个方法对应一个产品类型。
4.  具体工厂（Concrete Factory）：实现抽象工厂接口，负责创建特定产品家族的具体产品实例。

C++代码实现：

假设我们有一个跨平台的GUI库，需要创建不同操作系统风格的按钮和文本框。

```cpp
#include <iostream>
#include <string>

// 抽象产品A：按钮
class Button {
public:
    virtual ~Button() = default;
    virtual void paint() const = 0;
};

// 具体产品A1：Windows风格按钮
class WindowsButton : public Button {
public:
    void paint() const override {
        std::cout << "Rendering a Windows Button." << std::endl;
    }
};

// 具体产品A2：Mac风格按钮
class MacButton : public Button {
public:
    void paint() const override {
        std::cout << "Rendering a Mac Button." << std::endl;
    }
};

// 抽象产品B：文本框
class TextField {
public:
    virtual ~TextField() = default;
    virtual void display() const = 0;
};

// 具体产品B1：Windows风格文本框
class WindowsTextField : public TextField {
public:
    void display() const override {
        std::cout << "Displaying a Windows TextField." << std::endl;
    }
};

// 具体产品B2：Mac风格文本框
class MacTextField : public TextField {
public:
    void display() const override {
        std::cout << "Displaying a Mac TextField." << std::endl;
    }
};

// 抽象工厂：创建产品家族的接口
class GUIFactory {
public:
    virtual ~GUIFactory() = default;
    virtual Button* createButton() const = 0;
    virtual TextField* createTextField() const = 0;
};

// 具体工厂1：创建Windows风格的产品家族
class WindowsFactory : public GUIFactory {
public:
    Button* createButton() const override {
        return new WindowsButton();
    }
    TextField* createTextField() const override {
        return new WindowsTextField();
    }
};

// 具体工厂2：创建Mac风格的产品家族
class MacFactory : public GUIFactory {
public:
    Button* createButton() const override {
        return new MacButton();
    }
    TextField* createTextField() const override {
        return new MacTextField();
    }
};

// 客户端代码
void clientCode(GUIFactory* factory) {
    Button* button = factory->createButton();
    TextField* textField = factory->createTextField();

    button->paint();
    textField->display();

    delete button;
    delete textField;
}

// int main() {
//     std::cout << "Client: Testing with Windows factory." << std::endl;
//     WindowsFactory* windowsFactory = new WindowsFactory();
//     clientCode(windowsFactory);
//     delete windowsFactory;

//     std::cout << "\nClient: Testing with Mac factory." << std::endl;
//     MacFactory* macFactory = new MacFactory();
//     clientCode(macFactory);
//     delete macFactory;

//     return 0;
// }
```

使用场景：

*   多平台支持：当系统需要支持多个产品家族，并且这些产品家族中的产品是相互关联的，例如，不同操作系统下的GUI组件（Windows风格、Mac风格）。
*   配置管理：当系统需要根据不同的配置或环境创建不同的产品组合时。
*   产品族扩展：当需要方便地切换整个产品族时，而不需要修改客户端代码。

优缺点：

*   优点：
    *   隔离具体类：客户端代码与具体产品类完全解耦，只依赖于抽象接口。
    *   易于切换产品族：切换整个产品族非常容易，只需更换具体工厂即可。
    *   保证产品族一致性：一个具体工厂创建的产品都属于同一个产品族，保证了产品之间的一致性。
*   缺点：
    *   难以扩展新产品：如果需要增加新的产品类型（例如，除了按钮和文本框，还要增加复选框），就需要修改抽象工厂接口及其所有具体工厂的实现，这违反了“开闭原则”。
    *   类数量增加：每增加一个产品族或产品类型，都会导致类的数量增加，系统复杂度更高。

#### 4、建造者模式

原理与意图：

建造者模式将一个复杂对象的构建与它的表示分离，使得同样的构建过程可以创建不同的表示。它允许你分步骤创建复杂对象，并且可以控制构建过程的每个细节。

解决的问题：

*   当创建复杂对象的算法应该独立于该对象的组成部分以及它们的装配方式时。
*   当构建过程需要允许不同的表示时。
*   当一个对象的构造函数参数过多，或者构造过程复杂且包含多个可选步骤时。

核心思想：

1.  产品（Product）：被构建的复杂对象。它通常包含多个部分，这些部分可以通过不同的方式组合。
2.  抽象建造者（Builder）：定义创建产品各个部件的抽象接口，以及一个返回最终产品的方法。
3.  具体建造者（Concrete Builder）：实现抽象建造者接口，负责构建产品的具体部件，并组装产品。
4.  指挥者（Director）：负责安排构建过程，它接收一个建造者对象，并调用其构建部件的方法，但不关心具体部件的创建细节。

C++代码实现：

假设我们要构建一个复杂的汽车对象，它有引擎、车轮、车身等部件，并且可以有不同的配置（例如，跑车、SUV）。

```cpp
#include <iostream>
#include <string>
#include <vector>

// 产品：汽车
class Car {
public:
    void addPart(const std::string& part) {
        parts.push_back(part);
    }
    void show() const {
        std::cout << "Car parts: ";
        for (const auto& part : parts) {
            std::cout << part << " ";
        }
        std::cout << std::endl;
    }
private:
    std::vector<std::string> parts;
};

// 抽象建造者
class CarBuilder {
public:
    virtual ~CarBuilder() = default;
    virtual void buildEngine() = 0;
    virtual void buildWheels() = 0;
    virtual void buildBody() = 0;
    virtual Car* getResult() = 0;
};

// 具体建造者：跑车建造者
class SportsCarBuilder : public CarBuilder {
public:
    SportsCarBuilder() : car(new Car()) {}
    ~SportsCarBuilder() { delete car; }

    void buildEngine() override {
        car->addPart("Sports Engine");
    }
    void buildWheels() override {
        car->addPart("Sports Wheels");
    }
    void buildBody() override {
        car->addPart("Sports Body");
    }
    Car* getResult() override {
        return car; // 返回构建好的产品
    }
private:
    Car* car;
};

// 具体建造者：SUV建造者
class SUVBuilder : public CarBuilder {
public:
    SUVBuilder() : car(new Car()) {}
    ~SUVBuilder() { delete car; }

    void buildEngine() override {
        car->addPart("SUV Engine");
    }
    void buildWheels() override {
        car->addPart("SUV Wheels");
    }
    void buildBody() override {
        car->addPart("SUV Body");
    }
    Car* getResult() override {
        return car; // 返回构建好的产品
    }
private:
    Car* car;
};

// 指挥者
class Director {
public:
    void construct(CarBuilder* builder) {
        builder->buildEngine();
        builder->buildBody();
        builder->buildWheels();
    }
};

// int main() {
//     Director director;

//     SportsCarBuilder sportsCarBuilder;
//     director.construct(&sportsCarBuilder);
//     Car* sportsCar = sportsCarBuilder.getResult();
//     std::cout << "Constructed Sports Car: ";
//     sportsCar->show();
//     // 注意：getResult() 返回的是一个指针，需要客户端负责delete
//     // delete sportsCar; // 实际使用中，智能指针会更好地管理

//     SUVBuilder suvBuilder;
//     director.construct(&suvBuilder);
//     Car* suv = suvBuilder.getResult();
//     std::cout << "Constructed SUV: ";
//     suv->show();
//     // delete suv; // 实际使用中，智能指针会更好地管理

//     return 0;
// }
```

使用场景：

*   复杂对象的构建：当一个对象的构建过程非常复杂，包含多个步骤，并且这些步骤的顺序或组合方式可能不同时。
*   不同表示的创建：当同样的构建过程可以创建出不同表示（配置）的产品时。例如，生成不同格式的文档（HTML、PDF）、不同类型的计算机（台式机、笔记本）。
*   避免“构造函数爆炸”：当一个类的构造函数参数过多，导致难以维护和使用时，可以使用建造者模式来简化对象的创建。

优缺点：

*   优点：
    *   分离构建与表示：将复杂对象的构建过程与它的表示分离，使得构建过程可以独立于产品的具体构成。
    *   精细化控制：可以对产品的构建过程进行精细化控制，一步步地构建对象。
    *   易于扩展：增加新的具体建造者可以创建新的产品表示，符合“开闭原则”。
    *   减少构造函数参数：避免了复杂对象构造函数参数过多的问题。
*   缺点：
    *   类数量增加：每增加一个具体产品，就需要增加一个具体建造者，导致类的数量增多。
    *   产品内部结构变化困难：如果产品内部的组成部分变化频繁，抽象建造者和具体建造者都需要频繁修改。

#### 5、原型模式

原理与意图：

原型模式通过复制现有对象来创建新对象，而不是通过实例化类。它允许你创建对象的副本，而无需知道其具体类型。这种模式在创建复杂对象时特别有用，因为复制一个现有对象通常比从头开始创建一个新对象更高效。

解决的问题：

*   当创建新对象的成本（如时间、资源）很高时。
*   当需要创建大量相似对象，但它们的具体类型在运行时才能确定时。
*   当避免与对象创建相关的客户端代码时。

核心思想：

1.  抽象原型（Prototype）：声明一个克隆自身的接口（通常是一个`clone()`方法）。
2.  具体原型（Concrete Prototype）：实现克隆接口，负责复制自身。
3.  客户端（Client）：通过调用原型对象的克隆方法来创建新对象。

C++代码实现：

假设我们有一个图形编辑器，需要复制各种形状（圆形、矩形等）。

```cpp
#include <iostream>
#include <string>
#include <map>

// 抽象原型
class Shape {
public:
    virtual ~Shape() = default;
    virtual Shape* clone() const = 0; // 克隆自身的接口
    virtual void draw() const = 0;
};

// 具体原型：圆形
class Circle : public Shape {
public:
    Circle(int r = 0) : radius(r) {}
    Circle* clone() const override {
        return new Circle(*this); // 调用拷贝构造函数进行深拷贝或浅拷贝
    }
    void draw() const override {
        std::cout << "Drawing a Circle with radius: " << radius << std::endl;
    }
    void setRadius(int r) { radius = r; }
private:
    int radius;
};

// 具体原型：矩形
class Rectangle : public Shape {
public:
    Rectangle(int w = 0, int h = 0) : width(w), height(h) {}
    Rectangle* clone() const override {
        return new Rectangle(*this);
    }
    void draw() const override {
        std::cout << "Drawing a Rectangle with width: " << width << ", height: " << height << std::endl;
    }
    void setDimensions(int w, int h) { width = w; height = h; }
private:
    int width;
    int height;
};

// 原型管理器（可选）：用于管理和获取原型对象
class ShapeManager {
public:
    void registerShape(const std::string& key, Shape* prototype) {
        prototypes[key] = prototype;
    }
    Shape* createShape(const std::string& key) {
        if (prototypes.count(key)) {
            return prototypes[key]->clone();
        }
        return nullptr;
    }
    ~ShapeManager() {
        for (auto const& [key, val] : prototypes) {
            delete val;
        }
    }
private:
    std::map<std::string, Shape*> prototypes;
};

// int main() {
//     ShapeManager manager;

//     // 注册原型对象
//     manager.registerShape("circle", new Circle(10));
//     manager.registerShape("rectangle", new Rectangle(20, 30));

//     // 通过原型管理器创建新对象
//     Shape* circle1 = manager.createShape("circle");
//     circle1->draw();

//     Shape* rectangle1 = manager.createShape("rectangle");
//     rectangle1->draw();

//     // 修改副本，不影响原型
//     Circle* circle2 = static_cast<Circle*>(manager.createShape("circle"));
//     circle2->setRadius(15);
//     circle2->draw();

//     // 原始原型未改变
//     Shape* originalCircle = manager.createShape("circle");
//     originalCircle->draw();

//     delete circle1;
//     delete rectangle1;
//     delete circle2;
//     delete originalCircle;

//     return 0;
// }
```

使用场景：

*   创建复杂对象：当一个对象的创建过程非常复杂或耗时，但又需要创建大量相似对象时，通过复制现有对象可以提高效率。
*   避免子类化：当系统需要创建的对象类型在运行时才能确定，并且不希望通过子类化来创建新对象时。
*   动态加载：当需要动态加载或配置对象时，可以通过原型管理器来管理和创建对象。

优缺点：

*   优点：
    *   提高创建效率：通过复制现有对象来创建新对象，通常比从头开始创建更高效。
    *   避免子类化：无需为每个具体产品类都定义一个工厂类，减少了类的数量。
    *   运行时创建：可以在运行时动态地创建新对象，而无需硬编码具体类。
*   缺点：
    *   深拷贝/浅拷贝问题：如果原型对象包含引用类型成员，需要仔细处理深拷贝和浅拷贝问题，否则可能导致意外行为。
    *   复杂对象复制困难：如果对象内部结构复杂，包含循环引用等，实现正确的克隆可能非常困难。
    *   每个类都需要实现克隆：每个具体原型类都需要实现克隆接口，增加了代码量。

## 二、结构型模式

结构型模式关注如何将类和对象组合成更大的结构，以实现新的功能。它们描述了如何通过继承和组合来组织对象，从而形成更灵活、更高效的结构。

#### 1、适配器模式

原理与意图：

适配器模式（Adapter Pattern），也称为包装器（Wrapper）模式，允许一个接口不兼容的类与另一个类协同工作。它的核心思想是将一个类的接口转换成客户希望的另一个接口，使得原本由于接口不兼容而不能一起工作的那些类可以一起工作。

解决的问题：

*   当你想使用一个已经存在的类，但它的接口不符合你的需求时。
*   当你想创建一个可复用的类，该类可以与一些不相关的或不可预见的类（这些类接口可能不兼容）协同工作时。

核心思想：

适配器模式有两种主要实现方式：

1.  类适配器（Class Adapter）：通过多重继承实现。适配器类同时继承目标接口和被适配者类。这种方式在C++中比较常见，但由于C++多重继承的复杂性，有时会引入一些限制。
2.  对象适配器（Object Adapter）：通过组合实现。适配器类包含一个被适配者类的实例，并实现目标接口。这是更常用和推荐的方式，因为它更灵活，且不依赖于多重继承。

C++代码实现：

假设我们有一个旧的音频播放器（`OldAudioPlayer`），它只能播放MP3格式。现在我们有一个新的需求，需要播放WAV格式的音频，但我们不想修改旧播放器的代码。我们可以创建一个适配器来解决这个问题。

##### 对象适配器示例

```cpp
#include <iostream>
#include <string>

// 目标接口：新播放器期望的接口
class NewAudioPlayer {
public:
    virtual ~NewAudioPlayer() = default;
    virtual void playWAV(const std::string& filename) = 0;
};

// 被适配者：旧的音频播放器，只支持MP3
class OldAudioPlayer {
public:
    void playMP3(const std::string& filename) {
        std::cout << "Playing MP3 file: " << filename << std::endl;
    }
};

// 适配器：将OldAudioPlayer的接口适配到NewAudioPlayer的接口
class AudioAdapter : public NewAudioPlayer {
private:
    OldAudioPlayer* oldPlayer; // 包含被适配者实例

public:
    AudioAdapter() : oldPlayer(new OldAudioPlayer()) {}
    ~AudioAdapter() { delete oldPlayer; }

    void playWAV(const std::string& filename) override {
        std::cout << "Adapter converting WAV to MP3..." << std::endl;
        // 在这里进行格式转换（模拟），然后调用旧播放器的MP3播放方法
        oldPlayer->playMP3(filename); 
    }
};

// 客户端代码
void clientPlayAudio(NewAudioPlayer* player, const std::string& filename) {
    player->playWAV(filename);
}

// int main() {
//     // 直接使用新接口的播放器
//     // NewAudioPlayer* newPlayer = new ConcreteNewAudioPlayer(); // 假设有这样的实现
//     // clientPlayAudio(newPlayer, "song.wav");
//     // delete newPlayer;

//     // 使用适配器来播放WAV文件
//     NewAudioPlayer* adapter = new AudioAdapter();
//     clientPlayAudio(adapter, "another_song.wav");
//     delete adapter;

//     return 0;
// }
```

使用场景：

*   遗留系统集成：当需要集成一个接口不兼容的遗留系统或第三方库时。
*   统一接口：当需要统一多个不同接口的类的行为时，例如，将不同数据源的接口统一为一种。
*   代码复用：在不修改现有类的前提下，使其能够与新的接口协同工作，从而实现代码复用。

优缺点：

*   优点：
    *   解耦：将客户端与被适配者解耦，客户端只依赖于目标接口。
    *   复用性：可以复用现有的类，而无需修改其源代码。
    *   灵活性：可以随时更换适配器或被适配者，而不会影响客户端代码。
*   缺点：
    *   增加复杂度：引入了新的类（适配器），增加了系统的复杂性。
    *   性能开销：在某些情况下，适配器可能会引入一些性能开销（例如，数据转换）。
    *   类适配器限制：类适配器受限于C++的单继承特性，且无法适配被适配者的子类。

#### 2、桥接模式

原理与意图：

桥接模式（Bridge Pattern）旨在将抽象部分与实现部分分离，使它们可以独立地变化。它通过组合而不是继承来连接抽象和实现，从而避免了类爆炸问题，并提高了系统的灵活性和可扩展性。

解决的问题：

*   当一个类存在两个或多个独立变化的维度时，例如，一个图形的形状和颜色，或者一个消息的类型和发送方式。
*   当抽象和实现之间需要动态地切换时。
*   当不希望在编译时绑定抽象和实现时。

核心思想：

桥接模式的核心是将一个大类或一系列紧密相关的类分成两个独立的层次结构：抽象（Abstraction）和实现（Implementation）。这两个层次结构通过组合（而不是继承）连接起来，形成一座“桥”。

1.  抽象（Abstraction）：定义抽象的接口，并包含一个指向实现接口的引用。
2.  扩展抽象（Refined Abstraction）：抽象的子类，扩展抽象接口。
3.  实现（Implementation）：定义实现类的接口，不与抽象接口直接关联。
4.  具体实现（Concrete Implementation）：实现实现接口的具体类。

C++代码实现：

假设我们有一个图形绘制系统，图形有不同的形状（圆形、矩形）和不同的绘制API（OpenGL、DirectX）。

```cpp
#include <iostream>
#include <string>

// 实现部分接口：绘制API
class DrawingAPI {
public:
    virtual ~DrawingAPI() = default;
    virtual void drawCircle(double x, double y, double radius) = 0;
    virtual void drawRectangle(double x, double y, double width, double height) = 0;
};

// 具体实现1：OpenGL绘制API
class OpenGLAPI : public DrawingAPI {
public:
    void drawCircle(double x, double y, double radius) override {
        std::cout << "OpenGL: Drawing Circle at (" << x << "," << y << ") with radius " << radius << std::endl;
    }
    void drawRectangle(double x, double y, double width, double height) override {
        std::cout << "OpenGL: Drawing Rectangle at (" << x << "," << y << ") with width " << width << ", height " << height << std::endl;
    }
};

// 具体实现2：DirectX绘制API
class DirectXAPI : public DrawingAPI {
public:
    void drawCircle(double x, double y, double radius) override {
        std::cout << "DirectX: Drawing Circle at (" << x << "," << y << ") with radius " << radius << std::endl;
    }
    void drawRectangle(double x, double y, double width, double height) override {
        std::cout << "DirectX: Drawing Rectangle at (" << x << "," << y << ") with width " << width << ", height " << height << std::endl;
    }
};

// 抽象部分：形状
class Shape {
protected:
    DrawingAPI* drawingAPI; // 包含实现部分的引用
public:
    Shape(DrawingAPI* api) : drawingAPI(api) {}
    virtual ~Shape() = default;
    virtual void draw() = 0;
};

// 扩展抽象1：圆形
class CircleShape : public Shape {
private:
    double x, y, radius;
public:
    CircleShape(double x, double y, double radius, DrawingAPI* api)
        : Shape(api), x(x), y(y), radius(radius) {}
    void draw() override {
        drawingAPI->drawCircle(x, y, radius);
    }
};

// 扩展抽象2：矩形
class RectangleShape : public Shape {
private:
    double x, y, width, height;
public:
    RectangleShape(double x, double y, double width, double height, DrawingAPI* api)
        : Shape(api), x(x), y(y), width(width), height(height) {}
    void draw() override {
        drawingAPI->drawRectangle(x, y, width, height);
    }
};

// int main() {
//     // 使用OpenGL绘制API
//     DrawingAPI* opengl = new OpenGLAPI();
//     Shape* circle1 = new CircleShape(1.0, 2.0, 3.0, opengl);
//     Shape* rect1 = new RectangleShape(4.0, 5.0, 6.0, 7.0, opengl);

//     circle1->draw();
//     rect1->draw();

//     delete circle1;
//     delete rect1;
//     delete opengl;

//     std::cout << "\n";

//     // 使用DirectX绘制API
//     DrawingAPI* directx = new DirectXAPI();
//     Shape* circle2 = new CircleShape(8.0, 9.0, 10.0, directx);
//     Shape* rect2 = new RectangleShape(11.0, 12.0, 13.0, 14.0, directx);

//     circle2->draw();
//     rect2->draw();

//     delete circle2;
//     delete rect2;
//     delete directx;

//     return 0;
// }
```

使用场景：

*   多维度变化：当一个系统需要在抽象和实现之间有多个独立变化的维度时。例如，操作系统和文件系统，或者消息的发送方式和消息内容。
*   避免类爆炸：当使用继承会导致类的数量急剧增加时（例如，`CircleOpenGL`, `CircleDirectX`, `RectangleShapeOpenGL`, `RectangleShapeDirectX`...）。
*   运行时切换实现：当需要在运行时动态地切换对象的实现时。

优缺点：

*   优点：
    *   分离抽象与实现：将抽象和实现分离，使它们可以独立地变化和扩展，符合“开闭原则”。
    *   减少类数量：避免了继承带来的类爆炸问题，降低了系统复杂度。
    *   提高灵活性：可以在运行时动态地切换实现，而无需修改客户端代码。
*   缺点：
    *   增加复杂度：引入了两个层次结构，增加了系统的理解和设计难度。
    *   不适用于所有场景：只有当存在两个或多个独立变化的维度时，才适合使用桥接模式。

#### 3、组合模式

原理与意图：

组合模式（Composite Pattern）允许你将对象组合成树形结构以表示“部分-整体”的层次结构。它使得客户端对单个对象和组合对象的使用具有一致性。这意味着客户端可以像处理单个对象一样处理组合对象，而无需区分它们。

解决的问题：

*   当需要表示对象的部分-整体层次结构时。
*   当希望客户端能够忽略组合对象和单个对象的区别，统一地处理它们时。

核心思想：

组合模式的关键在于定义一个抽象组件（Component）接口，它既可以表示单个对象（叶子节点），也可以表示组合对象（枝节点）。

1.  组件（Component）：为组合中的对象声明接口，该接口定义了所有对象（包括叶子和枝节点）的通用行为，以及管理子组件的方法（对于叶子节点，这些方法通常是空实现或抛出异常）。
2.  叶子（Leaf）：在组合中表示叶子对象，叶子没有子节点。它实现了组件接口中定义的所有行为。
3.  组合（Composite）：在组合中表示枝节点，它包含子组件，并实现组件接口中定义的所有行为。它通常会实现管理子组件的方法，并将操作委托给其子组件。

C++代码实现：

假设我们有一个文件系统，包含文件（叶子）和文件夹（组合）。

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // For std::remove

// 组件接口
class FileSystemComponent {
public:
    virtual ~FileSystemComponent() = default;
    virtual void display(int depth = 0) const = 0; // 显示组件信息
    virtual void add(FileSystemComponent* component) { /* 默认空实现或抛出异常 */ }
    virtual void remove(FileSystemComponent* component) { /* 默认空实现或抛出异常 */ }
    virtual FileSystemComponent* getChild(int i) { return nullptr; /* 默认空实现 */ }
};

// 叶子：文件
class File : public FileSystemComponent {
private:
    std::string name;
public:
    File(const std::string& n) : name(n) {}
    void display(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "- " << name << " (File)" << std::endl;
    }
};

// 组合：文件夹
class Directory : public FileSystemComponent {
private:
    std::string name;
    std::vector<FileSystemComponent*> children;
public:
    Directory(const std::string& n) : name(n) {}
    ~Directory() {
        for (FileSystemComponent* comp : children) {
            delete comp;
        }
    }

    void add(FileSystemComponent* component) override {
        children.push_back(component);
    }
    void remove(FileSystemComponent* component) override {
        // 移除指定组件
        children.erase(std::remove(children.begin(), children.end(), component), children.end());
    }
    FileSystemComponent* getChild(int i) override {
        if (i >= 0 && i < children.size()) {
            return children[i];
        }
        return nullptr;
    }
    void display(int depth = 0) const override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "+ " << name << " (Directory)" << std::endl;
        for (const auto& comp : children) {
            comp->display(depth + 1);
        }
    }
};

// int main() {
//     // 构建文件系统结构
//     Directory* root = new Directory("Root");
//     Directory* folder1 = new Directory("Folder1");
//     Directory* folder2 = new Directory("Folder2");

//     File* file1 = new File("file1.txt");
//     File* file2 = new File("file2.doc");
//     File* file3 = new File("file3.pdf");

//     root->add(folder1);
//     root->add(file3);

//     folder1->add(file1);
//     folder1->add(folder2);

//     folder2->add(file2);

//     // 显示整个文件系统结构
//     root->display();

//     // 清理内存
//     delete root;

//     return 0;
// }
```

使用场景：

*   部分-整体层次结构：当需要表示对象的部分-整体层次结构时，例如，文件系统中的文件和文件夹、组织结构中的部门和员工、GUI中的容器和组件。
*   统一处理：当希望客户端能够统一处理单个对象和组合对象时，而无需区分它们。
*   树形结构：当需要构建和操作树形数据结构时。

优缺点：

*   优点：
    *   简化客户端代码：客户端可以统一处理单个对象和组合对象，无需区分，简化了代码。
    *   易于扩展：增加新的叶子或组合类型很容易，符合“开闭原则”。
    *   清晰的层次结构：清晰地表示了部分-整体的层次结构。
*   缺点：
    *   设计复杂性：如果组件接口定义不当，可能导致叶子节点实现不相关的方法，或者组合节点无法实现某些方法。
    *   类型安全问题：在某些情况下，可能需要运行时类型检查来区分叶子和组合，这会降低类型安全性。

#### 4、装饰器模式

原理与意图：

装饰器模式（Decorator Pattern）允许在不改变原有对象结构的情况下，动态地给对象添加额外的职责。它通过将对象包装在一个或多个装饰器对象中来实现，这些装饰器对象都实现了相同的接口，从而使得客户端可以透明地使用被装饰的对象。

解决的问题：

*   当需要动态地给一个对象添加额外的功能，而不是通过继承来扩展功能时。
*   当继承会导致子类数量爆炸时（例如，咖啡有加糖、加奶、加糖加奶等多种组合）。
*   当对象的功能可以被撤销时。

核心思想：

装饰器模式的关键在于创建一个与被装饰对象具有相同接口的装饰器类，并在装饰器类中包含一个指向被装饰对象的引用。这样，装饰器就可以在调用被装饰对象的方法前后添加额外的行为。

1.  组件（Component）：定义一个抽象接口，用于规范被装饰对象和装饰器对象的行为。
2.  具体组件（Concrete Component）：实现组件接口，是被装饰的原始对象。
3.  装饰器（Decorator）：抽象装饰器类，实现组件接口，并包含一个指向组件对象的引用。它通常会将请求转发给被装饰对象。
4.  具体装饰器（Concrete Decorator）：扩展装饰器类，向被装饰对象添加新的职责。

C++代码实现：

假设我们有一个咖啡店系统，咖啡（`Beverage`）可以添加牛奶（`MilkDecorator`）和糖（`SugarDecorator`）。

```cpp
#include <iostream>
#include <string>

// 组件接口：饮料
class Beverage {
public:
    virtual ~Beverage() = default;
    virtual std::string getDescription() const = 0;
    virtual double getCost() const = 0;
};

// 具体组件：浓缩咖啡
class Espresso : public Beverage {
public:
    std::string getDescription() const override {
        return "Espresso";
    }
    double getCost() const override {
        return 1.99;
    }
};

// 具体组件：美式咖啡
class Americano : public Beverage {
public:
    std::string getDescription() const override {
        return "Americano";
    }
    double getCost() const override {
        return 2.50;
    }
};

// 抽象装饰器
class CondimentDecorator : public Beverage {
protected:
    Beverage* beverage; // 包含被装饰对象的引用
public:
    CondimentDecorator(Beverage* b) : beverage(b) {}
    ~CondimentDecorator() { delete beverage; } // 负责销毁被装饰对象
};

// 具体装饰器：牛奶
class MilkDecorator : public CondimentDecorator {
public:
    MilkDecorator(Beverage* b) : CondimentDecorator(b) {}
    std::string getDescription() const override {
        return beverage->getDescription() + ", Milk";
    }
    double getCost() const override {
        return beverage->getCost() + 0.50;
    }
};

// 具体装饰器：糖
class SugarDecorator : public CondimentDecorator {
public:
    SugarDecorator(Beverage* b) : CondimentDecorator(b) {}
    std::string getDescription() const override {
        return beverage->getDescription() + ", Sugar";
    }
    double getCost() const override {
        return beverage->getCost() + 0.20;
    }
};

// int main() {
//     // 一杯纯浓缩咖啡
//     Beverage* espresso = new Espresso();
//     std::cout << espresso->getDescription() << " Cost: $" << espresso->getCost() << std::endl;

//     // 一杯加牛奶的浓缩咖啡
//     Beverage* espressoWithMilk = new MilkDecorator(new Espresso());
//     std::cout << espressoWithMilk->getDescription() << " Cost: $" << espressoWithMilk->getCost() << std::endl;

//     // 一杯加牛奶和糖的美式咖啡
//     Beverage* americanoWithMilkAndSugar = new SugarDecorator(new MilkDecorator(new Americano()));
//     std::cout << americanoWithMilkAndSugar->getDescription() << " Cost: $" << americanoWithMilkAndSugar->getCost() << std::endl;

//     delete espresso;
//     delete espressoWithMilk;
//     delete americanoWithMilkAndSugar;

//     return 0;
// }
```

使用场景：

*   动态添加功能：当需要在运行时动态地给对象添加或撤销功能时。
*   避免继承爆炸：当使用继承来扩展功能会导致类的数量急剧增加时（例如，各种组合的咖啡）。
*   透明地扩展功能：当希望客户端能够透明地使用被装饰对象和装饰器对象时。

优缺点：

*   优点：
    *   比继承更灵活：避免了继承带来的静态性和类爆炸问题，可以动态地组合功能。
    *   符合开闭原则：可以在不修改原有代码的情况下，扩展对象的功能。
    *   高内聚：每个装饰器只负责添加一项特定的功能。
*   缺点：
    *   增加复杂度：引入了更多的对象和类，增加了系统的复杂性。
    *   多层包装：如果过度使用，可能会导致对象被多层包装，增加调试难度。
    *   装饰器顺序：不同的装饰器顺序可能会导致不同的行为。

#### 5、外观模式

原理与意图：

外观模式（Facade Pattern）为子系统中的一组接口提供一个统一的接口。外观模式定义了一个高层接口，这个接口使得子系统更容易使用。它隐藏了子系统的复杂性，并提供了一个简化的接口，供客户端使用。

解决的问题：

*   当一个子系统非常复杂，包含许多类和接口，客户端难以直接使用时。
*   当需要将客户端与子系统解耦时。
*   当需要为子系统提供一个简化的入口点时。

核心思想：

外观模式的核心是创建一个外观（Facade）类，它提供一个简化的接口，并将客户端的请求委派给子系统中相应的对象。外观类本身不包含任何业务逻辑，它只是一个协调者。

1.  外观（Facade）：提供一个统一的、简化的接口，隐藏子系统的复杂性，并将客户端的请求委派给子系统中的相应对象。
2.  子系统类（Subsystem Classes）：实现子系统的具体功能，它们之间可能存在复杂的交互关系。

C++代码实现：

假设我们有一个家庭影院系统，包含DVD播放器、投影仪、音响等复杂组件。

```cpp
#include <iostream>
#include <string>

// 子系统类1：DVD播放器
class DVDPlayer {
public:
    void on() { std::cout << "DVD Player On" << std::endl; }
    void off() { std::cout << "DVD Player Off" << std::endl; }
    void play(const std::string& movie) { std::cout << "Playing movie: " << movie << std::endl; }
    void stop() { std::cout << "Movie stopped" << std::endl; }
};

// 子系统类2：投影仪
class Projector {
public:
    void on() { std::cout << "Projector On" << std::endl; }
    void off() { std::cout << "Projector Off" << std::endl; }
    void wideScreenMode() { std::cout << "Projector in widescreen mode" << std::endl; }
};

// 子系统类3：音响
class Amplifier {
public:
    void on() { std::cout << "Amplifier On" << std::endl; }
    void off() { std::cout << "Amplifier Off" << std::endl; }
    void setVolume(int volume) { std::cout << "Amplifier volume set to " << volume << std::endl; }
};

// 外观类：家庭影院外观
class HomeTheaterFacade {
private:
    DVDPlayer* dvdPlayer;
    Projector* projector;
    Amplifier* amplifier;

public:
    HomeTheaterFacade() {
        dvdPlayer = new DVDPlayer();
        projector = new Projector();
        amplifier = new Amplifier();
    }
    ~HomeTheaterFacade() {
        delete dvdPlayer;
        delete projector;
        delete amplifier;
    }

    // 观看电影的简化操作
    void watchMovie(const std::string& movie) {
        std::cout << "Get ready to watch a movie..." << std::endl;
        amplifier->on();
        amplifier->setVolume(10);
        projector->on();
        projector->wideScreenMode();
        dvdPlayer->on();
        dvdPlayer->play(movie);
    }

    // 结束电影的简化操作
    void endMovie() {
        std::cout << "Shutting down home theater..." << std::endl;
        dvdPlayer->stop();
        dvdPlayer->off();
        projector->off();
        amplifier->off();
    }
};

// int main() {
//     HomeTheaterFacade* homeTheater = new HomeTheaterFacade();

//     homeTheater->watchMovie("The Matrix");
//     std::cout << "\n";
//     homeTheater->endMovie();

//     delete homeTheater;

//     return 0;
// }
```

使用场景：

*   简化复杂子系统：当一个子系统非常复杂，包含大量类和接口，客户端难以理解和使用时。
*   解耦客户端与子系统：当需要将客户端代码与子系统的内部实现解耦，使得子系统的变化不会影响客户端时。
*   分层架构：在多层架构中，为每一层提供一个外观，以简化层与层之间的交互。

优缺点：

*   优点：
    *   简化接口：为复杂的子系统提供一个简化的接口，降低了客户端的使用难度。
    *   解耦：将客户端与子系统的内部实现解耦，提高了系统的独立性和可维护性。
    *   提高安全性：客户端无需直接访问子系统内部，降低了误用或滥用的风险。
*   缺点：
    *   违反开闭原则：如果子系统中的接口发生变化，可能需要修改外观类，这在一定程度上违反了开闭原则。
    *   功能限制：外观模式提供的接口通常是简化的，如果客户端需要更复杂的功能，可能仍然需要直接访问子系统。

#### 6、享元模式

原理与意图：

享元模式（Flyweight Pattern）旨在通过共享大量细粒度对象来减少内存使用和提高性能。它通过将对象的状态分为内部状态（Intrinsic State）和外部状态（Extrinsic State）来实现。内部状态是对象固有的、不随环境变化的，可以被共享；外部状态是对象随环境变化的，不能被共享，需要由客户端提供。

解决的问题：

*   当一个应用程序需要创建大量细粒度对象，导致内存消耗过大时。
*   当这些对象的大部分状态可以被外部化，从而实现共享时。

核心思想：

1.  享元（Flyweight）：定义一个接口，包含接收外部状态作为参数的方法。
2.  具体享元（Concrete Flyweight）：实现享元接口，存储内部状态，并根据外部状态执行操作。
3.  享元工厂（Flyweight Factory）：负责创建和管理享元对象。它维护一个享元池，当客户端请求一个享元时，如果池中已存在，则直接返回；否则，创建新的享元并放入池中。
4.  客户端（Client）：维护外部状态，并向享元传递外部状态。

C++代码实现：

假设我们有一个文本编辑器，需要显示大量字符。每个字符的字体、颜色等是内部状态（可以共享），而字符的位置是外部状态（不能共享）。

```cpp
#include <iostream>
#include <string>
#include <map>

// 享元接口：字符
class Character {
public:
    virtual ~Character() = default;
    virtual void display(int x, int y) const = 0; // x, y 是外部状态
};

// 具体享元：具体字符（存储内部状态：字体、颜色等）
class ConcreteCharacter : public Character {
private:
    char symbol; // 内部状态
    std::string font; // 内部状态
    std::string color; // 内部状态

public:
    ConcreteCharacter(char s, const std::string& f, const std::string& c)
        : symbol(s), font(f), color(c) {
        std::cout << "Creating ConcreteCharacter: " << symbol << " (" << font << ", " << color << ")" << std::endl;
    }

    void display(int x, int y) const override {
        std::cout << "Displaying character '" << symbol << "' at (" << x << "," << y << ") with font '


    "' << font << "', color '


    "' << color << "'." << std::endl;
    }
};

// 享元工厂：管理和创建享元对象
class CharacterFactory {
private:
    std::map<std::string, Character*> characters; // 享元池

    // 辅助函数，用于生成享元键
    std::string getKey(char symbol, const std::string& font, const std::string& color) const {
        return std::string(1, symbol) + "_" + font + "_" + color;
    }

public:
    ~CharacterFactory() {
        for (auto const& [key, val] : characters) {
            delete val;
        }
    }

    Character* getCharacter(char symbol, const std::string& font, const std::string& color) {
        std::string key = getKey(symbol, font, color);
        if (characters.find(key) == characters.end()) {
            characters[key] = new ConcreteCharacter(symbol, font, color);
        }
        return characters[key];
    }

    int getNumberOfFlyweights() const {
        return characters.size();
    }
};

// int main() {
//     CharacterFactory factory;

//     // 客户端使用享元对象
//     Character* charA1 = factory.getCharacter('A', "Arial", "Black");
//     charA1->display(10, 20);

//     Character* charB1 = factory.getCharacter('B', "Times New Roman", "Blue");
//     charB1->display(30, 20);

//     Character* charA2 = factory.getCharacter('A', "Arial", "Black"); // 共享已存在的享元
//     charA2->display(50, 20);

//     Character* charC1 = factory.getCharacter('C', "Arial", "Black");
//     charC1->display(70, 20);

//     std::cout << "\nTotal number of unique flyweight characters created: " << factory.getNumberOfFlyweights() << std::endl;

//     // 预期输出：
//     // Creating ConcreteCharacter: A (Arial, Black)
//     // Displaying character 'A' at (10,20) with font 'Arial', color 'Black'.
//     // Creating ConcreteCharacter: B (Times New Roman, Blue)
//     // Displaying character 'B' at (30,20) with font 'Times New Roman', color 'Blue'.
//     // Displaying character 'A' at (50,20) with font 'Arial', color 'Black'.
//     // Creating ConcreteCharacter: C (Arial, Black)
//     // Displaying character 'C' at (70,20) with font 'Arial', color 'Black'.
//     //
//     // Total number of unique flyweight characters created: 3

//     return 0;
// }
```

使用场景：

*   大量细粒度对象：当系统中存在大量相似的对象，且这些对象的内部状态可以被共享时。例如，文本编辑器中的字符、游戏中的树木或草地。
*   内存优化：当内存资源有限，需要通过共享对象来减少内存消耗时。

优缺点：

*   优点：
    *   减少内存占用：通过共享内部状态，极大地减少了系统中对象的数量，从而降低了内存消耗。
    *   提高性能：减少了对象的创建和销毁开销，提高了系统性能。
*   缺点：
    *   增加复杂性：将对象的状态分为内部和外部，增加了系统的设计和实现复杂性。
    *   运行时开销：需要额外的逻辑来管理享元池和传递外部状态，可能会带来一些运行时开销。
    *   不适用于所有场景：只有当对象存在大量可共享的内部状态时，才适合使用享元模式。

#### 7、代理模式

原理与意图：

代理模式（Proxy Pattern）为另一个对象提供一个替身或占位符，以控制对这个对象的访问。代理对象和真实对象实现相同的接口，使得客户端可以透明地使用代理对象，而代理对象则在内部管理对真实对象的访问。

解决的问题：

*   当需要控制对某个对象的访问时。
*   当需要在访问对象时添加额外的功能，例如延迟加载、权限控制、日志记录、缓存等。

核心思想：

代理模式的关键在于代理对象和真实对象都实现同一个接口，这样客户端就可以像使用真实对象一样使用代理对象。代理对象在接收到客户端的请求后，可以根据需要执行一些预处理或后处理操作，然后将请求转发给真实对象。

1.  抽象主题（Subject）：定义真实对象和代理对象共同实现的接口，使得客户端可以透明地使用它们。
2.  真实主题（Real Subject）：被代理的真实对象，包含核心业务逻辑。
3.  代理（Proxy）：实现抽象主题接口，并包含一个指向真实主题的引用。它控制对真实主题的访问，并可以在访问前后执行额外的操作。

C++代码实现：

假设我们有一个图片加载器，加载大图片可能很耗时。我们可以使用代理模式来实现图片的延迟加载。

```cpp
#include <iostream>
#include <string>

// 抽象主题：图片接口
class Image {
public:
    virtual ~Image() = default;
    virtual void display() = 0;
};

// 真实主题：真实图片加载器（加载大图很耗时）
class RealImage : public Image {
private:
    std::string filename;
    void loadImageFromServer() {
        std::cout << "Loading " << filename << " from server... (This takes time)" << std::endl;
        // 模拟耗时操作
        // std::this_thread::sleep_for(std::chrono::seconds(2));
    }

public:
    RealImage(const std::string& fn) : filename(fn) {
        loadImageFromServer(); // 构造时立即加载
    }
    void display() override {
        std::cout << "Displaying " << filename << std::endl;
    }
};

// 代理：图片代理（实现延迟加载）
class ProxyImage : public Image {
private:
    std::string filename;
    RealImage* realImage; // 真实对象的引用

public:
    ProxyImage(const std::string& fn) : filename(fn), realImage(nullptr) {}
    ~ProxyImage() {
        if (realImage) {
            delete realImage;
        }
    }

    void display() override {
        if (realImage == nullptr) {
            // 只有在第一次需要显示时才加载真实图片
            realImage = new RealImage(filename);
        }
        realImage->display();
    }
};

// int main() {
//     // 客户端使用代理对象
//     Image* image1 = new ProxyImage("photo1.jpg");
//     Image* image2 = new ProxyImage("photo2.jpg");

//     std::cout << "\nFirst call to display image1:" << std::endl;
//     image1->display(); // 第一次调用，会加载真实图片

//     std::cout << "\nSecond call to display image1:" << std::endl;
//     image1->display(); // 第二次调用，直接显示，不再加载

//     std::cout << "\nFirst call to display image2:" << std::endl;
//     image2->display(); // 第一次调用，会加载真实图片

//     delete image1;
//     delete image2;

//     return 0;
// }
```

使用场景：

*   远程代理（Remote Proxy）：为远程对象提供本地代理，隐藏远程通信的复杂性。
*   虚拟代理（Virtual Proxy）：实现对象的延迟加载，只有在真正需要时才创建和加载真实对象，例如加载大图片、大文件。
*   保护代理（Protection Proxy）：控制对真实对象的访问权限，例如根据用户角色进行权限验证。
*   智能引用（Smart Reference）：在访问真实对象时执行额外的操作，例如引用计数、日志记录、缓存等。

优缺点：

*   优点：
    *   控制访问：可以在客户端和真实对象之间插入一层，从而控制对真实对象的访问。
    *   功能增强：可以在不修改真实对象的情况下，为真实对象添加额外的功能。
    *   延迟加载：可以实现对象的延迟加载，提高系统性能。
*   缺点：
    *   增加复杂性：引入了新的类，增加了系统的复杂性。
    *   性能开销：在某些情况下，代理可能会引入一些性能开销。
    *   滥用：如果过度使用，可能会导致代码难以理解和维护。

## 三、行为型模式

行为型模式关注对象之间的职责分配和交互方式。它们描述了对象和类如何协同工作，以完成某个任务，从而提高系统的灵活性和可复用性。

#### 1、责任链模式

原理与意图：

责任链模式（Chain of Responsibility Pattern）使多个对象都有机会处理请求，从而避免请求的发送者和接收者之间的耦合关系。将这些对象连成一条链，并沿着这条链传递该请求，直到有一个对象处理它为止。

解决的问题：

*   当有多个对象可以处理一个请求，但具体由哪个对象处理在运行时才能确定时。
*   当不希望请求的发送者与接收者之间存在紧密的耦合关系时。
*   当需要动态地指定处理请求的对象集合时。

核心思想：

责任链模式的核心是构建一个处理者（Handler）链。每个处理者都包含对下一个处理者的引用。当一个请求到达链的头部时，它会沿着链依次传递，直到某个处理者能够处理该请求为止。如果一个处理者无法处理请求，它会将请求传递给链中的下一个处理者。

1.  抽象处理者（Handler）：定义一个处理请求的接口，并实现对下一个处理者的引用。
2.  具体处理者（Concrete Handler）：实现抽象处理者接口，处理它所负责的请求。如果它不能处理该请求，则将请求转发给它的下一个处理者。
3.  客户端（Client）：创建责任链，并向链中的第一个处理者发送请求。

C++代码实现：

假设我们有一个请假审批系统，不同天数的请假需要不同级别的领导审批。

```cpp
#include <iostream>
#include <string>

// 抽象处理者：审批人
class Approver {
protected:
    Approver* nextApprover; // 下一个审批人

public:
    Approver() : nextApprover(nullptr) {}
    virtual ~Approver() = default;

    void setNextApprover(Approver* next) {
        this->nextApprover = next;
    }

    virtual void processRequest(int days) = 0;
};

// 具体处理者：小组长
class TeamLeader : public Approver {
public:
    void processRequest(int days) override {
        if (days <= 3) {
            std::cout << "Team Leader approved " << days << " days leave." << std::endl;
        } else if (nextApprover != nullptr) {
            std::cout << "Team Leader cannot approve " << days << " days leave, passing to Manager." << std::endl;
            nextApprover->processRequest(days);
        } else {
            std::cout << "No one can approve " << days << " days leave." << std::endl;
        }
    }
};

// 具体处理者：经理
class Manager : public Approver {
public:
    void processRequest(int days) override {
        if (days <= 7) {
            std::cout << "Manager approved " << days << " days leave." << std::endl;
        } else if (nextApprover != nullptr) {
            std::cout << "Manager cannot approve " << days << " days leave, passing to Director." << std::endl;
            nextApprover->processRequest(days);
        } else {
            std::cout << "No one can approve " << days << " days leave." << std::endl;
        }
    }
};

// 具体处理者：总监
class Director : public Approver {
public:
    void processRequest(int days) override {
        if (days <= 30) {
            std::cout << "Director approved " << days << " days leave." << std::endl;
        } else if (nextApprover != nullptr) {
            std::cout << "Director cannot approve " << days << " days leave, passing to CEO." << std::endl;
            nextApprover->processRequest(days);
        } else {
            std::cout << "No one can approve " << days << " days leave." << std::endl;
        }
    }
};

// int main() {
//     // 构建责任链
//     TeamLeader* leader = new TeamLeader();
//     Manager* manager = new Manager();
//     Director* director = new Director();

//     leader->setNextApprover(manager);
//     manager->setNextApprover(director);
//     // director->setNextApprover(nullptr); // 链的末端

//     // 发送请求
//     leader->processRequest(2);  // Team Leader approved
//     leader->processRequest(5);  // Manager approved
//     leader->processRequest(15); // Director approved
//     leader->processRequest(40); // No one can approve

//     // 清理内存
//     delete leader;
//     delete manager;
//     delete director;

//     return 0;
// }
```

使用场景：

*   审批流程：如请假、报销等需要多级审批的流程。
*   事件处理：如GUI事件处理，事件可以沿着组件层次结构传递。
*   日志记录：不同级别的日志（Debug, Info, Error）可以由不同的处理器处理。
*   过滤器/拦截器：在请求到达目标之前，经过一系列的过滤器处理。

优缺点：

*   优点：
    *   降低耦合度：请求的发送者和接收者之间解耦，发送者无需知道哪个对象会处理请求。
    *   增强灵活性：可以动态地增加、删除或重新组织处理者，改变处理请求的顺序。
    *   符合开闭原则：增加新的处理者无需修改现有代码。
*   缺点：
    *   请求不被处理：如果链中没有合适的处理者，请求可能得不到处理，导致程序错误。
    *   调试困难：链的结构和请求的传递路径可能不直观，增加了调试的难度。
    *   性能问题：如果链过长，或者每个处理者的处理逻辑复杂，可能会影响性能。

#### 2、命令模式

原理与意图：

命令模式（Command Pattern）将一个请求封装为一个对象，从而使你可用不同的请求、队列或日志来参数化客户端。命令模式也支持可撤销的操作。它将请求的发送者和接收者解耦，使得发送者无需知道接收者的具体操作。

解决的问题：

*   当需要将请求的发送者和接收者解耦时。
*   当需要支持可撤销的操作时。
*   当需要将操作排队、记录日志或支持宏命令（组合多个命令）时。

核心思想：

命令模式的核心是将一个操作封装成一个独立的命令对象。这个命令对象包含了执行操作所需的所有信息（接收者、方法、参数）。

1.  命令（Command）：声明一个执行操作的接口（通常是`execute()`方法）。
2.  具体命令（Concrete Command）：实现命令接口，将一个接收者对象绑定到一个动作，并调用接收者相应的操作。
3.  接收者（Receiver）：执行与请求相关的操作。任何类都可以作为接收者。
4.  调用者（Invoker）：负责调用命令对象，它持有命令对象，但不关心命令的具体实现。
5.  客户端（Client）：创建具体命令对象，并设置其接收者。

C++代码实现：

假设我们有一个遥控器，可以控制灯的开关。

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <memory> // For std::unique_ptr

// 接收者：灯
class Light {
public:
    void turnOn() {
        std::cout << "Light is ON" << std::endl;
    }
    void turnOff() {
        std::cout << "Light is OFF" << std::endl;
    }
};

// 命令接口
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0; // 支持撤销操作
};

// 具体命令：开灯命令
class LightOnCommand : public Command {
private:
    Light* light; // 接收者

public:
    LightOnCommand(Light* l) : light(l) {}
    void execute() override {
        light->turnOn();
    }
    void undo() override {
        light->turnOff(); // 撤销开灯就是关灯
    }
};

// 具体命令：关灯命令
class LightOffCommand : public Command {
private:
    Light* light;

public:
    LightOffCommand(Light* l) : light(l) {}
    void execute() override {
        light->turnOff();
    }
    void undo() override {
        light->turnOn(); // 撤销关灯就是开灯
    }
};

// 调用者：遥控器
class RemoteControl {
private:
    std::unique_ptr<Command> onCommand; // 使用智能指针管理命令对象
    std::unique_ptr<Command> offCommand;
    std::unique_ptr<Command> lastCommand; // 用于撤销

public:
    void setCommands(std::unique_ptr<Command> onCmd, std::unique_ptr<Command> offCmd) {
        onCommand = std::move(onCmd);
        offCommand = std::move(offCmd);
    }

    void pressOnButton() {
        if (onCommand) {
            onCommand->execute();
            lastCommand = std::move(onCommand); // 记录最后执行的命令
        }
    }

    void pressOffButton() {
        if (offCommand) {
            offCommand->execute();
            lastCommand = std::move(offCommand); // 记录最后执行的命令
        }
    }

    void pressUndoButton() {
        if (lastCommand) {
            std::cout << "Undo last operation: ";
            lastCommand->undo();
            lastCommand.reset(); // 清除记录
        }
    }
};

// int main() {
//     Light* livingRoomLight = new Light();

//     // 创建命令对象，并设置接收者
//     std::unique_ptr<Command> lightOn = std::make_unique<LightOnCommand>(livingRoomLight);
//     std::unique_ptr<Command> lightOff = std::make_unique<LightOffCommand>(livingRoomLight);

//     // 创建调用者，并设置命令
//     RemoteControl* remote = new RemoteControl();
//     remote->setCommands(std::move(lightOn), std::move(lightOff));

//     // 客户端通过调用者执行命令
//     remote->pressOnButton();
//     remote->pressOffButton();
//     remote->pressUndoButton(); // 撤销关灯，灯又亮了

//     delete livingRoomLight;
//     delete remote;

//     return 0;
// }
```

使用场景：

*   菜单系统/按钮点击：将菜单项或按钮点击操作封装为命令，实现解耦。
*   宏命令：将一系列命令组合成一个宏命令，批量执行。
*   撤销/重做功能：通过记录命令历史，实现操作的撤销和重做。
*   任务队列/日志：将命令放入队列中异步执行，或将命令记录到日志中以便恢复。

优缺点：

*   优点：
    *   解耦：将请求的发送者和接收者解耦，发送者无需知道接收者的具体操作。
    *   支持撤销/重做：通过存储命令历史，可以方便地实现撤销和重做功能。
    *   易于扩展：增加新的命令无需修改现有代码，符合“开闭原则”。
    *   支持宏命令：可以将多个命令组合成一个宏命令。
*   缺点：
    *   增加复杂性：引入了更多的类（命令、具体命令、调用者），增加了系统的复杂性。
    *   命令过多：如果命令数量很多，会导致类的数量急剧增加。

#### 3、解释器模式

原理与意图：

解释器模式（Interpreter Pattern）给定一个语言，定义它的文法的一种表示，并定义一个解释器，该解释器使用该表示来解释语言中的句子。它适用于那些可以表示为一个语言的问题，并且该语言的文法可以表示为抽象语法树（AST）。

解决的问题：

*   当一个问题可以被描述为一个语言，并且该语言的文法可以表示为抽象语法树时。
*   当需要解释器来处理该语言中的句子时。

核心思想：

解释器模式的核心是将语言的每个规则表示为一个类，并构建一个抽象语法树来表示语言中的句子。然后，通过遍历抽象语法树来解释句子。

1.  抽象表达式（Abstract Expression）：声明一个解释操作的接口。
2.  终结符表达式（Terminal Expression）：实现与文法中的终结符相关联的解释操作。
3.  非终结符表达式（Nonterminal Expression）：实现与文法中的非终结符相关联的解释操作。它通常包含对其他表达式对象的引用。
4.  上下文（Context）：包含解释器之外的一些全局信息。
5.  客户端（Client）：构建抽象语法树，并调用解释操作。

C++代码实现：

假设我们有一个简单的算术表达式解释器，可以处理加法和减法。

```cpp
#include <iostream>
#include <string>
#include <map>
#include <stack>
#include <memory>

// 上下文：存储变量及其值
class Context {
private:
    std::map<char, int> variables;
public:
    void assign(char var, int value) {
        variables[var] = value;
    }
    int getValue(char var) {
        return variables[var];
    }
};

// 抽象表达式
class AbstractExpression {
public:
    virtual ~AbstractExpression() = default;
    virtual int interpret(Context& context) = 0;
};

// 终结符表达式：变量
class VariableExpression : public AbstractExpression {
private:
    char name;
public:
    VariableExpression(char n) : name(n) {}
    int interpret(Context& context) override {
        return context.getValue(name);
    }
};

// 非终结符表达式：加法
class AddExpression : public AbstractExpression {
private:
    std::unique_ptr<AbstractExpression> left;
    std::unique_ptr<AbstractExpression> right;
public:
    AddExpression(std::unique_ptr<AbstractExpression> l, std::unique_ptr<AbstractExpression> r)
        : left(std::move(l)), right(std::move(r)) {}
    int interpret(Context& context) override {
        return left->interpret(context) + right->interpret(context);
    }
};

// 非终结符表达式：减法
class SubtractExpression : public AbstractExpression {
private:
    std::unique_ptr<AbstractExpression> left;
    std::unique_ptr<AbstractExpression> right;
public:
    SubtractExpression(std::unique_ptr<AbstractExpression> l, std::unique_ptr<AbstractExpression> r)
        : left(std::move(l)), right(std::move(r)) {}
    int interpret(Context& context) override {
        return left->interpret(context) - right->interpret(context);
    }
};

// 客户端构建抽象语法树并解释
// int main() {
//     Context context;
//     context.assign("a", 10);
//     context.assign("b", 5);
//     context.assign("c", 2);

//     // 构建表达式：a + b - c
//     // (a + b) - c
//     std::unique_ptr<AbstractExpression> expression = 
//         std::make_unique<SubtractExpression>(
//             std::make_unique<AddExpression>(
//                 std::make_unique<VariableExpression>("a"),
//                 std::make_unique<VariableExpression>("b")
//             ),
//             std::make_unique<VariableExpression>("c")
//         );

//     int result = expression->interpret(context);
//     std::cout << "Result of (a + b - c) is: " << result << std::endl; // 10 + 5 - 2 = 13

//     // 复杂表达式：(a - c) + b
//     std::unique_ptr<AbstractExpression> expression2 = 
//         std::make_unique<AddExpression>(
//             std::make_unique<SubtractExpression>(
//                 std::make_unique<VariableExpression>("a"),
//                 std::make_unique<VariableExpression>("c")
//             ),
//             std::make_unique<VariableExpression>("b")
//         );
//     int result2 = expression2->interpret(context);
//     std::cout << "Result of (a - c + b) is: " << result2 << std::endl; // (10 - 2) + 5 = 13

//     return 0;
// }
```

使用场景：

*   特定领域语言（DSL）：当需要为特定领域定义一种简单的语言，并需要解释执行该语言中的表达式时。例如，SQL查询解析、正则表达式解析。
*   规则引擎：当需要实现一个规则引擎，根据一系列规则来执行操作时。
*   表达式求值：当需要对数学表达式、布尔表达式等进行求值时。

优缺点：

*   优点：
    *   易于扩展文法：增加新的解释规则只需增加新的表达式类，符合“开闭原则”。
    *   易于实现：文法规则和类结构一一对应，实现相对简单。
*   缺点：
    *   文法复杂性：当文法规则非常复杂时，类的数量会急剧增加，导致系统难以管理。
    *   性能问题：对于复杂的表达式，解释器模式的性能可能不如直接编译执行。
    *   不适用于所有语言：只适用于那些可以表示为抽象语法树的简单语言。

#### 4、迭代器模式

原理与意图：

迭代器模式（Iterator Pattern）提供一种方法顺序访问一个聚合对象中各个元素，而又无需暴露该对象的内部表示。它将遍历集合的职责从集合本身分离出来，使得集合和遍历逻辑可以独立变化。

解决的问题：

*   当需要访问一个聚合对象（如列表、树）的内部元素，但又不想暴露其内部结构时。
*   当需要为聚合对象提供多种遍历方式时。
*   当需要支持在遍历过程中对聚合对象进行修改时。

核心思想：

迭代器模式的核心是定义一个迭代器接口，它包含访问和遍历元素的方法。聚合对象则提供一个创建迭代器的方法。

1.  迭代器（Iterator）：定义访问和遍历元素的接口，通常包含`first()`, `next()`, `isDone()`, `currentItem()`等方法。
2.  具体迭代器（Concrete Iterator）：实现迭代器接口，负责跟踪在聚合对象中的当前位置。
3.  聚合（Aggregate）：定义一个创建相应迭代器对象的接口。
4.  具体聚合（Concrete Aggregate）：实现聚合接口，返回一个具体迭代器实例。

C++代码实现：

假设我们有一个自定义的集合类，需要提供迭代器来遍历其元素。

```cpp
#include <iostream>
#include <vector>
#include <string>

// 抽象迭代器
template <typename T>
class Iterator {
public:
    virtual ~Iterator() = default;
    virtual void first() = 0;
    virtual void next() = 0;
    virtual bool isDone() const = 0;
    virtual T currentItem() const = 0;
};

// 抽象聚合
template <typename T>
class Aggregate {
public:
    virtual ~Aggregate() = default;
    virtual Iterator<T>* createIterator() = 0;
};

// 具体聚合：自定义列表
template <typename T>
class ConcreteAggregate : public Aggregate<T> {
private:
    std::vector<T> items;
public:
    void add(const T& item) {
        items.push_back(item);
    }
    T& getItem(int index) {
        return items[index];
    }
    int count() const {
        return items.size();
    }

    Iterator<T>* createIterator() override {
        return new ConcreteIterator<T>(this); // 创建具体迭代器
    }

    // 友元类，允许迭代器访问私有成员
    friend class ConcreteIterator<T>;
};

// 具体迭代器
template <typename T>
class ConcreteIterator : public Iterator<T> {
private:
    ConcreteAggregate<T>* aggregate; // 聚合对象引用
    int currentIndex; // 当前遍历位置

public:
    ConcreteIterator(ConcreteAggregate<T>* agg) : aggregate(agg), currentIndex(0) {}

    void first() override {
        currentIndex = 0;
    }
    void next() override {
        currentIndex++;
    }
    bool isDone() const override {
        return currentIndex >= aggregate->count();
    }
    T currentItem() const override {
        return aggregate->getItem(currentIndex);
    }
};

// int main() {
//     ConcreteAggregate<std::string>* names = new ConcreteAggregate<std::string>();
//     names->add("Alice");
//     names->add("Bob");
//     names->add("Charlie");
//     names->add("David");

//     Iterator<std::string>* iterator = names->createIterator();

//     std::cout << "Iterating through names:" << std::endl;
//     for (iterator->first(); !iterator->isDone(); iterator->next()) {
//         std::cout << iterator->currentItem() << std::endl;
//     }

//     delete iterator;
//     delete names;

//     return 0;
// }
```

使用场景：

*   遍历集合：当需要遍历各种不同的集合对象（如数组、链表、树），但又不想暴露其内部结构时。
*   多种遍历方式：当需要为同一个集合提供多种遍历方式时（如正向遍历、反向遍历）。
*   解耦：将集合的遍历逻辑与集合本身解耦，使得它们可以独立变化。

优缺点：

*   优点：
    *   支持多种遍历：可以为同一个集合提供多种遍历方式。
    *   简化集合接口：集合类无需包含复杂的遍历逻辑，职责更单一。
    *   解耦：将遍历逻辑与集合本身解耦，提高了代码的灵活性和可维护性。
*   缺点：
    *   增加复杂性：引入了新的类（迭代器、具体迭代器），增加了系统的复杂性。
    *   对于简单集合可能过度设计：对于简单的集合，直接使用循环遍历可能更简单。

#### 5、中介者模式

原理与意图：

中介者模式（Mediator Pattern）用一个中介对象来封装一系列的对象交互。中介者使各对象不需要显式地相互引用，从而使其耦合松散，而且可以独立地改变它们之间的交互。它将多对多的复杂交互关系转换为一对多的关系，由中介者来协调所有交互。

解决的问题：

*   当对象之间存在大量的、复杂的交互关系，导致系统难以理解和维护时。
*   当对象之间的耦合度过高，导致修改一个对象会影响其他多个对象时。

核心思想：

中介者模式的核心是引入一个中介者对象，所有的对象都通过中介者进行通信，而不是直接相互通信。中介者负责协调和管理对象之间的交互。

1.  中介者（Mediator）：定义一个接口，用于各同事对象之间的通信。
2.  具体中介者（Concrete Mediator）：实现中介者接口，协调和管理各同事对象之间的交互。
3.  同事（Colleague）：定义一个与中介者通信的接口。每个同事对象都持有对中介者对象的引用。
4.  具体同事（Concrete Colleague）：实现同事接口，并与中介者进行通信。

C++代码实现：

假设我们有一个聊天室系统，用户之间通过聊天室进行消息发送。

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// 前向声明
class ChatRoomMediator;

// 同事：用户
class User {
protected:
    ChatRoomMediator* mediator; // 中介者引用
    std::string name;

public:
    User(ChatRoomMediator* m, const std::string& n) : mediator(m), name(n) {}
    virtual ~User() = default;

    std::string getName() const { return name; }
    virtual void sendMessage(const std::string& message) = 0;
    virtual void receiveMessage(const std::string& message) = 0;
};

// 具体同事：聊天室用户
class ChatUser : public User {
public:
    ChatUser(ChatRoomMediator* m, const std::string& n) : User(m, n) {}

    void sendMessage(const std::string& message) override;
    void receiveMessage(const std::string& message) override {
        std::cout << name << " received: " << message << std::endl;
    }
};

// 具体中介者：聊天室
class ChatRoomMediator {
private:
    std::vector<User*> users;

public:
    void addUser(User* user) {
        users.push_back(user);
    }

    void sendMessage(const std::string& message, User* sender) {
        for (User* user : users) {
            if (user != sender) { // 不发给自己
                user->receiveMessage("[" + sender->getName() + "]: " + message);
            }
        }
    }
};

// 解决循环依赖，在ChatRoomMediator定义后实现sendMessage
void ChatUser::sendMessage(const std::string& message) {
    std::cout << name << " sending: " << message << std::endl;
    mediator->sendMessage(message, this);
}

// int main() {
//     ChatRoomMediator* chatRoom = new ChatRoomMediator();

//     ChatUser* user1 = new ChatUser(chatRoom, "Alice");
//     ChatUser* user2 = new ChatUser(chatRoom, "Bob");
//     ChatUser* user3 = new ChatUser(chatRoom, "Charlie");

//     chatRoom->addUser(user1);
//     chatRoom->addUser(user2);
//     chatRoom->addUser(user3);

//     user1->sendMessage("Hi everyone!");
//     user2->sendMessage("Hello Alice!");

//     delete user1;
//     delete user2;
//     delete user3;
//     delete chatRoom;

//     return 0;
// }
```

使用场景：

*   GUI组件交互：当多个GUI组件（如按钮、文本框、列表框）之间存在复杂的交互关系时，可以使用中介者来协调它们。
*   工作流引擎：当需要协调多个任务或步骤的执行顺序和交互时。
*   聊天室：用户之间通过聊天室进行消息发送。

优缺点：

*   优点：
    *   降低耦合度：将对象之间的多对多交互关系转换为一对多的关系，降低了对象之间的耦合度。
    *   简化对象交互：将复杂的交互逻辑封装在中介者中，使得各个对象职责更单一。
    *   易于维护和扩展：修改交互逻辑只需修改中介者，增加新的同事无需修改其他同事。
*   缺点：
    *   中介者膨胀：如果交互逻辑非常复杂，中介者可能会变得非常庞大和复杂，成为“上帝对象”。
    *   增加复杂性：引入了新的中介者类，增加了系统的复杂性。

#### 6、备忘录模式

原理与意图：

备忘录模式（Memento Pattern）在不破坏封装性的前提下，捕获一个对象的内部状态，并在该对象之外保存这个状态。这样以后就可以将该对象恢复到原先保存的状态。它通常用于实现撤销（Undo）功能。

解决的问题：

*   当需要保存一个对象的内部状态，以便以后可以恢复到该状态时。
*   当不希望破坏对象的封装性，即不暴露对象的内部结构时。

核心思想：

备忘录模式的核心是引入一个备忘录（Memento）对象，用于存储原发器（Originator）对象的内部状态。备忘录对象由原发器创建，并由负责人（Caretaker）管理。

1.  原发器（Originator）：创建并存储其内部状态的备忘录对象，并可以使用备忘录恢复其内部状态。
2.  备忘录（Memento）：存储原发器对象的内部状态。备忘录对象通常是不可变的，并且只能由原发器访问其内部状态。
3.  负责人（Caretaker）：负责保存和恢复备忘录。它不检查备忘录的内容，只负责存储和传递备忘录。

C++代码实现：

假设我们有一个文本编辑器，需要实现撤销功能。

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// 备忘录：存储编辑器状态
class EditorMemento {
private:
    std::string content; // 编辑器内容

public:
    EditorMemento(const std::string& c) : content(c) {}
    std::string getSavedContent() const { return content; }
};

// 原发器：文本编辑器
class TextEditor {
private:
    std::string currentContent;

public:
    TextEditor(const std::string& initialContent = "") : currentContent(initialContent) {}

    void type(const std::string& text) {
        currentContent += text;
        std::cout << "Current content: " << currentContent << std::endl;
    }

    // 创建备忘录，保存当前状态
    EditorMemento* save() {
        std::cout << "Saving current state..." << std::endl;
        return new EditorMemento(currentContent);
    }

    // 从备忘录恢复状态
    void restore(EditorMemento* memento) {
        currentContent = memento->getSavedContent();
        std::cout << "Restored content: " << currentContent << std::endl;
    }

    std::string getCurrentContent() const { return currentContent; }
};

// 负责人：历史记录管理器
class HistoryManager {
private:
    std::vector<EditorMemento*> history; // 存储备忘录

public:
    ~HistoryManager() {
        for (EditorMemento* memento : history) {
            delete memento;
        }
    }

    void addMemento(EditorMemento* memento) {
        history.push_back(memento);
    }

    EditorMemento* getMemento(int index) {
        if (index >= 0 && index < history.size()) {
            return history[index];
        }
        return nullptr;
    }

    EditorMemento* getLastMemento() {
        if (!history.empty()) {
            return history.back();
        }
        return nullptr;
    }

    void removeLastMemento() {
        if (!history.empty()) {
            delete history.back();
            history.pop_back();
        }
    }
};

// int main() {
//     TextEditor* editor = new TextEditor("Hello");
//     HistoryManager* history = new HistoryManager();

//     history->addMemento(editor->save()); // 保存初始状态

//     editor->type(" World");
//     history->addMemento(editor->save()); // 保存新状态

//     editor->type(" C++");

//     // 撤销到上一个状态
//     std::cout << "\nUndoing..." << std::endl;
//     history->removeLastMemento(); // 移除当前状态的备忘录
//     editor->restore(history->getLastMemento()); // 恢复到上一个保存的状态

//     delete editor;
//     delete history;

//     return 0;
// }
```

使用场景：

*   撤销/重做功能：实现文本编辑器、图形编辑器等应用程序的撤销和重做功能。
*   游戏存档：保存游戏状态，以便玩家可以从上次保存的地方继续游戏。
*   数据库事务：在事务处理中，保存操作前的状态，以便在发生错误时回滚。

优缺点：

*   优点：
    *   不破坏封装性：在不暴露对象内部结构的前提下，保存和恢复对象状态。
    *   简化原发器：原发器无需管理历史记录，只需负责创建和恢复备忘录。
    *   支持多点回滚：可以保存多个状态，实现多点回滚。
*   缺点：
    *   资源消耗：如果需要保存的状态信息量大，或者需要保存多个历史状态，可能会消耗大量内存。
    *   复杂性：如果原发器状态复杂，创建和恢复备忘录的逻辑可能会变得复杂。

#### 7、观察者模式

原理与意图：

观察者模式（Observer Pattern）定义对象间的一种一对多的依赖关系，当一个对象的状态发生改变时，所有依赖于它的对象都得到通知并被自动更新。它通常用于实现事件处理系统。

解决的问题：

*   当一个对象的改变需要通知其他多个对象，但又不想这些对象之间紧密耦合时。
*   当需要实现事件驱动的系统时。

核心思想：

观察者模式的核心是引入了主题（Subject）和观察者（Observer）两个角色。主题是被观察的对象，它维护一组观察者，并在状态改变时通知它们。观察者是观察主题的对象，它注册到主题上，并在收到通知时执行相应的操作。

1.  主题（Subject）/ 可观察者（Observable）：定义一个接口，用于注册、删除和通知观察者。
2.  具体主题（Concrete Subject）：实现主题接口，维护观察者列表，并在状态改变时通知所有注册的观察者。
3.  观察者（Observer）：定义一个接收更新通知的接口。
4.  具体观察者（Concrete Observer）：实现观察者接口，并在收到通知时执行相应的操作。

C++代码实现：

假设我们有一个股票行情系统，当股票价格变化时，所有关注该股票的用户都会收到通知。

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// 抽象观察者
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& stockName, double price) = 0;
};

// 抽象主题
class Subject {
public:
    virtual ~Subject() = default;
    virtual void attach(Observer* observer) = 0; // 注册观察者
    virtual void detach(Observer* observer) = 0; // 删除观察者
    virtual void notify() = 0; // 通知观察者
};

// 具体主题：股票
class Stock : public Subject {
private:
    std::vector<Observer*> observers;
    std::string stockName;
    double price;

public:
    Stock(const std::string& name, double initialPrice) : stockName(name), price(initialPrice) {}

    void attach(Observer* observer) override {
        observers.push_back(observer);
        std::cout << observer << " attached to " << stockName << std::endl;
    }
    void detach(Observer* observer) override {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
        std::cout << observer << " detached from " << stockName << std::endl;
    }
    void notify() override {
        for (Observer* obs : observers) {
            obs->update(stockName, price);
        }
    }

    void setPrice(double newPrice) {
        if (price != newPrice) {
            price = newPrice;
            notify(); // 价格变化时通知所有观察者
        }
    }

    double getPrice() const { return price; }
    std::string getName() const { return stockName; }
};

// 具体观察者：投资者
class Investor : public Observer {
private:
    std::string name;

public:
    Investor(const std::string& n) : name(n) {}
    void update(const std::string& stockName, double price) override {
        std::cout << name << " received update: " << stockName << " new price is " << price << std::endl;
    }
};

// int main() {
//     Stock* googleStock = new Stock("Google", 1500.0);

//     Investor* investor1 = new Investor("Alice");
//     Investor* investor2 = new Investor("Bob");
//     Investor* investor3 = new Investor("Charlie");

//     googleStock->attach(investor1);
//     googleStock->attach(investor2);
//     googleStock->attach(investor3);

//     std::cout << "\nChanging Google stock price..." << std::endl;
//     googleStock->setPrice(1510.5);

//     std::cout << "\nDetaching Bob..." << std::endl;
//     googleStock->detach(investor2);

//     std::cout << "\nChanging Google stock price again..." << std::endl;
//     googleStock->setPrice(1505.0);

//     delete googleStock;
//     delete investor1;
//     delete investor2;
//     delete investor3;

//     return 0;
// }
```

使用场景：

*   GUI事件处理：当用户界面中的某个组件（如按钮）发生事件时，需要通知其他组件进行响应。
*   发布-订阅系统：当一个对象（发布者）的状态变化需要通知多个订阅者时。
*   消息队列：当消息到达时，通知所有注册的消费者。
*   MVC架构：模型（Model）变化时通知视图（View）和控制器（Controller）更新。

优缺点：

*   优点：
    *   降低耦合度：主题和观察者之间解耦，它们可以独立变化。
    *   支持广播通信：一个主题可以通知多个观察者。
    *   符合开闭原则：增加新的观察者无需修改主题代码。
*   缺点：
    *   通知顺序不确定：如果观察者之间存在依赖关系，通知顺序可能导致问题。
    *   循环引用：如果主题和观察者相互持有智能指针，可能导致循环引用问题（可以使用`weak_ptr`解决）。
    *   性能问题：如果观察者数量过多，通知所有观察者可能会影响性能。

#### 8、状态模式

原理与意图：

状态模式（State Pattern）允许一个对象在其内部状态改变时改变它的行为，对象看起来似乎修改了它的类。它将特定于状态的行为封装在独立的状态类中，从而避免了在上下文类中使用大量的条件语句。

解决的问题：

*   当一个对象的行为取决于它的状态，并且它在运行时可以改变它的状态时。
*   当一个对象的方法中包含大量的条件语句（`if-else`或`switch-case`），这些条件语句根据对象的状态来决定行为时。

核心思想：

状态模式的核心是引入一个抽象状态（State）接口，每个具体状态都实现该接口。上下文（Context）类持有对当前状态对象的引用，并将请求委派给当前状态对象。当状态改变时，上下文会切换到不同的具体状态对象。

1.  上下文（Context）：定义客户端感兴趣的接口，并维护一个具体状态类的实例，这个实例定义了对象的当前状态。
2.  抽象状态（State）：定义一个接口，封装与上下文的特定状态相关的行为。
3.  具体状态（Concrete State）：实现抽象状态接口，处理来自上下文的请求，并可能在处理过程中改变上下文的状态。

C++代码实现：

假设我们有一个电梯系统，电梯有开门、关门、运行、停止等状态。

```cpp
#include <iostream>
#include <string>

// 前向声明
class Elevator;

// 抽象状态
class ElevatorState {
public:
    virtual ~ElevatorState() = default;
    virtual void openDoor(Elevator* elevator) = 0;
    virtual void closeDoor(Elevator* elevator) = 0;
    virtual void run(Elevator* elevator) = 0;
    virtual void stop(Elevator* elevator) = 0;
};

// 具体状态：停止状态
class StoppedState : public ElevatorState {
public:
    void openDoor(Elevator* elevator) override;
    void closeDoor(Elevator* elevator) override {
        std::cout << "Elevator is already stopped, closing door." << std::endl;
        // 保持在停止状态
    }
    void run(Elevator* elevator) override;
    void stop(Elevator* elevator) override {
        std::cout << "Elevator is already stopped." << std::endl;
    }
};

// 具体状态：开门状态
class OpenedState : public ElevatorState {
public:
    void openDoor(Elevator* elevator) override {
        std::cout << "Elevator door is already opened." << std::endl;
    }
    void closeDoor(Elevator* elevator) override;
    void run(Elevator* elevator) override {
        std::cout << "Cannot run with door opened. Please close door first." << std::endl;
    }
    void stop(Elevator* elevator) override {
        std::cout << "Elevator is stopped, but door is opened." << std::endl;
    }
};

// 具体状态：运行状态
class RunningState : public ElevatorState {
public:
    void openDoor(Elevator* elevator) override {
        std::cout << "Cannot open door while running. Please stop first." << std::endl;
    }
    void closeDoor(Elevator* elevator) override {
        std::cout << "Elevator is running, door is already closed." << std::endl;
    }
    void run(Elevator* elevator) override {
        std::cout << "Elevator is already running." << std::endl;
    }
    void stop(Elevator* elevator) override;
};

// 上下文：电梯
class Elevator {
private:
    ElevatorState* currentState; // 当前状态

public:
    Elevator() : currentState(nullptr) {
        // 初始状态为停止
        setState(new StoppedState());
    }
    ~Elevator() {
        delete currentState;
    }

    void setState(ElevatorState* state) {
        if (currentState) {
            delete currentState;
        }
        currentState = state;
    }

    void openDoor() { currentState->openDoor(this); }
    void closeDoor() { currentState->closeDoor(this); }
    void run() { currentState->run(this); }
    void stop() { currentState->stop(this); }
};

// 解决循环依赖，在Elevator定义后实现状态转换
void StoppedState::openDoor(Elevator* elevator) {
    std::cout << "Elevator door opened." << std::endl;
    elevator->setState(new OpenedState());
}
void StoppedState::run(Elevator* elevator) {
    std::cout << "Elevator started running." << std::endl;
    elevator->setState(new RunningState());
}

void OpenedState::closeDoor(Elevator* elevator) {
    std::cout << "Elevator door closed." << std::endl;
    elevator->setState(new StoppedState());
}

void RunningState::stop(Elevator* elevator) {
    std::cout << "Elevator stopped." << std::endl;
    elevator->setState(new StoppedState());
}

// int main() {
//     Elevator* elevator = new Elevator();

//     elevator->openDoor();  // 门开了
//     elevator->closeDoor(); // 门关了
//     elevator->run();       // 运行了
//     elevator->openDoor();  // 运行中不能开门
//     elevator->stop();      // 停止了
//     elevator->openDoor();  // 门开了

//     delete elevator;

//     return 0;
// }
```

使用场景：

*   工作流/状态机：当一个对象的行为在不同状态下有显著差异，并且状态之间有明确的转换规则时。例如，订单状态（待支付、已支付、已发货）、游戏角色状态（站立、行走、跳跃）。
*   消除条件语句：当一个方法中包含大量基于状态的条件逻辑时，可以使用状态模式来消除这些条件语句，使代码更清晰。

优缺点：

*   优点：
    *   消除条件语句：将状态相关的行为封装在独立的状态类中，避免了大量的`if-else`或`switch-case`语句，使代码更清晰、易于维护。
    *   符合开闭原则：增加新的状态只需增加新的状态类，无需修改上下文或其他状态类。
    *   职责单一：每个状态类只负责处理特定状态下的行为。
*   缺点：
    *   增加类数量：每个状态都需要一个独立的类，增加了类的数量，对于简单的状态机可能过度设计。
    *   状态转换复杂：如果状态转换逻辑非常复杂，可能会导致状态类之间的依赖关系复杂。

#### 9、策略模式

原理与意图：

策略模式（Strategy Pattern）定义一系列的算法，把它们一个个封装起来，并且使它们可相互替换。本模式使得算法可独立于使用它的客户而变化。它允许客户端在运行时选择不同的算法，而无需修改客户端代码。

解决的问题：

*   当一个类有多种行为，并且这些行为可以在运行时根据不同的条件进行切换时。
*   当一个算法有多种实现方式，并且这些实现方式可以相互替换时。
*   当需要避免在客户端代码中使用大量的条件语句来选择算法时。

核心思想：

策略模式的核心是定义一个抽象策略（Strategy）接口，每个具体策略都实现该接口。上下文（Context）类持有对当前策略对象的引用，并将请求委派给当前策略对象。客户端选择并设置具体的策略。

1.  抽象策略（Strategy）：定义一个接口，封装了具体算法的公共行为。
2.  具体策略（Concrete Strategy）：实现抽象策略接口，提供具体的算法实现。
3.  上下文（Context）：持有对抽象策略对象的引用，并负责调用策略对象的算法。它不关心具体算法的实现细节。

C++代码实现：

假设我们有一个电商网站，需要根据不同的促销活动计算商品价格。

```cpp
#include <iostream>
#include <string>

// 抽象策略：价格计算策略
class PricingStrategy {
public:
    virtual ~PricingStrategy() = default;
    virtual double calculatePrice(double originalPrice) const = 0;
};

// 具体策略1：正常价格策略
class NormalPricingStrategy : public PricingStrategy {
public:
    double calculatePrice(double originalPrice) const override {
        std::cout << "Applying normal pricing." << std::endl;
        return originalPrice;
    }
};

// 具体策略2：打折策略（例如，8折）
class DiscountPricingStrategy : public PricingStrategy {
private:
    double discountRate;
public:
    DiscountPricingStrategy(double rate) : discountRate(rate) {}
    double calculatePrice(double originalPrice) const override {
        std::cout << "Applying discount pricing (Rate: " << discountRate * 100 << "%)." << std::endl;
        return originalPrice * discountRate;
    }
};

// 具体策略3：满减策略（例如，满100减20）
class CouponPricingStrategy : public PricingStrategy {
private:
    double threshold;
    double deduction;
public:
    CouponPricingStrategy(double t, double d) : threshold(t), deduction(d) {}
    double calculatePrice(double originalPrice) const override {
        std::cout << "Applying coupon pricing (Threshold: " << threshold << ", Deduction: " << deduction << ")." << std::endl;
        if (originalPrice >= threshold) {
            return originalPrice - deduction;
        } else {
            return originalPrice;
        }
    }
};

// 上下文：商品订单
class ShoppingCart {
private:
    PricingStrategy* strategy; // 当前价格计算策略
    double itemPrice;

public:
    ShoppingCart(double price) : itemPrice(price), strategy(nullptr) {}
    ~ShoppingCart() {
        if (strategy) {
            delete strategy;
        }
    }

    void setPricingStrategy(PricingStrategy* s) {
        if (strategy) {
            delete strategy;
        }
        strategy = s;
    }

    double checkout() const {
        if (strategy) {
            return strategy->calculatePrice(itemPrice);
        } else {
            std::cout << "No pricing strategy set, using original price." << std::endl;
            return itemPrice;
        }
    }
};

// int main() {
//     ShoppingCart* cart = new ShoppingCart(120.0);

//     // 使用正常价格策略
//     cart->setPricingStrategy(new NormalPricingStrategy());
//     std::cout << "Final price: $" << cart->checkout() << std::endl; // 120.0

//     std::cout << "\n";

//     // 使用打折策略
//     cart->setPricingStrategy(new DiscountPricingStrategy(0.8)); // 8折
//     std::cout << "Final price: $" << cart->checkout() << std::endl; // 96.0

//     std::cout << "\n";

//     // 使用满减策略
//     cart->setPricingStrategy(new CouponPricingStrategy(100.0, 20.0)); // 满100减20
//     std::cout << "Final price: $" << cart->checkout() << std::endl; // 100.0

//     delete cart;

//     return 0;
// }
```

使用场景：

*   多种算法选择：当一个问题有多种算法实现，并且需要在运行时动态选择时。例如，排序算法（冒泡、快速、归并）、加密算法。
*   消除条件语句：当一个方法中包含大量基于条件的算法选择逻辑时，可以使用策略模式来消除这些条件语句。
*   行为可插拔：当需要将对象的行为从对象本身中分离出来，使其可以独立变化和替换时。

优缺点：

*   优点：
    *   消除条件语句：将算法选择逻辑封装在策略类中，避免了大量的`if-else`或`switch-case`语句，使代码更清晰、易于维护。
    *   符合开闭原则：增加新的算法只需增加新的策略类，无需修改上下文或其他策略类。
    *   算法独立：算法可以独立于使用它的客户端而变化。
*   缺点：
    *   增加类数量：每个算法都需要一个独立的类，增加了类的数量，对于简单的算法可能过度设计。
    客户端需要了解所有策略：客户端需要知道所有可用的具体策略，并选择合适的策略。

#### 10、模板方法模式

原理与意图：

模板方法模式（Template Method Pattern）在一个抽象类中定义一个操作中的算法的骨架，而将一些步骤延迟到子类中。模板方法使得子类可以在不改变算法结构的情况下，重新定义该算法的某些特定步骤。它通常用于构建框架。

解决的问题：

*   当需要定义一个算法的骨架，并将某些步骤的实现延迟到子类中时。
*   当多个子类有共同的行为，但某些细节有所不同时。
*   当需要控制子类对算法的修改范围时。

核心思想：

模板方法模式的核心是抽象类中定义一个模板方法，该方法封装了算法的骨架。模板方法中包含了一系列抽象方法或钩子方法，这些方法由子类实现，从而完成算法的特定步骤。

1.  抽象类（Abstract Class）：定义模板方法，其中包含算法的骨架。模板方法中可以包含具体操作、抽象操作（由子类实现）和钩子操作（子类可选实现）。
2.  具体类（Concrete Class）：实现抽象类中的抽象操作，从而完成算法的特定步骤。

C++代码实现：

假设我们有一个制作饮料的流程，包括烧水、冲泡、倒入杯中、加调料。其中冲泡和加调料的步骤对于咖啡和茶是不同的。

```cpp
#include <iostream>
#include <string>

// 抽象类：饮料制作模板
class BeverageMaker {
public:
    virtual ~BeverageMaker() = default;

    // 模板方法：定义制作饮料的骨架
    void makeBeverage() {
        boilWater();
        brew(); // 抽象方法，由子类实现
        pourInCup();
        addCondiments(); // 抽象方法，由子类实现
        std::cout << "Beverage is ready!\n" << std::endl;
    }

protected:
    // 具体操作：烧水、倒入杯中
    void boilWater() {
        std::cout << "Boiling water." << std::endl;
    }
    void pourInCup() {
        std::cout << "Pouring into cup." << std::endl;
    }

    // 抽象操作：冲泡、加调料（由子类实现）
    virtual void brew() = 0;
    virtual void addCondiments() = 0;

    // 钩子方法（可选）：子类可以重写以改变默认行为
    // virtual bool customerWantsCondiments() { return true; }
};

// 具体类：咖啡制作器
class CoffeeMaker : public BeverageMaker {
protected:
    void brew() override {
        std::cout << "Dripping coffee through filter." << std::endl;
    }
    void addCondiments() override {
        std::cout << "Adding sugar and milk." << std::endl;
    }
};

// 具体类：茶制作器
class TeaMaker : public BeverageMaker {
protected:
    void brew() override {
        std::cout << "Steeping the tea bag." << std::endl;
    }
    void addCondiments() override {
        std::cout << "Adding lemon." << std::endl;
    }
};

// int main() {
//     std::cout << "Making coffee:" << std::endl;
//     CoffeeMaker* coffee = new CoffeeMaker();
//     coffee->makeBeverage();

//     std::cout << "Making tea:" << std::endl;
//     TeaMaker* tea = new TeaMaker();
//     tea->makeBeverage();

//     delete coffee;
//     delete tea;

//     return 0;
// }
```

使用场景：

*   框架设计：在框架中定义算法的骨架，让开发者通过继承实现特定步骤。
*   代码复用：多个子类有共同的行为，但某些细节不同时，可以将共同行为提取到模板方法中。
*   控制算法流程：通过模板方法，可以控制子类对算法的修改范围，确保算法的整体结构不变。

优缺点：

*   优点：
    *   代码复用：将共同行为提取到抽象类中，避免了代码重复。
    *   控制算法流程：通过模板方法，可以控制子类对算法的修改范围。
    *   符合开闭原则：增加新的具体实现只需增加新的子类，无需修改抽象类。
*   缺点：
    *   类数量增加：每个具体实现都需要一个独立的子类，增加了类的数量。
    *   继承限制：由于是基于继承的，如果子类需要修改算法的结构，可能会比较困难。

#### 11、访问者模式

原理与意图：

访问者模式（Visitor Pattern）表示一个作用于某对象结构中的各元素的操作。它允许你在不改变各元素的类的前提下定义作用于这些元素的新操作。它将数据结构和数据操作分离，使得操作可以独立于数据结构而变化。

解决的问题：

*   当一个对象结构（如抽象语法树、图）包含多种类型的元素，并且需要对这些元素执行多种不同的操作时。
*   当需要对对象结构中的元素执行的操作经常变化时。
*   当需要避免在元素类中混合与元素类型相关的操作时。

核心思想：

访问者模式的核心是引入一个访问者（Visitor）接口，它为对象结构中的每种元素类型定义一个`visit()`方法。每个具体访问者都实现这些`visit()`方法，从而为不同类型的元素提供不同的操作。对象结构中的元素则提供一个`accept()`方法，用于接受访问者。

1.  访问者（Visitor）：定义一个接口，为对象结构中的每个具体元素类声明一个`visit()`操作。
2.  具体访问者（Concrete Visitor）：实现访问者接口，为每个具体元素类提供具体的访问操作。
3.  元素（Element）：定义一个接受访问者的接口（`accept()`方法）。
4.  具体元素（Concrete Element）：实现元素接口，接受访问者并调用访问者相应的`visit()`方法。
5.  对象结构（Object Structure）：可以是一个集合、列表或复合对象，它能够枚举其元素，并提供一个高级接口来允许访问者访问其元素。

C++代码实现：

假设我们有一个图形结构，包含圆形和矩形。我们需要对这些图形执行不同的操作，例如绘制和计算面积。

```cpp
#include <iostream>
#include <vector>
#include <string>

// 前向声明
class Circle; 
class Rectangle;

// 抽象访问者
class ShapeVisitor {
public:
    virtual ~ShapeVisitor() = default;
    virtual void visit(Circle* circle) = 0;
    virtual void visit(Rectangle* rectangle) = 0;
};

// 抽象元素：图形
class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(ShapeVisitor* visitor) = 0;
};

// 具体元素：圆形
class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}
    double getRadius() const { return radius; }
    void accept(ShapeVisitor* visitor) override {
        visitor->visit(this);
    }
};

// 具体元素：矩形
class Rectangle : public Shape {
private:
    double width;
    double height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    void accept(ShapeVisitor* visitor) override {
        visitor->visit(this);
    }
};

// 具体访问者：绘制访问者
class DrawVisitor : public ShapeVisitor {
public:
    void visit(Circle* circle) override {
        std::cout << "Drawing Circle with radius: " << circle->getRadius() << std::endl;
    }
    void visit(Rectangle* rectangle) override {
        std::cout << "Drawing Rectangle with width: " << rectangle->getWidth() << ", height: " << rectangle->getHeight() << std::endl;
    }
};

// 具体访问者：面积计算访问者
class AreaVisitor : public ShapeVisitor {
public:
    void visit(Circle* circle) override {
        std::cout << "Area of Circle: " << 3.14159 * circle->getRadius() * circle->getRadius() << std::endl;
    }
    void visit(Rectangle* rectangle) override {
        std::cout << "Area of Rectangle: " << rectangle->getWidth() * rectangle->getHeight() << std::endl;
    }
};

// int main() {
//     std::vector<Shape*> shapes;
//     shapes.push_back(new Circle(5.0));
//     shapes.push_back(new Rectangle(4.0, 6.0));
//     shapes.push_back(new Circle(3.0));

//     DrawVisitor* drawVisitor = new DrawVisitor();
//     AreaVisitor* areaVisitor = new AreaVisitor();

//     std::cout << "\n--- Drawing Shapes ---" << std::endl;
//     for (Shape* shape : shapes) {
//         shape->accept(drawVisitor);
//     }

//     std::cout << "\n--- Calculating Areas ---" << std::endl;
//     for (Shape* shape : shapes) {
//         shape->accept(areaVisitor);
//     }

//     // 清理内存
//     for (Shape* shape : shapes) {
//         delete shape;
//     }
//     delete drawVisitor;
//     delete areaVisitor;

//     return 0;
// }
```

使用场景：

*   复杂对象结构：当一个对象结构（如抽象语法树、图）包含多种类型的元素，并且需要对这些元素执行多种不同的操作时。
*   操作经常变化：当需要对对象结构中的元素执行的操作经常变化时，例如，编译器中的代码优化、代码生成。
*   分离数据结构与操作：当需要将数据结构和数据操作分离，使得操作可以独立于数据结构而变化时。

优缺点：

*   优点：
    *   增加新操作容易：增加新的操作只需增加新的访问者类，无需修改元素类，符合“开闭原则”。
    *   将相关操作集中：将与特定元素类型相关的操作集中在一个访问者类中，提高了内聚性。
    *   分离数据结构与操作：将数据结构和数据操作分离，使得它们可以独立变化。
*   缺点：
    *   增加新元素困难：如果需要增加新的元素类型，就需要修改所有访问者接口及其所有具体访问者的实现，这违反了“开闭原则”。
    *   破坏封装性：访问者需要访问元素类的内部状态，可能破坏了元素的封装性。
    *   复杂性：引入了新的类（访问者、具体访问者），增加了系统的复杂性。

## 四、总结与最佳实践

设计模式是软件开发中宝贵的经验总结，它们提供了一套经过验证的解决方案，用于解决在软件设计过程中反复出现的问题。掌握设计模式，不仅能帮助我们编写出更健壮、可扩展、易于维护的代码，还能提升我们与团队成员沟通的效率，因为设计模式提供了一种通用的语言。

如何学习和应用设计模式？

1.  理解原理而非死记硬背： 重要的是理解每种模式解决的核心问题、其背后的设计思想以及它带来的优缺点。不要仅仅记住模式的名称和结构。
2.  从实践中学习： 尝试在自己的项目中应用设计模式。从小规模的实验开始，逐步将模式融入到实际代码中。通过实践，你会更深刻地理解模式的适用场景和局限性。
3.  阅读优秀代码： 学习开源项目或成熟框架中的设计模式应用。观察它们是如何在真实世界中被巧妙地使用的。
4.  批判性思维： 设计模式并非银弹。在应用模式之前，要仔细权衡其利弊，确保它真正适合当前的问题和上下文。过度设计和滥用模式反而会增加系统的复杂性。
5.  持续学习： 设计模式是不断演进的。随着新的编程范式和技术出现，新的设计思想也会随之产生。保持好奇心，持续学习。

设计模式的价值：

*   提高代码复用性： 模式提供了一种通用的结构，可以在不同项目中重复使用。
*   增强代码可读性： 遵循模式的代码更容易被理解，因为它们遵循了公认的结构和命名约定。
*   提升代码可维护性： 模式有助于降低模块间的耦合度，使得修改和扩展更加容易。
*   促进团队协作： 模式提供了一种共同的语言，有助于团队成员之间更有效地沟通设计思想。
*   应对复杂性： 模式将复杂问题分解为更小、更易于管理的部分，从而降低了系统的整体复杂性。

希望这篇博文能帮助您深入理解C++设计模式，并在您的软件开发旅程中提供有益的指导。祝您在构建健壮、可扩展的C++应用道路上越走越远！
