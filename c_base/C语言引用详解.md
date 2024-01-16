### **1、基本概念**

百度百科是这样解释的：C++是C语言的继承，它可进行过程化程序设计，又可以进行以抽象数据类型为特点的基于对象的程序设计，还可以进行以继承和多态为特点的面向对象的程序设计。引用（reference）就是C++对C语言的重要扩充。引用就是某一变量（目标）的一个别名，对引用的操作与对变量直接操作完全一样，编译器不会为引用变量开辟内存空间，它和它引用的变量共用同一块内存空间。引用的声明方法：类型标识符 &引用名=目标变量名。别名，又可以说是外号，代称，比如水浒传里几乎是别名最多的地方。林冲，在家称为"林教头"，江湖上人称"豹子头"。教头和豹子头就是林冲的别名。

### **2、区分**

&就是引用，但是&这个操作符和取地址&操作符是重叠的，所以它们需要不同的场景规范：当 &b单独存在时，这时就代表取地址，为取出变量的地址。但是如果这样：

```C
int main(){            
int a = 10;            
int& b = a; // 引用            
int* p = &b; // 取地址            
return 0;
}
```

**当 & 位于类型和变量名之间时，为引用。**

### **3、本质**

调试查看一下 a 和 b 的关系：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=NzdiYjZmZmY2NjA2NmI3MDkwMmRjMjViNTdkM2NkNjFfcVlIeUdBd2piZ0w2bzVYbnA4R0hDcndKRTNyb3p2MUZfVG9rZW46Ym94Y25WWTVYZmZiVzM2SHVKczh0b2VoMkZkXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

我们发现a和b的值不仅相等，连它的地址也是相同的。这就可以说明，b就是a ，但是在语法层面上，这里b并不是开辟的新空间，而是对原来的a取了一个新名称，叫做b。就好比林冲被叫做豹子头一样，林冲还是林冲，豹子头也是它；而a就是a，但是b也是 a 。

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=YmRlNjk0MGQzYTE1MzU5NzIxOTc5Y2M4MmI2NzNiMGRfeTY5dHIxb2Z5a2VZUkFWdWRiVXpGTldnMFNnNUZFamdfVG9rZW46Ym94Y25QaVB4YTZrYkpSd255VUpPTzVENkdjXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

而如果这时候对 a 或 b 任意一个修改，那么 a 和 b 都会发生修改。

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=YWZiYzhiY2ZhOTg4ZTIxNWRkOWNlN2U2M2VhOTQ2NjNfRHVhZHh0TUhuWmZFN2pxVVpSZTdJNEFHWElHUENGMU5fVG9rZW46Ym94Y25UMVhVOUJKc2U3TkdlZ1FFdlBzYjFmXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

### **4、特性**

引用有以下3点是必须注意的！！！

1. **引用必须在定义时初始化**

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=MmYxYmJkZWI4MGRiOGEwMDU2NTQzZTdlOGMzNTQ3MzlfNXRMZVRlWkNKSGJPNlJBNUJZcmtldmh1S0RGU1hzZ0FfVG9rZW46Ym94Y24yVG5xbWw3ZGg1UlRmUlhLQldqRUNmXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

引用是取别名，所以在定义的时候必须明确是谁的别名。

2. **一个变量可以有多个引用**

就和林冲一样，他可以叫豹子头也可以叫林教头，这都是它。所以一个变量也可以有多个别名。

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=NGM5ODBiODFkYTg4MDMwNDI5OWExOThlMzAyOWE1ZjVfUW1RQ0d3bkxvUTdMMnVRWFVPaEp0T2w4eWZhZHhnRWRfVG9rZW46Ym94Y241SHZQNUNkUWI0MjhPZlh5a0M5MGRlXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

而对于一个起过别名的变量，对它的别名取别名也是可以的。

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=NzJlZmQ1YmFmNDFjNDNhZmYwYjU4NjIwYzllZjA3ZThfOHYzbEhuU3J0SXl5dHJwQmlUUUpuSGphdTFldHY1RnRfVG9rZW46Ym94Y25vS3dHN1NnTGpURTc1c1l3Rm5ObTRlXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

