# Lecture 1~2 实践笔记：WSL2/Ubuntu C++/CMake SLAM 入门操作与常见问题

> 内容依据《视觉SLAM十四讲》第2讲，覆盖终端和 VS Code 两种使用方式。  
> 当前实际环境：WSL2 + Ubuntu 26.04 LTS，g++ 15.2.0、CMake 4.2.3；WSL 可见硬件资源为 8 核 16 线程、约 7.4 GiB 内存和 2 GiB Swap。工程位于 `/home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2`。
> 当前工程已经包含单文件程序、静态库和两个 CMake 可执行目标；本文中的命令和文件名均按这个实际工程整理。构建产物会随是否执行过 CMake 而变化，以下目录树会明确区分源码、手动编译产物和 CMake 产物。  
> 建议使用外部构建（out-of-source build），将 `build` 目录与源码目录分离，避免在源码目录直接执行 `cmake .`。

## 先看当前实际状态

当前打开的讲义文件与 C++ 工程不在同一个目录：

| 内容 | 实际路径 | 用途 |
| --- | --- | --- |
| 本讲义 | `/home/cyfer/SLAM_Learning_for_myself/lecture_note/SLAM_lecture_1~2/Readme.md` | 学习记录 |
| C++ 工程源码根目录 | `/home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2` | CMake 的源码目录 |
| CMake 构建目录 | `/home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2/build` | 编译缓存、目标文件、库和程序 |

实际工程文件如下：

```text
/home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2/
├── CMakeLists.txt       # 定义 1 个静态库和 2 个可执行目标
├── main.cpp             # 独立的 Hello SLAM 程序
├── hello.h              # printHello() 函数声明
├── libhello.cpp         # printHello() 函数实现
├── useHello.cpp         # 调用静态库的程序
└── build/               # 外部构建目录；内容由 CMake 配置和构建后生成
    ├── CMakeFiles/      # CMake 内部文件
    └── Debug/           # 当前目录中保留的构建变体目录
```

这里要特别区分三个位置：`main.cpp` 是源文件；当前源码根目录中的 `main` 是手动执行 g++ 后留下的可执行文件；CMake 配置成功并执行构建后，才会在构建目录中生成 `helloslam`、`useHello` 和 `libhello.a`。当前 `build` 目录尚未保留这些顶层产物，因此不能把它们写成“当前已经存在”的文件。CMake 不会使用源码根目录中的手动编译产物，也不会自动删除它。

可以用下面的命令重新确认状态：

```bash
cd /home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2
find . -maxdepth 1 -type f -printf '%f\n' | sort
find build -maxdepth 1 -type f -printf '%f\n' | sort
```

### 当前实际系统与硬件资源

以下信息来自当前 WSL2 实例，表示 Linux/WSL 能够看到的资源，不一定等于 Windows 宿主机的完整硬件配置：

| 项目 | 当前值 | 说明 |
| --- | --- | --- |
| 系统 | Ubuntu 26.04 LTS | 运行在 WSL2 中 |
| WSL 内核 | `6.18.33.2-microsoft-standard-WSL2` | WSL2 虚拟化内核 |
| CPU | AMD Ryzen 7 8845HS w/ Radeon 780M Graphics | WSL 可见 8 核 16 线程 |
| 内存 | 总计约 7.4 GiB | 当前 WSL 实例可用内存，可能受 WSL 配置限制 |
| Swap | 2.0 GiB | WSL 交换空间 |
| 根文件系统 | 约 1007 GiB | 当前 `/` 所在虚拟磁盘容量 |
| 图形环境 | WSLg 可用 | `DISPLAY=:0`、`WAYLAND_DISPLAY=wayland-0` |
| GPU | Radeon 780M 集成显卡 | 当前 `nvidia-smi` 不可用，不应按 NVIDIA CUDA 环境配置 |

硬件和资源信息可以用下面的命令重新确认：

