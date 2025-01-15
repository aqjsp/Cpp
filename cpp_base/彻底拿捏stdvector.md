# 从源码的角度深度讲解std::vector，建议收藏~

大家好，我是Q。

说起vector，那肯定是C++岗位面试中非常高频的一个考点。

为什么它会经常被问到？因为它是C++STL最具有代表的容器之一，也是在日常开发中非常常用的容器之一。

面试过程中一定会被问到的问题，你会怎么回答？

**`std::vector` 是 C++ 标准库中的一种动态数组容器，它提供了连续的内存存储，并支持动态调整大小。**

其实这样简单一句话足以概括vector，不过这样可能不太友好哦~

今天就带大家一起从底层来分析一下vector的角角落落。

### 一、底层原理

#### 1、基本实现

先看源码：

```c++
template<typename _Tp, typename _Alloc = std::allocator<_Tp> >

    class vector : protected _Vector_base<_Tp, _Alloc>
    {};
```

可以明显看到，使用了模板和保护继承。

其中：

`template<typename _Tp, typename _Alloc = std::allocator<_Tp> >`

- `_Tp`：表示 `std::vector` 中存储的元素类型。
- `_Alloc`：表示用于分配内存的分配器，默认为 `std::allocator<_Tp>`，即标准库提供的默认分配器。

`class vector : protected _Vector_base<_Tp, _Alloc>{}`

- `std::vector` 通过 **保护继承** (`protected`) 从 `_Vector_base<_Tp, _Alloc>` 继承。保护继承意味着 `_Vector_base` 的公有成员在 `std::vector` 中变为保护成员，而保护成员保持不变。
- `_Vector_base` 是一个内部基类，负责管理内存分配和释放等底层操作。通过将这些功能放在基类中，可以简化 `std::vector` 的实现，并且使代码更模块化和易于维护。

那再来看看内部基类 `_Vector_base`：

