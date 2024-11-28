# 面向对象篇

## 1、C++函数模板

- 模板的意义：对类型也可以进行参数化了。
- 函数模板 《= 是不进行编译的，因为类型不知道。
- 模板的实例化 《= 函数调用点进行实例化。
- 模板函数 《= 才是被编译器所编译的。
- 模板类型参数。
- 模板非类型参数。
- 模板的实参推演 =》 可以根据用户传入的实参的类型，来推导模板类型。
- 模板的特例化。
- 函数模板、模板的特例化、非模板函数的重载关系。
- 模板代码是不能在一个文件中定义，在另外一个文件中使用的。
- 模板代码调用之前，一定要看到模板定义的地方，这样的话，模板才能够进行正常的实例化，产生能够被编译器编译的代码。
- 所以，模板代码都是放在头文件当中的，然后在源文件当中直接进行#includ包含。

## 2、泛型算法

- 泛型算法参数接收的都是迭代器！
- 泛型函数 - 全局的函数 - 给所有容器用的
- 泛型函数，有一套方式，，能够统一的遍历所有的容器的元素 - 迭代器。

## 3、拷贝赋值和移动赋值

1. 拷贝赋值是通过拷贝构造函数来赋值，在创建对象时，使用同一类中之前创建的对象来初始化新创建的对象。
2. 移动赋值是通过移动构造函数来赋值，二者的主要区别在于：

- 拷贝构造函数的形参是一个左值引用，而移动构造函数的形参是一个右值引用。
- 拷贝构造函数完成的是整个对象或变量的拷贝，而移动构造函数是生成一个指针指向源对象或变量的地址，接管源对象的内存，相对于大量数据的拷贝节省时间和内存空间。

## 4、虚函数、静态绑定、动态绑定

**虚函数表位于只读数据段（.rodata）**，也就是C++内存模型中的常量区；而**虚函数则位于代码段（.text）**，也就是C++内存模型中的代码区。

一个类添加了虚函数，对这个类有什么影响？

- 如果类里面定义了虚函数，那么编译阶段，编译器给这个类类型产生一个唯一的vftable虚函数表，虚函数表中主要存储的内容就是RTTI指针和虚函数的地址。当程序运行时，每一张虚表函数都会加载到内存的 .rodata区。
- 一个类里面定义了虚函数，那么这个类定义的对象，其运行时，内存中开始部分，多存储一个vfptr虚函数指针，指向相应类型的虚函数表vftable。一个类型定义的n个对象，它们的vfptr指向的都是同一张虚函数表。
- 一个类里面虚函数的个数，不影响对象内存大小(vfptr)，影响的是虚函数表的大小
- 如果派生类中的方法，和基类继承来的某个方法，返回值、函数名、参数列表都相同，而且基类的方法是virtual虚函数，那么派生类的这个方法，自动处理成虚函数。

静态绑定和动态绑定：绑定指的是函数调用

- 静态绑定在编译时期，绑定的是普通函数的调用   指令 ： call Base::show(地址)
- 动态绑定在运行时期，绑定的一定是虚函数的调用   指令： 编译的是call寄存器 运行时才知道

覆盖：基类和派生类的方法，返回值、函数名以及参数列表都相同，而且基类的方法是虚函数，那么派生类的方法就是自动处理成虚函数，他们之间成为覆盖关系。

在类内部添加一个虚拟函数表指针，该指针指向一个虚拟函数表，该虚拟函数表包含了所有的虚拟函数的入口地址，每个类的虚拟函数表都不一样，在运行阶段可以循环脉络找到自己的函数入口。纯虚函数相当于占位符，现在虚函数占一个位置由派生类实现后再把真正的函数指针填进去。

## 5、虚析构函数

1. 哪些函数不能实现成虚函数？

虚函数依赖：

- 虚函数能产生地址，存储在vfptr当中
- 对象必须存在(vfptr -> vftable  ->  虚函数地址)

构造函数：没有虚构造函数！！！

