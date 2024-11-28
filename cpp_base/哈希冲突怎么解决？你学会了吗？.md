# 哈希冲突怎么解决？你学会了吗？

## 1、什么是哈希冲突？

在使用哈希函数时，不同的输入（键）通过哈希函数映射到同一个哈希值或者同一个哈希表索引位置的情况。这种现象是不可避免的，因为哈希函数的输入域是无限的，而哈希表的地址是有限的。

## 2、为什么会产生哈希冲突？

### 1. 输入域的无限性和输出域的有限性

- 输入域的无限性：哈希函数的输入可以是任意长度的数据，例如字符串、文件、数字等。这些输入的组合是无限的。
- 输出域的有限性：哈希函数的输出是一个固定长度的值（通常是固定长度的整数或二进制串）。例如，一个32位的哈希值只有 2^32 种可能的值。 

由于输入域是无限的，而输出域是有限的，根据鸽巢原理，必然会有多个不同的输入映射到同一个输出值。这就是哈希冲突的根本原因。

### 2. 哈希函数的不均匀分布

- 哈希函数的设计：即使哈希函数设计得再好，也不可能完全避免冲突。哈希函数的目标是将输入均匀地分布到输出域中，但现实中很难做到完美均匀。
- 数据的分布：输入数据的分布情况也会影响哈希冲突的发生。如果输入数据分布不均匀，某些哈希值可能会更频繁地出现，导致更多的冲突。

### 3. 哈希表的大小限制

- 哈希表的大小：哈希表的大小通常是固定的，而输入数据的数量可能是无限的。当哈希表的大小不足以容纳所有可能的输入时，冲突不可避免。
- 负载因子：负载因子是哈希表中存储的元素数量与哈希表大小的比值。当负载因子过高时，哈希表中的空闲位置减少，冲突的概率增加。

### 4. 特殊数据模式

- 特殊输入：某些特殊的输入模式可能会导致哈希函数产生相同的哈希值。例如，如果哈希函数对输入的某些部分不敏感，那么具有相似部分的输入可能会映射到同一个哈希值。
- 恶意攻击：在某些情况下，攻击者可能会故意构造输入数据，使其产生相同的哈希值，以引发哈希冲突，这种攻击称为哈希洪水攻击。

## 3、哈希冲突会产生什么影响？

### 1. 数据检索效率降低

- 增加查找时间：当发生哈希冲突时，多个键值对可能被映射到同一个哈希表位置。这意味着在查找特定键值对时，需要进行额外的比较和处理，从而增加查找时间。例如，在链地址法中，需要遍历链表中的所有元素；在开放地址法中，需要进行多次探测以找到目标元素。
- 时间复杂度增加：理想情况下，哈希表的查找时间复杂度为 O(1)。然而，随着冲突的增多，查找时间复杂度可能退化为 O(n)，特别是在极端情况下，哈希表退化为链表或数组。

### 2. 空间利用率降低

- 空间浪费：为了减少冲突，哈希表的大小通常需要设置得比实际存储的元素数量大得多。然而，即使这样，冲突仍然可能发生，导致部分哈希表位置被浪费。例如，在开放地址法中，为了减少冲突，哈希表需要保持较低的负载因子，这会导致大量的空闲位置。
- 存储冗余：在链地址法中，每个哈希表位置需要维护一个链表，即使链表中只有一个元素，也需要额外的存储空间来维护链表结构。

### 3. 插入和删除操作复杂度增加

- 插入操作：在插入新元素时，如果发生冲突，需要进行额外的处理，如寻找下一个空闲位置（开放地址法）或将元素插入链表（链地址法）。这不仅增加了插入时间，还可能导致数据结构的复杂性增加。
- 删除操作：在删除元素时，如果使用链地址法，只需从链表中删除相应节点；但如果使用开放地址法，删除操作需要特殊处理，以避免影响后续的查找操作。例如，可以使用标记删除法，但这会增加额外的开销。

### 4. 系统性能下降

- 性能瓶颈：哈希冲突可能导致系统性能下降，特别是在高并发环境下。例如，多个线程同时访问哈希表时，冲突处理可能会成为性能瓶颈。
- 资源消耗增加：为了处理冲突，系统需要进行额外的计算和内存操作，这会增加CPU和内存的负担，从而影响整体性能。

### 5. 安全性问题