而从根本上看，就可以这么理解：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=MTUwNTJmZWI4NzMxNzk0Yjg4OGEzYzkwYWVkMWI3OGFfeEU4b2g5cFdZSXo1VVE4RVc5Mlhnc3V0UU1McW1oa1hfVG9rZW46Ym94Y25YREJiZ0IzeFRuUW1CYXhhVmVscFdnXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

**本质上还是一个变量。**但是别名不能和正式名字冲突，就比如取过别名，就不能定义和别名重名的变量，即使它们的类型并不相同。

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=ZGQ0NmM1ZjRmN2U2ODM5YTQzMDU5NjQzMzcyZThhMTJfZWt3VFluUGVYS1k1UkVFWHA5ZFBBdDdVTngzNEx0RFFfVG9rZW46Ym94Y25iQ0FHODZxMk01REtVblBGUmVFSkpkXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

所以说这里的报错信息并不准确，实际上是命名冲突。

1. 引用一旦引用一个实体，就不能引用其他实体

```C
int main(){    
    int a = 10;            
    int& b = a;            
    int c = 20;           
    b = c;            
    return 0;
}
```

对于下一组代码，有什么含义？

- 让 b 变成 c 的别名？
- 还是把 c 赋值给 b ？

这里的代码意思是第二个含义，就是赋值，我们调试看看：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=NzE3Mjc4ZGRlYmIzNWUzYjEzZmNhYzRjOTI2ZDg2NjJfUVVRb2Rvc0IwbXVsUTg2cGpYMWlLUDZzbG5JQ09XbmJfVG9rZW46Ym94Y253QlNrbzZESlJQWHE2NFlteTVnMTVnXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

调试我们也可以看到，我们只是把 c 的值赋值给了 b ，b 的地址还是没变的 ，并且 a 的值也改变了。这就说明引用一旦引用某一个实体，就不能引用其他的实体，引用是不会发生改变的。因为它们是完全独立的两个变量，仅有的关联也只是值相等，改变 b 并不能影响 c ，但是此时 b 是 a 的别名，所以改变 b 就会影响 a 。图：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=NTNiODBiYzljYzEyMGU4YTA3OWZlNzE3M2Y2ZTMxMTNfOEpNUGVVYjNHbkoxOENOdjRXR3pvWmNPVHVYcDdtblFfVG9rZW46Ym94Y25HdnVtcU5LNzZ6OVJRV2hRSDFUY3lmXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

但是对于指针，则是截然不同的：

```C
int main(){            
    int a = 10;            
    int c = 20;            
    int* p = &a;            
    p = &c;            
    return 0;
}
```

对于指针来说，指针就可以时刻修改：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=NTQzMDViZTYxYWNhOTA5ZTAwY2UxZGYxMjBmNTBmNjBfQ3ZGYzZaVWowNkpnRXBXeXJ4ZjdnYnYya2haYmNIcTNfVG9rZW46Ym94Y25GWHFSTllobEpQTm40SmVXbDJoellmXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

p原本指向 a ，现在指向 c.但是引用也有局限性，因为引用之后的变量是不可修改引用的，比如链表，节点是要不断更替迭代的，所以还需要指针配合，C++才可以写出一个链表。

### **5、应用**

1. #### **做参数**

我们知道实参的改变不影响形参，所以这种写法并不能改变值，因为此刻是 传值调用 ：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=MDRmZmE1Njc3YmNmNDljMGFlMzVkNjIxODRhOTBlNmJfemlib3FxZkZ4b3pPdmlEVERrcDhQNGpHd05KR2w1d2VfVG9rZW46Ym94Y242SHlETlpURW8yMFhKV3J0Q2huYXdnXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

按照之前 c 的写法，我们使用 传址调用 ，用指针修改：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=MjE4NjBmOTE0ZDg1YWY2NDU5ZGJjYjk4MDg3OGVhOWRfNktzRG9rVXhWRjBsbHhvYnZLOUtnRUN2bHhaMkQ1bmlfVG9rZW46Ym94Y25tR2hBZDY3RlNrbDBtUVdybUdQdmZiXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

