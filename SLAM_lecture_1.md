# Ubuntu 24.04 C++/CMake SLAM 入门操作与常见问题

> 内容依据《视觉SLAM十四讲》第2讲，覆盖终端和 VS Code 两种使用方式。  
> 环境：Ubuntu 24.04、g++ 13、CMake 3.28、vim、VS Code C/C++ 工具链。  
> 目标：完成 Hello SLAM 单文件编译、CMake 外部构建、静态库工程、VS Code 断点调试，并记录常见报错现象、原因和修复方法。  
> 注意事项：建议使用外部构建（out-of-source build），将 build 目录与源码目录分离，避免在源码目录直接执行 `cmake .`。

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

### 2.1 目录与源码

```text
~/桌面/code/
└── main.cpp
```

#### main.cpp

```cpp
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
    cout << "Hello SLAM" << endl;
    return 0;
}
```

### 2.2 终端操作步骤

```bash
# 1. 进入工作目录
cd ~/桌面/code

# 2. 创建源码文件
touch main.cpp

# 3. 使用 vim 编辑源码
vim main.cpp
# --- vim 内操作 ---
# 按 i 进入插入模式，写入上述代码
# 按 Esc 退回普通模式
# 输入 :wq  保存并退出
# ------------------

# 4. 编译，-o 指定输出可执行文件名
g++ main.cpp -o main

# 5. 运行程序
./main
```

**输出结果**：`Hello SLAM`

> 说明：不写 `-o main` 时，g++ 默认生成名为 `a.out` 的可执行文件，运行命令为 `./a.out`。

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

#### 问题 4：运行时提示 `bash: ./main: 没有那个文件或目录`

- **两种原因**：
  1. 编译失败：存在语法或链接错误，没有生成可执行文件（只要终端有红色报错，就不会产出程序）。  
  2. 文件名不匹配：`g++ main.cpp` 默认生成 `a.out`，却执行 `./main`。  
- **判断编译成功**：终端无任何输出、无红色报错，即为编译成功。

#### 问题 5：`‘cout’ was not declared in this scope`

- **现象**：编译报错，cout 未声明。  
- **原因**：cout、endl 属于 C++ 标准库 `std` 命名空间，未加前缀也未引入命名空间。  
- **两种处理方式**：
  - 方案 A（工程中更常用，减少命名冲突）：加 `std::` 前缀

  ```cpp
  std::cout << "Hello SLAM" << std::endl;
  ```cpp
  - 方案 B（小型练习可用）：文件头引入全局命名空间  
  ```cpp
  #include <iostream>
  using namespace std;
  ```

> 补充：从性能角度，优先使用 `'\n'` 替代 `std::endl`；endl 会强制刷新 IO 缓冲区，在循环中频繁使用可能影响性能。

---

## 三、步骤 2：CMake 最小工程与外部构建

对应书本 2.4.3 节，SLAM 项目通常使用 CMake 管理工程，建议采用 build 外部构建。

### 3.1 目录结构

```text
~/桌面/code/
├─ CMakeLists.txt    # CMake 构建脚本，位于源码根目录
└── main.cpp
build/               # 手动创建，编译产物全部放在这里
```

### 3.2 CMakeLists.txt 最小示例

```cpp
# 指定 CMake 最低版本
cmake_minimum_required(VERSION 3.10)
# 声明工程名称
project(HelloSLAM)
# 生成可执行程序：add_executable(程序名 源文件.cpp)
add_executable(main main.cpp)
```

### 3.3 终端操作步骤

```bash
# 1. 进入源码根目录
cd ~/桌面/code

# 2. 新建独立 build 目录（外部构建）
mkdir build

# 3. 进入 build 目录
cd build

# 4. cmake 读取上一级目录的 CMakeLists.txt，生成构建文件
# .. 代表上一级目录（源码目录）
cmake ..

# 5. 执行编译
make

# 6. 在 build 目录运行程序
./main
```

### 3.4 清理工程

可以直接删除 build 文件夹，源码目录不受影响：

```bash
rm -rf build
```

### 3.5 常见问题

#### 问题 1：CMake 报错 `No SOURCES given to target: main`

- **原因**：`add_executable()` 只写了程序名，缺少源代码文件参数。  
- **错误写法**：`add_executable(main)`  
- **正确写法**：`add_executable(main main.cpp)`

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
  mkdir build && cd build
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
learn-slam-ch2/          # 项目源码根目录，VS Code 可打开此文件夹
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
> 3. build 目录可随时删除重建，不会丢失源代码。

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

```cpp
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

### 4.3 标准工作流

```bash
# 1. 进入项目源码根目录
cd ~/桌面/learn-slam-ch2

# 2. 创建并进入 build 目录
mkdir -p build
cd build

# 3. cmake 配置，读取上一级 CMakeLists.txt
cmake ..

# 4. 编译
make

