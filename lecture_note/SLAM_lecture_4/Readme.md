# Lecture 4 实践笔记：Sophus 与 SO(3)/SE(3) 李群/李代数

> 本文基于当前工作区中的第四讲示例工程整理，示例代码位于：
> `lecture_files/lecture_4/useSophus/useSophus.cpp`
>
> 配套依赖源目录：
> `lecture_files/lecture_4/useSophus/Sophus-main/`
>
> 工程配置文件：
> `lecture_files/lecture_4/useSophus/CmakeLists.txt`
>
> 本讲以 `Sophus` 库为主线，说明如何用 `SO3d` 与 `SE3d` 表示三维旋转与刚体位姿，并通过 `log()`、`exp()`、`hat()`、`vee()` 完成李群与李代数之间的映射与增量更新。

---

## 一、工程目录与源文件说明

当前第四讲示例工程目录结构如下：

```text
lecture_files/lecture_4/useSophus/
├── CmakeLists.txt
├── useSophus.cpp
└── Sophus-main/
```

其中：

- `useSophus.cpp`：示例程序，演示 `SO3d` 与 `SE3d` 的构造、李代数映射、`hat()` / `vee()` 转换以及位姿增量更新；
- `CmakeLists.txt`：CMake 配置文件，使用 `find_package(Sophus REQUIRED)`；
- `Sophus-main/`：当前本地安装/下载的 Sophus 源代码目录，提供 `sophus/so3.hpp` 与 `sophus/se3.hpp`。

工程配置通常写为：

```cmake
cmake_minimum_required(VERSION 3.10)
project(useSophus)

find_package(Sophus REQUIRED)
include_directories(${Sophus_INCLUDE_DIRS})

add_executable(useSophus useSophus.cpp)
target_link_libraries(useSophus ${Sophus_LIBRARIES})
```

---

## 二、依赖准备与 Sophus 安装

### 2.1 系统依赖

本讲主要依赖：

```bash
sudo apt update
sudo apt install build-essential cmake libeigen3-dev libfmt-dev
```

其中：

- `build-essential`：提供 `g++` 与 `make`；
- `cmake`：构建配置与生成 Makefile；
- `libeigen3-dev`：Eigen 头文件与几何类型支持；
- `libfmt-dev`：安装 Sophus 时的附属依赖。

### 2.2 Sophus 源码获取

当前工作区中已存在 `Sophus-main/`。如果环境中不存在该目录，需要从源码压缩包或 GitHub 镜像仓库下载，解压后放置到示例工程目录中。

### 2.3 安装流程

建议在源码目录执行以下流程：

```bash
cd lecture_files/lecture_4/useSophus/Sophus-main
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

其中：

- `cmake ..`：在源码目录外部构建时配置 Sophus；
- `make`：编译 Sophus 的头文件和示例单元；
- `sudo make install`：安装头文件与 CMake 配置；
- `sudo ldconfig`：刷新动态库缓存。

> 注意：当前安装过程可能出现 `Ceres` 查找警告，但该警告仅说明测试依赖未找到，不影响本讲的 `SO3d` 与 `SE3d` 示例程序的核心功能。

---

## 三、代码主线：`useSophus.cpp`

示例程序 `useSophus.cpp` 的执行逻辑可以提炼为：

1. 用 `AngleAxisd` 构造旋转角轴；
2. 用 `Eigen::Matrix3d`、`Sophus::SO3d::exp()` 和 `Quaterniond` 表示同一个旋转；
3. 用 `SO3d::log()` 还原对应的三维李代数向量；
4. 用 `hat()` / `vee()` 实现向量与反对称矩阵的互换；
5. 用 `SO3d::exp(update_so3) * SO3_R` 完成 SO(3) 增量暴露更新；
6. 用 `SE3d(R, t)` 或 `SE3d(q, t)` 构造 `SE3d` 位姿；
7. 用 `SE3d::log()` 得到 6D 的 `se(3)` 向量；
8. 用 `SE3d::hat()` 与 `SE3d::vee()` 完成 `se(3)` 的矩阵 / 向量转换；
9. 用 `SE3d::exp(update_se3) * SE3_Rt` 完成 SE(3) 位姿增量更新。

示例代码的关键片段如下：

```cpp
Eigen::Matrix3d R = Eigen::AngleAxisd(M_PI / 2,
                                      Eigen::Vector3d(0, 0, 1)).toRotationMatrix();