但是学习引用之后，完全可以用引用修改：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=YWUxODhkN2Y5Y2JlMmU3MmVlNWVlMzQ4YmQwNTViMmRfdWI2Y1IwWG8yY2lWdlREWTYyQW4yNXNvWVF2cTlUSmNfVG9rZW46Ym94Y245VUZCNjA0a3Z2UDFHSjd6QU1tT1dnXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

x 和 y 分别是 a 和 b 的引用，对 x 和 y 进行修改，就是对 a 和 b 进行修改，所以值也被修改成功了。调试一下：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=ZmVmYWY3NjViNDFlMTQxODZkNmZiNzcyY2E4NjEyMTdfSTAzeTI0enFIVXhLeHdCNGxqZWJ0akluZnhEb1hwOWZfVG9rZW46Ym94Y25tNDI5NE5mOGo2WlpHbFRNTlJ2S09KXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

它们的地址是完全相同的。而这里这里既不是传值调用，也不是传址调用，而是传引用调用。

> 思考：上面三个函数是否构成函数重载？构成，但无法调用。

根据函数名修饰规则，传值和传引用的是不一样的，比如会加上 R 做区分。但是不能同时调用传值和传引用，因为有歧义，就会导致调用不明确 ，编译器并不知道调用哪个:

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=MTBiZDFhZjc1M2EzMDRhMDg4ZjkzZTYxMzk5M2ExZWRfSHE0cndoUmwwTVpuc0gzeXROcUEwUTF5UTk1d05vVTlfVG9rZW46Ym94Y25UU2ZvYmN4NUw3NjRhaFlJUkZHN3dnXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

引用解决二级指针生涩难懂的问题 ：

讲单链表时，我们写的由于是没有头结点的链表，所以修改时，需要二级指针，对于指针概念不清晰的小伙伴们可能比较难理解。

但是学了引用，就可以解决这个问题：结构定义：

```C
typedef struct SListNode{            
    int data;            
    struct SListNode* next;
}SLTNode;
```

原代码：

```C++
void SListPushFront(SLTNode** pphead, SLTDateType x){    
    SLTNode* newnode = BuyListNode(x);    
    newnode->next = *pphead;     
    *pphead = newnode;
}// 调用
SLTNode* pilst = NULL;
SListPushFront(&plist);
```

修改后:

```C++
void SListPushFront(SLTNode*& pphead, SLTDateType x) // 改
{            
    SLTNode* newnode = BuyListNode(x);            
    newnode->next = *pphead;             
    *pphead = newnode;
}// 调用
SLTNode* pilst = NULL;
SListPushFront(plist); // 改
```

修改之后的代码里的二级指针被替换成了引用。而这里的意思就是给一级指针取了一个别名，传过来的是plist，而plist 是一个一级指针，所以会出现 * ，而这里就相当于 pphead 是 plist 的别名。而这里修改 pphead ，也就可以对 plist 完成修改。但是有时候也会这么写 ：结构改造：

```C++
typedef struct SListNode{            
    int data;            
    struct SListNode* next;
}SLTNode, *PSLTNode;
```

这里的意思就是将 `struct SListNode*` 类型重命名为 `PSLTNode` 。

代码：

```C++
void SListPushFront(PSLTNode& pphead, SLTDateType x) // 改
{            
    PSLTNode newnode = BuyListNode(x);            
    newnode->next = pphead;             
    pphead = newnode;
}// 调用 
PSLTNode plist = NULL;
SListPushFront(plist);
```

在 `typedef `之后，`PSLTNode` 就是结构体指针，所以传参过去，只需要在形参那边用引用接收，随后进行操作，就可以达成目的。而形参的改变影响实参的参数叫做输出型参数，对于输出型参数，使用引用就十分舒适。如果了解引用，那么这一部分是相当好理解的，一些数据结构教科书上也是这么写的，但是如果不懂引用，甚至会觉得比二级指针还难以理解。在我们学习了引用之后，之后也可以这么写代码，更加方便。

2. #### **做返回值**

要搞清楚这一块，我们先进行些许铺垫。

```C++
int add(int a, int b){            
    int c = a + b;            
    return c;
}
int main(){            
    int ret = add(1, 2);            
    cout << ret << endl;            
    return 0;
}
```

