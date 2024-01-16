# C++智能指针

## 一、写在前面

C++里面的四个智能指针: auto_ptr, unique_ptr,shared_ptr, weak_ptr 其中后三个是C++11支持，并且第一个已经被C++11弃用。

## 二、C++11智能指针介绍

智能指针主要用于管理在堆上分配的内存，它将普通的指针封装为一个栈对象。当栈对象的生存周期结束后，会在析构函数中释放掉申请的内存，从而防止内存泄漏。C++ 11中最常用的智能指针类型为shared_ptr,它采用引用计数的方法，记录当前内存资源被多少个智能指针引用。该引用计数的内存在堆上分配。当新增一个时引用计数加1，当过期时引用计数减一。只有引用计数为0时，智能指针才会自动释放引用的内存资源。对shared_ptr进行初始化时不能将一个普通指针直接赋值给智能指针，因为一个是指针，一个是类。可以通过make_shared函数或者通过构造函数传入普通指针。并可以通过get函数获得普通指针。

## 三、为什么要使用智能指针

智能指针的作用是管理一个指针，因为存在以下这种情况：申请的空间在函数结束时忘记释放，造成内存泄漏。使用智能指针可以很大程度上的避免这个问题，因为智能指针就是一个类，当超出了类的作用域是，类会自动调用析构函数，析构函数会自动释放资源。所以智能指针的作用原理就是在函数结束时自动释放内存空间，不需要手动释放内存空间。

## 四、原始指针容易发生内存泄漏

C 语言中最常使用的是`malloc()`函数分配内存，`free()`函数释放内存，而 C++ 中对应的是`new`、`delete`关键字。`malloc()`只是分配了内存，而`new`则更进一步，不仅分配了内存，还调用了构造函数进行初始化。使用示例：

```C
int main(){    
    // malloc返回值是 void
    int argC = (int)malloc(sizeof(int));    
    free(argC);
    char age = new int(25); // 做了两件事情 1.分配内存 2.初始化    
    delete age;
}
```

`new`和`delete`必须成对出现，有时候是不小心忘记了`delete`，有时候则是很难判断在这个地方自己是不是该`delete`，这个和资源的生命周期有关，这个资源是属于我这个类管理的还是由另外一个类管理的（其它类可能要使用），如果是别人管理的就由别人`delete`。如果需要自己管理内存的话，最好显示的将自己的资源传递进去，这样的话，就能知道是该资源确实应该由自己来管理。

```C
char getName(char v, size_t bufferSize) 
{    
    //do something
    return v;
}
```

上面还是小问题，自己小心一点，再仔细看看文档，还是有机会避免这些情况的。但是在 C++ 引入异常的概念之后，程序的控制流就发生了根本性的改变，在写了 delete 的时候还是有可能发生内存泄漏。如下例：

```C++
void badThing(){    
    throw 1;// 抛出一个异常
}
void test() {    
    char* a = new char[1000];
    badThing();    // do something    
    delete[] a;
}
int main() {    
    try {        
        test();    
    }    
    catch (int i){        
        cout << "error happened " << i << endl;    
    }
}
```

上面的`new`和`delete`是成对出现的，但是程序在中间的时候抛出了异常，由于没有立即捕获，程序从这里退出了，并没有执行到`delete`，内存泄漏还是发生了。

## 五、使用构造函数和析构函数解决内存泄漏问题

C++ 中的构造函数和析构函数十分强大，可以使用构造和析构解决上面的内存泄漏问题，比如：

```C
class SafeIntPointer {
public:    
    explicit SafeIntPointer(int v) : m_value(new int(v)) { }    
~SafeIntPointer() {        
    delete m_value;        
    cout << "~SafeIntPointer" << endl;    
}    
int get() { 
    return m_value; 
}
private:    
    int m_value;
};
void badThing(){    
    throw 1;    // 抛出一个异常
}
void test() {    
    SafeIntPointer a(5);
    badThing();
}
int main() {    
    try {        
        test();    
    }    
    catch (int i){        
        cout << "error happened " << i << endl;    
    }
}
// 结果
// ~SafeIntPointer
// error happened 1
```

可以看到，就算发生了异常，也能够保证析构函数成功执行！这里的例子是这个资源只有一个人使用，我不用了就将它释放掉。但还有种情况，一份资源被很多人共同使用，要等到所有人都不再使用的时候才能释放掉，对于这种问题，就需要对上面的`SafeIntPointer`增加一个引用计数，如下：