Sophus::SO3d SO3_R(R);
Sophus::SO3d SO3_v = Sophus::SO3d::exp(Eigen::Vector3d(0, 0, M_PI / 2));
Eigen::Quaterniond q(R);
Sophus::SO3d SO3_q(q);

Eigen::Vector3d so3 = SO3_R.log();
cout << "so3 = " << so3.transpose() << endl;

cout << "so3 hat = \n" << Sophus::SO3d::hat(so3) << endl;
cout << "so3 hat vee = " << Sophus::SO3d::vee(Sophus::SO3d::hat(so3)).transpose() << endl;

Eigen::Vector3d update_so3(1e-4, 0, 0);
Sophus::SO3d SO3_updated = Sophus::SO3d::exp(update_so3) * SO3_R;
```

对应地，`SE3d` 的构造与更新分为：

```cpp
Eigen::Vector3d t(1, 0, 0);
Sophus::SE3d SE3_Rt(R, t);
Sophus::SE3d SE3_qt(q, t);

typedef Eigen::Matrix<double, 6, 1> Vector6d;
Vector6d se3 = SE3_Rt.log();

Vector6d update_se3;
update_se3.setZero();
update_se3(0, 0) = 1e-4;
Sophus::SE3d SE3_updated = Sophus::SE3d::exp(update_se3) * SE3_Rt;
```

---

## 四、关键数学概念与对象关系

### 4.1 SO(3) 与旋转李代数 `so(3)`

`SO(3)` 表示三维旋转群，群元素可表示为旋转矩阵 `R`。对应的李代数 `so(3)` 是三维切空间，常由一个三维向量 `\omega` 表示。

群元素与切空间通过指数映射与对数映射连接：

$$
R = \exp(\hat{\omega}), \qquad \omega \in \mathfrak{so}(3)
$$

$$
\omega = \log(R)
$$

这里 `\hat{\omega}` 表示反对称矩阵，也就是由旋转向量构成的反对称化矩阵。

### 4.2 `hat()` 与 `vee()`

`SO3d::hat()` 与 `SO3d::vee()` 完成三维向量与反对称矩阵之间的互换：

$$
\hat{\omega} \in so(3), \qquad \omega = \vee(\hat{\omega})
$$

对应的显式矩阵形式为：

$$
\hat{\omega} =
\begin{bmatrix}
0 & -\omega_z & \omega_y \\
\omega_z & 0 & -\omega_x \\
-\omega_y & \omega_x & 0
\end{bmatrix}
$$

它表明：

- 向量 `so3` 可以看成旋转扰动在切空间中的局部参数；
- 反对称矩阵则是它在矩阵形式下的表达；
- 两者通过 `hat()` 与 `vee()` 可以互相恢复。

### 4.3 SO(3) 的增量更新模型

示例使用左扰动方式写增量更新：

$$
R_{new} = \exp(\delta \omega) R
$$

代码中对应：

```cpp
Sophus::SO3d SO3_updated = Sophus::SO3d::exp(update_so3) * SO3_R;
```

这里：

- `update_so3` 是一个非常小的旋转增量向量；
- `SO3d::exp(update_so3)` 把该增量映射为一个新的 `SO(3)` 群元素；
- 左乘旧旋转，构成旋转对旧姿态的局部修正。

### 4.4 SE(3) 与 `se(3)`

`SE(3)` 表示三维刚体运动群，包含旋转和平移：

$$
T = \begin{bmatrix}
R & t \\
0 & 1
\end{bmatrix}
$$

其中：

- `R` 是 `SO(3)` 的旋转矩阵；
- `t` 是三维平移向量；
- `T` 即为位姿矩阵的齐次表示。

`SE3d::log()` 返回一个 6D 向量，通常写为：

```text
se3 = [t_x, t_y, t_z, \omega_x, \omega_y, \omega_z]^T
```

其结构明显符合 `Sophus` 的顺序：

- 前 3 个元素表示平移增量；
- 后 3 个元素表示旋转增量。

对应的`SE(3)` 增量更新模型为：

$$
T_{new} = \exp(\delta \xi) T
$$

代码中对应：

```cpp
Sophus::SE3d SE3_updated = Sophus::SE3d::exp(update_se3) * SE3_Rt;
```

这说明 `SE(3)` 的更新同样可以用 `exp()` 映射一个局部李代数增量，再与旧位姿进行左乘组合。

---

## 五、`Eigen` 与 `Sophus` 的映射关系

第四讲的核心不是单纯替换 `Eigen` 表示，而是用 `Sophus` 统一几何对象的群结构表达。

对应关系如下：

```text
AngleAxisd / Quaterniond / Matrix3d -> SO3d
Isometry3d -> SE3d
log / exp / hat / vee -> 李代数与群映射
```

在技术上看：

- `Eigen` 侧重数值几何对象表达，如旋转矩阵、四元数、齐次位姿；
- `Sophus` 侧重抽象群对象表达，如 `SO3d` 与 `SE3d`；
- `log()` 与 `exp()` 提供群元素与切空间之间的分析关系；
- `hat()` 与 `vee()` 提供反对称矩阵与向量之间的互换表达。

因此，本讲更接近“从几何表示层进入李群/李代数层”的过渡内容。

---

## 六、编译与运行

### 6.1 手工编译

```bash
cd lecture_files/lecture_4/useSophus