- 哈希洪水攻击：恶意用户可能故意构造输入数据，使其产生相同的哈希值，以引发大量冲突。这种攻击称为哈希洪水攻击（Hash Flooding Attack），可以导致系统资源耗尽，甚至崩溃。
- 数据完整性受损：在某些应用场景中，哈希冲突可能导致数据完整性受损。例如，在分布式系统中，哈希冲突可能导致数据分布不均，影响系统的可靠性和一致性。

## 4、怎么预防哈希冲突？

### 1. 选择合适的哈希函数

选择一个良好的哈希函数是预防哈希冲突的关键。一个优秀的哈希函数应该具备以下特点：

- 均匀分布：哈希函数应将输入值均匀地映射到哈希表的不同位置，避免某些位置过于密集，而其他位置空闲。
- 低冲突率：哈希函数应尽量减少不同输入值映射到同一位置的概率。
- 计算效率高：哈希函数的计算应简单且快速，以提高整体性能。

常见的哈希函数包括：

- 除留余数法：H(key)=key%m，其中m是哈希表的大小。选择一个质数作为m可以减少冲突。
- 平方取中法：先计算输入值的平方，然后取中间几位作为哈希值。这种方法可以增加哈希值的随机性。
- 分段叠加法：将输入值分成若干段，然后将这些段相加，取结果的一部分作为哈希值。
- 数字分析法：根据输入值的特征选择某些位作为哈希值。

### 2. 增加哈希表大小

通过增加哈希表的大小，可以减少哈希冲突的概率。哈希表的大小决定了哈希值的范围，更大的哈希表意味着更小的哈希值范围，从而降低了哈希冲突的可能性。

- 负载因子：负载因子α是哈希表中已存储的元素数量与哈希表大小的比值。通常建议将负载因子保持在0.7以下，以减少冲突。
- 动态调整：当哈希表的负载因子超过某个阈值时，可以动态增加哈希表的大小，并重新计算所有元素的哈希值，将其重新插入新的哈希表中。

### 3. 使用哈希盐值

哈希盐值是一种用于增加哈希函数复杂性的随机值。通过在原始数据上添加盐值，可以使相同的输入值产生不同的哈希值，从而降低哈希冲突的概率。

- 盐值选择：盐值可以是一个固定的随机数，也可以是一个动态生成的随机数。选择一个足够大的盐值空间可以有效减少冲突。

- 盐值应用：在计算哈希值时，将盐值与输入值组合在一起，例如

  H(key)=hash(key+salt)。

### 4. 使用多重哈希函数

使用多个不同的哈希函数可以进一步减少冲突。当使用第一个哈希函数发生冲突时，可以使用第二个哈希函数计算新的哈希值，直到找到一个空闲位置。

- 双哈希法：使用两个哈希函数f(key)和g(key)，当发生冲突时，计算新的哈希值Hi(下标i)=(f(key)+i⋅g(key))%m，其中i是冲突次数，m是哈希表的大小。
- 再哈希法：当发生冲突时，使用备用的哈希函数重新计算哈希值，直到找到一个空闲位置。

## 5、解决哈希冲突的方法？

### 链地址法

#### 原理

每个哈希表的桶（数组中的每个位置）都指向一个链表（或其他数据结构），当多个键发生冲突时，它们会被存储在同一个链表中。

#### 实现

```c
class SimpleHashMap {
private:
    vector<list<pair<int, int>>> buckets;

public:
    SimpleHashMap(int capacity) : buckets(capacity) {}

    void put(int key, int value) {
        int index = key % buckets.size();
        for (auto& kv : buckets[index]) {
            if (kv.first == key) {
                kv.second = value; // 更新值
                return;
            }
        }
        buckets[index].push_back({key, value});
    }

    int get(int key) {
        int index = key % buckets.size();
        for (const auto& kv : buckets[index]) {
            if (kv.first == key) {
                return kv.second;
            }
        }
        return -1; // 未找到
    }
};
```

#### 优缺点

- 优点：实现简单，冲突严重时性能相对较好，理论上允许哈希表的大小无限增长。
- 缺点：链表的长度可能变得很长，导致查找效率下降。每个桶的内存开销增加（存储指针）。

### 开放地址法

#### 原理

当发生哈希冲突时，直接在哈希表数组中寻找其他空闲位置存储冲突元素。

#### 常见策略

##### 线性探测

###### 原理

按顺序检查哈希表中下一个位置，直到找到一个空闲位置为止。

###### 实现