```bash
# 查看系统和 WSL 内核
cat /etc/os-release
uname -a

# 查看 CPU 核心、线程和型号
lscpu | grep -E 'Model name|CPU\(s\)|Thread|Core|Socket'

# 查看内存和 Swap
free -h

# 查看根文件系统空间
df -h /

# 查看 NVIDIA GPU；没有 NVIDIA 环境时可能提示命令不存在
nvidia-smi
```

由于当前 WSL 只分配约 7.4 GiB 内存，编译时建议使用 `make -j4` 或 `cmake --build build --parallel 4`，不建议盲目使用 `-j16`；并行任务过多会同时占用更多内存，可能导致系统使用 Swap 或编译变慢。磁盘空间充足，但 `build` 目录仍应定期清理，避免缓存和中间文件长期堆积。

---

## 一、环境安装与准备

### 1.1 安装基础编译工具链

先更新软件源索引，再安装所需工具，避免出现软件包找不到或版本过期的问题。

```bash
# 刷新软件源缓存，通常需要先执行，否则可能出现软件包无法定位或版本过期
sudo apt update
# 安装 C++ 编译器、构建工具、CMake、vim 编辑器、gdb 调试器
sudo apt install g++ make cmake vim gdb
```

### 1.2 安装 VS Code 及相关扩展

后续开发可以使用 VS Code，需要安装以下扩展：

1. **C/C++ (Microsoft)**：提供 C++ 语法高亮、代码跳转、智能补全和 gdb 调试支持。  
2. **CMake Tools**：用于 CMake 工程的配置、编译、运行和调试，自动管理构建目录。  
3. **Chinese (Simplified) Language Pack**（可选）：中文界面。

---

## 二、步骤 1：使用 g++ 编译单个源文件

对应书本 2.4.2 节 Hello SLAM，在终端中快速验证编译流程。

### 2.1 对应的实际文件

本步骤只验证 `main.cpp`，不会使用 `hello.h`、`libhello.cpp` 或 `useHello.cpp`。这些文件属于后面的静态库工程。

实际文件：`/home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2/main.cpp`

```text
/home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2/
└── main.cpp
```

#### main.cpp

```cpp
#include <iostream>

int main()
{
    std::cout << "Hello SLAM" << '\n';
    return 0;
}
```

### 2.2 终端操作步骤

```bash
# 1. 进入工作目录
cd /home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2

# 2. 确认实际源码存在（本工程中 main.cpp 已经存在）
ls -l main.cpp

# 3. 查看或编辑源码
vim main.cpp
# --- vim 内操作 ---
# 按 i 进入插入模式，写入上述代码
# 按 Esc 退回普通模式
# 输入 :wq  保存并退出
# ------------------

# 4. 不指定输出文件名，默认生成 a.out
g++ -std=c++17 -Wall -Wextra main.cpp

# 5. 运行默认生成的程序
./a.out

# 6. 指定输出文件名，生成 main
g++ -std=c++17 -Wall -Wextra main.cpp -o main

# 7. 运行指定名称的程序
./main

# 8. 查看当前目录中的手动编译产物
ls -l a.out main

# 9. a.out 可以删除；main 可以保留用于后面对照
rm a.out
```

**输出结果**：`./a.out` 和 `./main` 都会显示 `Hello SLAM`。`a.out` 是默认名称，`main` 是通过 `-o main` 指定的名称。它们都是手动编译产物，不属于 CMake 生成结果；`a.out` 通常可以删除，`main` 可以暂时保留用于对照。

> 说明：不写 `-o` 时，g++ 默认生成 `a.out`；写成 `-o main` 时生成 `main`。Linux 运行当前目录中的程序需要加 `./`。如果在 `build` 目录中编译，应使用 `g++ ../main.cpp -o main`，生成的文件会位于当前 `build` 目录；它仍然是手动编译产物，不属于 CMake 目标。

如果你是在 `build` 目录中完成第一步，流程对应如下：

```bash
cd /home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2/build
g++ ../main.cpp -o main
./main
```