- virtual+构造函数   NO！
- 构造函数中(调用的任何函数，都是静态绑定的)调用虚函数，也不会发生静态绑定

派生类对象构造过程：先调用的是基类的构造函数，才调用派生类的构造函数。

static静态成员方法  NO！ 

2. 虚析构函数  析构函数调用的时候，对象是存在的！

3. 什么时候把基类的析构函数必须实现成虚函数？

基类的指针（引用）指向堆上new出来的派生来对象的时候，delete pb（基类指针），它调用析构函数的时候，必须发生动态绑定，否则会导致派生类的析构函数无法调用

4. 虚函数和动态绑定 

问题：是不是虚函数的调用一定就是动态绑定？肯定不是！

在类的构造函数当中，调用虚函数，也是静态绑定（构造函数中调用其他函数（虚），不会发生动态绑定）

静态绑定  用对象本身调用虚函数，是静态绑定

动态绑定：

- 必须由指针调用虚函数
- 必须由引用变量调用虚函数
- 虚函数通过指针或者引用的调用，才发生动态绑定

## 6、如何解释多态

1. 静态（编译时期）的多态：函数重载、模板（函数模板和类模板）

   ```C++
   bool compare(int , int) { }
   bool cpmpare(double, double) { }
   compare(10,20); call compare_int_int  在编译阶段就确定好调用的函数版本
   compare(10.5, 20.5); call compare_double_double  在编译阶段就确定好调用的函数版本
   ```

   ```C++
   template<typename T>
   bool compare(T a, T b) { }
   compare<int>(10,20);  => int   实例化一个compare<int>
   compare(10.5 ,20.5);  => double  实例化一个 compare<double>
   ```

2. 动态（运行时期）的多态：

在继承结构中，基类指针（引用）指向派生类对象，通过指针（引用）调用同名覆盖方法（虚函数），基类指针指向哪个派生类对象，就会调用哪个派生类对象的同名覆盖方法，称为多态。

```
pbase->show();
```

多态底层是通过动态绑定来实现的，pbase->访问谁的vfptr ->继续访问谁的vftable  -> 当然调用的是对应的派生类对象的方法了。

## 7、继承

广义的继承有三种实现形式：

1. 实现继承：指使用基类的属性和方法而无需额外编码的能力。
2. 可视继承：子窗口使用父窗口的外观和实现代码。
3. 接口继承：仅使用属性和方法，实现滞后到子类

好处：

- 可以做代码的复用
- 在基类中给所有派生类提供统一的虚函数接口，让派生类重写，然后就可以使用多态了。

## 8、抽象类和普通类的区别

一般把什么类设计成抽象类？基类

//动物的基类  泛指  类 -> 抽象一个实体的类型

定义Animal的初衷，并不是让Animal抽象某个实体的类型

- string  _name;  让所有的动物实体类通过继承Animal直接复用该属性
- 给所有的派生类保留统一的覆盖/重写接口

拥有纯虚函数的类，叫抽象类！（Animal）

```
Animal a;  NO！！！
```

抽象类不能再实例化对象了，但是可以定义指针和引用变量。

```C++
class Animal
{
public:
    Animal(string name) : _name(name) { }
    virtual void bark() = 0; //纯虚函数
protected:
    string _name;
};
//以下是动物实体类
class Cat : public Animal
{
public:
    Cat(string name) : Animal(name) { }
    void bark() { cout << _name << "bark: miao miao!" << endl; }
};
class Dog :public Animal
{
public:
    Dog(string name):Animal(name) { }
    void bark() { cout << _name << "bark: wang wang!" << endl; }
};
class Pig :public Animal
{
    Pig(string name) :Animal(name) {        }
    void bark() { cout << _name << "bark: heng heng! " << endl; }
};
void bark(Animal* p)
{
    p->bark(); //Animal::bark虚函数，动态绑定了
}
int main()
{
    Cat cat("猫咪");
    Dog dog("二哈");
    Pig pig("佩奇");
    
    bark(&cat);
    bark(&dog);
    bark(&pig);

    return 0;
}
```

