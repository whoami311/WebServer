<style type="text/css" rel="stylesheet">
    .google-code {
        background-color: #fff;
        border: solid 1px #e1e4e5;
        font-size: 80%;
        color: #e74c3c;
        margin: 0 5px;
    }
</style>

# C++编码规范

## 1. 命名规范

### 1.1 标识符命名

| 类型 | 命名规则 |
| :- | :- |
| 类、结构体、联合体类型等的类型名，命名空间定义 | 大驼峰
| 函数（包括全局函数，作用域函数，成员函数） | 大驼峰
| 类的成员变量 | ‘m_’ + 小驼峰
| 全局变量 | ‘g_’ + 小驼峰
| 静态变量 | ‘s_’ + 小驼峰
| 局部变量、函数形参、结构体和联合体中的成员变量 | 小驼峰
| 宏、常量（const）、枚举值、goto标签 | 全大写，下划线分割
| 枚举类型名 | ‘E_’ + 大驼峰

```c++
#define MAX_USER_NUM 10     // 宏定义：全大写，下划线分割

enum E_ColorList {  // 枚举类型名：‘E_’ + 大驼峰
    COLOR_RED = 0,  // 枚举值：全大写，下划线分割
    COLOR_GREEN = 1,
    COLOR_BLUE = 2
};

struct MyStruct {   // 结构体：大驼峰
    int ipAddr;     // 结构体成员变量：小驼峰
};

int g_userId;       // 全局变量：‘g_’ + 小驼峰
static s_userCnt;   // 静态变量：‘s_’ + 小驼峰

class MyClass {     // 类名：大驼峰
public:
    int GetClassNum();  // 函数：大驼峰
private:
    int m_classNum;     // 成员变量：‘m_’ + 小驼峰
}
```

- 在保证别人能理解的前提下，适当使用缩写减少变量名长度。
一些约定俗成的单词缩写可以保证可读性的前提下很好地减少变量名长度，如count -> cnt，delete -> del，address -> addr等。

### 1.2 文件命名

- 文件命名采用全小写+下划线分割方式。

c++文件以.cpp结尾，头文件以.h结尾，同时避免与<code class="google-code">/usr/include/</code>（编译器搜索系统头文件的路径）下的文件名冲突。单独承载类的文件名要与类名对应，如<code class="google-code">foo_bar.h</code>和<code class="google-code">foo_bar.cpp</code>对应类<code class="google-code">FooBar</code>。

</br>

## 2. 头文件

### 2.1 内联函数

- 只有当函数只有10行甚至更少时才考虑将其定义为内联函数。

内联是以代码膨胀（复制）为代价，省去了函数调用的开销从而提高执行效率。所以只适合简单的不存在循环的函数使用。内联一个较大的函数将增加代码大小，得不偿失。

### 2.2 #include的路径及顺序

- 项目内头文件应按照项目源代码目录树结构排列，避免使用Unix特殊的快捷目录 .（当前目录）和..（上级目录）：

```cpp
#include "base/config.h"
```

- 头文件包含的次序：<code class="google-code">dir/foo.cpp</code>的主要作用是实现dir2/foo.h的功能，foo.cpp中包含头文件的次序如下：

1. dir2/foo2.h
2. C系统文件
3. C++系统文件
4. 其他库的.h文件
5. 本项目内.h文件

```cpp
#include "dir2/foo.h"

#include "sys/types.h"
#include "unistd.h"

#include "hash_map"
#include "vector"

#include "base/basictypes.h"
#include "base/commandlineflags.h"
#include "dir2/bar.h"
```

这样的优先顺序排序保证挡dir2/foo2.h遗漏某些必要的库时，dir/foo.cc的构建会立刻终止。

</br>

## 3. 类

### 3.1 结构体 VS 类

仅当只有数据成员时使用<code class="google-code">struct</code>，其它一概用<code class="google-code">class</code>。

### 3.2 类格式

访问控制块的声明依次序是<code class="google-code">public</code>: ，<code class="google-code">protected</code>: ，<code class="google-code">private</code>: ，省略空部分，不加缩进，各访问控制块间要留一空行：

```cpp
class MyClass : public OtherClass {
public:
    MyClass();
    ~MyClass();

    void some_function();

private:
    void private_function();

    int someVar;
}
```

</br>

## 4 函数

### 4.1 引用参数

- 所有按引用传递的参数必须加上<code class="google-code">const</code>。

```cpp
void Foo(const string &in, string *out);
```

约定：输入参数是值参或<code class="google-code">const</code>引用，输出参数为指针。输入参数可以是<code class="google-code">const</code>指针，但不能是非<code class="google-code">const</code>的引用参数。除非特殊要求，如<code class="google-code">swap()</code>;

### 4.2 编写简短函数

- 我们倾向于编写简短、凝练的函数。

长函数有时是合理的，并不硬性限制函数的长度。如果函数超过40行，可以思索下能不能在不影响程序结构的前提下对其进行分割。长函数被修改时容易导致难以发现的bug，不便于阅读和修改。

</br>

## 5. C++特性

### 5.1 变长数组和alloca()

