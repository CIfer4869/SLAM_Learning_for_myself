# SLAM 常用函数速查笔记

> 本笔记以功能模块为主线，整理 Eigen 库中与矩阵、几何、位姿相关的常用类型和函数，并预留后续功能模块目录，便于逐步扩展可视化、图像处理、优化等内容。  
> 参考代码：`lecture_files/lecture_3/useEigen/eigenMatrix.cpp`、`lecture_files/lecture_3/useGeometry/eigenGeometry.cpp` 及其详细注释。

---

## 目录

- [SLAM 常用函数速查笔记](#slam-常用函数速查笔记)
  - [目录](#目录)
  - [一、基础部分](#一基础部分)
    - [1.1 矩阵与向量类型](#11-矩阵与向量类型)
      - [1.1.1 固定尺寸与动态尺寸](#111-固定尺寸与动态尺寸)
      - [1.1.2 常用类型别名](#112-常用类型别名)
      - [1.1.3 模板类与命名空间](#113-模板类与命名空间)
      - [1.1.4 程序入口与命令行参数](#114-程序入口与命令行参数)
    - [1.2 初始化与元素访问](#12-初始化与元素访问)
      - [1.2.1 逗号初始化](#121-逗号初始化)
      - [1.2.2 索引访问与分量访问](#122-索引访问与分量访问)
      - [1.2.3 向量构造与分量写法](#123-向量构造与分量写法)
      - [1.2.4 类型转换 cast](#124-类型转换-cast)
      - [1.2.5 cout 与流重载输出](#125-cout-与流重载输出)
    - [1.3 矩阵基本运算](#13-矩阵基本运算)
      - [1.3.1 转置、求和、迹、数乘](#131-转置求和迹数乘)
      - [1.3.2 逆、行列式、伴随矩阵](#132-逆行列式伴随矩阵)
      - [1.3.3 矩阵乘法与维度匹配](#133-矩阵乘法与维度匹配)
    - [1.4 线性代数求解](#14-线性代数求解)
      - [1.4.1 特征值与特征向量](#141-特征值与特征向量)
      - [1.4.2 线性方程组求解](#142-线性方程组求解)
      - [1.4.3 计时方法](#143-计时方法)
      - [1.4.4 输出精度控制](#144-输出精度控制)
  - [二、应用部分](#二应用部分)
    - [2.1 旋转表示](#21-旋转表示)
      - [2.1.1 旋转向量 AngleAxis](#211-旋转向量-angleaxis)
      - [2.1.2 旋转矩阵与欧拉角](#212-旋转矩阵与欧拉角)
      - [2.1.3 四元数 Quaternion](#213-四元数-quaternion)
    - [2.2 刚体变换与位姿](#22-刚体变换与位姿)
      - [2.2.1 欧氏变换 Isometry3d](#221-欧氏变换-isometry3d)
      - [2.2.2 坐标变换](#222-坐标变换)
  - [三、后续功能模块目录（待补充）](#三后续功能模块目录待补充)

---

## 一、基础部分

### 1.1 矩阵与向量类型

#### 1.1.1 固定尺寸与动态尺寸

Eigen 中所有矩阵和向量都基于模板类 `Eigen::Matrix<Scalar, Rows, Cols>`：

```cpp
// 固定尺寸：编译期确定行列，栈上分配，效率高
Eigen::Matrix<float, 2, 3> matrix_23;          // 2行3列 float 矩阵
Eigen::Vector3d v_3d;                          // 3维 double 向量
Eigen::Matrix3d matrix_33;                     // 3x3 double 矩阵

// 动态尺寸：运行时确定行列，堆上分配，更灵活
Eigen::MatrixXd matrix_x;                      // 任意行列 double 矩阵
Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> matrix_dynamic;  // 等价写法
```

- 固定尺寸矩阵在编译期已知大小，适合小规模、性能敏感的场景。
- 动态尺寸矩阵适合处理大小不固定的数据，如图像尺寸、点云数量等。

#### 1.1.2 常用类型别名

Eigen 通过 `typedef` 提供了许多易用的类型别名，底层仍是 `Matrix` 模板：

| 别名 | 等价写法 | 含义 |
| ----- | -------- | ---- |
| `Vector3d` | `Matrix<double, 3, 1>` | 三维双精度列向量 |
| `Vector3f` | `Matrix<float, 3, 1>` | 三维单精度列向量 |
| `Matrix3d` | `Matrix<double, 3, 3>` | 3×3 双精度矩阵 |
| `Matrix3f` | `Matrix<float, 3, 3>` | 3×3 单精度矩阵 |
| `MatrixXd` | `Matrix<double, Dynamic, Dynamic>` | 动态双精度矩阵 |
| `VectorXd` | `Matrix<double, Dynamic, 1>` | 动态双精度列向量 |

使用别名可以简化代码，提高可读性。

#### 1.1.3 模板类与命名空间

- `Eigen::` 是命名空间，所有 Eigen 类型和函数都在其中。
- `::` 是作用域解析符，用于访问命名空间、类或结构体中的成员。
- `Matrix<float, 2, 3>` 是一个模板类实例化，`float` 是元素类型，`2` 和 `3` 是行列数。

示例：

```cpp
Eigen::Matrix<float, 2, 3> matrix_23;
// 解读：从 Eigen 命名空间中取出 Matrix 模板类，用 float、2、3 实例化，创建对象 matrix_23
```

在真实源代码注释里，Eigen 的基础使用方式通常进一步写成：

```cpp
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
using namespace Eigen;
```

意思是：

- `Eigen/Core` 提供基本矩阵与向量类型；
- `Eigen/Dense` 提供稠密矩阵的常用运算；
- `Eigen/Geometry` 提供旋转、位姿、四元数等几何表达；
- `using namespace Eigen`让后续可直接写 `Matrix3d`、`Vector3d`、`Quaterniond` 等类型而不重复写 `Eigen::`。

#### 1.1.4 程序入口与命令行参数

真实代码文件中还提供了最基本的 C++ 程序入口写法：

```cpp
int main(int argc, char** argv)
```

这里：

- `argc`：命令行参数个数，类型为整数；
- `argv`：命令行参数字符串数组，等价于 `char* argv[]`；

这说明在写 Eigen 示例时，入口函数应该理解成“首先处理命令行输入，再构建矩阵对象或者几何对象”。

### 1.2 初始化与元素访问

#### 1.2.1 逗号初始化

Eigen 提供了一种特殊的初始化语法，可以按行填充矩阵或向量：

```cpp
matrix_23 << 1, 2, 3,
             4, 5, 6;        // 2x3 矩阵按行填充

v_3d << 3, 2, 1;             // 向量填充
```

- 逗号初始化从左到右、从上到下填充元素，换行只是为了可读性。
- 相较于逐个赋值，逗号初始化更简洁，适合小矩阵的手动赋值。
- 如果不用 `<<`，则要写成逐个位置赋值，例如 `matrix_23(0,0)=1;`、`matrix_23(0,1)=2;`，这正是代码注释里强调的“普通 C++ 对应写法”。

#### 1.2.2 索引访问与分量访问

- 矩阵元素通过 `(行, 列)` 访问，索引从 **0** 开始：

  ```cpp
  double val = matrix_23(1, 2);   // 第二行第三列
  ```

- 向量可通过索引或分量名访问：

  ```cpp
  double x = v_3d[0];             // 索引访问
  double y = v_3d.y();            // 分量名访问
  v_3d.x() = 5;                   // 分量名也可用于修改
  ```

#### 1.2.3 向量构造与分量写法

在几何示例代码中，向量构造和向量分量读写是典型的语法：

```cpp
Eigen::Vector3d v(1, 0, 0);
// 通过括号传入三个参数，分别表示 x, y, z

v[0] <==> v.x();
v[1] <==> v.y();
v[2] <==> v.z();
```

这里要注意：

- `v(1,0,0)` 这种构造方式类似于 `Matrix` 的参数写法；
- `v[0]`、`v.x()` 都表示第一个分量；
- `v[0]` 更像数组下标，`v.x()` 更接近数学对象表示；
- `v.x()` 既可读也可写，适合对分量做数值修改。

#### 1.2.4 类型转换 cast

不同数值类型的矩阵不能直接混合运算，需要使用 `.cast<T>()` 显式转换：

```cpp
Eigen::Matrix<double, 2, 1> result = matrix_23.cast<double>() * v_3d;
```

- `.cast<double>()` 将 `matrix_23` 的元素类型从 `float` 转换为 `double`。
- 类型转换后可以与其他 `double` 类型矩阵进行运算。
- 务必保持运算双方类型一致，否则编译报错。

#### 1.2.5 cout 与流重载输出

真实代码注释还专门解释了 `<<` 在标准输出里的含义：

```cpp
cout << matrix_23 << endl;
cout << result << endl;
```

这里：

- `cout` 是标准输出流对象；
- `<<` 是重载后的输出运算符，表示“把右侧对象送入输出流”；
- 多个对象可以连写，形成链式输出；
- `endl` 表示换行。

这条语法和 `cout.precision(3)` 一样，都是输出成文档时很适合补充的“程序读法”。

额外补一句：在几何示例中，`cout.precision(3)` 用来控制总的输出有效数字位数，不是小数点后多少位，它和 `<<` 的重载输出共同构成了图示式打印矩阵与向量对象的通用模板。

### 1.3 矩阵基本运算

#### 1.3.1 转置、求和、迹、数乘

```cpp
matrix_33.transpose();    // 转置
matrix_33.sum();          // 所有元素之和
matrix_33.trace();        // 迹（对角线元素和）
10 * matrix_33;           // 数乘（每个元素乘以10）
```

- 这些函数返回新的表达式或值，不修改原矩阵（除非使用 `.transposeInPlace()` 等）。
- `sum()` 和 `trace()` 返回标量；`transpose()` 返回转置矩阵表达式。

#### 1.3.2 逆、行列式、伴随矩阵

```cpp
matrix_33.inverse();      // 逆矩阵
matrix_33.determinant();  // 行列式
matrix_33.adjoint();      // 伴随矩阵（共轭转置，对实矩阵即转置）
```

- `inverse()` 计算矩阵的逆，要求矩阵可逆。
- `determinant()` 返回标量行列式值。
- `adjoint()` 对于实数矩阵等价于转置，对于复数矩阵则是共轭转置。

#### 1.3.3 矩阵乘法与维度匹配

矩阵乘法使用 `*` 运算符，要求左侧矩阵列数等于右侧矩阵行数：

```cpp
Eigen::Matrix<double, 2, 1> result = matrix_23.cast<double>() * v_3d;  // 2x3 * 3x1 = 2x1
```

- 维度不匹配会导致编译错误。
- 类型不匹配也会导致编译错误，务必先统一类型。

### 1.4 线性代数求解

#### 1.4.1 特征值与特征向量

对于对称矩阵，可使用 `SelfAdjointEigenSolver`：

```cpp
Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigen_solver(matrix_33.transpose() * matrix_33);
std::cout << "Eigenvalues:\n" << eigen_solver.eigenvalues() << std::endl;
std::cout << "Eigenvectors:\n" << eigen_solver.eigenvectors() << std::endl;
```

- 模板参数指定矩阵类型（此处为 `Matrix3d`）。
- 构造函数接收一个对称矩阵，计算特征值和特征向量。
- `eigenvalues()` 返回特征值向量（按升序排列）。
- `eigenvectors()` 返回特征向量矩阵，**每一列**对应一个特征向量。

#### 1.4.2 线性方程组求解

求解 `Ax = b` 的两种方式：

```cpp
// 方法1：直接求逆（简单但不推荐）
Eigen::Matrix<double, N, 1> x = matrix_NN.inverse() * v_Nd;

// 方法2：QR分解求解（更稳定、快速）
x = matrix_NN.colPivHouseholderQr().solve(v_Nd);
```

- `colPivHouseholderQr()` 返回一个 QR 分解对象，调用其 `.solve(b)` 得到方程解。
- 对于大规模或病态矩阵，分解方法比求逆更数值稳定。

#### 1.4.3 计时方法

在比较不同求解方法效率时，可用 `clock()` 计时：

```cpp
clock_t time_stt = clock();
// ... 执行代码 ...
double elapsed_ms = 1000.0 * (clock() - time_stt) / CLOCKS_PER_SEC;
```

- `clock()` 返回程序运行的 CPU 时钟数。
- `CLOCKS_PER_SEC` 是每秒时钟数，用于换算为秒。
- 乘以 1000 得到毫秒。

#### 1.4.4 输出精度控制

几何示例中还出现了标准输出格式控制：

```cpp
cout.precision(3);
```

这里的含义是：

- `cout` 是标准输出流；
- `precision(3)` 设置输出时总共保留 3 位有效数字；
- `<<` 运算符可以连续输出多个对象，形成链式打印。

这条语法最适合看懂几何示例中输出旋转矩阵、欧拉角和四元数序列的风格。

---

## 二、应用部分

### 2.1 旋转表示

#### 2.1.1 旋转向量 AngleAxis

旋转向量（角轴）由旋转角和旋转轴组成，紧凑且直观：

```cpp
Eigen::AngleAxisd rotation_vector(M_PI / 4, Eigen::Vector3d(0, 0, 1));  // 绕 Z 轴旋转 45°
```

- 第一个参数为旋转角度（弧度），第二个参数为旋转轴向量（单位向量）。
- `AngleAxisd` 是模板类实例化，`d` 表示 `double` 精度。
- 在源代码注释中，`AngleAxisd` 被解释为“存储两个成员：`angle(double)` 与 `axis(Vector3d)`”，而 `Vector3d(0,0,1)` 表示旋转轴方向向量。

转换为旋转矩阵：

```cpp
Eigen::Matrix3d R = rotation_vector.toRotationMatrix();
// 或
Eigen::Matrix3d R2 = rotation_vector.matrix();
```

- `toRotationMatrix()` 和 `matrix()` 都返回对应的 3×3 旋转矩阵。
- 内部通过罗德里格斯公式计算。

AngleAxis 同时也可以像矩阵一样参与重载后的乘法：

```cpp
Eigen::Vector3d v_rotated = rotation_vector * v;
```

这表示角轴对象可以直接看成一个“带有旋转能力”的矩阵对象，和旋转矩阵、四元数一样都可用于向量旋转。

补充一句：在真实注释中，`AngleAxisd` 对象可看成一个“含有旋转轴与旋转角的对象”，它的底层虽然不是 `Matrix3d`，但由于重载了乘法和矩阵转换，因此在代码里可以直接参与 `R * v` 或 `AngleAxisd * Vector3d` 的运算思路。

#### 2.1.2 旋转矩阵与欧拉角

旋转矩阵是 3×3 的正交矩阵，可直接用 `Matrix3d` 表示：

```cpp
Eigen::Matrix3d rotation_matrix = Eigen::Matrix3d::Identity();   // 单位阵表示无旋转
```

从旋转矩阵提取欧拉角（ZYX 顺序，即 yaw-pitch-roll）：

```cpp
Eigen::Vector3d euler_angles = rotation_matrix.eulerAngles(2, 1, 0);
```

- 参数 `(2,1,0)` 表示绕 Z 轴、Y 轴、X 轴的旋转顺序。
- 返回的向量中：`[0]` 为绕 Z 轴转角（yaw），`[1]` 为绕 Y 轴转角（pitch），`[2]` 为绕 X 轴转角（roll）。
- 单位为弧度。

#### 2.1.3 四元数 Quaternion

四元数是一种无奇异性的旋转表示，适合插值和连续姿态估计：

```cpp
Eigen::Quaterniond q(rotation_vector);           // 从角轴构造
Eigen::Quaterniond q2(rotation_matrix);          // 从旋转矩阵构造
```

- `Quaterniond` 存储四个分量 `(x, y, z, w)`，其中 `w` 为实部，`(x, y, z)` 为虚部。
- `coeffs()` 返回系数向量，顺序为 `(x, y, z, w)`：

  ```cpp
  std::cout << q.coeffs() << std::endl;   // 输出 (x, y, z, w)
  ```

真实代码注释中进一步强调：

- 四元数可以由角轴构造，也可以由旋转矩阵构造；
- `coeffs()` 的顺序必须记成 `(x, y, z, w)`；
- 在 Eigen 中，四元数与向量乘法使用重载后的乘法，数学上等价于 `q v q^{-1}`，只是计算写法被简化成 `q * v`；
- 代码中也把 `rotation_vector.matrix()`、`rotation_vector.toRotationMatrix()` 和 `Quaterniond` 的不同表示方式串成统一理解链路。

使用四元数旋转向量：

```cpp
Eigen::Vector3d v_rotated = q * v;   // 数学上等价于 q * v * q^{-1}
```

- Eigen 重载了四元数与向量的乘法，直接得到旋转后的向量。

### 2.2 刚体变换与位姿

#### 2.2.1 欧氏变换 Isometry3d

`Isometry3d` 表示三维刚体变换（旋转 + 平移），内部为 4×4 齐次矩阵：

```cpp
Eigen::Isometry3d T = Eigen::Isometry3d::Identity();   // 初始化为单位变换
T.rotate(rotation_vector);                             // 添加旋转（右乘）
T.pretranslate(Eigen::Vector3d(1, 3, 4));              // 添加平移（右乘）
```

- `Identity()` 返回单位变换（无旋转、无平移）。
- `rotate()` 在当前位姿上右乘一个旋转，相当于 `T = T * R`。
- `pretranslate()` 在当前位姿上右乘一个平移，相当于 `T = T * T_t`，平移发生在局部坐标系。

真实注释还把内部结构解释得很直接：

- `Isometry3d` 本质上由内部 `Matrix3d` 旋转矩阵和 `Vector3d` 平移向量构成；
- `rotation()` 返回内部旋转矩阵引用，可直接覆盖旧旋转；
- `translation()` 返回内部平移向量引用，可直接修改；
- 平移与旋转分量读写之间，不会因为做 `rotate()` 或 `pretranslate()` 的叠加而影响彼此的内部存储结构。

也可直接读写内部旋转和平移：

```cpp
T.rotation()     // 返回内部旋转矩阵的引用（可修改）
T.translation()  // 返回内部平移向量的引用（可修改）
```

获取完整的 4×4 变换矩阵：

```cpp
std::cout << "Transform matrix = \n" << T.matrix() << std::endl;
```

补充说明：这里的 `T.matrix()` 只负责打印完整的 4×4 齐次矩阵形式，真正用来做坐标变换的是 `T * v` 的重载语义。也就是说，`Isometry3d` 既能看成“内部旋转+平移的位姿对象”，又能通过 `matrix()` 转变成“一张 4×4 的齐次矩阵”描述刚体运动。

#### 2.2.2 坐标变换

使用变换矩阵对向量进行坐标变换：

```cpp
Eigen::Vector3d v_transformed = T * v;   // 等价于 v' = R * v + t
```

- 在齐次坐标中，`T` 是 4×4 矩阵，`v` 被自动扩展为 `(v, 1)`，运算后取前三维。
- 这是 SLAM 中世界坐标系与相机坐标系之间变换的核心操作。

---

## 三、后续功能模块目录（待补充）

> 以下模块将在后续章节中逐步补充，保持功能分类的一致性。

- 可视化与交互：三维窗口创建与管理、绘制坐标系/轨迹/点云、实时交互与相机姿态显示（常用库：Pangolin）
- 图像处理与特征提取：图像数据结构与基本操作、特征点检测与描述子、特征匹配与筛选（常用库：OpenCV）
- 相机模型与投影：针孔相机模型、内参矩阵与畸变系数、世界坐标到像素坐标的转换、深度图与点云生成
- 非线性优化与求解器：图优化基本概念、位姿图优化、Bundle Adjustment（常用库：Ceres、g2o）

---

> **说明**：本笔记以功能模块组织，当前重点覆盖 Eigen 的矩阵、几何与位姿操作，后续将按功能逐步补充其他常用库和接口，保持结构清晰、易于查找。
