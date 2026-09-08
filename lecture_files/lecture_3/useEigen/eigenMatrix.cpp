#include <iostream>
using namespace std;

#include <ctime>
// Eigen 部分
#include <Eigen/Core>
// 稠密矩阵的代数运算（逆，特征值等）
#include <Eigen/Dense>
using namespace Eigen;

#define MATRIX_SIZE 50

/****************************
* 本程序演示了 Eigen 基本类型的使用
****************************/

// C++语法：argc = argument count：参数个数（整数）
// C++语法：argv = argument vector：参数数组，字符串指针数组，char** argv等价于 char* argv[]
int main( int argc, char** argv )
{
    // Eigen 中所有向量和矩阵都是Eigen::Matrix，它是一个模板类。它的前三个参数为：数据类型、行、列
    // 声明一个2*3的float矩阵
    Eigen::Matrix<float, 2, 3> matrix_23;
    /*  
    C++语法：
    1.“Eigen::”解析
        Eigen是库的名字空间。:: 表示 “从 Eigen 这个库里拿东西/使用函数”。
        就像 std::cout，std是标准库名字空间。
    2.“Matrix”解析
        是 Eigen 库里面的模板类，专门用来表示矩阵
        普通类：`class A {};`
        模板类：`template<typename T, int R, int C> class Matrix {};`
        模板可以接收多个参数，放在 `<>`尖括号里面——typename T 表示数据类型，int R 表示行数，int C 表示列数。
    3.“matrix_23”解析
        这是变量名，表示一个矩阵对象。它是 Eigen::Matrix<float, 2, 3> 类型的对象。
    Matrix的大小和形状在编译阶段就全部定死，运行时不能改 2 行 3 列，这叫固定尺寸矩阵。
    固定尺寸矩阵相比于动态尺寸矩阵，速度更快，效率更高。
    */



    // 同时，Eigen 通过 typedef 提供了许多内置类型，不过底层仍是Eigen::Matrix
    // 例如 Vector3d 实质上是 Eigen::Matrix<double, 3, 1>，即三维向量
    Eigen::Vector3d v_3d;
	// 这是一样的
    Eigen::Matrix<float,3,1> vd_3d;
    // 例如 Matrix3f 实质上是 Eigen::Matrix<float, 3, 3>
    Eigen::Matrix3f matrix_33f; // 3*3的浮点数矩阵
    Eigen::Matrix<float, 3, 3> matrix_33f_alt; // 同样的定义以一个3*3的浮点数矩阵
    
    // Matrix3d 实质上是 Eigen::Matrix<double, 3, 3>
    // 矩阵的初始化
    Eigen::Matrix3d matrix_33 = Eigen::Matrix3d::Zero(); //初始化为零
    /*
    C++语法：Eigen::Matrix3d::Zero() 表示创建一个3*3的零矩阵
    */
    
    // 如果不确定矩阵大小，可以使用动态大小的矩阵
    Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic > matrix_dynamic;
    // double：元素是双精度浮点数；Eigen::Dynamic：行数运行时才定；Eigen::Dynamic：列数运行时才定

    // 更简单的
    Eigen::MatrixXd matrix_x;
    // `MatrixXd` 就是上面长长一整串的 typedef 别名简写，二者完全等价：Eigen::MatrixXd   ===   Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic >
    // 这种类型还有很多，我们不一一列举

    
    // 下面是对Eigen阵的操作
    // 输入数据（初始化）——这是 Eigen 独有的逗号初始化语法，普通 C++ 没有这个写法
    matrix_23 << 1, 2, 3, 4, 5, 6;
    /*
    `matrix_23` 是 2 行 3 列矩阵：
            [1 2 3]
            [4 5 6]
    
    不用`<<`的话，只能一个个元素赋值：
            matrix_23(0,0) = 1;
            matrix_23(0,1) = 2;
            matrix_23(0,2) = 3;
            matrix_23(1,0) = 4;
            matrix_23(1,1) = 5;
            matrix_23(1,2) = 6;

    可以换行写，可读性更好（逗号放末尾）：
            matrix_23 << 1, 2, 3,
                         4, 5, 6;
    */

    // 输出
    cout << matrix_23 << endl;

    // 用()访问矩阵中的元素，matrix_23(i,j)表示第i行第j列的元素，注意i和j都是从0开始计数的
    for (int i=0; i<2; i++) {
        for (int j=0; j<3; j++)
            cout<<matrix_23(i,j)<<"\t";
        cout<<endl;
    }

    // 也可以用[]访问向量中的元素，v_3d[i]表示第i个元素，注意i也是从0开始计数的，相当于数组的下标
    for (int i=0; i<3; i++) {
        cout << v_3d[i] << "\t";
    }
    cout << endl;

    // 矩阵和向量相乘（实际上仍是矩阵和矩阵）
    v_3d << 3, 2, 1;
    vd_3d << 4, 5, 6;
    // 但是在Eigen里你不能混合两种不同类型的矩阵，像这样是错的
    // Eigen::Matrix<double, 2, 1> result_wrong_type = matrix_23 * v_3d;
    // double类型的矩阵和float类型的矩阵是不同的类型，Eigen不允许这两类矩阵直接相乘
    // 应该显式转换
    Eigen::Matrix<double, 2, 1> result = matrix_23.cast<double>() * v_3d;
    // 语法：.cast<目标类型>() 是 Eigen 成员函数：强制元素类型转换
    cout << result << endl;

    Eigen::Matrix<float, 2, 1> result2 = matrix_23 * vd_3d;
    cout << result2 << endl;

    // 同样你不能搞错矩阵的维度
    // 试着取消下面的注释，看看Eigen会报什么错
    // Eigen::Matrix<double, 2, 3> result_wrong_dimension = matrix_23.cast<double>() * v_3d;

    // 一些矩阵运算
    // 四则运算就不演示了，直接用+-*/即可。
    matrix_33 = Eigen::Matrix3d::Random();      // 随机数矩阵
    cout << matrix_33 << endl << endl;

    cout << matrix_33.transpose() << endl;      // 转置
    cout << matrix_33.sum() << endl;            // 各元素和
    cout << matrix_33.trace() << endl;          // 迹
    cout << 10*matrix_33 << endl;               // 数乘
    cout << matrix_33.inverse() << endl;        // 逆
    cout << matrix_33.determinant() << endl;    // 行列式
    cout << matrix_33.adjoint() << endl;        // 伴随矩阵

    // 特征值
    // 实对称矩阵可以保证对角化成功
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigen_solver ( matrix_33.transpose()*matrix_33 );
    /*
    语法：
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigen_solver( ... )
    模板参数 `<Eigen::Matrix3d>`——告诉求解器：要处理的矩阵类型是 `Matrix3d`（3×3 double）。
    `SelfAdjointEigenSolver` = 自伴随（对称）矩阵特征求解器
    `eigen_solver`：对象名字
    matrix_33.transpose() * matrix_33——传入的矩阵是 `matrix_33` 的转置乘以 `matrix_33`，这是一个对称矩阵
    这个构造函数会直接计算出特征值和特征向量，
    */
    cout << "Eigen values = \n" << eigen_solver.eigenvalues() << endl;
    //# `.eigenvalues()`—— `SelfAdjointEigenSolver` 对象的成员函数，专门用来取出矩阵的特征值
    cout << "Eigen vectors = \n" << eigen_solver.eigenvectors() << endl;
    // .eigenvectors()——eigen_solver.eigenvectors()，成员函数，返回 `Matrix3d`矩阵。每一列是一个特征向量：第 0 列对应第 0 个特征值，第 1 列对应第 1 个特征值……

    // 解方程
    // 我们求解 matrix_NN * x = v_Nd 这个方程
    // N的大小在前边的宏里定义，它由随机数生成
    // 直接求逆自然是最直接的，但是求逆运算量大

    Eigen::Matrix< double, MATRIX_SIZE, MATRIX_SIZE > matrix_NN;
    matrix_NN = Eigen::MatrixXd::Random( MATRIX_SIZE, MATRIX_SIZE );
    Eigen::Matrix< double, MATRIX_SIZE,  1> v_Nd;
    v_Nd = Eigen::MatrixXd::Random( MATRIX_SIZE,1 );

    clock_t time_stt = clock(); // 计时
    // 直接求逆
    Eigen::Matrix<double,MATRIX_SIZE,1> x = matrix_NN.inverse()*v_Nd;
    cout <<"time use in normal inverse is " << 1000* (clock() - time_stt)/(double)CLOCKS_PER_SEC << "ms"<< endl;
    
	// 通常用矩阵分解来求，例如QR分解，速度会快很多
    time_stt = clock();
    x = matrix_NN.colPivHouseholderQr().solve(v_Nd);
    cout <<"time use in Qr decomposition is " <<1000*  (clock() - time_stt)/(double)CLOCKS_PER_SEC <<"ms" << endl;

    return 0;
}