g++ -std=c++17 -I/usr/include/eigen3 \
    -I/home/cyfer/SLAM_Learning_for_myself/lecture_files/lecture_4/useSophus/Sophus-main \
    useSophus.cpp -o useSophus

./useSophus
```

### 6.2 CMake 编译

```bash
cd lecture_files/lecture_4/useSophus
mkdir -p build
cd build
cmake ..
make
./useSophus
```

---

## 七、常见问题与排查点

### 7.1 `no such file or directory`：找不到 `sophus/so3.hpp`

原因通常有三类：

1. 没有把 Sophus 源码目录加入 `include` 路径；
2. 没有执行 `sudo make install`；
3. `CMakeLists.txt` 中没有正确配置 `find_package`。

具体解决方式如下：

```bash
cd lecture_files/lecture_4/useSophus/Sophus-main
mkdir -p build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

完成上述流程后，头文件会被安装到标准路径下，例如：

```bash
ls /usr/local/include/sophus
ls /usr/include/sophus
```

如果仍然查不到头文件，应当进一步检查：

```bash
which cmake
which make
pkg-config --cflags --libs eigen3
```

并确认工程目录中的 `CmakeLists.txt` 已使用 `find_package(Sophus REQUIRED)`，同时在编译命令或 CMake 配置中加入 `include_directories(${Sophus_INCLUDE_DIRS})` 或相同语义的 `target_include_directories`。

### 7.2 构造接口不兼容

当前 `Sophus-main` 使用模板版头文件，接口与旧版示例代码有差别。编译时应以当前源码目录中的头文件声明为准，避免在旧版 `operator<<`、旧版 `SO3`/`SE3` 构造写法上直接套用。

在本示例中，解决思路是：

```cpp
Sophus::SO3d SO3_R(R);
Sophus::SO3d SO3_v = Sophus::SO3d::exp(Eigen::Vector3d(0, 0, M_PI / 2));
Eigen::Quaterniond q(R);
Sophus::SO3d SO3_q(q);
```

其中，传入的对象统一按当前 `Sophus-main` 的模板接口规范来构建，避免使用旧版本中可能存在的不同构造语义。

### 7.3 输出格式控制

如果需要控制输出小数位数，可使用：

```cpp
cout.precision(3);
```

它与第三讲中对 Eigen 流输出格式的说明一致，但本讲主要关注李代数、李群和位姿增量的结构化表示。

---

## 八、数学部分补充笔记：李群与李代数：从矩阵指数到SO(3)扰动模型