这个 `main` 可以和后面 CMake 生成的 `helloslam` 同时存在，二者都来自 `main.cpp`，但生成方式不同。

### 2.3 vim 基本操作

| 模式 | 进入方式 | 功能 | 常用命令 |
| --- | --- | --- | --- |
| 普通模式 | 打开 vim 默认 / Esc | 执行命令、移动光标，不能直接输入文字 | `:w` 保存   `:q` 退出   `:wq` 保存退出   `:q!` 不保存强制退出 |
| 插入模式 | 普通模式按 `i` | 编辑代码、输入文字 | Esc 退回普通模式 |
| 命令模式 | 普通模式按 `:` | 执行保存、退出等指令 | 回车执行命令 |

> vim 使用中的常见情况：普通模式误按 `/` 会进入搜索，未输入关键词时可能提示 `E348: 光标处没有字符串`，按两次 `Esc` 即可恢复。

### 2.4 常见问题

#### 问题 1：终端提示 `找不到命令 “vim”`

- **现象**：输入 vim 后报错，提示可通过 `sudo apt install vim` 安装。  
- **原因**：Ubuntu minimal 版本默认只带精简版 vim-tiny，没有完整 vim。  
- **解决**：先 `sudo apt update`，再 `sudo apt install vim`。

#### 问题 2：`undefined reference to std::cout` 链接错误

- **现象**：使用 gcc 编译 cpp 文件时，报标准库函数未定义。  
- **原因**：gcc 是 C 语言编译器，编译 .cpp 时不会自动链接 C++ 标准库 `libstdc++`。  
- **正确做法**：C++ 代码应使用 `g++` 编译。  

```bash
# 不推荐
gcc main.cpp
# 推荐
g++ main.cpp -o main
```

#### 问题 3：修改代码后编译仍报旧错误

- **现象**：编辑器里改了代码，保存后编译还是旧报错。  
- **原因**：编辑器修改不一定已写入磁盘，文件可能没有真正保存。  
- **验证方法**：`cat main.cpp` 查看磁盘上的真实文件内容，核对是否为最新代码。  
- **解决**：vim 必须执行 `:w` 写盘；gedit 等图形编辑器需确认标签页无未保存圆点。

#### 问题 4：运行时提示 `bash: ./a.out: 没有那个文件或目录`

**两种原因**：

- 编译失败：存在语法或链接错误，没有生成可执行文件（只要终端有红色报错，就不会产出程序）。  
- 文件名不匹配：没有使用 `-o` 时生成的是 `a.out`，却执行了其他文件名；或者已经执行 `rm a.out` 删除了它。
- **判断编译成功**：终端无任何输出、无红色报错，即为编译成功。

#### 问题 5：`‘cout’ was not declared in this scope`

- **现象**：编译报错，cout 未声明。  
- **原因**：cout、endl 属于 C++ 标准库 `std` 命名空间，未加前缀也未引入命名空间。  
- **两种处理方式**：
  - 方案 A（工程中更常用，减少命名冲突）：加 `std::` 前缀

    ```cpp
    std::cout << "Hello SLAM" << std::endl;
    ```

  - 方案 B（小型练习可用）：文件头引入全局命名空间  
  
    ```cpp
    #include <iostream>
    using namespace std;
    ```

> 补充：从性能角度，优先使用 `'\n'` 替代 `std::endl`；endl 会强制刷新 IO 缓冲区，在循环中频繁使用可能影响性能。

---

## 三、步骤 2：CMake 最小工程与外部构建

对应书本 2.4.3 节，SLAM 项目通常使用 CMake 管理工程，建议采用 build 外部构建。

### 3.1 当前 CMake 工程的目录结构

```text
/home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2/
├── CMakeLists.txt    # 当前工程的完整构建脚本
├── main.cpp          # 生成 helloslam
├── hello.h           # 静态库接口
├── libhello.cpp      # 静态库实现
└── useHello.cpp      # 调用静态库
build/                # 外部构建目录，构建产物放在这里
```