# 5. 运行生成的程序
./helloslam
./useHello
```

编译完成后，build 目录会生成 `libhello.a` 静态库文件。静态库链接时，会将库代码直接拷贝进可执行程序。

### 4.4 修改配置后的清理操作

修改 CMakeLists.txt 或新增源文件后，建议清空 build 目录重建，避免旧缓存干扰：

```bash
cd build
rm -rf *    # 清空 build 内所有产物
cmake ..
make
```

### 4.5 链接错误排查

典型报错：`undefined reference to 'printHello()'`

> 编译成功不等于链接成功。编译阶段主要检查语法，链接阶段才会查找函数实体。

**常见原因（按优先级排查）**：

1. 函数只有头文件声明，没有对应的 cpp 实现代码。  
2. cpp 实现文件没有写入 `add_library()`，未参与编译，没有打入静态库。  
3. 写了 `add_library`，但未写 `target_link_libraries`，可执行程序没有链接库。  
4. 函数名拼写不一致：声明、实现、调用三处名称不匹配。

### 4.6 错误类型区分

| 错误阶段 | 典型现象 | 原因分类 |
| --- | --- | --- |
| cmake 配置阶段 | 执行 `cmake ..` 直接红色报错 | CMakeLists 语法错误、执行位置错误、找不到文件 |
| 编译阶段 | `Building CXX xxx.o` 时报错 | C++ 语法错误、头文件找不到、变量未定义 |
| 链接阶段 | `Linking CXX executable` 时报错 | 函数未实现、未链接库、符号不匹配 |

---

## 五、VS Code 配置与调试

VS Code 配合 gdb 可以实现代码编辑、断点调试等功能，适合后续 SLAM 开发。

### 5.1 打开工程

文件 → 打开文件夹 → 选择项目源码根目录（例如 `learn-slam-ch2`），不要打开 build 文件夹。

### 5.2 C/C++ 扩展配置：c_cpp_properties.json

用于配置代码解析、编译器路径、头文件索引，解决头文件红色波浪线、代码跳转失效等问题。

按 `Ctrl+Shift+P`，输入 `C/C++: Edit Configurations (JSON)`，自动生成 `.vscode/c_cpp_properties.json`，修改为：

```json
{
    "configurations": [
        {
            "name": "Linux",
            "intelliSenseMode": "linux-gcc-x64",
            "compilerPath": "/usr/bin/g++",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "compileCommands": "${workspaceFolder}/build/compile_commands.json"
        }
    ],
    "version": 4
}
```

> `compile_commands.json` 由 CMake Tools 扩展生成，配置后可更准确地识别头文件路径。

### 5.3 调试配置：launch.json（gdb 断点调试）

用于配置 gdb 调试器，实现断点、单步运行、变量查看。

按 `Ctrl+Shift+D` 打开调试面板，点击“创建 launch.json”，选择 `C++ (GDB/LLDB)`，修改为：

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug useHello",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/useHello",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "启用 gdb 整齐打印",
                    "text": "-enable-pretty-printing on",
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

> 断点失效排查：CMake 需设置 `CMAKE_BUILD_TYPE "Debug"`，Release 模式无调试信息，断点可能变灰无效。

### 5.4 CMake Tools 扩展使用

安装扩展后，底部状态栏出现 CMake 工具条，常用功能：

- **选择 Kit**：指定编译器为 g++ 13。  
- **构建变体**：切换 Debug/Release 模式。  
- 🔧 **Configure**：执行 cmake 配置，等价于 `cmake ..`。  
- 🔨 **Build**：执行编译，等价于 `make`。  
- ▶ **Run**：一键运行选中的可执行程序。  
- 🐞 **Debug**：一键启动调试，无需手写 launch.json。

### 5.5 VS Code 常见问题

1. **头文件有红色波浪线，但编译能通过**  
   - 解决：执行 CMake Tools 的 Configure，生成 `compile_commands.json`；确认 `c_cpp_properties.json` 配置正确。  
2. **断点灰色不生效**  
   - 原因：CMake 是 Release 模式，没有调试符号。  
   - 解决：切换为 Debug 构建变体，重新编译。

---

## 六、常见问题排查速查表

| 错误分类 | 报错现象 | 原因 | 解决方案 |
| --- | --- | --- | --- |
| 终端操作 | 找不到 vim 命令 | 系统未安装完整 vim | sudo apt update && sudo apt install vim |
| 终端操作 | 直接输入 `..` 报错 | .. 是路径不是命令 | 搭配 cd 使用：`cd ..` |
| 终端操作 | ./xxx 找不到文件 | 编译失败 / 文件名不匹配 | 检查编译报错；核对输出文件名 |
| CMake 配置 | No SOURCES given to target | add_executable 缺源文件 | 补全源代码文件名参数 |
| CMake 配置 | Cannot find source file | 文件名拼写错误 | 核对 CMakeLists 与实际文件名 |
| CMake 配置 | 修改后旧错误依旧 | CMake 缓存未清理 | 删除 build 文件夹重建 |
| CMake 配置 | 版本过低警告 | cmake_minimum_required 版本低 | 提高到 3.10 及以上 |
| C++ 编译 | cout 未声明 | 缺少 std 命名空间 | 加 std:: 前缀或 using namespace std |
| 链接错误 | undefined reference to xxx | 未实现/未编译/未链接/拼写错 | 按常见原因逐条排查 |
| VS Code | 头文件红色波浪线 | 索引未配置 | CMake Tools 执行 Configure |
| VS Code | 断点不生效 | Release 模式无调试信息 | 切换 Debug 模式重新编译 |

---

## 七、要点总结

### 7.1 gcc 与 g++ 的区别

- **gcc**：C 语言编译器，处理 `.c` 文件；编译 .cpp 时不会自动链接 C++ 标准库。  
- **g++**：C++ 编译器，处理 `.cpp`/`.cc` 文件，自动链接 `libstdc++`。  
- 编写 C++ 代码时建议使用 g++。

### 7.2 CMake 使用注意事项

1. 修改 `CMakeLists.txt` 后，通常需要清除旧缓存才能生效。  
2. 优先采用外部构建（build 目录），避免在源码目录执行 `cmake .`。

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