```c++
  template<typename _Tp, typename _Alloc>
    struct _Vector_base
    {
      typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template
	rebind<_Tp>::other _Tp_alloc_type;
      typedef typename __gnu_cxx::__alloc_traits<_Tp_alloc_type>::pointer
       	pointer;

      struct _Vector_impl_data
      {
	pointer _M_start;
	pointer _M_finish;
	pointer _M_end_of_storage;

	_Vector_impl_data() _GLIBCXX_NOEXCEPT
	: _M_start(), _M_finish(), _M_end_of_storage()
	{ }

#if __cplusplus >= 201103L
	_Vector_impl_data(_Vector_impl_data&& __x) noexcept
	: _M_start(__x._M_start), _M_finish(__x._M_finish),
	  _M_end_of_storage(__x._M_end_of_storage)
	{ __x._M_start = __x._M_finish = __x._M_end_of_storage = pointer(); }
#endif

	void
	_M_copy_data(_Vector_impl_data const& __x) _GLIBCXX_NOEXCEPT
	{
	  _M_start = __x._M_start;
	  _M_finish = __x._M_finish;
	  _M_end_of_storage = __x._M_end_of_storage;
	}

	void
	_M_swap_data(_Vector_impl_data& __x) _GLIBCXX_NOEXCEPT
	{
	  // Do not use std::swap(_M_start, __x._M_start), etc as it loses
	  // information used by TBAA.
	  _Vector_impl_data __tmp;
	  __tmp._M_copy_data(*this);
	  _M_copy_data(__x);
	  __x._M_copy_data(__tmp);
	}
      };

      struct _Vector_impl
	: public _Tp_alloc_type, public _Vector_impl_data
      {
	_Vector_impl() _GLIBCXX_NOEXCEPT_IF(
	    is_nothrow_default_constructible<_Tp_alloc_type>::value)
	: _Tp_alloc_type()
	{ }

	_Vector_impl(_Tp_alloc_type const& __a) _GLIBCXX_NOEXCEPT
	: _Tp_alloc_type(__a)
	{ }

#if __cplusplus >= 201103L
	// Not defaulted, to enforce noexcept(true) even when
	// !is_nothrow_move_constructible<_Tp_alloc_type>.
	_Vector_impl(_Vector_impl&& __x) noexcept
	: _Tp_alloc_type(std::move(__x)), _Vector_impl_data(std::move(__x))
	{ }

	_Vector_impl(_Tp_alloc_type&& __a) noexcept
	: _Tp_alloc_type(std::move(__a))
	{ }

	_Vector_impl(_Tp_alloc_type&& __a, _Vector_impl&& __rv) noexcept
	: _Tp_alloc_type(std::move(__a)), _Vector_impl_data(std::move(__rv))
	{ }
#endif
    public:
      typedef _Alloc allocator_type;

      _Tp_alloc_type&
      _M_get_Tp_allocator() _GLIBCXX_NOEXCEPT
      { return this->_M_impl; }

      const _Tp_alloc_type&
      _M_get_Tp_allocator() const _GLIBCXX_NOEXCEPT
      { return this->_M_impl; }

      allocator_type
      get_allocator() const _GLIBCXX_NOEXCEPT
      { return allocator_type(_M_get_Tp_allocator()); }

#if __cplusplus >= 201103L
      _Vector_base() = default;
#else
      _Vector_base() { }
#endif

      _Vector_base(const allocator_type& __a) _GLIBCXX_NOEXCEPT
      : _M_impl(__a) { }

      // Kept for ABI compatibility.
#if !_GLIBCXX_INLINE_VERSION
      _Vector_base(size_t __n)
      : _M_impl()
      { _M_create_storage(__n); }
#endif

      _Vector_base(size_t __n, const allocator_type& __a)
      : _M_impl(__a)
      { _M_create_storage(__n); }

#if __cplusplus >= 201103L
      _Vector_base(_Vector_base&&) = default;

      // Kept for ABI compatibility.
# if !_GLIBCXX_INLINE_VERSION
      _Vector_base(_Tp_alloc_type&& __a) noexcept
      : _M_impl(std::move(__a)) { }

      _Vector_base(_Vector_base&& __x, const allocator_type& __a)
      : _M_impl(__a)
      {
	if (__x.get_allocator() == __a)
	  this->_M_impl._M_swap_data(__x._M_impl);
	else
	  {
	    size_t __n = __x._M_impl._M_finish - __x._M_impl._M_start;
	    _M_create_storage(__n);
	  }
      }
# endif

      _Vector_base(const allocator_type& __a, _Vector_base&& __x)
      : _M_impl(_Tp_alloc_type(__a), std::move(__x._M_impl))
      { }
#endif

      ~_Vector_base() _GLIBCXX_NOEXCEPT
      {
	_M_deallocate(_M_impl._M_start,
		      _M_impl._M_end_of_storage - _M_impl._M_start);
      }

    public:
      _Vector_impl _M_impl;

      pointer
      _M_allocate(size_t __n)
      {
	typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type> _Tr;
	return __n != 0 ? _Tr::allocate(_M_impl, __n) : pointer();
      }

      void
      _M_deallocate(pointer __p, size_t __n)
      {
	typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type> _Tr;
	if (__p)
	  _Tr::deallocate(_M_impl, __p, __n);
      }

    protected:
      void
      _M_create_storage(size_t __n)
      {
	this->_M_impl._M_start = this->_M_allocate(__n);
	this->_M_impl._M_finish = this->_M_impl._M_start;
	this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
      }
    };
```

删掉了一些无关紧要的代码，从上面的`_Vector_base`可以看出它定义了一个`_Vector_impl_data`结构体：

```
struct _Vector_impl_data {
	pointer _M_start;
	pointer _M_finish;
	pointer _M_end_of_storage;
	
	// ...构造函数等等
};
```

内部维护了三个指针来管理内存和元素：

- `_M_start`：指向当前已分配内存块的第一个元素。
- `_M_finish`：指向当前已使用内存块的最后一个元素之后的位置。
- `_M_end_of_storage`：指向已分配但未使用的内存位置。

这些指针用于管理容器的容量（capacity）和大小（size）。

具体来说：

> - **size** = `_M_finish - _M_start`，表示当前容器中实际存储的元素个数。
> - **capacity** = `_M_end_of_storage - _M_start`，表示已分配但不一定使用的内存大小。

以上便是vector底层的一些实现。

#### 2、扩容策略

看完了上面的一些底层实现，可以很清晰的知道容器的容量（capacity）和大小（size）是怎么来的。