### 3.2 当前实际的 CMakeLists.txt

当前文件已经是静态库工程的完整配置，不是只有 `main` 一个目标：

```cmake
# 指定 CMake 最低版本
cmake_minimum_required(VERSION 3.10)
# 声明工程名称
project(helloSLAM)
add_executable(helloslam main.cpp)
add_library(hello libhello.cpp)
add_executable(useHello useHello.cpp)
target_link_libraries(useHello hello)
set(CMAKE_BUILD_TYPE "Debug")
```

当前 `CMakeLists.txt` 没有显式设置 `CMAKE_CXX_STANDARD`，因此 CMake 不会主动加入 `-std=c++17`；示例代码使用的语法在当前 g++ 默认配置下可以编译。如果希望工程固定使用某个标准，应在 `project()` 后明确写入：

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

文件与目标的对应关系：

| CMake 配置 | 生成结果 | 作用 |
| --- | --- | --- |
| `add_executable(helloslam main.cpp)` | `build/helloslam` | 直接运行 `main.cpp` |
| `add_library(hello libhello.cpp)` | `build/libhello.a` | 编译 `printHello()` 的静态库 |
| `add_executable(useHello useHello.cpp)` + `target_link_libraries` | `build/useHello` | 调用静态库中的 `printHello()` |

因此，当前工程中运行 CMake 生成的 Hello SLAM 程序应使用 `./helloslam`，不是 `./main`。

### 3.3 终端操作步骤

```bash
# 1. 进入源码根目录
cd /home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2

# 2. 创建独立 build 目录（已存在时不会报错）
mkdir -p build

# 3. 进入 build 目录
cd build

# 4. cmake 读取上一级目录的 CMakeLists.txt，生成构建文件
# .. 代表上一级目录（源码目录）
cmake ..

# 5. 执行编译
make

# 6. 确认并运行当前 CMake 目标
ls -l helloslam useHello libhello.a
./helloslam
./useHello
```

实际输出：

```text
Hello SLAM
Hello SLAM static library!
```

### 3.4 清理工程

可以直接删除 build 文件夹，源码目录不受影响：

```bash
rm -rf build
```

### 3.5 常见问题

#### 问题 1：CMake 报错 `No SOURCES given to target`

- **原因**：`add_executable()` 只写了程序名，缺少源代码文件参数。  
- **错误示例**：`add_executable(main)`
- **正确写法**：`add_executable(helloslam main.cpp)`

#### 问题 2：`Cannot find source file: mian.cpp`

- **原因**：文件名拼写错误，CMake 对文件名大小写、拼写敏感。  
- **解决**：核对 CMakeLists.txt 里的文件名与实际文件名完全一致。

#### 问题 3：修改 CMakeLists.txt 后旧错误仍出现

- **现象**：已修正语法错误，重新 cmake 还是报之前的错。  
- **原因**：CMake 缓存机制，生成的 `CMakeCache.txt`、`CMakeFiles` 会保留旧配置，修改后不会自动刷新。  
- **两种处理方式**：
  - 源码内构建时（仅临时调试用）：手动删除缓存  

  ```bash
  rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makefile
  cmake .
  ```

  - 外部构建（更规范）：直接删除整个 build 文件夹重建  

    ```bash
    rm -rf build
    mkdir build
    cd build
    cmake ..
    make
    ```

#### 问题 4：CMake Deprecation Warning 版本警告

- **现象**：黄色警告，提示低版本兼容性将被移除。  
- **原因**：`cmake_minimum_required` 版本号设置过低。  
- **说明**：警告不阻断编译，程序可以正常生成；建议将版本提高到 3.10 或更高。

#### 问题 5：路径写法错误

- **绝对路径**：以 `/` 开头，从根目录开始，例如 `cd /home/xxx/桌面/code`。  
- **相对路径**：不以 `/` 开头，基于当前目录，例如 `cd build`。  
- **常见错误**：`cd home/xxx/桌面/code` 漏掉开头的 `/`，导致路径找不到。