```C
class SafeIntPointer {
public:    
    explicit SafeIntPointer(int v) : 
    m_value(new int(v)), m_used(new int(1)) { }    
    ~SafeIntPointer() {        
    cout << "~SafeIntPointer" << endl;        
    (m_used) --; // 引用计数减1
    if(m_used <= 0){            
    delete m_used;            
    delete m_value;            
    cout << "real delete resources" << endl;        
    }    
}        
SafeIntPointer(const SafeIntPointer& other) {        
    m_used = other.m_used;        
    m_value = other.m_value;        
    (m_used)++; // 引用计数加1    
}    
SafeIntPointer& operator= (const SafeIntPointer& other) {        
    if (this == &other) // 避免自我赋值!!
    return this;
    m_used = other.m_used;        
    m_value = other.m_value;        
    (m_used)++; // 引用计数加1
    return this;    
}
int get() { 
    return m_value; 
}    
int getRefCount() {        
    return m_used;    
}
private:    
    int* m_used; // 引用计数
    int* m_value;
};
int main() {    
    SafeIntPointer a(5);    
    cout << "ref count = " << a.getRefCount() << endl;    
    SafeIntPointer b = a;    
    cout << "ref count = " << a.getRefCount() << endl;    
    SafeIntPointer c = b;    
    cout << "ref count = " << a.getRefCount() << endl;
}
/*
ref count = 1
ref count = 2
ref count = 3
~SafeIntPointer
~SafeIntPointer
~SafeIntPointerreal 
delete resources
*/
```

可以看到每一次赋值，引用计数都加一，最后每次析构一次后引用计数减一，知道引用计数为 0，才真正释放资源。要写出一个正确的管理资源的包装类还是蛮难的，比如上面那个例子就不是线程安全的，只能属于一个玩具，在实际工程中简直没法用。所以 C++11 中引入了智能指针（Smart Pointer），它利用了一种叫做 RAII（资源获取即初始化）的技术将普通的指针封装为一个栈对象。当栈对象的生存周期结束后，会在析构函数中释放掉申请的内存，从而防止内存泄漏。这使得智能指针实质是一个对象，行为表现的却像一个指针。智能指针主要分为`shared_ptr`、`unique_ptr`和`weak_ptr`三种，使用时需要引用头文件`<memory>`。C++98 中还有`auto_ptr`，基本被淘汰了，不推荐使用。而 C++11 中`shared_ptr`和`weak_ptr`都是参考`boost`库实现的。

### 5.1、auto_ptr

（C++98的方案，C++11已经抛弃）采用所有权模式。

```C
auto_ptr<string> p1 (new string ("I reigned lonely as a cloud.")); 
auto_ptr<string> p2; 
p2 = p1; //auto_ptr不会报错.
```

此时不会报错，p2剥夺了p1的所有权，但是当程序运行时访问p1将会报错。所以auto_ptr的缺点是：存在潜在的内存崩溃问题！

### 5.2、unique_ptr

#### 5.2.1、基础原理

（替换auto_ptr）unique_ptr实现独占式拥有或严格拥有概念，保证同一时间内只有一个智能指针可以指向该对象。它对于避免资源泄露(例如“以new创建对象后因为发生异常而忘记调用delete”)特别有用。采用所有权模式，还是上面那个例子

```C
unique_ptr<string> p3 (new string ("auto"));   //#4
unique_ptr<string> p4；//#5
p4 = p3;//此时会报错！！
```

编译器认为p4=p3非法，避免了p3不再指向有效数据的问题。尝试复制p3时会编译期出错，而auto_ptr能通过编译期从而在运行期埋下出错的隐患。因此，unique_ptr比auto_ptr更安全。另外unique_ptr还有更聪明的地方：当程序试图将一个 unique_ptr 赋值给另一个时，如果源 unique_ptr 是个临时右值，编译器允许这么做；如果源 unique_ptr 将存在一段时间，编译器将禁止这么做，比如：

```C
unique_ptr<string> pu1(new string ("hello world")); 
unique_ptr<string> pu2; 
pu2 = pu1;                                      // #1 不允许
unique_ptr<string> pu3; 
pu3 = unique_ptr<string>(new string ("You"));   // #2 允许
```