## 9、抽象类（有纯虚函数的类）/虚基类

**virtual**

- 修饰成员方法的虚函数
- 可以修饰继承方式，是虚继承。被虚继承的类，称作虚基类。

```C++
class A
{
public:
    virtual void func() { cout << "call A::func" << endl; }
    void operator delete(void* ptr)
    {
        cout << "operator delete p:" << ptr << endl;
        free(ptr);
    }
private:
    int ma;
};
class B :virtual public A
{
public:
    void func() { cout << "call B::func" << endl; }
    void* operator new(size_t size)
    {
        void* p = malloc(size);
        cout << "operator new p:" << p << endl;
        return p;
    }
private:
    int mb;
};

A a; 4个字节
B b; ma,mb  8个字节

int main()
{
    B b;
    A* p = &b;
    cout << "main p:" << p << endl;
    p->func();
    return 0;
}
```

基类指针指向派生类对象，永远指向的是派生类基类部分数据的起始地址。

## 10、C++多继承

菱形继承的问题：派生类有多份间接基类的数据， 设计的问题

使用虚继承

**好处**：可以做更多代码的复用。

C++语言级别提供的四种类型转换方式：

- const_cast：去掉常量属性的一个类型转换。
- static_cast：提供编译器认为安全的类型转换（没有任何联系的类型之间的转换就被否定）。
- reinterpret_cast：类似于C风格的强制类型转换。
- dynamic_cast：主要用于在继承结构中，可以支持RTTI类型识别的上下转换。

## 11、函数对象

把有operator() 小括号运算符重载函数的对象，称作函数对象或者仿函数。

- 通过函数对象调用operator()，可以省略函数的调用开销，比通过函数指针调用函数（不能够inline内联调用）效率高。
- 因为函数对象是用类生成的，所以可以添加相关的成员变量，用来记录函数对象使用时的信息。

```C++
//函数对象
template<typename T>
class mygreater
{
public:
    bool operator()(T a, T b) // 二元函数对象
    {
        return a > b;
    }
};
template<typename T>
class myless
{
public:
    bool operator()(T a, T b)
    {
            return a < b;
    }
};
// complate是C++的库函数模板
template<typename T, typename Compare>
bool cpmpare(T a, T b, Compare comp)
{
    // 通过函数指针调用函数，是没有办法内联的，效率很低，因为有函数调用开销
    return comp(a, b); // operator()(a, b);
}
int main()
{
    cout << compare(10, 20, mygreater<int>()) << endl;
    cout << compare(10, 20, myless<int>()) << endl;
    
    return 0;
}
```

## 12、菱形继承

多重继承-菱形继承的问题：

- 好处：可以做更多代码的复用。

基类被多个派生类用就需要是虚继承，不然就会报错。

基类需要被最后的派生类初始化。

## 13、对象的构造函数

- 构造函数是一种特殊的成员函数。
- 构造函数不需要用户调用，而是在建立对象时自动执行。
- 构造函数的名字必须与类名相同，而不能由用户任意命名。
- 够赞函数的功能是由用户定义的，用户根据初始化的要求设计函数体和函数参数。

## 14、不同对象执行析构函数的动机

- 对于函数中定义的自动局部对象，当函数被调用结束时，对象释放，在对象释放前自动执行析构函数。
- static局部对象只在main()函数结束或调用exit函数结束程序时，调用static局部对象的析构函数。
- 对于全局对象，在程序的流程离开其作用域时（如main函数结束或调用exit函数）时，调用该全局对象的析构函数。
- 只要对象的生命周期结束，程序就自动执行实现设计好的析构函数来完成相关工作。

## 15、派生类的构造函数和析构函数

