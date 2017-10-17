# C++模板，有爱小教程

### 0. Keep in mind

1. template是一个**编译时**的概念/机制，在得到可执行文件之后，template就不存在了。

### 1. 函数模板

首先从最简单的情况开始，即函数模板，如下：

```c++
template <typename T>
int compare(const T &v1, const T &v2) {
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    
    return 0;
}
```

这个最简单的情况是无需多言的，涉及了几个基本概念：

(1) template关键字后面的，尖括号内的东西，是**模板参数列表（template parameter list）**

(2) 同理，模板参数列表里面是若干个用逗号分隔的**模板参数（template parameter）**。一般习惯于把模板参数写作T，但是本质上可用任意名字。

(3) 在实际编译的时候，编译器会把具体的类型绑定到模板参数上，这时这个具体的类型称为**模板实参（template argument）**

(4) 编译器利用模板实参，可以从函数模板中进行**实例化（instantiate）**，并且得到模板的**实例（instantiation）**

接下来，我们需要了解一下，在编译过程中发生了什么：

（1）编译器遇到了一个函数模板定义，可以认为**什么都不做**。当然，编译器通常还是会形式化检查一下语法错误，比如少了一个分号之类的。

（2）编译器遇到了对函数模板的使用，寻找对应的函数模板定义并完成实例化，之后对实例化的函数进行编译。编译器首先会检查一下实参数据、类型是否有问题。之后，主要的编译问题会和实例化的过程有关，根据编译器的实现方式不同，甚至**可能在链接时才报错**。

（3）编译器在实例化的过程中，需要完全了解函数模板的声明+定义，具体来说，所有不依赖于模板参数的名字，都必须是编译器直接可见的，这一点程序员必须保证。实际上，常用的写法是，**在头文件里面同时写函数模板/类模版成员函数声明和定义**。

举一个常见的模板实例化的错误：

先看一眼上面那个compare函数模板，这个函数模板其实并不适用于任意的类型T，而是蕴含了一个要求：**类型T必须支持运算符“<”**。如果在使用compare函数的时候，类型T不支持这个运算符，就会在实例化的时候报错了。

### 2.类模版

基本上，类模版和函数模板没有很大的区别。有一点需要注意，即类模版的参数类型**无法通过编译器自动推断完成**，必须在程序里面显式给出。

```c++
int a = 0, b = 0;
compare(a, b);  // 编译器会自动推断模板参数类型为int
```

类模版实例化出来的，是不同的类。**这里的“不同”是真正的“不同”**，例如一个模版类，按照两个不同的参数类型T完成了实例化，这两个实例化出来的类定义之间没有任何关联。

此外，类模板实例化之后，如果类成员函数没有被使用，则暂时不会被实例化。（即，只有在成员函数被用到的时候才会被实例化）

下面给出一个类模版的例子（来自C++ Primer）：

```c++
template <typename T> class Blob {
public:
    typedef T value_type;
    typedef typename std::vector<T>::size_type size_type;

    // 构造函数
    Blob();
    Blob(std::initializer_list<T> il);

    // Blob中的元素数目
    size_type size() const { return data->size(); }

    // 添加/删除/清空元素
    void push_back(const T &t) { data->push_back(t); }
    void push_back(T &&t) { data->push_back(std::move(t)); }
    void pop_back();
    bool empty() const { return data->empty(); }

    // 元素访问
    T& back();
    T& opeator[](size_type i);
private:
    std::shared_ptr<std::vector<T> > data;
    void check(size_type i, const std::string &msg) const;
}
```

（不用太在意例子，这个例子是书里面贯穿若干章节的）

在一个类模版里面，如果需要在类定义的外面定义函数，则需要带最开始的template那一行，例如：

```c++
template <typename T>
void Blob<T>::check(size_type i, const std::string &msg) const {
    if (i >= data->size())
        throw std::out_of_range(msg);
}
```

类模版在被实例化之后，实际上相当于定义了一个新的类。为了简化，可以给这个新的类一个别名，例如：

```c++
typedef Blob<string> StrBlob;
```

C++ 11带来了一种更灵活的别名，T本身也可以成为别名的一个部分：
```c++
template<typename T> using partNo = pair<T, unsigned>;
partNo<string> books; // pair<string, unsigned>
partNo<Student> kids; // pair<Student, unsigned>
```

### 3. 模板带来的新问题

#### 3-1. 到底是不是类型？

```c++
template <typename T>
typename T::value_type top(const T& c) {
    if (!c.empty())
        return c.back();
    else
        return typename T::value_type();
}
```

编译器在遇到 :: 符号的时候，会优先认为后面接的是一个具体的变量/函数名字，而不是一个类型。如果我们真的希望指定一个类型，则需要用typename显式指定。

#### 3-2. 模板的默认实参

模板参数可以指定一个默认类型，例如：

```c++
template <class T = int> class Numbers {
public:
    Numbers(T v = 0): val(v) { }

private:
    T val;
}

Numbers<long double> a;
Numbers<> b; // 使用默认的类型，函数模板使用默认类型时不写尖括号
```

#### 3-3. 类模版 + 函数模板

在类模版里面，可以定义函数模板，这两套模板参数列表是各自独立的，例如：

```c++
template <typename T> class Blob {
    template <typename It> Blob(It b, It e);
    ...
};
```

如果希望在类模板外面写函数模板的定义，就需要把两套模板参数列表都写上，例如：

```c++
template <typename T>
template <typename It>
    Blob<T>::Blob(It b, It e) { ... }
```

#### 3-4. 显式实例化

C++ 11提供了显式实例化（explicit instantiation）机制，来避免因为“遇到时才实例化”导致的多个object文件中重复实例化同一个东西的性能消耗。

做法很简单，例如对于模板函数，显示实例化包含下面这两个东西：

```c++
extern template int compare(const int&, const int&); // 声明
template int compare(const int&, const int&);        // 定义
```

一个“声明”和一个“定义”对应，用int做了compare函数模板的实例化。具体在写程序的时候，需要用到int版本compare的每个地方都先写一下“声明”。这时编译器会暂时不去做实例化，等着最后链接的时候找到“定义”的部分，使用那一份实例化。

有一点和一般情况不同，这种显式实例化如果用于类模版，则需要实例化类模版的所有成员函数。（一般情况下，成员函数只在用到时才实例化）

#### 3-5. 模板参数推断能做到什么，做不到什么

对于函数模板来说，编译器会尽量通过调用时的参数类型进行模板参数的推断，但是这种推断是有逻辑上的极限的。（当然，也可以都帮编译器指定好）

```c++
template <typename T1, typename T2, typename T3> 
T1 sum(T2, T3);
```

在上面这个函数模板里面，T2和T3都是可以推断得到的，但是T1并不能。所有有这么几种正确的调用方式：

```c++

```