其中#1留下悬挂的unique_ptr(pu1)，这可能导致危害。而#2不会留下悬挂的unique_ptr，因为它调用 unique_ptr 的构造函数，该构造函数创建的临时对象在其所有权让给 pu3 后就会被销毁。这种随情况而已的行为表明，unique_ptr 优于允许两种赋值的auto_ptr 。注：如果确实想执行类似与#1的操作，要安全的重用这种指针，可给它赋新值。C++有一个标准库函数std::move()，让你能够将一个unique_ptr赋给另一个。尽管转移所有权后 还是有可能出现原有指针调用（调用就崩溃）的情况。但是这个语法能强调你是在转移所有权，让你清晰的知道自己在做什么，从而不乱调用原有指针。（额外：boost库的boost::scoped_ptr也是一个独占性智能指针，但是它不允许转移所有权，从始而终都只对一个资源负责，它更安全谨慎，但是应用的范围也更狭窄。）例如：

```C
unique_ptr<string> ps1, ps2;
ps1 = demo("hello");
ps2 = move(ps1);
ps1 = demo("alexia");
cout << ps2 << *ps1 << endl;
```

#### 5.2.2、unique_ptr常用操作

```C
unique_ptr<T> u1 // 空unique_ptr，可以指向类型为T的对象。u1会使用delete来释放它的指针
unique_ptr<T, D> u2 // u2会使用一个类型为D的可调用对象来释放它的指针
unique_ptr<T, D> u(d) // 空unique_ptr，指向类型为T的对象，用类型为D的对象d替代
deleteu = nullptr // 释放u指向的对象，将u置为空
u.release() // u放弃对指针的控制权，返回指针，并将u置为空
u.reset() // 释放u指向的对象
u.reset(q) // 如果提供了内置指针q，另u指向这个对象；否则将u置为空
u.reset(nullptr)  
```

虽然我们不能拷贝或赋值unique_ptr，但可以通过调用 release 或 reset 将指针的所有权从一个（非const）unique_ptr转移给另一个unique_ptr：

```C
unique_ptr<string> p1(new string("Stegosaurus"));// 将所有权从pl (指向string Stegosaurus)转移给p2 
unique_ptr<string> p2(p1, release()); // release 将 p1 置为空 
unique_ptr<string> p3(new string("Trex"));// 将所有权从p3转移给p2
p2.reset(p3.release()); // reset 释放了 p2 原来指向的内存
```

调用 release 会切断unique_ptr和它原来管理的对象间的联系，如果我们不用另一个智能指针来保存 release 返回的指针，我们的程序就要负责资源的释放：

```C
p2.release(); // 错误：p2不会释放内存，而且我们丢失了指针
auto p = p2.release(); // 正确，但我们必须记得 delete(p)
delete(p);
```

### 5.3、shared_ptr

#### 5.3.1、基础原理

shared_ptr实现共享式拥有概念。多个智能指针可以指向相同对象，该对象和其相关资源会在“最后一个引用被销毁”时候释放。从名字share就可以看出了资源可以被多个指针共享，它使用计数机制来表明资源被几个指针共享。可以通过成员函数use_count()来查看资源的所有者个数。除了可以通过new来构造，还可以通过传入auto_ptr, unique_ptr,weak_ptr来构造。当我们调用release()时，当前指针会释放资源所有权，计数减一。当计数等于0时，资源会被释放。shared_ptr 是为了解决 auto_ptr 在对象所有权上的局限性(auto_ptr 是独占的), 在使用引用计数的机制上提供了可以共享所有权的智能指针。成员函数：use_count 返回引用计数的个数unique 返回是否是独占所有权( use_count 为 1)swap 交换两个 shared_ptr 对象(即交换所拥有的对象)reset 放弃内部对象的所有权或拥有对象的变更, 会引起原有对象的引用计数的减少get 返回内部对象(指针), 由于已经重载了()方法, 因此和直接使用对象是一样的.如

```C
shared_ptr<int> sp(new int(1));
```

sp 与 sp.get()是等价的。share_ptr的例子：