这里看似很简单，就是把add函数计算结束的结果返回，但是这里包含了 传值返回 。若从栈帧角度看，会先创建 main 函数的栈帧，里面就会有 call 指令，开始调用 add 函数。而 add 函数也会形成栈帧，而栈帧中也有两块小空间，用来接受参数，分别为 a 和 b，而里面的 c 则用来计算结果并返回。

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=MzY3NDJhM2FhYjE2ZmZmZjc5NGZmYTk2MDkzN2FmN2NfajVSaVY0NjRSZlBFM0xmUlV0ZTBBSFZ6T1pHandVTVJfVG9rZW46Ym94Y25ONzZ5V3lCYlc4ZjZQNTBuaUlTT1dkXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

而对于传值返回，返回的并不是 c ，而是返回的是 c 的拷贝。而这其中会有一个临时变量，返回的是临时变量(见函数栈帧)如果返回的是 c 的话，由于 add 的函数栈帧已经销毁了，就会产生很多奇怪的问题。c 能不能取到都是未知，而这时都是非法访问，因为空间已经被归还给系统了，所以必定是c拷贝后的数据被返回。但是临时变量在哪？

- 如果 c 比较小(4/8 byte)，一般是寄存器充当临时变量，例如eax
- 如果 c 比较大，临时变量放在调用 add 函数的栈帧中，

最后将临时变量中的值赋值给ret

图：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=MGE1YmE4Y2JhNzIwMTBlNzgzN2JiZjJiMWQ4MjBmZjhfejdRSmIyd3hHcnJEMFdWNGFKTnpYNDhYeGJtbVZtWmVfVG9rZW46Ym94Y25mVkV2RTlNV2pmanYxM2Q5UkFnYnpiXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

所有的传值返回都会生成一个拷贝便于理解，看一下汇编：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=ZThlNDdmNDk0MDk3M2JlNzkxOTYzMWQ3Mjg4N2NjYTlfaERiUzZ1RDJIN3ljZ0dRd0NMWkhadU1Zb3hUb1puaVBfVG9rZW46Ym94Y25jTTc1MmwyNkZUUnJOWkZBSnpjd2diXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

看第四句话，这里是说，把 eax 中的值，拷贝到 ret 中。而再函数调用返回时：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=ZGZhNThjYzEwYjIyYzFlN2UyMDE2MTdkOTQzMTcwNmNfdkx0YXk2TGtUa1REOGFqVTU1V3M2UWphUnVuOTUzRVpfVG9rZW46Ym94Y25Tdm5rMW5YYTJmU1VsS1J0QVVSVE5iXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

这里是将 c 的值放到 eax 中的。这也就印证了返回时，是以临时拷贝形式返回的，由于返回值是 int ，所以是直接用的 eax 寄存器。

而不论这个函数结束后，返回的那个值会不会被销毁，都会创建临时变量返回，例如这段代码 ：

```C++
int fun(){            
    static int n = 0;        
    n++;        
    return n;
}
int main(){    
    int ret = fun();            
    cout << ret << endl;            
    return 0;
}
```

对于该函数，编译器仍然是创建临时变量返回；因为编译器不会对其进行特殊处理。

看一下汇编：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=YWI3NjQ5NGYwN2YxMzk4YzJmNmY5ZGI2OTc3YzMyZjNfMEFiTmVEMkpZRzJyWklWcm8xZXA4ZkRwakhUdGtOSnRfVG9rZW46Ym94Y25vcnE4SlYzN1V3eUtGWU9tSDN4S21kXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

仍然是放到 eax 寄存器中返回的。

> 埋个伏笔:你觉不觉的这个临时变量创建的很冤枉，明明这块空间一直存在，我却依然创建临时变量返回了？能不能帮它洗刷冤屈。

如果我改成引用返回会发生什么情况吗？

```C++
int& add(int a, int b){            
    int c = a + b;            
    return c;
}
int main(){            
    int ret = add(1, 2);            
    cout << ret << endl;            
    return 0;
}
```

引用返回就是不生成临时变量，直接返回 c 的引用。而这里产生的问题就是 非法访问 。

造成的问题：