> 本笔记对应《视觉SLAM十四讲》第四讲内容，重点记录对李群与李代数底层数学结构的理解，包括矩阵指数的定义动机、BCH公式的几何含义，以及SO(3)扰动模型推导中需要形成肌肉记忆的核心步骤。这部分内容是在学习过程中反复推敲过的，记录在此以备后续查阅。

### 8.1 矩阵指数：从微分方程到幂级数定义

### 1. 矩阵指数的定义方式

矩阵指数 $e^{\mathbf{A}}$ 在数学上由幂级数定义：

$$
e^{\mathbf{A}} = \sum_{n=0}^{\infty} \frac{1}{n!} \mathbf{A}^n = \mathbf{I} + \mathbf{A} + \frac{1}{2!}\mathbf{A}^2 + \frac{1}{3!}\mathbf{A}^3 + \cdots
$$

这并非近似，而是精确的等式。该定义是标量指数 $e^x = \sum x^n/n!$ 在矩阵空间上的自然推广。需要理解的是，这个级数定义并非凭空构造，其动机可以从常系数一阶线性微分方程的解中自然引出。

### 2. 从微分方程看矩阵指数的来源

考虑常系数一阶线性微分方程：

$$
\dot{\mathbf{x}}(t) = \mathbf{A} \mathbf{x}(t), \quad \mathbf{x}(0) = \mathbf{x}_0
$$

其中 $\mathbf{A}$ 为常矩阵。使用逐次逼近法（Picard迭代）求解：将方程改写为积分形式 $\mathbf{x}(t) = \mathbf{x}_0 + \int_0^t \mathbf{A} \mathbf{x}(\tau) d\tau$，从零次近似 $\mathbf{x}_0(t) = \mathbf{x}_0$ 开始迭代：

矩阵指数 $e^{\mathbf{A}}$ 在数学上由幂级数定义：

$$
e^{\mathbf{A}} = \sum_{n=0}^{\infty} \frac{1}{n!} \mathbf{A}^n = \mathbf{I} + \mathbf{A} + \frac{1}{2!}\mathbf{A}^2 + \frac{1}{3!}\mathbf{A}^3 + \cdots
$$

一次近似：

$$
\mathbf{x}_1(t) = \mathbf{x}_0 + \mathbf{A} \mathbf{x}_0 t
$$

二次近似：

$$
\mathbf{x}_2(t) = \mathbf{x}_0 + \mathbf{A}\mathbf{x}_0 t + \frac{1}{2!}\mathbf{A}^2\mathbf{x}_0 t^2
$$

$n$ 次近似：

$$
\mathbf{x}_n(t) = \left(\mathbf{I} + \mathbf{A}t + \frac{1}{2!}\mathbf{A}^2 t^2 + \cdots + \frac{1}{n!}\mathbf{A}^n t^n\right)\mathbf{x}_0
$$

当 $n \to \infty$ 时，括号内的级数正好是 $\sum (\mathbf{A}t)^n/n!$，数学家将此级数定义为矩阵指数 $e^{\mathbf{A}t}$。于是微分方程的解可简洁地写为 $\mathbf{x}(t) = e^{\mathbf{A}t}\mathbf{x}_0$。

这个推导揭示了矩阵指数的两个关键性质：

- 它天然满足 $\frac{d}{dt}e^{\mathbf{A}t} = \mathbf{A}e^{\mathbf{A}t}$（这是由级数逐项求导直接得到的）
- 当 $\mathbf{A}$ 退化为标量时，完全兼容经典的指数函数

### 3. 在SLAM语境下：为什么可以截断？

在SLAM的扰动模型中，我们关心的是 $\exp(\delta\boldsymbol{\phi}^{\wedge})$ 在 $\delta\boldsymbol{\phi} = 0$ 附近的展开。将 $\mathbf{A} = \delta\boldsymbol{\phi}^{\wedge}$ 代入级数定义：

$$
\exp(\delta\boldsymbol{\phi}^{\wedge}) = \mathbf{I} + \delta\boldsymbol{\phi}^{\wedge} + \frac{1}{2!}(\delta\boldsymbol{\phi}^{\wedge})^2 + \cdots
$$