```C
int main()
{    
    string s1 = new string("s1");
    shared_ptr<string> ps1(s1);    
    shared_ptr<string> ps2;    
    ps2 = ps1;
    cout << ps1.use_count()<<endl;        //2    
    cout<<ps2.use_count()<<endl;        //2    
    cout << ps1.unique()<<endl;        //0
    string *s3 = new string("s3");    
    shared_ptr<string> ps3(s3);
    cout << (ps1.get()) << endl;        //033AEB48    
    cout << ps3.get() << endl;        //033B2C50swap(ps1, ps3);        //交换所拥有的对象    
    cout << (ps1.get())<<endl;        //033B2C50    
    cout << ps3.get() << endl;        //033AEB48
    cout << ps1.use_count()<<endl;        //1    
    cout << ps2.use_count() << endl;        //2    
    ps2 = ps1;    
    cout << ps1.use_count()<<endl;        //2    
    cout << ps2.use_count() << endl;        //2    
    ps1.reset();        //放弃ps1的拥有权，引用计数的减少    
    cout << ps1.use_count()<<endl;        //0    
    cout << ps2.use_count()<<endl;        //1
}
```

#### 5.3.2、shared_ptr的初始化

最安全的分配和使用动态内存的方法是调用一个名为 make_shared 的标准库函数。此函数在动态内存中分配一个对象并初始化它，返回指向此对象的 shared_ptr。与智能指针一样，make_shared 也定义在头文件 memory 中。

```C
// 指向一个值为42的int的shared_ptr
shared_ptr<int> p3 = make_shared<int>(42);
// p4 指向一个值为"9999999999"的string
shared_ptr<string> p4 = make_shared<string>(10,'9');
// p5指向一个值初始化的int
shared_ptr<int> p5 = make_shared<int>();
```

我们还可以用 new 返回的指针来初始化智能指针，不过接受指针参数的智能指针构造函数是 explicit 的。因此，我们不能将一个内置指针隐式转换为一个智能指针，必须使用直接初始化形式来初始化一个智能指针：

```C
shared_ptr<int> pi = new int (1024); // 错误：必须使用直接初始化形式
shared_ptr<int> p2(new int(1024));    // 正确：使用了直接初始化形式
```

出于相同的原因，一个返回 shared_ptr 的函数不能在其返回语句中隐式转换一个普通指针：

```C
shared_ptr<int> clone(int p){    
    return new int(p); // 错误：隐式转换为 shared_ptr<int>
}
```

#### 5.3.3、shared_ptr的常用操作

下面列出了shared_ptr独有的操作：

```C
make_shared<T>(args) // 返回一个shared_ptr，指向一个动态分配的类型为T的对象。使用args初始化此对象
shared_ptr<T> p(q) // p是shared_ptr q的拷贝；此操作会递增q中的引用计数。q中的指针必须能转换成T
*p = q // p和q都是shared_ptr，所保存的指针必须能相互转换。此操作会递减p中的引用计数，递增q中的引用计数。若p中的引用计数变为0，则将其管理的原内存释放
p.unique() // 若p.use_count()为1，返回true；否则返回false
p.use_count() // 返回与p共享对象的智能指针数量；可能很慢，主要用于调试
```

下面介绍一些改变shared_ptr的其他方法：

```C
p.reset () //若p是唯一指向其对象的shared_ptr，reset会释放此对象。
p.reset(q) //若传递了可选的参数内置指针q，会令P指向q，否则会将P置为空。
p.reset(q, d) //若还传递了参数d,将会调用d而不是delete 来释放q
```

### 5.4、weak_ptr

#### 5.4.1、基础原理

share_ptr虽然已经很好用了，但是有一点share_ptr智能指针还是有内存泄露的情况，当两个对象相互使用一个shared_ptr成员变量指向对方，会造成循环引用，使引用计数失效，从而导致内存泄漏。weak_ptr 是一种不控制对象生命周期的智能指针, 它指向一个 shared_ptr 管理的对象. 进行该对象的内存管理的是那个强引用的shared_ptr， weak_ptr只是提供了对管理对象的一个访问手段。weak_ptr 设计的目的是为配合 shared_ptr 而引入的一种智能指针来协助 shared_ptr 工作, 它只可以从一个 shared_ptr 或另一个 weak_ptr 对象构造, 它的构造和析构不会引起引用记数的增加或减少。weak_ptr是用来解决shared_ptr相互引用时的死锁问题,如果说两个shared_ptr相互引用,那么这两个指针的引用计数永远不可能下降为0,资源永远不会释放。它是对对象的一种弱引用，不会增加对象的引用计数，和shared_ptr之间可以相互转化，shared_ptr可以直接赋值给它，它可以通过调用lock函数来获得shared_ptr。