- 存在非法访问，因为 add 的返回值是 c 的引用，所以 add 栈帧销毁后，会访问 c 位置空间，而这是读操作，不一定检查出来，但是本质是错的。
- 如果 add 函数栈帧销毁，空间被清理，那么取 c 值时取到的就是随机值，取决于编译器的决策。

ps：虽然vs销毁栈帧没有清理空间数据，但是会二次覆盖

来看个有意思的：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=Njg5YmM1MmRjNDc5NmUzZWUxNzIxNzI5MDk3NmU5NjVfMGd5Q0JGUWx1dU9hOXRsckE1a0s1ZFhXY3lHaGhMek5fVG9rZW46Ym94Y25PcVlrdUczeDgwOHRzbzJ6bFBhSk5nXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

例如这里，当调用 add 函数之后，返回 c 的引用，接收返回值是用的ret相当于是 c 的引用，这时由于没有清理栈帧数据，所以打印3；

但是第二次调用，重新建立栈帧，由于栈帧大小相同，第二次建立栈帧可能还是在原位置，之前空间的数据被覆盖，继续运算，但是此时，ret 那块空间的值就被修改了，而这时没有接收返回值，但是原先的那块 c 的值被修改，所以打印出来 ret 是 30 。

所以使用引用返回时，一旦返回后，返回值的空间被修改，那么都可能会造成错误，使用要小心！

> 引用返回有一个原则：如果函数返回时，出了函数作用域，如果返回对象还在(还没还给系统)，则可以使用引用返回，如果已经还给系统了，则必须使用传值返回。

它俩的区别就是一个生成拷贝，一个不生成拷贝。而这时 static 修饰的静态变量不委屈了：

```C++
int& fun(){            
    static int n = 0;        
    n++;        
    return n;
}
```

因为 static 修饰的变量在静态区，出了作用域也存在，这时就可以引用返回。

我们可以理解引用返回也有一个返回值，但是这个返回值的类型是 int& ，中间并不产生拷贝，因为返回的是别名。这就相当于返回的就是它本身。

有时引用返回可以发挥出意想不到的结果：

```C++
#include <cassert>
#define N 10
typedef struct Array{            
    int a[N];            
    int size;
}AY;
int& PostAt(AY& ay, int i){            
    assert(i < N);            
    return ay.a[i];
}
int main(){            
    AY ay;            
    PostAt(ay, 1);     // 修改返回值            
    for (int i = 0; i < N; i++) {                        
        PostAt(ay, i) = i * 3;            
    }           
    for (int i = 0; i < N; i++) {                        
        cout << PostAt(ay, i) << ' ';            
    }            
    return 0;
}
```

由于PostAt 的形参 ay 为 main 中 局部变量 ay的别名，所以 ay 一直存在；这时可以使用引用返回。引用返回 减少了值拷贝 ，不必将其拷贝到临时变量中返回；并且由于是引用返回，我们也可以 修改返回对象 。

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=Y2UyMTM5YjQwMWNjOThiYzA3YWUzZjMxMTlmMzFkYjNfbGhqSlhyaFRNWU9TaEN3ZENzY2drMkJ6WjVyYkJzUWFfVG9rZW46Ym94Y25BRHdLdVBSVXlhenpycU0wa0RXcHBkXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

总结提炼：如果出了作用域，返回变量（静态，全局，上一层栈帧，malloc等）仍然存在，则可以使用引用返回。

### **6、效率比较**

值和引用的作为返回值类型的性能比较：

```C++
#include <time.h>
struct A { 
    int a[10000]; 
};
A a;// 值返回A 
TestFunc1() { 
    return a; 
} // 拷贝// 引用返回A& 
TestFunc2() { 
    return a; 
} // 不拷贝
void TestReturnByRefOrValue(){    // 以值作为函数的返回值类型            
    size_t begin1 = clock();    
    for (size_t i = 0; i < 100000; ++i)            
        TestFunc1();    
    size_t end1 = clock();    // 以引用作为函数的返回值类型           
    size_t begin2 = clock();    
    for (size_t i = 0; i < 100000; ++i)            
        TestFunc2();    
    size_t end2 = clock();    // 计算两个函数运算完成之后的时间            
    cout << "TestFunc1 time:" << end1 - begin1 << endl;    
    cout << "TestFunc2 time:" << end2 - begin2 << endl;
 }
 int main(){    
     TestReturnByRefOrValue();    
     return 0;
 }
```