由于 $\delta\boldsymbol{\phi}$ 是无穷小量，从二次项开始均为高阶无穷小。在计算导数（线性化）时，这些高阶项不贡献结果，可以直接舍去，即：

$$
\exp(\delta\boldsymbol{\phi}^{\wedge}) \approx \mathbf{I} + \delta\boldsymbol{\phi}^{\wedge}
$$

此处需注意：截断仅在求导线性化的语境下成立。对于非无穷小的旋转，必须保留完整的级数（其闭合形式即为罗德里格斯公式）。

---

### 8.2 矩阵指数乘法的非交换性：BCH公式

### 1. 为何 $e^{\mathbf{A}}e^{\mathbf{B}} \neq e^{\mathbf{A}+\mathbf{B}}$？

根本原因在于矩阵乘法不满足交换律（$\mathbf{A}\mathbf{B} \neq \mathbf{B}\mathbf{A}$）。将两边展开至二阶项即可直观看出差异：

$$
e^{\mathbf{A}}e^{\mathbf{B}} = \left(\mathbf{I} + \mathbf{A} + \frac{1}{2}\mathbf{A}^2 + \cdots\right)\left(\mathbf{I} + \mathbf{B} + \frac{1}{2}\mathbf{B}^2 + \cdots\right)
$$

其一阶项为 $\mathbf{A} + \mathbf{B}$，二阶交叉项为 $\mathbf{A}\mathbf{B}$。

另一方面：

$$
e^{\mathbf{A}+\mathbf{B}} = \mathbf{I} + (\mathbf{A}+\mathbf{B}) + \frac{1}{2}(\mathbf{A}+\mathbf{B})^2 + \cdots
$$

其二阶交叉项为 $\frac{1}{2}(\mathbf{A}\mathbf{B} + \mathbf{B}\mathbf{A})$。

二者之差为 $\frac{1}{2}(\mathbf{A}\mathbf{B} - \mathbf{B}\mathbf{A}) = \frac{1}{2}[\mathbf{A}, \mathbf{B}]$，其中 $[\mathbf{A}, \mathbf{B}] = \mathbf{A}\mathbf{B} - \mathbf{B}\mathbf{A}$ 即为李括号。 当且仅当 $\mathbf{A}$ 与 $\mathbf{B}$ 可交换时，该差值为零，标量的指数乘法法则才成立。

### 2. BCH公式及其几何含义

两个矩阵指数的精确乘积由Baker-Campbell-Hausdorff（BCH）公式给出：

$$
e^{\mathbf{A}}e^{\mathbf{B}} = \exp\left(\mathbf{A} + \mathbf{B} + \frac{1}{2}[\mathbf{A}, \mathbf{B}] + \frac{1}{12}[\mathbf{A}, [\mathbf{A}, \mathbf{B}]] - \frac{1}{12}[\mathbf{B}, [\mathbf{A}, \mathbf{B}]] + \cdots\right)
$$

该公式表明，李群上的乘法对应到李代数上并非简单的向量加法，而是由一串无穷级数修正的运算。这正是旋转矩阵没有加法运算的数学根源——两个旋转矩阵的和不一定是旋转矩阵，因此两个旋转向量（李代数）也不能直接相加后取指数来合成旋转。

### 3. 在SLAM中的实际意义：微小扰动下的线性化

在后端优化中，每次迭代的更新量 $\Delta\boldsymbol{\phi}$ 是微小的。将BCH公式应用于左乘扰动模型：

$$
\exp(\boldsymbol{\phi}^{\wedge}) \exp(\Delta\boldsymbol{\phi}^{\wedge}) = \exp\left((\boldsymbol{\phi} + \Delta\boldsymbol{\phi})^{\wedge} + \frac{1}{2}[\boldsymbol{\phi}, \Delta\boldsymbol{\phi}] + \cdots\right)
$$