```C
class B;        //声明
class A{
public:      
    shared_ptr<B> pb_;
    ~A(){              
    cout << "A delete\n";
    }
};
class B{
public:      
    shared_ptr<A> pa_;
    ~B(){              
    cout << "B delete\n";
    }
};
void fun(){      
    shared_ptr<B> pb(new B());      
    shared_ptr<A> pa(new A());      
    cout << pb.use_count() << endl;        //1      
    cout << pa.use_count() << endl;        //1      
    pb->pa_ = pa;      
    pa->pb_ = pb;      
    cout << pb.use_count() << endl;        //2      
    cout << pa.use_count() << endl;        //2
}
int main(){    
    fun();    
    return 0;
}
```

可以看到fun函数中pa ，pb之间互相引用，两个资源的引用计数为2，当要跳出函数时，智能指针pa，pb析构时两个资源引用计数会减1，但是两者引用计数还是为1，导致跳出函数时资源没有被释放（A、B的析构函数没有被调用）运行结果没有输出析构函数的内容，造成内存泄露。如果把其中一个改为weak_ptr就可以了，我们把类A里面的shared_ptr pb_，改为weak_ptr pb_ ，运行结果如下：

```C
1112B 
deleteA delete
```

这样的话，资源B的引用开始就只有1，当pb析构时，B的计数变为0，B得到释放，B释放的同时也会使A的计数减1，同时pa析构时使A的计数减1，那么A的计数为0，A得到释放。注意：我们不能通过weak_ptr直接访问对象的方法，比如B对象中有一个方法print()，我们不能这样访问，pa->pb_->print()，因为pb_是一个weak_ptr，应该先把它转化为shared_ptr，如：

```C
shared_ptr<B> p = pa->pb_.lock();
p->print();
```

weak_ptr 没有重载*和->但可以使用 lock 获得一个可用的 shared_ptr 对象. 注意, weak_ptr 在使用前需要检查合法性.expired 用于检测所管理的对象是否已经释放, 如果已经释放, 返回 true; 否则返回 false.lock 用于获取所管理的对象的强引用(shared_ptr). 如果 expired 为 true, 返回一个空的 shared_ptr; 否则返回一个 shared_ptr, 其内部对象指向与 weak_ptr 相同.use_count 返回与 shared_ptr 共享的对象的引用计数.reset 将 weak_ptr 置空.weak_ptr 支持拷贝或赋值, 但不会影响对应的 shared_ptr 内部对象的计数.

#### 5.4.2、weak_ptr常用操作

```C
weak_ptr<T> w;        // 空weak_ptr可以指向类型为T的对象
weak_ptr<T> w(shared_ptr p);        // 与p指向相同对象的weak_ptr, T必须能转换为sp指向的类型
w = p;        // p可以是shared_ptr或者weak_ptr，赋值后w和p共享对象
w.reset();        // weak_ptr置为空
w.use_count();        // 与w共享对象的shared_ptr的计数
w.expired();        // w.use_count()为0则返回true，否则返回false
w.lock();        // w.expired()为true，返回空的shared_ptr;否则返回指向w的shared_ptr
```

### 5.5、share_ptr和weak_ptr的核心实现

weakptr的作为弱引用指针，其实现依赖于counter的计数器类和share_ptr的赋值，构造，所以先把counter和share_ptr简单实现

#### 5.5.1、Counter简单实现

```C
class Counter{
public:    
    Counter() : s(0), w(0){};      
    int s;        //share_ptr的引用计数      
    int w;        //weak_ptr的引用计数
};
```

counter对象的目地就是用来申请一个块内存来存引用基数，s是share_ptr的引用计数，w是weak_ptr的引用计数，当w为0时，删除Counter对象。

#### 5.5.2、share_ptr的简单实现