- 不要使用变长数组和<code class="google-code">alloca()</code>。

变长数组和<code class="google-code">alloca()</code>不是标准C++的组成部分。更重要的是，他们根据数据大小动态分配堆栈内存，会引起难以发现的内存越界bug。可以改用更安全的分配器，就像<code class="google-code">std::vector</code>或<code class="google-code">std::unique_ptr<T[]></code>。
### 5.2 类型转换

- 使用C++的类型转换，如<code class="google-code">static_cast<>()</code>，不要使用<code class="google-code">int y = (int)x</code>或<code class="google-code">int y = int(x)</code>等转换方式。

C++的类型转换在查找时更醒目。

### 5.3 自增运算符

- 在不使用返回值情况下，基本类型值使用后置自增（<code class="google-code">i++</code>），迭代器和模板类型使用前置自增（<code class="google-code">++i</code>）。

一方面前置自增要比后置自增效率更高，因为后置自增需要对表达式的值进行一次拷贝。另一方面后置自增更符合自然语言的主语在谓语前面的特点。现代编译器都会对基本类型值的后置自增优化来避免无意义的拷贝，所以对基本类型使用语法更友好的后置自增，对迭代器和模板类型使用前置自增提高效率。

### 5.4 const

- 如果函数不会修改你传入的引用或指针类型参数, 该参数应声明为<code class="google-code">const</code>，以表明变量不可被篡改；
- 提倡<code class="google-code">const</code>写在类型前面。如<code class="google-code">const int x</code>；
- 尽量使用<code class="google-code">const</code>代替宏定义来定义常量；

使用<code class="google-code">const</code>变量、数据成员、函数和参数为编译时类型检测增加了一层保障，便于尽早发现错误。

### 5.5 整型

- C++内建整型中，仅使用<code class="google-code">int</code>。如果程序中需要不同大小的变量，可以使用<code class="google-code"><stdint.h></code>中长度精确的整型，如<code class="google-code">int16_t</code>和<code class="google-code">int64_t</code>。
  
<code class="google-code"><stdint.h></code>定义了<code class="google-code">int16_t</code>、<code class="google-code">uint32_t</code>、<code class="google-code">int64_t</code>等整型，在需要确保整型大小时可以使用他们代替<code class="google-code">short</code>、<code class="google-code">unsigned long long</code>等。在C++整型中，只使用<code class="google-code">int</code>。如果已知整数不会太大，我们通常会使用<code class="google-code">int</code>。如果需要64位整型，用<code class="google-code">int64_t</code>或<code class="google-code">uint64_t</code>。

### 5.6 预处理宏
- 使用宏时要非常谨慎，尽量以内联函数、枚举和常量代替。
- 在马上要使用时才进行<code class="google-code">#define</code>，使用后要立即<code class="google-code">#undef</code>。
- 使用宏定义多语句块时可以使用<code class="google-code">do {} while(0)</code>来保证结构化。

宏意味着你和编译器看到的代码是不同的，这可能会导致异常行为，尤其因为宏具有全局作用域。但在C++中，宏不像在C中那么必不可少。以往用宏展开性能关键的代码，现在可以用内联函数代替。用宏表示常量可被<code class="google-code">const</code>变量代替。

宏可以做一些其他技术无法实现的事情（如用<code class="google-code">#</code>字符串化，用<code class="google-code">##</code>连接等），但在使用前要仔细考虑能不能不使用宏达到同样的目的。

### 5.7 nullptr
- 整数用<code class="google-code">0</code>，实数用<code class="google-code">0.0</code>，指针用<code class="google-code">nullptr</code>而不是<code class="google-code">NULL</code>。

C++中的<code class="google-code">NULL</code>具有二义性，<code class="google-code">nullptr</code>才是纯粹的null pointer。

### 5.8 sizeof

- 尽可能用<code class="google-code">sizeof(varname)</code>代替<code class="google-code">sizeof(type)</code>。

```cpp
Struct data;
memset(&data, 0, sizeof(data));
```

使用<code class="google-code">sizeof(varname)</code>是因为当代码中变量类型改变时会自动更新。

### 5.9 auto

- <code class="google-code">auto</code>只用于局部变量。

<code class="google-code">auto</code>可以避免繁琐的类型名，使用时要注意可读性好，让读者一目了然变量是什么类型。不要用在文件作用域变量、命名空间作用域变量和类数据成员里。永远别列表初始化<code class="google-code">auto</code>变量。

```cpp
// good
sparse_hash_map<string, int>::iterator iter = m.find(val);
auto iter = m.find(val);        // 改用auto避免繁琐的类型名

// bad, 使用auto列表初始化，难以理解是什么类型
auto x(3);
auto y{3};
```

</br>

## 6 格式
### 6.1 行长度

- 每一行代码字符数不能超过120。拆分出的新行要有4个空格的缩进。一些不适合断行的语句（带有命令示例、URL）可以超过120个字符。

### 6.2 对齐与缩进