#### 问题 6：直接输入 `..` 回车报错

- **现象**：`..：未找到命令`  
- **原因**：`..` 是路径参数，不是独立命令，需要搭配 `cd` 使用。  
- **正确用法**：`cd ..` 返回上一级目录。

---

## 四、步骤 3：CMake 静态库工程

对应书本 2.4.4 节，SLAM 中常用第三方库（Eigen、OpenCV 等）都涉及静态库的编译、链接和调用。

### 4.1 目录结构

```text
/home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2/          # 项目源码根目录，VS Code 可打开此文件夹
├─ CMakeLists.txt        # CMake 构建脚本
├─ main.cpp              # 测试程序 1
├─ useHello.cpp          # 测试程序 2：调用静态库
├─ hello.h               # 头文件：函数声明
└─ libhello.cpp          # 库源码：函数实现
build/                   # 编译输出目录，全部自动生成，不放任何源码
```

> 说明：
>
> 1. `.h`、`.cpp`、`CMakeLists.txt` 通常放在源码根目录。  
> 2. 不建议在 build 文件夹里新建或编辑源码文件。  
> 3. 如果从空目录开始，先创建下面四个源码文件；当前工程中它们已经存在，可以直接检查内容。  
> 4. build 目录可随时删除重建，不会丢失源码。

### 4.2 源码文件

#### 1. hello.h（头文件，函数声明）

```cpp
#ifndef HELLO_H_
#define HELLO_H_

// 函数声明：告诉编译器存在该函数，实现在 libhello.cpp 中
void printHello();

#endif
```

> 作用：防止头文件被重复包含导致重定义；也可使用 `#pragma once`，效果类似。

#### 2. libhello.cpp（库实现文件，不含 main 函数）

```cpp
#include <iostream>
#include "hello.h"

void printHello()
{
    std::cout << "Hello SLAM static library!" << '\n';
}
```

#### 3. useHello.cpp（调用库的可执行程序）

```cpp
#include "hello.h"

int main(int argc, char** argv)
{
    printHello();
    return 0;
}
```

#### 4. CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(helloSLAM)

# 可执行目标 1：直接编译 main.cpp
add_executable(helloslam main.cpp)

# 静态库目标：生成 libhello.a
add_library(hello libhello.cpp)

# 可执行目标 2：编译 useHello.cpp
add_executable(useHello useHello.cpp)

# 将静态库 hello 链接到 useHello 可执行程序
target_link_libraries(useHello hello)

# 设置 Debug 编译模式，开启调试信息（断点调试需要）
set(CMAKE_BUILD_TYPE "Debug")
```

### 4.3 从文件到产物的完整操作

如果这些文件还不存在，可以先创建空文件，再分别写入上面的内容：

```bash
cd /home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2
touch hello.h libhello.cpp useHello.cpp CMakeLists.txt
ls -l hello.h libhello.cpp useHello.cpp CMakeLists.txt
```

`touch` 只负责创建文件，不会自动写入代码；必须用 VS Code、vim 等编辑器保存内容。写完后可用下面的命令确认磁盘中的实际内容：

```bash
cat hello.h
cat libhello.cpp
cat useHello.cpp
cat CMakeLists.txt
```

### 4.4 标准工作流

```bash
# 1. 进入项目源码根目录
cd /home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2

# 2. 创建并进入 build 目录
mkdir -p build
cd build

# 3. cmake 配置，读取上一级 CMakeLists.txt
cmake ..

# 4. 编译
make

# 5. 查看 CMake 生成的产物
ls -l helloslam useHello libhello.a