由于 $\Delta\boldsymbol{\phi}$ 为一阶小量，李括号项 $[\boldsymbol{\phi}, \Delta\boldsymbol{\phi}]$ 为二阶小量，在求导线性化时可忽略。因此有：

$$
\exp(\boldsymbol{\phi}^{\wedge}) \exp(\Delta\boldsymbol{\phi}^{\wedge}) \approx \exp((\boldsymbol{\phi} + \Delta\boldsymbol{\phi})^{\wedge})
$$

这正是SLAM优化中使用李代数加法近似李群乘法的数学基础：微小的旋转更新在局部近似为向量空间的线性叠加。

---

### 8.3 SO(3)扰动模型：需要形成肌肉记忆的三行推导

在SLAM后端优化的雅可比矩阵推导中，SO(3)左乘扰动模型是最核心的求导操作。以下三行推导应做到脱离书本即可默写：

### 第1行——明确求导目标（标量函数对向量的导数）

$$
\frac{\partial (\mathbf{R}\mathbf{p})}{\partial \delta\boldsymbol{\phi}}
$$

### 第2行——代入扰动并一阶线性化

$$
\frac{\partial (\mathbf{R} \cdot \exp(\delta\boldsymbol{\phi}^{\wedge}) \cdot \mathbf{p})}{\partial \delta\boldsymbol{\phi}}
\approx \frac{\partial (\mathbf{R} \cdot (\mathbf{I} + \delta\boldsymbol{\phi}^{\wedge}) \cdot \mathbf{p})}{\partial \delta\boldsymbol{\phi}}
= \frac{\partial (\mathbf{R}\mathbf{p} + \mathbf{R} \cdot \delta\boldsymbol{\phi}^{\wedge} \cdot \mathbf{p})}{\partial \delta\boldsymbol{\phi}}
$$

### 第3行——利用反对称矩阵的互换性质化简

关键恒等式：$\mathbf{a}^{\wedge} \cdot \mathbf{b} = -\mathbf{b}^{\wedge} \cdot \mathbf{a}$（叉积交换顺序引入负号）。

据此有：

$$
\mathbf{R} \cdot \delta\boldsymbol{\phi}^{\wedge} \cdot \mathbf{p} = -\mathbf{R} \cdot \mathbf{p}^{\wedge} \cdot \delta\boldsymbol{\phi}
$$

代入后得到雅可比矩阵：

$$
\frac{\partial (\mathbf{R}\mathbf{p})}{\partial \delta\boldsymbol{\phi}} = -\mathbf{R} \cdot \mathbf{p}^{\wedge}
$$

此处的负号和反对称矩阵的形式直接影响后端优化中雅可比矩阵的书写——符号错误将导致优化发散。因此，这组推导需要形成肌肉记忆。

> 关于SE(3)的完整雅可比：SE(3)的扰动雅可比为 $6 \times 6$ 矩阵，涉及左雅可比 $\mathbf{J}_l$ 的完整表达式，其推导包含大量级数展开运算，实际工程中由Sophus或g2o等库自动计算。只需了解其结构（左上块为 $-\mathbf{R}\mathbf{p}^{\wedge}$，右上块为 $\mathbf{R}$），无需记忆完整推导过程。

---

## 九、总结

第四讲的核心内容是将 `Eigen` 的旋转矩阵、四元数、齐次变换表示进一步抽象为 `Sophus` 中的 `SO3d` 与 `SE3d`。在这个过程中，`SO(3)` 与 `SE(3)` 不再仅仅作为矩阵对象，而是作为带有群结构的位姿对象，配合 `exp()`、`log()`、`hat()`、`vee()` 完成从群元素到切空间的映射。

数学表达上可概括为：

```text
Eigen 表示：旋转矩阵 / 四元数 / 欧氏变换
    ↓
Sophus 表示：SO(3) / SE(3) 李群对象
    ↓
李代数表示：so(3) / se(3) 向量
    ↓
指数 / 对数映射：exp() 与 log()
    ↓
位姿增量更新与局部扰动建模
```

本讲对应的工程路径与源码结构，为后续 `SLAM` 中的位姿估计、误差建模、线性化与优化实现奠定了基础。
