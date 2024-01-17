# C++可调用对象

一组执行任务的语句都可以视为一个函数，一个可调用对象。在平常程序设计的过程中，我们习惯性的将那些具有复用性的一组语句抽象为函数，将变化的部分抽象为函数的参数。可调用对象用处广泛，比如在使用一些基于范围的模板函数时（如 sort()、all_of()、find_if() 等），常常需要我们传入一个可调用对象，以指明我们需要对范围中的每个元素进行怎样的处理。又比如，在处理一些回调函数、触发函数时，也常常会使用可调用对象。

那么，**满足以下条件之一就可称为可调用对象**：

- 是一个函数指针。
- 是一个具有operator()成员函数的类对象(传说中的仿函数)，lambda表达式。
- 是一个可被转换为函数指针的类对象。
- 是一个类成员(函数)指针。
- bind表达式或其它函数对象。

函数的使用极大的减少了代码重复率，提高了代码的灵活性。那么，在C++中的可调用对象都有哪些？

![image-20240117204702027](https://raw.githubusercontent.com/aqjsp/Pictures/main/202401172047222.png)

接下来就将这些可调用对象展开详细讲讲：

## 1、普通函数

这种函数比较简单，一般都声明在文件开头。在这就举个简单的例子：

```C
#include <iostream>

int add(int a, int b) {
    return a + b;
}

int main() {
    int num1, num2;
    std::cout << "Enter two numbers: ";
    std::cin >> num1 >> num2;

    int sum = add(num1, num2);

    std::cout << "The sum of " << num1 << " and " << num2 << " is: " << sum << std::endl;

    return 0;
}
```

以上就是最简单的一个函数调用的例子，大家看起来肯定就一目了然了。

## 2、类成员函数

### 2.1、基础概念

类的成员函数是**指那些把定义和原型写在类定义内部的函数，就像类定义中的其他变量一样。**类成员函数是类的一个成员，它可以操作类的任意对象，可以访问对象中的所有成员。

### 2.2、实例

先看看自己定义的Box类，我们要使用成员函数来访问类的成员，而不是直接访问这些类的成员：

```C
class Box{
public:
    double length;         // 长度
    double breadth;        // 宽度
    double height;         // 高度
    double getVolume(void);// 返回体积
};
```

成员函数可以定义在类定义内部，或者单独使用**范围解析运算符 ::** 来定义。在类定义中定义的成员函数把函数声明为**内联**的，即便没有使用 inline 标识符。所以我们就可以按照下面这个方式定义getVolume()函数：

```C
class Box{
public:
    double length;      // 长度
    double breadth;     // 宽度
    double height;      // 高度

    double getVolume(void)
    {
        return length * breadth * height;
    }
};
```

也可以在类的外部使用范围解析运算符::定义这个函数：

```C
double Box::getVolume(void)
{
    return length * breadth * height;
}
```

**需要注意的是，在范围解析运算符之前必须使用类名。**

调用成员函数的时候，是在对象上使用点运算符（.），这样的话，它就能操作与该对象相关的数据：

```C
Box myBox;          // 创建一个对象
 
myBox.getVolume();  // 调用该对象的成员函数
```

下边就简单写一下这个具体的实现：

```C
#include <iostream>
 
using namespace std;
 
class Box{
public:
    double length;         // 长度
    double breadth;        // 宽度
    double height;         // 高度

    // 成员函数声明
    double getVolume(void);
    void setLength( double len );
    void setBreadth( double bre );
    void setHeight( double hei );
};
 
// 成员函数定义
double Box::getVolume(void)
{
    return length * breadth * height;
}
 
void Box::setLength( double len )
{
    length = len;
}
 
void Box::setBreadth( double bre )
{
    breadth = bre;
}
 
void Box::setHeight( double hei )
{
    height = hei;
}

int main( )
{
   Box box;                // 声明 box，类型为 Box
   double volume = 0.0;     // 用于存储体积
 
   // 详述
   box.setLength(6.0); 
   box.setBreadth(7.0); 
   box.setHeight(5.0);
 
   // 体积
   volume = box.getVolume();
   cout << "box 的体积：" << volume <<endl;

   return 0;
}
```

## 3、类静态函数

**类中的静态成员函数作用在整个类的内部**，对应类的所有实例是共享静态成员函数的，在调用静态成员函数的时候跟调用非静态成员函数是有区别的。另外，静态成员函数只能访问对应类内部的静态数据成员，否则会出现编译错误。

```C++
class Box{
private:
    int _non_static;
    static int _static;
public:
    int a(){
        return _non_static;
    }
    static int b(){
        //_non_static=0; 错误
        //静态成员函数不能访问非静态成员变量
        return _static;
     }
    static int f(){
        //a();        （不对，静态成员函数不能访问非静态成员函数）
        return b();
    }
};
int Box::_static= 0;// static静态成员变量可以在类的外部修改
int main(){
    Box box;
    Box* pointer=&box;
    box.a();
    pointer->a();
    Box::b(); // 类名::静态成员函数名
    return 0;
}
```

另外，对于非静态成员变量，需要用到类的对象来进行调用，这里就涉及到this指针的概念：

静态成员函数由于没有传递this指针，所以静态成员函数只能访问static 成员，不能访问非static成员。this作用域是在类内部，当在类的非静态成员函数中访问类的非静态成员的时候，编译器会自动将对象本身的地址作为一个隐含参数传递给函数。换句话说，你没有写上this指针，编译器在编译的时候也是加上this的，它作为非静态成员函数的隐含形参，对各成员的访问均通过this进行。在成员函数的内部，我们可以直接使用调用该函数的对象的成员，而无需通过成员访问运算符来做到这一点，因为this所指的正是这个对象。任何对类成员的直接访问都被看成this的隐式使用。this的目的总是指向这个对象，所以this是一个常量指针，C++中不允许改变this中保存的地址。

说这么多的意思就只有下边两点：（静态成员函数和普通成员函数的区别）

- **静态成员函数没有 this 指针，只能访问静态成员（包括静态成员变量和静态成员函数）。**
- **普通成员函数有 this 指针，可以访问类中的任意成员；而静态成员函数没有 this 指针。**

当然类的static函数在类内声明、类外定义时，类内要写明static 类外则不能加static关键字，否则也会**出现编译错误**，比方说：

```C
class Box{
public:
    static int f();
};
/*错误的写法
static int Box::f(){
    return 0;
}
*/
//正确的定义
int Box::f(){
    return 0;
}
int main(){
    return 0;
}
```

## 4、仿函数

### 4.1、什么是仿函数？

#### 4.1.1、基本概念

仿函数其实就是**重载了operator()运算符的类对象**。可以视为一个一般的函数，只不过这个函数功能是在一个类中的运算符operator()中实现，是一个函数对象，它将函数作为参数传递的方式来使用。（行为类似函数，故称仿函数）。实际上就是创建一个类，该类重载了operator()运算符，使得类的实例可以像函数一样被调用。这允许你在函数对象内部保存状态，并在调用时执行操作。

#### 4.1.2、仿函数的引入

先看个例子：

```C
class Foo
{
    void operator()()
    {
        cout << __FUNCTION__ << endl;
    }
};
 
int main()
{
    Foo a;
    //定义对象调用
    a.operator()();
    //直接通过对象调用
    a();
    //通过临时对象调用
    Foo()();
}
```

在类中重载了一个括号运算符，有几种方法可以调用这个函数呢？

1. 定义一个结构体对象，然后显式访问这个函数：operator()，需要注意的是，后面得再加上一个括号。
2. 通过结构体对象直接访问函数。
3. 通过结构体匿名对象，访问并调用函数。

上面这3种方法都可以调用这个重载运算符函数，那么这样有什么用呢，接下来的例子调用方函数会用到。

### 4.2、仿函数有什么用呢？

> 问题：分别统计一个vector中每个元素等于3，小于3，大于3的个数。

1. 在使用的时候，可以像普通函数那样调用，可以有参数，可以有返回值：

```C
#include<iostream>
#include<vector>

int equal_count(const std::vector<int>::iterator& a, const std::vector<int>::iterator& b, const int& val)
{
    int count_num = 0;
    for (auto it = a; it != b; it++)
    {
        if (*it == val)
        {
            count_num++;
        }
    }
    return count_num;
}

int greater_count(const std::vector<int>::iterator& a, const std::vector<int>::iterator& b, const int& val)
{
    int count_num = 0;
    for (auto it = a; it != b; it++)
    {
        if (*it > val)
        {
            count_num++;
        }
    }
    return count_num;
}

int less_count(const std::vector<int>::iterator& a, const std::vector<int>::iterator& b, const int& val)
{
    int count_num = 0;
    for (auto it = a; it != b; it++)
    {
        if (*it < val)
        {
            count_num++;
        }
    }
    return count_num;
}
int main()
{
    std::vector<int> a{ 1,2,3,4,4,5,6,8,7 };
    int num1 = equal_count(a.begin(), a.end(), 3);
    std::cout << "等于3: " << num1 << "个" << std::endl;
    int num2 = greater_count(a.begin(), a.end(), 3);
    std::cout << "大于3: " << num2 << "个" << std::endl;
    int num3 = less_count(a.begin(), a.end(), 3);
    std::cout << "小于3: " << num3 << "个" << std::endl;
}
```

结果：

![image-20240117204748566](https://raw.githubusercontent.com/aqjsp/Pictures/main/202401172047862.png)

上面这些代码，我们肉眼可见的长，这些函数不仅长，而且它们的功能都是类似的，那我们为何要写这么长三个函数呢？所以我们就要思考，是否可以用一个函数的主体，然后写三个子函数，再传入主体函数呢？

这就是**函数的可调用对象写法**

2. 函数的可调用对象写法

```C
#include <iostream>
#include <vector>
/*
可调用对象
*/
template <class FUN> // 
int count_if(const std::vector<int>::iterator& a, const std::vector<int>::iterator& b,FUN func) // FUN func用于判断的函数对象
{
    int count_num = 0;
    for (auto it = a; it != b; it++)
    {
        if (func(*it))
        {//传进去的是值
            count_num++;
        }
    }
    return count_num;
}
bool _equal(int a)
{
    return a == 3;
}
bool _greater(int a)
{
    return a > 3;
}
bool _less(int a)
{
    return a < 3;
}
int main()
{
    std::vector<int> a{ 1,2,3,4,4,5,6,8,7};
    int num1 = count_if(a.begin(), a.end(), _equal);
    std::cout << num1 << std::endl;
    int num2 = count_if(a.begin(), a.end(), _greater);
    std::cout << num2 << std::endl;
    int num3 = count_if(a.begin(), a.end(), _less);
    std::cout << num3 << std::endl;
}
```

这样的话，这些函数看起来就清晰很多了，使用一个模板参数用作函数指针，然后再写三个简单的子函数，在主函数中调用三个子函数，当条件为true时，就可以起到统计次数的作用。

我们其实还可以使用Lambda表达式来写：

```C
int num1 = count_if(a.begin(), a.end(), [](const int& data) {return data == 3; });
cout << num1 << endl;
int num2 = count_if(a.begin(), a.end(), [](const int& data) {return data > 3; });
cout << num2 << endl;
int num3 = count_if(a.begin(), a.end(), [](const int& data) {return data < 3; });
cout << num3 << endl;
```

显而易见，使用**可调用对象这个方法的话，比第一种普通函数有着极大的提升，不用写那几个子函数，而是根据它们的共性：都具有统计的功能，只改变子函数的功能，就可以完成这些功能。**

其实这个方法已经够日常使用了，但是还有一个更优的写法，那就是**仿函数。**

3. 仿函数写法

```C
template <class FUN>
int count_if(const std::vector<int>::iterator& a, const std::vector<int>::iterator& b, FUN func)
{
        int count_num = 0;
        for (auto it = a; it != b; it++)
        {
                if (func(*it))
                {//传进去的是值
                        count_num++;
                }
        }
        return count_num;
}

/*
仿函数
*/
struct Equal
{
    int val;
    Equal(const int& val) :val(val) {}
    bool operator()(const int& a)
    {
        return a == val;
    }
};

struct Greater
{
    int val;
    Greater(const int& val) :val(val) {}
    bool operator()(const int& a)
    {
        return a > val;
    }
};

struct Less
{
    int val;
    Less(const int& val) :val(val) {}
    bool operator()(const int& a)
    {
        return a < val;
    }
};
int main()
{
    std::vector<int> a{ 1,2,3,4,4,5,6,8,7 };
    /*
    仿函数 传递数值的方式
    */
    Less less(10);        //可以创建对象来调用
    int num1 = count_if(a.begin(), a.end(), Equal(4));        //匿名对象
    std::cout << "等于4的个数：" << num1 << std::endl;
    int num2 = count_if(a.begin(), a.end(), Greater(3));        //匿名对象
    std::cout << "大于3的个数："  << num2 << std::endl;
    int num3 = count_if(a.begin(), a.end(), less);                //临时对象
    std::cout << "小于10的个数：" << num3 << std::endl;
}
```

结果：

![image-20240117204844826](https://raw.githubusercontent.com/aqjsp/Pictures/main/202401172048217.png)

上面这个就是仿函数的写法了，表面上看起来是挺长的，看起来还不如第二种方式，可以直接在函数中调用，但是第二种方式还有一个很麻烦的缺点：我们在每次修改数值的时候，第二种方式直接就是在函数中写死了，或者说是在函数调用的时候，只会看到调用的函数名称，并不会看到实际比较的数值。

但是第三种方式仿函数，使用过程中可以指定任意数值，并且还重载了括号运算符，我们可以看到具体的数值，这就是符合我们日常维护的习惯。

### 4.3、优缺点

1. 优点
   - 仿函数比函数指针的执行速度快，函数指针是通过地址调用，而仿函数是对运算符operator进行自定义来提高调用的效率。
   - 仿函数比一般函数灵活，可以同时拥有两个不同的状态实体，一般函数不具备此种功能。
   - 仿函数可以作为模板参数使用，因为每个仿函数都拥有自己的类型。
2. 缺点
   - 需要单独实现一个类。
   - 定义形式比较复杂。

## 5、函数指针

### 5.1、函数指针的引出

与数据项相似，函数也有地址。函数的地址就是存储它的机器语言代码内存的开始地址。通常情况下，这些地址对用户而言，既不重要，也没有什么用处，但对于程序而言，却很有用。比方说，可以编写将另一个函数的地址作为参数的函数，这样第一个函数就能够找到第二个函数，并运行它。与直接调用另一个函数相比，这种方法虽然很笨拙，但是它允许在不同的时间传递不同函数的地址，这也就意味着可以在不同的时间使用不同的函数。

### 5.2、函数指针的使用

通过上面的讲述，我们可以通过一个例子来阐述一下这个过程。假设要设计一个名为estimate()的函数，估算编写指定行数的代码所需的时间，并且希望不同的程序员都将使用该函数。对于所有的用户来说，estimate()中一部分代码都是相同的，但该函数允许每个程序员提供自己的算法来估算时间。为了实现这个目标，采用的机制是，将程序员要使用的算法函数的地址传递给estimate()。那就要能够完成下面的一些工作：

- 获取函数的地址；
- 声明一个函数指针；
- 使用函数指针来调用函数。

#### 5.2.1、获取函数的地址

获取函数的地址很简单：只要使用函数名（后面不跟参数）即可。也就是说，如果think()是一个函数，则think就是该函数的地址。要将函数作为参数进行传递，必须传递函数名。一定要区分传递的是函数的地址还是函数的返回值：

```C
process(think);
thought(think());
```

process()调用使得process()函数能够在其内部调用think()函数。thought()调用首先调用think()函数，然后think()的返回值传递给thought()函数。

#### 5.2.2、声明函数指针

声明指向某种数据类型的指针时，必须指定指针指向的类型。同样，声明指向函数的指针式，也必须指定指针指向的函数类型。这也就意味着声明应指定函数的返回类型以及函数的参数列表。也就是说，声明应像函数原型那样指出有关函数的信息。例如，编写一个估算时间的函数，其原型如下：

```C
double proto(int);
```

则正确的指针类型声明如下：

```C
double (*pf)(int);
```

这与proto()声明类似，只是将proto替换为(*pf)。由于proto是函数，因此(*pf)也是函数，而如果(*pf)是函数，那pf就是函数指针。

> 温馨提示：如果要声明指向指定类型的函数的指针，可以首先编写这种函数的原型，然后用(*pf)替换函数名，这样pf就是这类函数的指针。

还有就是优先级，我们在声明的时候，必须在声明中使用括号将*pf括起来。括号的优先级比*运算符高。因此，*pf(int)的意思就是pf()是一个返回指针的函数，而(*pf)(int)的意思就是pf是一个指向函数的指针：

```C
double (*pf)(int);
double *pf(int);
```

正确的声明pf后，就可以将相应函数的地址赋给它：

```C
double proto(int);
double (*pf)(int);
pf = proto;
```

需要注意的是，proto()的参数列表和返回类型必须与pf相同，如果不相同，编译器将拒绝这种赋值：

```C
double ned(double);
int ted(int);
double (*pf)(int);
pf = ned;  // 
pf = ted; // 返回类型不匹配
```

那么现在再看一下前面提到的estimate()函数。假设要将将要编写的代码行数和估算算法的地址传递给它，原型如下：

```C
void estimate(int lines, double (*pf)(int));
```

上述声明中，第二个参数是一个函数指针，它指向的函数接收一个int参数，并返回一个double值。要让estimate()使用proto()函数，需要将proto()的地址传递给它：

```C
estimate(50, proto);
```

显而易见，使用函数指针时，比较棘手的是编写原型，而传递地址则非常简单。

#### 5.2.3、使用指针来调用函数

现在进入最后一步，使用指针来调用被指向的函数。线索来自指针声明。前面也说过，(*pf)扮演的角色与函数名相同，因此使用(*pf)时，只需将它看做函数名即可：

```C
double proto(int);
double (*pf)(int);
pf = proto;
double x = proto(4);
double y = (*pf)(5);
```

实际上，C++也允许像使用函数名那样使用pf：

```C
double y = pf(5;)
```

第一种格式显然不太好看，但它给出了强有力的提示：代码正在使用函数指针。

## 6、Lambda函数

### 6.1、Lambda表达式概述

 Lambda表达式是现代C++在C ++ 11和更高版本中的一个新的语法糖 ，在C++11、C++14、C++17和C++20中Lambda表达的内容还在不断更新。 lambda表达式（也称为lambda函数）是在调用或作为函数参数传递的位置处定义**匿名函数对象**的便捷方法。通常，lambda用于封装传递给算法或异步方法的几行代码 。

### 6.2、Lambda表达式定义

#### 6.2.1、Lambda表达式示例

Lambda有很多叫法，有Lambda表达式、Lambda函数、匿名函数，这个总结的话就用lambda表达式来进行叙述。

```C
#include <iostream>

int main() {
    // Lambda 表达式示例
    auto sum = [](int a, int b) { return a + b; };

    // 使用 Lambda 表达式计算和
    int result = sum(5, 3);

    std::cout << "The sum is: " << result << std::endl;

    return 0;
}
```

Lambda表达式允许在代码中直接定义匿名函数，非常方便。在上述示例中，lambda表达式接受两个参数，并返回它们的和。这个lambda表达式可以像普通函数一样使用，可以传递参数，并获得返回值。

#### 6.2.2、Lambda表达式语法定义

```C
  [捕获列表]     [参数列表]  [可变规格]           [返回类型]    [函数体] 
[capture list] (parameters) mutable throw() -> return-type {statement}
                                   [异常说明]
```

说明：

1. 捕获列表。在C ++规范中也称为Lambda导入器， 捕获列表总是出现在Lambda函数的开始处。实际上，[]是Lambda引出符。编译器根据该引出符判断接下来的代码是否是Lambda函数，捕获列表能够捕捉上下文中的变量以供Lambda函数使用。
2. 参数列表。与普通函数的参数列表一致。如果不需要参数传递，则可以连同括号“()”一起省略。
3. 可变规格*。mutable修饰符， 默认情况下Lambda函数总是一个const函数，mutable可以取消其常量性。在使用该修饰符时，参数列表不可省略（即使参数为空）。*
4. 异常说明。用于Lamdba表达式内部函数抛出异常。
5. 返回类型。 追踪返回类型形式声明函数的返回类型。我们可以在不需要返回值的时候也可以连同符号”->”一起省略。此外，在返回类型明确的情况下，也可以省略该部分，让编译器对返回类型进行推导。
6. lambda函数体。内容与普通函数一样，不过除了可以使用参数之外，还可以使用所有捕获的变量。

### 6.3、Lambda表达式参数

#### 6.3.1、Lambda捕获列表

Lambda表达式与普通函数最大的区别是，除了可以使用参数以外，Lambda函数还可以通过捕获列表访问一些上下文中的数据。具体来说，捕捉列表就是描述了上下文中哪些数据可以被Lambda使用，以及使用方式（以值传递的方式或引用传递的方式）。

语法上，在“[]”包括起来的是捕获列表，捕获列表由多个捕获项组成，并以逗号分隔。捕获列表有以下几种形式：

- []表示不捕获任何变量

```C
auto function = ([]{
        std::cout << "Hello World!" << std::endl;
    }
);

function();
```

- [var]表示值传递方式捕获变量var

```C
int num = 100;
auto function = ([num]{
        std::cout << num << std::endl;
    }
);

function();
```

- [=]表示值传递方式捕获所有父作用域的变量（包括this）

```C
int num = 100;
auto function = ([num]{
        std::cout << num << std::endl;
    }
);

function();
```

- [&var]表示引用传递捕捉变量var

```C
int num = 100;
auto function = ([&num]{
        num = 1000;
        std::cout << "num: " << num << std::endl;
    }
);

function();
```

- [&]表示引用传递方式捕捉所有父作用域的变量（包括this）

```C
int index = 1;
int num = 100;
auto function = ([&]{
        num = 1000;
        index = 2;
        std::cout << "index: "<< index << ", " 
            << "num: "<< num << std::endl;
    }
);

function();
```

- [this]表示值传递方式捕捉当前的this指针

```C
#include <iostream>
using namespace std;
 
class Lambda
{
public:
    void sayHello() {
        std::cout << "Hello" << std::endl;
    };

    void lambda() {
        auto function = [this]{ 
            this->sayHello(); 
        };

        function();
    }
};
 
int main()
{
    Lambda demo;
    demo.lambda();
}
```

- [=, &] 拷贝与引用混合

  - [=,&a,&b]表示以引用传递的方式捕捉变量a和b，以值传递方式捕捉其它所有变量。

    - ```C
      int index = 1;
      int num = 100;
      auto function = ([=, &index, &num]{
              num = 1000;
              index = 2;
               std::cout << "index: "<< index << ", " 
                  << "num: "<< num << std::endl;
          }
      );
      
      function();
      ```

- [&,a,this]表示以值传递的方式捕捉变量a和this，引用传递方式捕捉其它所有变量。

需要注意的是，捕捉列表不允许变量重复传递。下面一些例子就是典型的重复，会导致编译时期的错误。例如：

- [=,a]这里已经以值传递方式捕捉了所有变量，但是重复捕捉a了，会报错的;
- [&,&this]这里&已经以引用传递方式捕捉了所有变量，再捕捉this也是一种重复。

#### 6.3.2、Lambda参数列表

除了捕获列表之外，lambda还可以接受输入参数。参数列表是可选的，并且在大多数方面类似于函数的参数列表。

```C
auto function = [] (int a, int b){
    return a + b;
};
        
function(100, 200);
```

#### 6.3.3、可变规格mutable

mutable修饰符， 默认情况下Lambda函数总是一个const函数，mutable可以取消其常量性。在使用该修饰符时，参数列表不可省略（即使参数为空）。

```C
#include <iostream>
using namespace std;

int main()
{
   int m = 0;
   int n = 0;
   [&, n] (int a) mutable { m = ++n + a; }(4);
   cout << m << endl << n << endl;
}
```

#### 6.3.4、异常说明

可以使用throw() 异常规范来指示lambda表达式不会引发任何异常。与普通函数一样，如果lambda表达式声明C4297异常规范且lambda体引发异常，编译器将生成警告 throw() 。

```C
int main() // C4297 expected 
{ 
    []() throw() { throw 5; }(); 
}
```

#### 6.3.5、返回类型

 Lambda表达式的返回类型会自动推导。除非你指定了返回类型，否则不必使用关键字。返回型类似于通常的方法或函数的返回型部分。但是，返回类型必须在参数列表之后，并且必须在返回类型->之前包含类型关键字。如果lambda主体仅包含一个return语句或该表达式未返回值，则可以省略Lambda表达式的return-type部分。如果lambda主体包含一个return语句，则编译器将从return表达式的类型中推断出return类型。否则，编译器将返回类型推导为void。

```C
auto x1 = [](int i){ return i; };
```

### 6.3.6、Lambda函数体

Lambda表达式的lambda主体（标准语法中的*复合语句*）可以包含普通方法或函数的主体可以包含的任何内容。普通函数和lambda表达式的主体都可以访问以下类型的变量：

> 捕获变量
>
> 形参变量
>
> 局部声明的变量
>
> 类数据成员，当在类内声明this并被捕获时
>
> 具有静态存储持续时间的任何变量，例如全局变量

```C
#include <iostream>
using namespace std;

int main()
{
   int m = 0;
   int n = 0;
   [&, n] (int a) mutable { m = ++n + a; }(4);
   cout << m << endl << n << endl;
}
```

### 6.4、Lambda表达式的优缺点

#### 6.4.1、优点

1. 可以直接在需要调用函数的位置定义短小精悍的函数，而不需要预先定义好函数。

```C
std::find_if(v.begin(), v.end(), [](int& item){return item > 2});
```

2. 使用lambda表达式变得更加紧凑，结构层次更加明显、代码可读性更好。

#### 6.4.2、缺点

1. lambda表达式语法比较灵活，增加了代码的难度。
2. 对于函数复用无能为力。

### 6.5、为何使用lambda表达式

三个维度：距离、简洁、效率

1. 距离

因为lambda表达式的定义和使用在同一个地方进行，而函数的话，不能在函数内部定义其他函数，所以函数的定义可能离使用它的地方很远，所以lambda才是最理想的选择。

2. 简洁

函数符代码比函数和lambda代码更繁琐，函数和lambda表达式的简洁程度相当，一个显而易见的例外是，需要使用同一个lambda两次：

```C
count1 = std::count_if(n1.begin(), n1.end(),
            [](int x) {return x % 3 == 0;});
count2 = std::count_if(n2.begin(),n2.end(),
            [](int x) {return x %3 == 0;});
```

但并非必须编写lambda两次，而可给lambda表达式指定一个名称，并使用该名称两次：

```C
auto mod3 = [](int x) {return x % 3 == 0;}
count1 = std::count_if(n1.begin(), n1.end(), mod3);
count2 = std::count_if(n2.begin(), n2.end(), mod3);
```

甚至可以像使用常规函数那样使用有名称的lambda表达式：

```C
bool result = mod3(3); // 如果z%3==0,返回true
```

不同于常规函数，可在函数内部定义有名称的lambda。mod3的实际类型随实现而异，它取决于编译器使用什么类型跟踪lambda。

3. 效率

上面这三种方法的相对效率取决于编译器内联哪些东西。函数指针方法组织了内联，编译器传统上不会内联其它地址被获取的函数，因为函数地址的概念意味着非内联函数。而函数符合lambda通常不会阻止内联。

## 7、std::function与std::bind

### 7.1、由来

从上面介绍的可以看出：由于可调用对象的定义方式比较多，但是函数的调用方式较为类似，因此需要使用一个统一的方式保存可调用对象或者传递可调用对象。于是，std::function就诞生了。

**std::function**强大的地方在于，它能够兼容所有具有相同参数类型的函数实体。

### 7.2、std::function

#### 7.2.1、概念

std::function是一个可调用对象包装器，是一个类模板，可以容纳除了类成员函数指针之外的所有可调用对象，它可以用统一的方式处理函数、函数对象、函数指针，并允许保存和延迟它们的执行。std::function的实例可以存储、复制和调用任何可调用对象，存储的可调用对象称为std::function的目标，若std::function不含目标，则称它为空，调用空的std::function的目标会抛出std::bad_function_call异常。

#### 7.2.2、定义格式：

```C
# include <functional>
std::function<函数类型>
```

#### 7.2.3、示例

std::function可以取代函数指针的作用，因为它可以延迟函数的执行，特别适合作为回调函数使用。它比普通函数指针更加的灵活和便利。

举个例子：

```C
# include <iostream>
# include <functional>

typedef std::function<int(int, int)> comfun;

// 普通函数
int add(int a, int b) { return a + b; }

// lambda表达式
auto mod = [](int a, int b){ return a % b; };

// 函数对象类
struct divide{
    int operator()(int denominator, int divisor){
        return denominator/divisor;
    }
};

int main(){
    comfun a = add;
    comfun b = mod;
    comfun c = divide();
    std::cout << a(5, 3) << std::endl;
    std::cout << b(5, 3) << std::endl;
    std::cout << c(5, 3) << std::endl;
}
```

std::function可以取代函数指针的作用，因为它可以延迟函数的执行，特别适合作为**回调函数**使用。它比普通函数指针更加的灵活和便利。

#### 7.2.4、作用

1. std::function对C++中各种可调用实体(普通函数、Lambda表达式、函数指针、以及其它函数对象等)的封装，形成一个新的可调用的std::function对象，简化调用；
2. std::function对象是对C++中现有的可调用实体的一种类型安全的包裹(如：函数指针这类可调用实体，是类型不安全的)。

### 7.3、std::bind

1. std::bind可以看作一个通用的函数适配器，**它接受一个可调用对象，生成一个新的可调用对象来适应原对象的参数列表**。
2. std::bind将可调用对象与其参数一起进行绑定，绑定后的结果可以使用std::function保存。std::bind主要有以下两个作用：
   1. **将可调用对象和其参数绑定成一个仿函数**；
   2. **只绑定部分参数，减少可调用对象传入的参数**。

#### 7.3.1、std::bind绑定普通函数

```C
double my_divide (double x, double y) {return x/y;}auto fn_half = std::bind (my_divide,_1,2);  
std::cout << fn_half(10) << '\n';   // 5
```

- bind的第一个参数是函数名，普通函数做实参时，会隐式转换成函数指针。因此std::bind (my_divide,_1,2)等价于std::bind (&my_divide,_1,2)；
- _1表示占位符，位于<functional>中，std::placeholders::_1；

#### 7.3.2、std::bind绑定成员函数

```C
struct Foo {void print_sum(int n1, int n2)
{
    std::cout << n1+n2 << '\n';
}
    int data = 10;
};
int main() 
{
    Foo foo;
    auto f = std::bind(&Foo::print_sum, &foo, 95, std::placeholders::_1);
    f(5); // 100
}
```

- bind绑定类成员函数时，第一个参数表示对象的成员函数的指针，第二个参数表示对象的地址。
- 必须显示的指定&Foo::print_sum，因为编译器不会将对象的成员函数隐式转换成函数指针，所以必须在Foo::print_sum前添加&；
- 使用对象成员函数的指针时，必须要知道该指针属于哪个对象，因此第二个参数为对象的地址 &foo；

#### 7.3.3、std::bind绑定一个引用参数

默认情况下，bind的那些不是占位符的参数被拷贝到bind返回的可调用对象中。但是，与lambda类似，有时对有些绑定的参数希望以引用的方式传递，或是要绑定参数的类型无法拷贝。

```C
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std::placeholders;
using namespace std;

ostream & print(ostream &os, const string& s, char c)
{
    os << s << c;
    return os;
}
int main(){
    vector<string> words{"helo", "world", "this", "is", "C++11"};
    ostringstream os;
    char c = ' ';
    for_each(words.begin(), words.end(),
        [&os, c](const string & s){os << s << c;} );
    cout << os.str() << endl;

    ostringstream os1;
    // ostream不能拷贝，若希望传递给bind一个对象，而不拷贝它，就必须使用标准库提供的ref函数
    for_each(words.begin(), words.end(),bind(print, ref(os1), _1, c));
    cout << os1.str() << endl;
}
```

### 7.4、应用

```C
#include <iostream>
#include <functional>

class Calculator {
public:
    Calculator(double base) : base_(base) {}

    double add(double value) {
        return base_ + value;
    }

    double subtract(double value) {
        return base_ - value;
    }
    
    double multiply(double value) {
        return base_ * value;
    }

    double divide(double value) {
        if (value != 0.0) {
            return base_ / value;
        } else {
            std::cout << "Error: Division by zero!" << std::endl;
            return 0.0;
        }
    }

private:
    double base_;
};

int main() {
    Calculator calc(10.0);

    // 使用 std::function 包装成员函数作为可调用对象
    std::function<double(double)> operation = std::bind(&Calculator::add, &calc, std::placeholders::_1);

    // 调用操作函数
    double result = operation(5.0);
    std::cout << "Result: " << result << std::endl;

    return 0;
}
```

- 在 `main` 函数中，创建了一个 `Calculator` 的实例 `calc`，并使用 `std::function` 来包装 `add` 成员函数，形成一个可调用对象 `operation`。
- `std::bind` 函数用于将成员函数与对象绑定，`std::placeholders::_1` 表示占位符，用于在调用时传入参数。

### 7.5、回调函数

回调就是通过把函数等作为另外一个函数的参数的形式，在调用者层指定被调用者行为的方式。

通过上面的介绍，我们知道，可以使用函数指针，以及`std::function`作为函数参数类型，从而实现回调函数：

```C
#include <iostream>
#include <functional>

std::function<int(int, int)> ComputeFunction;
int (*compute_pointer)(int, int);

int compute1(int x, int y, ComputeFunction func) {
    // do something
    return func(x, y);
}

int compute2(int x, int y, compute_pointer func)
{
    // do something
    return func(x, y);
}
```

以上两种方式，对于一般函数，简单lambda函数而言是等效的。

但是如果对于带有捕获的lambda函数，类成员函数，有特殊参数绑定需要的场景，则只能使用`std::function`。

其实还有很多其他的实现回调函数的方式，如下面的标准面向对象的实现：

```C
#include <iostream>

// 定义标准的回调接口
class ComputeFunc
{
public:
    virtual int compute(int x, int y) const = 0;
};

// 实现回调接口
class ComputeAdd : public ComputeFunc
{
public:
    int compute(int x, int y) const override { return x + y; }
};

int compute3(int x, int y, const ComputeFunc& compute)
{
    // 调用接口方法
    return compute.compute(x, y);
}

// 调用方法如下
int main()
{
    ComputeAdd add_func; // 创建一个调用实例
    int sum = compute3(3, 4, add_func); // 传入调用实例
}
```

面向对象的方式更加灵活，因为这个回调的对象可以有很复杂的行为。

以上三种方法各有各的好处，根据你需要实现的功能的复杂性，扩展性和应用场景等决定使用。

另外，这些函数类型的参数可能为空，在调用之前，应该检查是否可以调用，如检查函数指针是否为空。