那么，它的扩容策略怎么实现，继续往下看。

##### 什么时候扩容？

当调用 `push_back()` 或其他插入方法时，`std::vector` 会检查是否有足够的空间来容纳新元素。如果没有足够空间，即 `_M_finish == _M_end_of_storage`，则会触发扩容操作。

对于`push_back()`，有两种方式：

一种是接受常量引用（`const T&`）的元素，另一种是接受右值引用（`T&&`）的元素。后一种（右值引用版本）支持通过移动语义高效地插入临时对象，而无需进行不必要的复制。

```c++
void push_back(const value_type& __x)
void push_back(value_type&& __x)
```

源码：

```c++
void push_back(const value_type& __x)
      {
	if (this->_M_impl._M_finish != this->_M_impl._M_end_of_storage)
	  {
	    _GLIBCXX_ASAN_ANNOTATE_GROW(1);
	    _Alloc_traits::construct(this->_M_impl, this->_M_impl._M_finish,
				     __x);
	    ++this->_M_impl._M_finish;
	    _GLIBCXX_ASAN_ANNOTATE_GREW(1);
	  }
	else
	  _M_realloc_insert(end(), __x);
      }

#if __cplusplus >= 201103L
      void push_back(value_type&& __x)
      { emplace_back(std::move(__x)); }
```

可以看出在接受右值引用（`T&&`）元素的时候，里面实际上调用的是`emplace_back(std::move(__x));`。

再看看接受常量引用（`const T&`）的元素时，`push_back()`的实现：

1. 如果`_M_finish`还没有到`_M_end_of_storage`的时候，直接会将元素添加到`_M_finish`后面；
2. 如果超出`_M_end_of_storage`，就会调用`_M_realloc_insert(end(), __x);`。

##### 怎么扩容？

这里就要从`_M_realloc_insert(end(), __x);`说起。

```c++
void
vector<_Tp, _Alloc>::
_M_realloc_insert(iterator __position, _Args&&... __args)
#else
template<typename _Tp, typename _Alloc>
void
vector<_Tp, _Alloc>::
_M_realloc_insert(iterator __position, const _Tp& __x)
#endif
{
    // 检查并确保向量有足够的容量来容纳新元素。
    const size_type __len =
        _M_check_len(size_type(1), "vector::_M_realloc_insert");

    pointer __old_start = this->_M_impl._M_start;
    pointer __old_finish = this->_M_impl._M_finish;

    // 计算插入位置之前的元素数量。
    const size_type __elems_before = __position - begin();

    // 分配新的内存块。
    pointer __new_start(this->_M_allocate(__len));
    pointer __new_finish(__new_start);

    __try
    {
        // 构造新元素到新分配的内存中。
        // 对于 C++11 及以上版本，使用完美转发构造参数；对于 C++11 之前版本，使用拷贝构造。
#if __cplusplus >= 201103L
        _Alloc_traits::construct(this->_M_impl,
                                 __new_start + __elems_before,
                                 std::forward<_Args>(__args)...);
#else
        _Alloc_traits::construct(this->_M_impl,
                                 __new_start + __elems_before,
                                 __x);
#endif

        // 如果支持快速重定位，则使用重定位方式移动元素，否则使用未初始化移动算法。
#if __cplusplus >= 201103L
        if _GLIBCXX17_CONSTEXPR (_S_use_relocate())
        {
            // 使用重定位方式移动元素。
            __new_finish = _S_relocate(__old_start, __position.base(),
                                       __new_start, _M_get_Tp_allocator());

            ++__new_finish;

            __new_finish = _S_relocate(__position.base(), __old_finish,
                                       __new_finish, _M_get_Tp_allocator());
        }
        else
#endif
        {
            // 使用未初始化移动算法移动元素。
            __new_finish
                = std::__uninitialized_move_if_noexcept_a
                  (__old_start, __position.base(),
                   __new_start, _M_get_Tp_allocator());

            ++__new_finish;

            __new_finish
                = std::__uninitialized_move_if_noexcept_a
                  (__position.base(), __old_finish,
                   __new_finish, _M_get_Tp_allocator());
        }
    }
    __catch(...)
    {
        // 异常处理：如果发生异常，销毁已构造的对象并释放新分配的内存。
        if (!__new_finish)
            _Alloc_traits::destroy(this->_M_impl,
                                   __new_start + __elems_before);
        else
            std::_Destroy(__new_start, __new_finish, _M_get_Tp_allocator());
        _M_deallocate(__new_start, __len);
        __throw_exception_again;
    }

    // 如果不支持快速重定位，则销毁旧内存中的元素。
#if __cplusplus >= 201103L
    if _GLIBCXX17_CONSTEXPR (!_S_use_relocate())
#endif
        std::_Destroy(__old_start, __old_finish, _M_get_Tp_allocator());

    // 注释掉旧内存并更新向量的状态。
    _GLIBCXX_ASAN_ANNOTATE_REINIT;
    _M_deallocate(__old_start,
                  this->_M_impl._M_end_of_storage - __old_start);

    // 更新向量的起始、结束和存储末尾指针。
    this->_M_impl._M_start = __new_start;
    this->_M_impl._M_finish = __new_finish;
    this->_M_impl._M_end_of_storage = __new_start + __len;
}
```