# 6. 运行生成的程序
./helloslam
./useHello
```

编译完成后，`build` 目录会生成 `helloslam`、`useHello` 和 `libhello.a`。其中 `CMakeFiles/`、`Makefile`、`CMakeCache.txt` 等是 CMake 的内部构建文件，不要手动修改；`build` 整体可以在需要时删除重建。

### 4.5 修改配置后的重新构建与清理

修改 `CMakeLists.txt` 或新增源文件后，通常先重新配置并构建，不必每次删除缓存：

```bash
cd /home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2/build
cmake ..
make
```

只有缓存异常、切换编译器或需要彻底重来时，才清空 `build`：

```bash
cd /home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2
rm -rf build
mkdir build
cd build
cmake ..
make
```

删除 `build` 只会删除 CMake 缓存和构建产物，不会影响源码目录中的五个源文件。源码根目录中的 `main` 是手动编译留下的文件，不在 CMake 的管理范围内；删除 `build` 不会删除它。

### 4.6 链接错误排查

典型报错：`undefined reference to 'printHello()'`

> 编译成功不等于链接成功。编译阶段主要检查语法，链接阶段才会查找函数实体。

**常见原因（按优先级排查）**：

1. 函数只有头文件声明，没有对应的 cpp 实现代码。  
2. cpp 实现文件没有写入 `add_library()`，未参与编译，没有打入静态库。  
3. 写了 `add_library`，但未写 `target_link_libraries`，可执行程序没有链接库。  
4. 函数名拼写不一致：声明、实现、调用三处名称不匹配。

### 4.7 错误类型区分

| 错误阶段 | 典型现象 | 原因分类 |
| --- | --- | --- |
| cmake 配置阶段 | 执行 `cmake ..` 直接红色报错 | CMakeLists 语法错误、执行位置错误、找不到文件 |
| 编译阶段 | `Building CXX xxx.o` 时报错 | C++ 语法错误、头文件找不到、变量未定义 |
| 链接阶段 | `Linking CXX executable` 时报错 | 函数未实现、未链接库、符号不匹配 |

---

## 五、VS Code 配置与调试

VS Code 配合 gdb 可以实现代码编辑、断点调试等功能，适合后续 SLAM 开发。

### 5.1 打开工程

当前 VS Code 工作区根目录是 `/home/cyfer/SLAM_Learning_for_myself`，仓库根目录下的 `.vscode` 包含当前活动文件编译和调试配置。学习本讲时可以直接使用这个工作区；如果希望 CMake Tools 只管理本工程，也可以单独打开 `/home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2`，但这时应重新选择或生成对应的 VS Code 配置。

无论使用哪种方式，都不要把 `build` 目录单独作为工作区打开；源码文件和 `CMakeLists.txt` 位于 `lecture_files/lecture_1~2`。

### 5.2 C/C++ 扩展配置：c_cpp_properties.json

用于配置代码解析、编译器路径、头文件索引，解决头文件红色波浪线、代码跳转失效等问题。

当前仓库实际的 `.vscode/c_cpp_properties.json` 使用工作区范围的头文件搜索路径，并额外加入 Eigen 路径：

```json
{
    "configurations": [
        {
            "name": "Linux",
            "intelliSenseMode": "linux-gcc-x64",
            "compilerPath": "/usr/bin/g++",
            "includePath": [
                "${workspaceFolder}/**",
                "/usr/include/eigen3"
            ],
            "defines": []
        }
    ],
    "version": 4
}
```

当前配置没有使用 `compile_commands.json`。如果希望让 IntelliSense 读取 CMake 的真实编译参数，可以在源码工程目录执行：

```bash
cd /home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```

然后把 `compileCommands` 配置为 `${workspaceFolder}/lecture_files/lecture_1~2/build/compile_commands.json`（当前工作区为仓库根目录）。如果单独打开源码工程，则使用 `${workspaceFolder}/build/compile_commands.json`。

### 5.3 调试配置：launch.json（gdb 断点调试）

用于配置 gdb 调试器，实现断点、单步运行、变量查看。

当前仓库实际的 `launch.json` 使用 `${fileDirname}/${fileBasenameNoExtension}`，即调试当前活动文件对应的可执行文件，而不是固定调试 `build/useHello`：

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) 启动",
            "type": "cppdbg",
            "request": "launch",
            "program": "${fileDirname}/${fileBasenameNoExtension}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${fileDirname}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

**调试操作**：

- 代码行号左侧点击 → 设置断点  
- `F5` 启动调试  
- `F10` 单步跳过（不进入函数内部）  
- `F11` 单步进入（进入函数内部）  
- `Shift+F11` 单步跳出函数  
- `Shift+F5` 停止调试  

> 该调试配置要求当前活动文件旁边已经存在同名可执行文件。例如打开 `main.cpp` 时，先用任务生成 `main`，再启动 gdb；它不会自动构建 CMake 目标 `useHello`。

### 5.4 当前任务配置：tasks.json

仓库根目录的 `.vscode/tasks.json` 当前配置的是“编译活动文件”任务，核心命令相当于：

```bash
/usr/bin/g++ -g 当前文件.cpp \
    -o 当前文件所在目录/当前文件名 \
    -I/usr/include/eigen3 -std=c++11
