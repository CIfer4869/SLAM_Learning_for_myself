# SLAM Learning for Myself

这是一个用于学习视觉 SLAM 基础知识和 C++ 工程实践的个人仓库，内容按照《视觉 SLAM 十四讲》的学习顺序逐步整理。

## 课程内容

### 第 1~2 讲：C++、g++ 与 CMake 入门

这一部分主要学习 SLAM 开发所需的基础工具链：

- 使用 `g++` 编译和运行单个 C++ 源文件。
- 理解源文件、头文件、静态库和可执行文件之间的关系。
- 使用 CMake 管理工程、生成构建文件并进行外部构建。
- 创建和链接静态库，理解编译阶段与链接阶段的区别。
- 使用 VS Code、C/C++ 扩展、CMake Tools 和 gdb 进行编辑、构建和调试。
- 排查路径错误、命名空间错误、头文件缺失和 `undefined reference` 等常见问题。

对应工程包含 `main.cpp` 单文件程序、`hello.h` 头文件、`libhello.cpp` 静态库实现和 `useHello.cpp` 调用程序。

[阅读第 1~2 讲详细笔记](./lecture_note/SLAM_lecture_1~2/Readme.md)

### 第 3 讲：Eigen 矩阵、几何变换与 Pangolin 可视化

这一部分开始进入 SLAM 中常用的数学库和可视化工具：

- 使用 Eigen 进行矩阵、向量、特征值和线性方程求解。
- 理解旋转矩阵、角轴、欧拉角、四元数和三维刚体变换。
- 使用 CMake 分别构建 `useEigen`、`useGeometry` 和 `visualizeGeometry` 三个示例。
- 使用 Pangolin 创建 3D 窗口、相机交互、坐标轴和 UI 信息面板。
- 排查 Pangolin 与 libtiff 的链接问题。
- 在 WSL2 中使用 WSLg 运行 Linux 图形程序，并了解 Pangolin 无头模式。
- 结合实际代码学习 Eigen 模板、运算符重载、引用、结构体和链式调用等 C++ 语法。

[阅读第 3 讲详细笔记](./lecture_note/SLAM_lecture_3/Readme.md)

### 第 4 讲：Sophus 与 SO(3)/SE(3) 李群/李代数

这一部分开始引入 SLAM 中常用的位姿表达与流形映射思想：

- 使用 Sophus 中的 `SO3d` 与 `SE3d` 表示三维旋转与刚体位姿。
- 理解 `SO(3)` 与 `SE(3)` 的李群结构，以及对应的 `so(3)` 与 `se(3)` 李代数空间。
- 学习使用 `exp()`、`log()`、`hat()` 与 `vee()` 完成群元素与切空间之间的相互映射。
- 通过 `SO3d::exp(update_so3) * SO3_R` 与 `SE3d::exp(update_se3) * SE3_Rt` 掌握左扰动式位姿增量更新方法。
- 将 Eigen 的旋转矩阵、四元数、齐次位姿表达方式，逐步抽象为 Sophus 的群对象表达。
- 配合 `CmakeLists.txt` 与源码工程结构完成 Sophus 工程的编译与运行。

[阅读第 4 讲详细笔记](./lecture_note/SLAM_lecture_4/Readme.md)

### SLAM 常用函数速查

这是一份以功能模块为主线的 Eigen 库常用函数速查笔记，整理矩阵、几何、位姿相关的常用类型和函数，并预留后续功能模块目录，便于逐步扩展可视化、图像处理、优化等内容。

[阅读 SLAM 常用函数速查笔记](./lecture_note/SLAM_常用函数/Readme.md)

## 工程目录

```text
SLAM_Learning_for_myself/
├── lecture_files/
│   ├── lecture_1~2/       # C++、静态库和 CMake 基础工程
│   ├── lecture_3/         # Eigen、几何变换和 Pangolin 示例
│   └── lecture_4/         # Sophus、SO(3)/SE(3) 与李群/李代数示例
├── lecture_note/
│   ├── SLAM_lecture_1~2/  # 第 1~2 讲笔记
│   ├── SLAM_lecture_3/    # 第 3 讲笔记
│   ├── SLAM_lecture_4/    # 第 4 讲笔记
│   └── SLAM_常用函数/      # SLAM常用函数速查笔记
└── Readme.md
```

后续讲次会继续补充到 `lecture_note` 和 `lecture_files` 目录中，函数会整理到 `lecture_files/SLAM_常用函数`。