这里面有一个函数至关重要，`_M_check_len`：

```c++
size_type
_M_check_len(size_type __n, const char* __s) const
{
    // 检查是否添加 __n 个元素会导致超过最大容量。
    if (max_size() - size() < __n)
        __throw_length_error(__N(__s));  // 如果会超过最大容量，则抛出长度错误异常

    // 计算新容量：
    // 新容量为当前大小加上当前大小和需要增加的元素数量中的较大值。
    // 这种策略确保了当需要增加大量元素时，容量能够一次性扩展足够大。
    const size_type __len = size() + (std::max)(size(), __n);

    // 确保新容量不会小于当前大小且不超过最大容量。
    // 如果新容量小于当前大小或超过最大容量，则返回最大容量。
    return (__len < size() || __len > max_size()) ? max_size() : __len;
}
```

那么**二倍**具体是怎么体现出来的呢！？

这行代码：`const size_type __len = size() + (std::max)(size(), __n);`

- 如果当前 `vector` 的大小较大，则新长度为 `size() + size()`，即原来的两倍。
- 如果插入的元素数量较大（如批量插入），则新长度为 `size() + __n`，这可能不是严格的二倍扩容，但通常情况下 `__n` 是 1，因此会实现二倍扩容。

### 二、一些成员函数

#### 1、`resize()`

允许你调整 `vector` 容器的大小，**增大或缩小容器的大小**。

源码：

```c++
void resize(size_type __new_size)
{
  // 如果新大小大于当前大小
  if (__new_size > size())
    // 调用 _M_default_append 添加 (新大小 - 当前大小) 个默认构造的元素
    _M_default_append(__new_size - size());
  // 如果新大小小于当前大小
  else if (__new_size < size())
    // 调用 _M_erase_at_end 删除从当前位置到末尾的多余元素
    _M_erase_at_end(this->_M_impl._M_start + __new_size);
}

_GLIBCXX20_CONSTEXPR
void resize(size_type __new_size, const value_type& __x)
{
  // 如果新大小大于当前大小
  if (__new_size > size())
    // 调用 _M_fill_insert 在末尾插入 (新大小 - 当前大小) 个由 __x 初始化的元素
    _M_fill_insert(end(), __new_size - size(), __x);
  // 如果新大小小于当前大小
  else if (__new_size < size())
    // 调用 _M_erase_at_end 删除从当前位置到末尾的多余元素
    _M_erase_at_end(this->_M_impl._M_start + __new_size);
}
```

- **增大大小**：当新的大小大于当前大小时，`resize()` 会向容器添加新的元素。这些新元素的值将被初始化为给定的 `value`（如果提供）或者类型 `T` 的默认值。
- **缩小大小**：当新的大小小于当前大小时，`resize()` 会删除多余的元素，容器的大小会被减少到指定的新大小。

其中核心函数有三个，分别是：

```c++
void _M_default_append(size_type __n)
{
  // 确保有足够的容量来容纳新的元素
  // 使用默认构造函数创建并添加 __n 个新元素
}
```