- 派生类中由基类继承而来的成员的初始化工作还是由基类的构造函数完成，派生类中新增的成员在派生类中构造函数中初始化。
- 如果基类中没有不带参数的构造函数，那么在派生类的构造函数中必须调用基类构造函数，以初始化积累成员。
- 派生类中构造函数的执行程序：
  - 先执行基类构造函数，调用顺序按照他们被继承时声明的顺序。
  - 再执行内嵌成员对象的构造函数，调用顺序按照他们在类中声明的顺序。
  - 最后执行派生类构造函数。
- 派生类析构函数的执行顺序
  - 先执行派生类的析构函数。
  - 再执行内嵌成员对象的析构函数，调用顺序按照他们在类中声明的反顺序。
  - 再执行其基类的析构函数，调用顺序按照他们被继承时声明的顺序。

## 16、类的封装

- 将有关的代码和数据封装在一个对象中，每个对象间相互独立，互不干扰。
- 将对象中的某些部分对外隐蔽，隐蔽内部细节，只留下少量接口。
- 类具有信息隐藏的能力，能够有效的把类的内部数据隐藏起来，使外部函数只有只有通过类的公有成员才能访问类的内部数据。
- 封装使类成为一个具有内部数据的自我隐藏能力，功能独立的软件模块。
- 类的公有成员也称为类的接口，外部函数要访问类的内部成员，只有通过类的公有成员才能实现。

## 17、形参带默认值的函数

- 给默认值的时候，从右向左给。
- 调用效率的问题。
- 定义出可以给形参默认值，声明也可以给形参默认值形参给默认值的时候，不管是定义处给，还是声明处给，形参默认值只能出现一次。

## 18、智能指针

1. unique_ptr
   - 表示互斥权，拷贝构造函数和拷贝赋值运算符都delete了。
   - 一个unique_ptr拥有一个对象，在某一时刻，只能由一个unique_ptr指向一个给定的对象。当unique_ptr被销毁，所致的对象也被销毁。
   - unique_ptr不能拷贝，不能赋值，可以移动。
   - 为动态分配的内存提供异常安全，unique_ptr可以理解为一个简单的指针（指向一个对象）或一对指针（包含释放器deleter的情况）。
   - 将动态分配内存的所有权传递给函数。
   - 从函数返回动态分配的内存。
2. shared_ptr：
   - shared_ptr表示共享所有权，可以共享一个对象。当两段代码都没有独享所有权（负责销毁对象）时，可以使用shared_ptr。shared_ptr是一种计数指针，当计数变为0时释放所指向的对象。
   - 其实就是一个指针套上了释放器，套上了计数器，拷贝的时候增加了引用，赋值也增加了引用，相应的也会有递减了引用计数。
3. weak_ptr：
   - 是一种不控制所指向对象生存期的智能指针，指向由一个shared_ptr管理的对象。
   - weak_ptr，不影响shared_ptr的引用计数。一旦shared_ptr被销毁，那么对象也会被销毁，即使weak_ptr还指向这个对象，这个对象也会被销毁。
4. auto_ptr:

被 c++11 弃用，原因是缺乏语言特性如 “针对构造和赋值” 的 `std::move` 语义，以及其他瑕疵。

**auto_ptr 与 unique_ptr 比较**

- auto_ptr 可以赋值拷贝，复制拷贝后所有权转移；unqiue_ptr 无拷贝赋值语义，但实现了`move` 语义；
- auto_ptr 对象不能管理数组（析构调用 `delete`），unique_ptr 可以管理数组（析构调用 `delete[]` ）；

## 19、纯虚函数和虚函数的区别