由于传值返回要拷贝，所以当拷贝量大，次数多时，比较耗费时间；而传引用返回就不会，因为返回的就是别名。

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=ZmRjYTE1MjFiYmYyMDIyZWJmYjM1ZTExNWY3YWExYWJfemJLQmFDUFVZS05MR3FUcDNBYmNIbG5QbjF0eGlGeTBfVG9rZW46Ym94Y25vUFg3WVkwOGRueGJaeGZzbFR2M0FjXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

对于返回函数作用域还在的情况，引用返回优先。

引用传参和传值传参效率比较 ：

```C++
#include <time.h>
struct A { 
    int a[10000]; 
};
void TestFunc1(A a) {}
void TestFunc2(A& a) {}
void TestRefAndValue(){    
    A a;    // 以值作为函数参数    
    size_t begin1 = clock();    
    for (size_t i = 0; i < 10000; ++i)            
        TestFunc1(a);    
    size_t end1 = clock();    // 以引用作为函数参数    
    size_t begin2 = clock();    
    for (size_t i = 0; i < 10000; ++i)            
        TestFunc2(a);    
    size_t end2 = clock();    // 分别计算两个函数运行结束后的时间    
    cout << "TestFunc1(A)-time:" << end1 - begin1 << endl;    
    cout << "TestFunc2(A&)-time:" << end2 - begin2 << endl;
}
int main(){    
    TestRefAndValue();
}
```

还是引用快，因为引用减少拷贝次数：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=YjU3ZjljNTkxNDBiNmUxNTMwOTgyODhmMGIxYmM2MzRfNzQ4eFlpT3BDdWpDWWY3dGtRYm5XRHhlamhTTm96NWlfVG9rZW46Ym94Y25sbHIyU2VMcFdaQWwwWnU3VEh4UWNkXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

总结：引用的作用主要体现在传参和传返回值

- 引用传参和传返回值，在有些场景下可以提高性能（大对象 and 深拷贝对象 – 之后会讲）。
- 引用传参和传返回值，在对于输出型参数和输出型返回值很舒服。说人话就是形参改变，实参也改变 or 返回对象（返回值改变）。

### **7、常引用**

const 修饰的是常变量，不可修改。

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=N2UwYjY1Y2QyNDljMmQyYmQ0NzlmYjVmNjQwOTU0YzNfSEFqTGJjS3RLZ09YaExIdkxWYXRBMWpEQlpGTHdZQUdfVG9rZW46Ym94Y25pd0JLcmpiSzNNN1NySHE4WHRzcXhmXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

a本身都不能修改，b为a的引用，那么b也不可以修改，这样就没意义了。a是只读，但是引用b具有可读可写的权利，该情况为权限放大，所以错误了。

这时，只要加 const 修饰 b ，让 b 的权限也只有只读，使得 权限不变 ，就没问题了：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=YzMxM2E4NTJkMDc4ZjA5YjRjZGFlZGZhN2ExMzE1ZDNfUkZJWEsxRnhKdE13Q0hNVW9uNFZIV3pTNVFXUFRBd1JfVG9rZW46Ym94Y250d3RvODJwVUdlZ0lKckpvUUcwR2ZmXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

而如果原先变量可读可写，但是别名用 const 修饰，也是可以的，这种情况为 权限缩小 ：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=YzQxZmQ2NjdiNjMwMTJlNDk4YmJiN2QzYjc0YTgyOTFfZlc0dFZNcWZObmxsa09ZeGpnczVJbHpYYkxJNFEzRDJfVG9rZW46Ym94Y24xOUFWeHFlNU43YldVemsxdkp2VmxoXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

对于函数的返回值来说，也不能权限放大，例如：

```C++
int fun(){    
    static int n = 0;    
    n++;    
    return n;
}
int main(){    
    int& ret = fun(); // error
    return 0;
}
```

这样也是不行的，因为返回方式为 传值返回 ，返回的是临时变量，具有 常性 ，是不可改的；而引用放大了权限，所以是错误的；这时加 const 修饰就没问题：

```C++
const int& ret = c(1, 2)
```