```

这个任务有几个需要注意的地方：

- `${file}` 表示当前在编辑器中打开的文件。
- `${fileDirname}` 表示当前文件所在目录，`${fileBasenameNoExtension}` 表示去掉扩展名后的文件名。
- 输出文件会生成在源文件旁边，所以它适合单文件练习，不等同于 CMake 的外部构建流程。
- `-std=c++11` 是当前任务显式指定的语言标准；它与 `CMakeLists.txt` 中未固定标准的配置并不相同。
- `-I/usr/include/eigen3` 对本讲 Hello SLAM 工程不是必须的，是为了兼容其他 Eigen 示例的活动文件编译。

因此，使用这个任务编译 `useHello.cpp` 不能生成可运行的完整静态库程序；`useHello.cpp` 依赖 `hello.h` 的声明和 `libhello.cpp` 中的函数实现，应该使用 CMake 构建并链接 `hello` 库。

### 5.5 CMake Tools 扩展使用

安装扩展后，底部状态栏出现 CMake 工具条，常用功能：

- **选择 Kit**：选择系统 GCC/G++ 工具链；当前环境可用 `g++ --version` 确认版本为 15.2.0。  
- **构建变体**：切换 Debug/Release 模式。  
- 🔧 **Configure**：执行 CMake 配置，等价于在 `build` 中执行 `cmake ..`。  
- 🔨 **Build**：执行编译，等价于进入 `build` 后执行 `make`。  
- ▶ **Run**：一键运行选中的可执行程序。  
- 🐞 **Debug**：一键启动调试，无需手写 launch.json。

### 5.6 VS Code 常见问题

1. **头文件有红色波浪线，但编译能通过**  
    - 解决：先确认 VS Code 打开的是 `/home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_1~2` 源码根目录；执行 CMake Tools 的 Configure，或按上面的命令生成 `compile_commands.json`；确认 `c_cpp_properties.json` 配置正确。
2. **断点灰色不生效**  
    - 原因：当前 CMakeLists 设置为 Debug；如果实际使用了其他构建目录或 Release 配置，可能没有调试符号。  
    - 解决：确认当前活动文件对应的可执行文件已经生成，并检查 `launch.json` 中的 `program` 是否仍为 `${fileDirname}/${fileBasenameNoExtension}`；如果要调试 `useHello`，应另行配置固定的 CMake 目标路径。

---

## 六、常见问题排查速查表

| 错误分类 | 报错现象 | 原因 | 解决方案 |
| --- | --- | --- | --- |
| 终端操作 | 找不到 vim 命令 | 系统未安装完整 vim | sudo apt update && sudo apt install vim |
| 终端操作 | 直接输入 `..` 报错 | .. 是路径不是命令 | 搭配 cd 使用：`cd ..` |
| 终端操作 | ./xxx 找不到文件 | 编译失败 / 文件名不匹配 | 检查编译报错；核对输出文件名 |
| CMake 配置 | No SOURCES given to target | `add_executable()` 缺少源文件 | 补全源代码文件名参数 |
| CMake 配置 | Cannot find source file | 文件名拼写错误 | 核对 CMakeLists 与实际文件名 |
| CMake 配置 | 修改后旧错误依旧 | 配置未重新生成或缓存异常 | 先执行 `cmake ..`；仍异常再重建 `build` |
| CMake 配置 | 版本过低警告 | cmake_minimum_required 版本低 | 提高到 3.10 及以上 |
| C++ 编译 | cout 未声明 | 缺少 std 命名空间 | 加 std:: 前缀或 using namespace std |
| 链接错误 | undefined reference to xxx | 未实现/未编译/未链接/拼写错 | 按常见原因逐条排查 |
| VS Code | 头文件红色波浪线 | 工作区或索引未配置 | 打开源码根目录并执行 Configure；需要时生成 `compile_commands.json` |
| VS Code | 断点不生效 | Release 模式无调试信息 | 切换 Debug 模式重新编译 |

---

## 七、要点总结

### 7.1 gcc 与 g++ 的区别

- **gcc**：C 语言编译器，处理 `.c` 文件；编译 .cpp 时不会自动链接 C++ 标准库。  
- **g++**：C++ 编译器，处理 `.cpp`/`.cc` 文件，自动链接 `libstdc++`。  
- 编写 C++ 代码时建议使用 g++。

### 7.2 CMake 使用注意事项

1. 修改 `CMakeLists.txt` 后，先重新执行 `cmake` 配置和构建；只有缓存异常时才清理 `build`。  
2. 优先采用外部构建（build 目录），避免在源码目录执行 `cmake .`。
3. 当前 CMake 目标是 `helloslam`、`hello` 和 `useHello`；手动执行 `g++ ../main.cpp -o main` 生成的 `main` 只属于手动编译流程，可以与 CMake 产物同时存在。

### 7.3 文件保存

编辑器中的修改不一定已写入磁盘。vim 需要 `:w`，图形编辑器需要点击保存。怀疑文件内容不符时，可用 `cat 文件名` 查看磁盘真实内容。

### 7.4 Linux 终端路径规则

- `/xxx`：绝对路径，从根目录开始。  
- 无 `/` 开头：相对路径，基于当前工作目录。  
- `cd ..`：返回上一级目录。

### 7.5 可执行文件运行规则

Linux 下运行当前目录的程序时，需要加 `./` 前缀，`./` 代表当前工作目录。

### 7.6 Warning 与 Error 的区别

- **Error**：红色，程序终止，编译失败，需要修复。  
- **Warning**：黄色，仅提醒，不阻断编译，程序可正常生成。

### 7.7 新建 C++ 项目的常规步骤

```bash
mkdir 项目名
cd 项目名
touch main.cpp CMakeLists.txt
# 编写代码与 CMake 配置
mkdir build
cd build
cmake ..
make
./程序名
```

### 7.8 报错自查顺序（从高到低）

1. C++ 代码是否使用 g++ 编译？是否误用 gcc？  
2. 文件是否已保存到磁盘？可用 cat 查看磁盘真实文件。  
3. CMakeLists 修改后是否清理了 build 缓存？  
4. std::cout / std::endl 命名空间是否处理？  
5. 运行程序是否加了 `./` 前缀？  
6. 文件名拼写是否正确（如 main 不是 mian）？  
7. 路径是否正确，绝对路径开头是否带 `/`？  
8. 链接错误：是否写了 target_link_libraries？  
9. 调试断点失效：是否使用 Debug 模式？

---

## 八、后续可学习内容

可以按以下顺序继续学习：

1. 共享库（SHARED）编译与使用。  
2. `find_package()` 导入第三方库（如 Eigen、OpenCV）。  
3. 库的安装与导出（`install()` 指令）。  
4. 多子目录 CMake 工程结构。  
5. 编译选项、警告等级、C++ 标准配置。