- 虚函数和纯虚函数可以定义在同一个类中，含有纯虚函数的类被称为抽象类，而只含有虚函数的类不能被称为抽象类。
- 虚函数可以直接被使用，也可以被子类重载以后以多态的形式调用，而纯虚函数必须在子类中实现该函数才可以使用，因为纯虚函数在基类只有声明而没有定义。
- 虚函数和纯虚函数都可以在子类中被重载，以多态的形式被调用。
- 虚函数和纯虚函数通常存在于抽象基类之中，被继承的子类重载，目的是提供一个统一的接口。
- 虚函数和纯虚函数的定义中不能有static标识符，被static修饰的函数在编译时候要求前期绑定，然而虚函数却是动态绑定，而且被两者修饰的函数声明周期也不一样。
- 虚函数必须实现，如果不实现，编译器将报错。
- 虚函数，父类和子类都有各自的版本。由多态方式调用的时候动态绑定。
- 实现了纯虚函数的子类，改纯虚函数在子类中就变成了虚函数，子类的子类可以覆盖。该虚函数，由多态方式调用的时候动态绑定
- 虚函数是C++中用于实现多态的机制。核心理念就是通过基类访问派生类定义的。
- **包含纯虚函数的类叫做抽象类（也称为接口类），抽象类不能实例化处对象。**

## 20、哈希函数

- 哈希冲突的产生原因：哈希是通过对数据进行再压缩，提高效率的一种解决方法。但由于通过哈希函数产生的哈希值是有限的，而数据可能比较多，导致经过哈希函数处理后仍然有不同的数据对应相同的值。这时候就产生了哈希冲突。
- Hash哈希冲突发生的场景：当关键字值域远大于哈希表的长度，而且事先并不知道关键字的具体取值时。hash冲突就会发生。
- Hash溢出发生的场景：当关键字的实际取值大于哈希表的长度时，而且表中已装满了记录，如果插入一个新纪录，不仅发生冲突，而且还会发生溢出。

解决哈希冲突的方法主要有：**开放地址法和拉链法。**

1. 开放地址法：
   1. 线性探测：按顺序决定值时，如果数据的值已经存在，则在原来值的基础上往后加一个单位，直至不发生哈希冲突。
   2. 再平方探测：按顺序决定值时，如果某数据的值已经存在，则在原来值的基础上先加1的平方个单位，若仍然存在则减1的平方个单位。随之是2的平方，3的平方等。直至不发生哈希冲突。
   3. 伪随机探测：按顺序决定值时，如果某数据已经存在，通过随机函数随机生成一个数，在原来值得基础上加上随机数，直至不发生哈希冲突。
2. 链式地址法：

对于相同的值，使用链表进行连接。使用数组存储每一个链表

- 优点：
  - 拉链法处理冲突简单，且无堆积现象，即非同义词绝不会发生冲突，因此平均查找长度较短。
  - 由于拉链中个链表的节点空间时动态申请的，故它更适合于造表前无法确定表长的情况。
  - 开放定址法为减少冲突，要求装填因子较小，故当节点规模较大时会浪费很多空间。而拉链法中可取a>=1,且节点较大时，拉链法中增加的指针域可忽略不计，因此节省空间。
  - 再用拉链法构造的散列表中，删除节点的操作易于实现，只要简单地山区链表上相应的节点即可。
- 缺点：
  - 指针占用较大空间时，会造成空间浪费，若空间用于增大散列表规模进而提高开放地址法的效率。
  - 建立公共溢出区存储所有哈希冲突的数据。
  - 对于冲突的哈希值再次进行哈希处理，直至没有哈希冲突。

### 1. 开放定址法（Open Addressing）

开放定址法也称为闭散列法，当发生哈希冲突时，会在哈希表中寻找下一个空闲的位置来存储冲突的元素。常见的开放定址法有以下几种：

#### 1.1 线性探测（Linear Probing）

线性探测是最简单的开放定址法。当发生冲突时，从冲突的位置开始，依次向后查找，直到找到一个空闲的位置。

公式： 