```c
class LinearProbingHashMap {
private:
    vector<pair<int, int>> table;
    int capacity;

public:
    LinearProbingHashMap(int capacity) : capacity(capacity), table(capacity, {-1, -1}) {}

    void put(int key, int value) {
        int index = key % capacity;
        while (table[index].first != -1 && table[index].first != key) {
            index = (index + 1) % capacity; // 线性探测
        }
        if (table[index].first == key) {
            table[index].second = value; // 更新值
        } else {
            table[index] = {key, value}; // 插入新值
        }
    }

    int get(int key) {
        int index = key % capacity;
        while (table[index].first != -1) {
            if (table[index].first == key) {
                return table[index].second;
            }
            index = (index + 1) % capacity; // 线性探测
        }
        return -1; // 未找到
    }
};
```

###### 优缺点

- 优点：不需要额外的存储空间（如链表），内存效率高。
- 缺点：可能导致较长的查找时间，特别是在哈希表接近满时。容易产生“聚集”问题，降低查找效率。

##### 二次探测

###### 原理

通过二次方的增量进行位置探测（如1, 4, 9, 16...），避免了线性探测中聚集的问题。

###### 实现

```c
class QuadraticProbingHashMap {
private:
    vector<pair<int, int>> table;
    int capacity;

public:
    QuadraticProbingHashMap(int capacity) : capacity(capacity), table(capacity, {-1, -1}) {}

    void put(int key, int value) {
        int index = key % capacity;
        int i = 0;
        while (table[index].first != -1 && table[index].first != key) {
            i++;
            index = (index + i * i) % capacity; // 二次探测
        }
        if (table[index].first == key) {
            table[index].second = value; // 更新值
        } else {
            table[index] = {key, value}; // 插入新值
        }
    }

    int get(int key) {
        int index = key % capacity;
        int i = 0;
        while (table[index].first != -1) {
            if (table[index].first == key) {
                return table[index].second;
            }
            i++;
            index = (index + i * i) % capacity; // 二次探测
        }
        return -1; // 未找到
    }
};
```

##### 双重哈希

###### 原理

使用两个不同的哈希函数，第二个哈希函数用于计算冲突后的探测步长，进一步减少冲突的机会。

###### 实现

```c
class DoubleHashingHashMap {
private:
    vector<pair<int, int>> table;
    int capacity;
    int secondHash(int key) {
        return 7 - (key % 7); // 第二个哈希函数
    }

public:
    DoubleHashingHashMap(int capacity) : capacity(capacity), table(capacity, {-1, -1}) {}

    void put(int key, int value) {
        int index = key % capacity;
        int step = secondHash(key);
        int i = 0;
        while (table[index].first != -1 && table[index].first != key) {
            i++;
            index = (index + step) % capacity; // 双重哈希
        }
        if (table[index].first == key) {
            table[index].second = value; // 更新值
        } else {
            table[index] = {key, value}; // 插入新值
        }
    }

    int get(int key) {
        int index = key % capacity;
        int step = secondHash(key);
        int i = 0;
        while (table[index].first != -1) {
            if (table[index].first == key) {
                return table[index].second;
            }
            i++;
            index = (index + step) % capacity; // 双重哈希
        }
        return -1; // 未找到
    }
};
```

### 再哈希法

#### 原理

当哈希表的负载因子超过某个阈值时，创建一个更大的哈希表，并将所有元素重新映射到新的哈希表中。

#### 实现

```c
class RehashingHashMap {
private:
    vector<pair<int, int>> table;
    int capacity;
    double loadFactorThreshold = 0.75;

    void rehash() {
        vector<pair<int, int>> oldTable = table;
        capacity *= 2;
        table = vector<pair<int, int>>(capacity, {-1, -1});

        for (const auto& kv : oldTable) {
            if (kv.first != -1) {
                put(kv.first, kv.second);
            }
        }
    }

public:
    RehashingHashMap(int initialCapacity) : capacity(initialCapacity), table(capacity, {-1, -1}) {}

    void put(int key, int value) {
        if (static_cast<double>(table.size()) / capacity > loadFactorThreshold) {
            rehash();
        }
        int index = key % capacity;
        while (table[index].first != -1 && table[index].first != key) {
            index = (index + 1) % capacity; // 线性探测
        }
        if (table[index].first == key) {
            table[index].second = value; // 更新值
        } else {
            table[index] = {key, value}; // 插入新值
        }
    }

    int get(int key) {
        int index = key % capacity;
        while (table[index].first != -1) {
            if (table[index].first == key) {
                return table[index].second;
            }
            index = (index + 1) % capacity; // 线性探测
        }
        return -1; // 未找到
    }
};
```

#### 优缺点

- 优点：可以恢复哈希表的性能，避免聚集。
- 缺点：需要一次性移动所有元素，可能引起短暂的性能下降。