```c++
void _M_fill_insert(iterator __position, size_type __n, const value_type& __x)
{
  // 确保有足够的容量来容纳新的元素
  // 在指定位置 __position 插入 __n 个由 __x 初始化的新元素
}
```

```c++
void _M_erase_at_end(pointer __pos)
{
  // 销毁从 __pos 开始到末尾的所有元素
  // 更新 vector 的结束指针
}
```

具体源码就在这里不展示了，大家有兴趣可以去看看。

#### 2、`reserve（）`

用于 **预先分配内存**，以避免频繁的内存重新分配和拷贝操作。通过提前分配足够的内存，能够提高程序的性能，尤其是在已知容器大小或元素数量时。

源码：

```c++
void vector<_Tp, _Alloc>::reserve(size_type __n)
{
  // 检查新容量是否超过最大允许的大小
  if (__n > this->max_size())
    __throw_length_error(__N("vector::reserve"));  // 如果超过，则抛出长度错误异常

  // 如果当前容量小于所需的新容量，则需要重新分配内存
  if (this->capacity() < __n)
  {
    const size_type __old_size = size();  // 记录当前向量的大小（元素数量）

    pointer __tmp;  // 用于存储新分配的内存指针

#if __cplusplus >= 201103L
    // 如果编译器支持 C++11 及以上版本，并且可以使用 relocate 优化
    if _GLIBCXX17_CONSTEXPR (_S_use_relocate())
    {
      // 分配新的内存块，大小为 __n
      __tmp = this->_M_allocate(__n);

      // 使用 relocate 方法将现有元素移动到新分配的内存中
      _S_relocate(this->_M_impl._M_start, this->_M_impl._M_finish,
                  __tmp, _M_get_Tp_allocator());
    }
    else
#endif
    {
      // 分配新的内存并复制或移动现有元素到新内存中
      __tmp = _M_allocate_and_copy(__n,
        _GLIBCXX_MAKE_MOVE_IF_NOEXCEPT_ITERATOR(this->_M_impl._M_start),
        _GLIBCXX_MAKE_MOVE_IF_NOEXCEPT_ITERATOR(this->_M_impl._M_finish));

      // 销毁旧内存中的元素
      std::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,
                    _M_get_Tp_allocator());
    }

    // 释放旧的内存块
    _M_deallocate(this->_M_impl._M_start,
                  this->_M_impl._M_end_of_storage - this->_M_impl._M_start);

    // 更新指向新内存的指针
    this->_M_impl._M_start = __tmp;
    this->_M_impl._M_finish = __tmp + __old_size;
    this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
  }
}
```

#### 3、`emplace_back()`

用于在容器末尾构造一个新元素。它与 `push_back` （上面已详细讲解，这里不再赘述）类似，但有一个重要的区别：`emplace_back` 直接在 `vector` 内部构造元素，而不是先创建元素后再将其复制或移动到 `vector` 中。这意味着 `emplace_back` 可能会比 `push_back` 更高效，尤其是在元素类型较为复杂、构造过程昂贵时。

源码：

```c++
vector<_Tp, _Alloc>::emplace_back(_Args&&... __args)
{
  // 检查当前向量的结束指针是否指向存储区的末尾
  if (this->_M_impl._M_finish != this->_M_impl._M_end_of_storage)
  {
    // 在当前结束位置构造一个新的元素，使用传递的参数进行完美转发
    _Alloc_traits::construct(this->_M_impl, this->_M_impl._M_finish,
                             std::forward<_Args>(__args)...);

    // 将结束指针向前移动一位，表示新元素已加入
    ++this->_M_impl._M_finish;
  }
  else
  {
    // 如果当前存储区已满，则需要重新分配内存并在新的位置插入元素
    _M_realloc_insert(end(), std::forward<_Args>(__args)...);
  }

#if __cplusplus > 201402L
  // 如果编译器支持 C++17 及以上版本，则返回新插入的元素引用
  return back();
#endif
}
```

#### 4、`erase()`

用于从容器中删除一个或多个元素。它具有灵活的使用方式，允许删除单个元素或删除一个元素范围。`erase` 会修改容器的大小，并且会重新调整容器中元素的排列顺序。

源码1：