那么这种情况为什么不可以？

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=MTE2MzRlNmVlNzMzMTYxODA1YzgzMWE5ZTZjYTJmNjFfRkRSNTlPUDN5Y011V29WcmFKbTBkajM5WW43SkdIUVNfVG9rZW46Ym94Y25MRVpMNHVLeVBIUzhvNTJuVzg4M2VnXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

而这样就可以了？

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=ODliMGQ5NTkxMWZiZjcxMjBkYjY3NTAyZmZlNTNiMWZfbXlnWnUyVENkNkZBR2t5T3FmOHR1MVBuY2MwbjAzUVlfVG9rZW46Ym94Y25FSmVoeDYwQU5ZUUdseVJCMzdoZ29UXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

因为类型转换会产生临时变量 ：

> 对于类型转换来说，在转换的过程中会产生一个个临时变量，例如 double d = i，把i转换后的值放到临时变量中，把临时变量给接收的值d

而临时变量具有常性，不可修改，引用就加了写权限，就错了，因为 权限被放大了 。

**小结：对于引用，引用后的变量所具权限可以缩小或不变，但是不能放大（指针也适用这个说法）作用 ：在一些场景下，假设 x 是一个大对象，或者是深拷贝对象，那一般都会用引用传参，减少拷贝，如果函数中不改变 x ，尽量用 const 引用传参。**

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=OGQ3M2RhODUyYjNlNmYzMWY4Y2E3OTIwYTc4YTdhOWFfcW1VQjhNY3R0V2RsOUZDalVXQk1lWjFaSUFGRUpZTUNfVG9rZW46Ym94Y25ib2VmaHU5andMbmpDRDBLbTRiSkhiXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

这样可以防止 x 被修改 ，而对于 const int& x 也可以接受权限对等或缩小的对象，甚至为常量：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=Y2E2MGM4MmI0YzQ2YzgxYTg1ZDQxNmJhYWU3ZTRiNGVfaE9DZjBwVlkwVVd0ajZVZ3hYbllwenh1Nk9TeE8xZ0pfVG9rZW46Ym94Y25qS1RxZ1BpSVpwNFd5aVZEZ3FlYkdnXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

结论 ：const type& 可以接收各种类型的对象（变量、常量、隐式转换）。对于输出型参数用引用，否则用 const type&，更加安全。

### **8、指针和引用区别**

从语法概念上来说，引用是没有开辟空间的，而指针是开辟了空间的，但是从底层实现上来说，则又不一样：

```C++
int main(){    
    int a = 10;
    int& ra = a;    
    ra = 20;
    int* pa = &a;    
    *pa = 20;    
    return 0;
}
```

汇编：

![img](https://vnkshn64w3.feishu.cn/space/api/box/stream/download/asynccode/?code=NjZlMTMxZmM2NzgwNzE2ODNmMTkyMmM2ZTVjYmI2MzJfTU9vWm5KcU41bGZibXFEUDZrc2pkUnFMZU9uaGlaWFNfVG9rZW46Ym94Y24wOUtONHVJcG11MUs5S2JEcXZ1c0lnXzE3MDI4MDY1MDA6MTcwMjgxMDEwMF9WNA)

lea 是取地址：我们发现无论引用和指针，都会取地址，且这些过程和指针一样。其实从汇编上，引用其实是开空间的，并且实现方式和指针一样，引用其实也是用指针实现的。区别汇总：

- 引用概念上定义一个变量的 别名 ，指针存储一个变量 地址。
- 引用 在定义时 必须初始化 ，指针最好初始化 ，但是不初始化也不会报错。
- 引用在初始化时引用一个实体后 ，就不能再引用其他实体 ，而指针可以在任何时候指向任何一个同类型。
- 没有NULL引用，但有NULL指针。
- 在sizeof中含义不同：引用结果为 引用类型的大小，但指针始终是 地址空间所占字节个数 (32位平台下占4个字节)。
- 引用自加即引用的实体增加1，指针自加即指针向后偏移一个类型的大小。
- 有多级指针，但是没有多级引用。
- 访问实体方式不同，指针需要显式解引用，引用编译器自己处理。
- 引用比指针使用起来相对更安全。