![公式](https://cdn.jsdelivr.net/gh/aqjsp/Pictures/image-20241117004537044.png)

优点：实现简单。

缺点：容易产生聚集现象，即冲突的元素会集中在某些区域，导致后续的插入操作变得缓慢。

示例：

```c
int hash(int key, int table_size) {
    return key % table_size;
}

int linear_probe(int table[], int key, int table_size) {
    int index = hash(key, table_size);
    int i = 0;
    while (table[(index + i) % table_size] != -1 && i < table_size) {
        i++;
    }
    return (index + i) % table_size;
}
```

#### 1.2 二次探测（Quadratic Probing）

二次探测通过增加探测步长的平方来减少线性探测中的聚集现象。

公式：

![公式](https://cdn.jsdelivr.net/gh/aqjsp/Pictures/image-20241117004646513.png)

优点：减少了线性探测中的聚集现象。

缺点：仍然可能存在二次聚集现象，即不同键的探测路径可能会重叠。

示例：

```c
int quadratic_probe(int table[], int key, int table_size) {
    int index = hash(key, table_size);
    int i = 0;
    while (table[(index + i * i) % table_size] != -1 && i < table_size) {
        i++;
    }
    return (index + i * i) % table_size;
}
```

#### 1.3 双散列（Double Hashing）

双散列使用两个不同的哈希函数来减少聚集现象。第一个哈希函数确定初始位置，第二个哈希函数确定步长。

公式：

![公式](https://cdn.jsdelivr.net/gh/aqjsp/Pictures/image-20241117004728110.png)

优点：进一步减少了聚集现象。

缺点：实现稍微复杂一些。

示例：

```c
int hash1(int key, int table_size) {
    return key % table_size;
}

int hash2(int key, int table_size) {
    return 1 + (key % (table_size - 1));
}

int double_hashing(int table[], int key, int table_size) {
    int index = hash1(key, table_size);
    int step = hash2(key, table_size);
    int i = 0;
    while (table[(index + i * step) % table_size] != -1 && i < table_size) {
        i++;
    }
    return (index + i * step) % table_size;
}
```

### 2. 链地址法（Separate Chaining）

链地址法也称为开散列法，当发生冲突时，将冲突的元素存储在一个链表中。每个哈希表的位置（桶）存储一个链表的头指针。

优点：

- 实现简单。
- 不容易产生聚集现象。
- 适合处理大量数据。

缺点：

- 需要额外的内存来存储链表节点。
- 插入和查找操作的性能取决于链表的长度。

示例：

```c
struct ListNode {
    int key;
    ListNode* next;
};

class HashTable {
private:
    int table_size;
    ListNode** table;

public:
    HashTable(int size) : table_size(size) {
        table = new ListNode*[table_size];
        for (int i = 0; i < table_size; i++) {
            table[i] = nullptr;
        }
    }

    ~HashTable() {
        for (int i = 0; i < table_size; i++) {
            ListNode* node = table[i];
            while (node) {
                ListNode* temp = node;
                node = node->next;
                delete temp;
            }
        }
        delete[] table;
    }

    int hash(int key) {
        return key % table_size;
    }

    void insert(int key) {
        int index = hash(key);
        ListNode* new_node = new ListNode();
        new_node->key = key;
        new_node->next = table[index];
        table[index] = new_node;
    }

    bool find(int key) {
        int index = hash(key);
        ListNode* node = table[index];
        while (node) {
            if (node->key == key) {
                return true;
            }
            node = node->next;
        }
        return false;
    }

    void remove(int key) {
        int index = hash(key);
        ListNode* node = table[index];
        ListNode* prev = nullptr;
        while (node) {
            if (node->key == key) {
                if (prev) {
                    prev->next = node->next;
                } else {
                    table[index] = node->next;
                }
                delete node;
                return;
            }
            prev = node;
            node = node->next;
        }
    }
};
```

### 3. 再哈希法（Rehashing）

再哈希法使用多个不同的哈希函数，当发生冲突时，依次使用这些哈希函数计算新的位置，直到找到一个空闲的位置。

优点：减少了聚集现象。

缺点：

- 计算时间增加。
- 需要管理多个哈希函数。

示例：

```c
int rehash1(int key, int table_size) {
    return key % table_size;
}

int rehash2(int key, int table_size) {
    return 1 + (key % (table_size - 1));
}

int rehash3(int key, int table_size) {
    return key % (table_size - 2);
}

int resolve_conflict(int table[], int key, int table_size) {
    int index = rehash1(key, table_size);
    if (table[index] == -1) {
        return index;
    }
    index = rehash2(key, table_size);
    if (table[index] == -1) {
        return index;
    }
    index = rehash3(key, table_size);
    if (table[index] == -1) {
        return index;
    }
    return -1; // 表已满
}
```

### 4. 建立公共溢出区（Overflow Area）

建立公共溢出区的方法是将哈希表分为基本表和溢出表两部分，当发生冲突时，将冲突的元素存储在溢出表中。

优点：简单易实现。

缺点：

- 需要额外的内存来存储溢出表。
- 插入和查找操作的性能取决于溢出表的长度。

示例：

```c
class HashTable {
private:
    int* basic_table;
    int* overflow_table;
    int basic_size;
    int overflow_size;
    int overflow_count;

public:
    HashTable(int size) : basic_size(size), overflow_size(size), overflow_count(0) {
        basic_table = new int[basic_size];
        overflow_table = new int[overflow_size];
        for (int i = 0; i < basic_size; i++) {
            basic_table[i] = -1;
        }
        for (int i = 0; i < overflow_size; i++) {
            overflow_table[i] = -1;
        }
    }

    ~HashTable() {
        delete[] basic_table;
        delete[] overflow_table;
    }

    int hash(int key) {
        return key % basic_size;
    }

    void insert(int key) {
        int index = hash(key);
        if (basic_table[index] == -1) {
            basic_table[index] = key;
        } else {
            if (overflow_count < overflow_size) {
                overflow_table[overflow_count++] = key;
            } else {
                std::cerr << "Overflow area is full!" << std::endl;
            }
        }
    }

    bool find(int key) {
        int index = hash(key);
        if (basic_table[index] == key) {
            return true;
        }
        for (int i = 0; i < overflow_count; i++) {
            if (overflow_table[i] == key) {
                return true;
            }
        }
        return false;
    }

    void remove(int key) {
        int index = hash(key);
        if (basic_table[index] == key) {
            basic_table[index] = -1;
        } else {
            for (int i = 0; i < overflow_count; i++) {
                if (overflow_table[i] == key) {
                    overflow_table[i] = -1;
                    break;
                }
            }
        }
    }
};
```

## 21、C++强制转换

1. static_cast

- static_cast 用于进行比较“自然”和低风险的转换，如整型和浮点型、字符型之间的互相转换。另外，如果对象所属的类重载了强制类型转换运算符 T（如 T 是 int、int* 或其他类型名），则 static_cast 也能用来进行对象到 T 类型的转换。
- static_cast 不能用于在不同类型的指针之间互相转换，也不能用于整型和指针之间的互相转换，当然也不能用于不同类型的引用之间的转换。因为这些属于风险比较高的转换。

2. reinterpret_cast

reinterpret_cast 用于进行各种不同类型的指针之间、不同类型的引用之间以及指针和能容纳指针的整数类型之间的转换。转换时，执行的是逐个比特复制的操作。

3. const_cast

const_cast 运算符仅用于进行去除 const 属性的转换，它也是四个强制类型转换运算符中唯一能够去除 const 属性的运算符。

4. dynamic_cast

用 reinterpret_cast 可以将多态基类（包含虚函数的基类）的指针强制转换为派生类的指针，但是这种转换不检查安全性，即不检查转换后的指针是否确实指向一个派生类对象。dynamic_cast专门用于将多态基类的指针或引用强制转换为派生类的指针或引用，而且能够检查转换的安全性。对于不安全的指针转换，转换结果返回 NULL 指针。

## 22、构造函数为什么不能是虚函数

虚函数对应一个虚指针，虚指针其实是存储在对象的内存空间的。如果构造函数是虚函数，就需要通过虚函数表中对应的虚函数指针（编译期间生成属于类）来调用，可对象目前还没有实例化，也即是还没有内存空间，何来的虚指针，所以构造函数不能是虚函数。

虚函数的作用在于通过父类的指针或者引用来调用它的成员函数的时候，能够根据动态类型来调用子类相应的成员函数。而构造函数是在创建对象时自动调用的，不可能通过父类的指针或者引用去调用，所以构造函数不能是虚函数。

## 23、虚函数可以是内联函数吗

- 虚函数可以是内联函数，内联是可以修饰虚函数的，但是当虚函数表现多态性的时候不能内联。
- 内联是在编译期建议编译器内联，而虚函数的多态性在运行期，编译器无法知道运行期调用哪个代码，因此虚函数表现为多态性时（运行期）不可以内联。
- `inline virtual` 唯一可以内联的时候是：编译器知道所调用的对象是哪个类（如 `Base::who()`），这只有在编译器具有实际对象而不是对象的指针或引用时才会发生。

## 24、strcat、strcpy、strcmp区别

1. **strcat函数原型**char* strcat(char* dest,const char* src); 
   1.  进行字符串的拼接，将第二个字符串连接到第一个字符串中第一个出现\0开始的地方。返回的是拼接后字符的首地址。并不检查第一个数组的大小是否可以容纳第二个字符串。如果第一个数组的已分配的内存不够容纳第二个字符串，则多出来的字符将会溢出到相邻的内存单元。
2. **strncat函数原型:strncat(dest,src,maxsize)**
   1.  功能跟strcat一致，不过它带有一个maxsize的参数，设置容纳的最大的字符长度。如在遇到\0之前达到了最大字符长度，则会只连接最大字符长度个数的字符。
3. **strcpy函数原型** char* strcpy(char* dest,const char* src); 
   1.  将第二个字符串\0之前的字符复制到第一个内存地址内。返回的是复制后的字符串的首地址。有char*返回值是为了函数能够支持链式表达式，增加了函数的“附加值”。 char a[7]="abcdef",char b[5]="xyz";
   2.  strcpy(a,b)函数 当将后面的数组赋值给前面那个时侯 除去五个元素后，从下标为5开始的元素仍旧是之前a[5]的元素。
4. **strncpy(str1,str2,numbe)**

是将str2中的前number个字符赋给str1，或是将\0之前的字符赋给str1.

5. **strcmp函数原型** int strcmp(const char *src1,const char* src2);

 进行两个字符串中从第一个开始的ASCII码的比较。遇到\0或者不一致时退出，如果前者大于后者返回1，小于返回-1 如果在两个中的任何一个的\0之前都保持一致，则返回0. 当src或src遇到\0时即停止比较.strcmp比较的是字符串，不是字符，字符之间的比较可以直接用==

6. **strncmp(str1,str2,numbe)**

函数在strcmp的基础上多了一个int参数，即指定比较前几个字符是否相等。

## 25、虚函数和静态函数的区别

静态函数在编译时就已经确定，而虚函数在运行时动态绑定。虚函数是实现多态重要手段，在函数前加virtual关键字即可。

## 26、静态函数与静态变量的区别

1. 类静态数据成员在编译时创建并初始化：在该类的任何对象建立之前就存在，不属于任何对象，而非静态类成员变量则是属于对象所有的。类静态数据成员只有一个拷贝，为所有此类的对象所共享。
2. 类静态成员函数属于整个类，不属于某个对象，由该类所有对象共享。
   - static 成员变量实现了同类对象间信息共享。
   - static 成员类外存储，求类大小，并不包含在内。
   - static 成员是命名空间属于类的全局变量，存储在 data 区的rw段。
   - static 成员只能类外初始化。
   - 可以通过类名访问（无对象生成时亦可），也可以通过对象访问。

## 27、public、protected、private区别

- public: 可以被该类中的函数、子类的函数、其友元函数访问，也可以由该类的对象访问。
- protected: 可以被该类中的函数、子类的函数、以及其友元函数访问，但不能被该类的对象访问。
- private: 只能由该类中的函数、其友元函数访问,不能被任何其他访问，该类的对象也不能访问。