```c++
erase(const_iterator __position)
{
  // 计算传入的 const_iterator 在 vector 中的位置索引
  // 通过将传入的迭代器减去 cbegin() 得到偏移量
  // 然后将这个偏移量加到 begin() 上，得到对应的普通迭代器位置
  return _M_erase(begin() + (__position - cbegin()));
}
```

这里面的`_M_erase()`是这样实现的：

非bool型：

```c++
typename vector<_Tp, _Alloc>::iterator
vector<_Tp, _Alloc>::_M_erase(iterator __position)
{
  // 如果要删除的元素不是最后一个元素
  if (__position + 1 != end())
  {
    // 将 [__position + 1, end()) 范围内的元素向前移动一位，覆盖被删除的元素
    _GLIBCXX_MOVE3(__position + 1, end(), __position);
  }

  // 更新结束指针，使其指向新的最后一个有效元素的位置
  --this->_M_impl._M_finish;

  // 销毁刚刚更新后的结束位置处的元素（即原来最后一个元素的位置）
  _Alloc_traits::destroy(this->_M_impl, this->_M_impl._M_finish);

  // 更新 AddressSanitizer 的注解，表明已缩小一个元素
  _GLIBCXX_ASAN_ANNOTATE_SHRINK(1);

  // 返回指向被删除元素位置的迭代器
  return __position;
}
```

bool型：

```c++
typename vector<bool, _Alloc>::iterator
vector<bool, _Alloc>::_M_erase(iterator __position)
{
  // 如果要删除的元素不是最后一个元素
  if (__position + 1 != end())
  {
    // 将 [__position + 1, end()) 范围内的元素向前移动一位，覆盖被删除的元素
    std::copy(__position + 1, end(), __position);
  }

  // 更新结束指针，使其指向新的最后一个有效元素的位置
  --this->_M_impl._M_finish;

  // 返回指向被删除元素位置的迭代器
  return __position;
}
```

源码2：

```c++
erase(const_iterator __first, const_iterator __last)
{
  // 获取 vector 的起始位置的普通迭代器和常量迭代器
  const auto __beg = begin();    // 普通迭代器，指向 vector 的第一个元素
  const auto __cbeg = cbegin();  // 常量迭代器，指向 vector 的第一个元素

  // 计算传入的范围 [__first, __last) 在 vector 中的位置索引
  // 将传入的 const_iterator 转换为对应的普通迭代器
  return _M_erase(__beg + (__first - __cbeg), __beg + (__last - __cbeg));
}
```

这里的`_M_erase`是这样实现的：

非bool型：

```c++
typename vector<_Tp, _Alloc>::iterator
vector<_Tp, _Alloc>::_M_erase(iterator __first, iterator __last)
{
  // 如果要删除的范围不为空（即 __first 和 __last 不相等）
  if (__first != __last)
  {
    // 如果要删除的范围不是到末尾的整个范围
    if (__last != end())
    {
      // 将 [__last, end()) 范围内的元素向前移动，覆盖被删除的元素
      _GLIBCXX_MOVE3(__last, end(), __first);
    }

    // 更新结束指针，使其指向新的最后一个有效元素的位置
    // 计算需要销毁的元素数量，并调用 _M_erase_at_end 来处理这些元素
    _M_erase_at_end(__first.base() + (end() - __last));
  }

  // 返回指向删除范围起始位置的迭代器
  return __first;
}
```

bool型：

```c++
typename vector<bool, _Alloc>::iterator
vector<bool, _Alloc>::_M_erase(iterator __first, iterator __last)
{
  // 如果要删除的范围不为空（即 __first 和 __last 不相等）
  if (__first != __last)
  {
    // 将 [__last, end()) 范围内的元素向前移动，覆盖被删除的元素
    // std::copy 返回指向新复制结束位置的迭代器
    iterator new_end = std::copy(__last, end(), __first);

    // 更新结束指针，使其指向新的最后一个有效元素的位置
    // 调用 _M_erase_at_end 来处理多余的元素并销毁它们
    _M_erase_at_end(new_end);
  }

  // 返回指向删除范围起始位置的迭代器
  return __first;
}
```

ok，上面基本上就是我今天想分享的内容，从源码的角度分析了一下vector的扩容策略和一些常用成员函数。

看完你一定会豁然开朗，原来vector这么简单！

记得三连支持一下哦~