```C
template <class T>
class WeakPtr; //为了用weak_ptr的lock()，来生成share_ptr用，需要拷贝构造用
template <class T>
class SharePtr{
public:
    SharePtr(T p = 0) : _ptr(p){        
    cnt = new Counter();
    if (p)            
        cnt->s = 1;        
        cout << "in construct " << cnt->s << endl;    
    }    
    ~SharePtr(){
    release();
    }    
    SharePtr(SharePtr<T> const &s){        
        cout << "in copy con" << endl;        
        _ptr = s._ptr;
        (s.cnt)->s++;        
        cout << "copy construct" << (s.cnt)->s << endl;        
        cnt = s.cnt;
    }
    SharePtr(WeakPtr<T> const &w) {//为了用weak_ptr的lock()，来生成share_ptr用，需要拷贝构造用       
        cout << "in w copy con " << endl;        
        _ptr = w._ptr;
        (w.cnt)->s++;        
        cout << "copy w  construct" << (w.cnt)->s << endl;        
        cnt = w.cnt;
    }    
    SharePtr<T> &operator=(SharePtr<T> &s){    
        if (this != &s) {        
            release();        
            (s.cnt)->s++;        
            cout << "assign construct " << (s.cnt)->s << endl;        
            cnt = s.cnt;        
            _ptr = s._ptr;      
        }      
        return *this;  
    }
    T &operator(){    
        return *_ptr;
    }
    T operator->(){    
        return _ptr;
    }
friend class WeakPtr<T>; //方便weak_ptr与share_ptr设置引用计数和赋值
protected:
    void release(){    
        cnt->s--;    
        cout << "release " << cnt->s << endl;    
        if (cnt->s < 1){        
            delete _ptr;        
            if (cnt->w < 1)        {            
                delete cnt;            
                cnt = NULL;        
            }    
        }
    }
private:    
    T *_ptr;    
    Counter *cnt;
};
```

share_ptr的给出的函数接口为：构造，拷贝构造，赋值，解引用，通过release来在引用计数为0的时候删除_ptr和cnt的内存。

#### 5.5.3、weak_ptr简单实现

```C
template <class T>
class WeakPtr{
public: //给出默认构造和拷贝构造，其中拷贝构造不能有从原始指针进行构造    
    WeakPtr(){    
        _ptr = 0;    
        cnt = 0;    
    }    
    WeakPtr(SharePtr<T> &s) : _ptr(s._ptr), cnt(s.cnt){        
        cout << "w con s" << endl;        
        cnt->w++;    
    }    
    WeakPtr(WeakPtr<T> &w) : _ptr(w._ptr), cnt(w.cnt){        
        cnt->w++;    
    }    
    ~WeakPtr(){
        release();
    }    
    WeakPtr<T> &operator=(WeakPtr<T> &w){        
        if (this != &w){            
            release();            
            cnt = w.cnt;            
            cnt->w++;            
            _ptr = w._ptr;        
        }        
        return this;    
    }    
    WeakPtr<T> &operator=(SharePtr<T> &s){        
        cout << "w = s" << endl;        
        release();        
        cnt = s.cnt;        
        cnt->w++;        
        _ptr = s._ptr;        
        return *this;    
    }    
    SharePtr<T> lock(){        
        return SharePtr<T>(this);    
    }    
    bool expired(){          
        if (cnt){
            if (cnt->s > 0){                
                cout << "empty" << cnt->s << endl;                
                return false;            
            }        
        }    
    return true;
    }  
    friend class SharePtr<T>; //方便weak_ptr与share_ptr设置引用计数和赋值
protected:
    void release(){
    if (cnt){
        cnt->w--;      
        cout << "weakptr release" << cnt->w << endl;      
            if (cnt->w < 1 && cnt->s < 1){
            //delete cnt;          
            cnt = NULL;       
            }   
        }
    }
private:    
    T *_ptr;    
    Counter *cnt;
};
```

weak_ptr一般通过share_ptr来构造，通过expired函数检查原始指针是否为空，lock来转化为share_ptr。

## 六、性能与安全的权衡

使用智能指针虽然能够解决内存泄漏问题，但是也付出了一定的代价。以shared_ptr举例：

- shared_ptr的大小是原始指针的两倍，因为它的内部有一个原始指针指向资源，同时有个指针指向引用计数。
- 引用计数的内存必须动态分配。虽然一点可以使用make_shared()来避免，但也存在一些情况下不能够使用make_shared()。
- 增加和减小引用计数必须是原子操作，因为可能会有读写操作在不同的线程中同时发生。比如在一个线程里有一个指向一块资源的shared_ptr可能调用了析构（因此所指向的资源的引用计数减一），同时，在另一线程里，指向相同对象的一个shared_ptr可能执行了拷贝操作（因此，引用计数加一）。原子操作一般会比非原子操作慢。但是为了线程安全，又不得不这么做，这就给单线程使用环境带来了不必要的困扰。