- 左大括号<code class="google-code">{</code> 位于行尾，右大括号<code class="google-code">}</code> 独占一行，与引用他们的语句左对齐，便于检查配对情况。
- 位于同一层<code class="google-code">{</code> 和<code class="google-code">}</code> 之内的代码在<code class="google-code">{</code> 右边4个空格处左对齐，即同层次代码在同层次的缩进层上。
- 使用4个空格缩进（可将编辑器设为Tab转4个空格）。
- <code class="google-code">if</code>、<code class="google-code">for</code>、<code class="google-code">while</code>、<code class="google-code">do</code>、<code class="google-code">{</code> 和<code class="google-code">}</code> 将其包含在内。

```cpp
while (condition) {
    DoSomething();
}
```

### 6.3 函数调用

- 函数参数与圆括号间不加空格。
- 需要换行时，次行缩进4空格。
- 如果可读性更好，可酌情按其结构来决定参数格式。

```cpp
bool retVal = DoSomething(arg1, arg2, arg3);
bool retVal2 = DoSomething(aVeryVeryVeryVeryLongArgument1, aVeryVeryVeryVeryLongArgument2,
    aVeryVeryVeryVeryLongArgument3);
// 矩阵转换函数
myWidget.Transform(x1, x2, x3,
                   y1, y2, y3,
                   z1, z2, z3);
```

### 6.4 初始化

- 将函数变量尽可能置于最小作用域内，并在定义变量同时进行初始化。有一个例外是，如果变量是一个对象，在循环中每次循环开始都要调用其构造函数，结束时调用析构函数，这会降低效率。

```cpp
// bad
for (int i=0; i<100; i++) {
    Foo f;                  // 构造函数和析构函数都调用100次
    f.DoSomething(i);
}

// good
Foo f;                      // 构造函数和析构函数都只调用一次
for (int i=0; i<100; i++) {
    f.DoSomething(i);
}
```

- 列表初始化格式与函数调用格式相同。如果列表初始化伴随着名字，比如变量或类型名，格式化时将名字视作函数调用名，<code class="google-code">{}</code>视作函数调用的括号。如果没有名字，就视作名字长度为0。

```cpp
return {foo, bar};
FunCall({foo, bar});
pair<int, int> p{foo, bar};
```

### 6.5 条件语句

- 关键字<code class="google-code">if</code>和<code class="google-code">else</code>后加空格，判断条件的右小括号后加空格。
- 执行体是单行语句时不必要加大括号。

```cpp
if (condition) {
    ...
} else if (...) {
    ...
} else {
    ...
}
```

### 6.6 开关选择语句

<code class="google-code">switch</code>语句应该总是包含一个<code class="google-code">default</code>匹配。

```cpp
switch (var) {
    case 0:
        ...
        break;
    case 1:
        ...
        break;
    default:
        ...
        break;
}
```

### 6.7 指针、引用、访问成员表达式
- 句点或箭头前后不要有空格，指针/地址操作符（<code class="google-code">*</code>, <code class="google-code">&</code>）之后不要有空格。

```cpp
x = *p;
p = &x;
x = r.y;
x = r->y;
```

- 声明指针变量时，星号‘<code class="google-code">*</code>’靠近变量名。

```cpp
char *c;
string *name;
```

### 6.8 布尔表达式

布尔表达式超过行宽需要换行时，逻辑操作符（<code class="google-code">&&</code>、<code class="google-code">||</code>）总位于行尾。

```cpp
if (thisOneThing > thisOtherThing &&
    aThirdThing == aFourThing &&
    yetAnother && lastOne) {
    ...
}  
```

### 6.9 函数返回值

- 不要在<code class="google-code">return</code>表达式里加上非必须的圆括号。

```cpp
// good
return val;
// bad
return (val);
```
- <code class="google-code">void</code>函数里使用<code class="google-code">return</code>语句时不在;前加空格。

```cpp
// good
return;
// bac
return ;
```

### 6.10 预处理指令

- 预处理指令不要缩进，从行首开始，即使是位于缩进代码中。

```cpp
    if (condition) {
#if DISASTER_PENDING
        do_something();
# if NOTIFY             // 有宏指令嵌套时可加空格表示
        notify_client();
# endif
#endif
        back_to_normal();
    }
```

### 6.11 空行

添加空行的原则是代码可读性，通常有以下原则：
- 相对独立的代码块（头文件包含、宏定义块、枚举类型定义、结构体类型定义、类定义、函数定义等）之间加空行。
- 两组逻辑上相关性低的代码块中间加空行。
- 函数体首位不要添加空行，无意义且浪费空间。

### 6.12 行内空格

- 关键字后加空格；
- 函数名后不加空格；
- 条件语句与圆括号间不加空格；
- 函数参数与圆括号间不加空格；
- 赋值、算术、关系、逻辑等二元运算符前后各加一空格（<code class="google-code">for</code>循环的<code class="google-code">()</code>中语句为了格式紧凑美观可以不加空格），一元运算符前后一般不加空格；
- <code class="google-code">,</code> <code class="google-code">;</code>符号后加一个空格（位于行尾时不加空格）；
- 不要在行尾加没意义的空格。

```cpp
int i = 0, j = 5, k = 7;
for (; i<10; i++) {
    if (i == j) {
        DoSomething(i, k);
        break;
    }
}
```
