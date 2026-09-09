#include <iostream>
#include <cmath>
using namespace std; 

#include <Eigen/Core>
#include <Eigen/Geometry>

// 新版模板Sophus，后缀hpp，类名SO3d / SE3d
#include <sophus/so3.hpp>
#include <sophus/se3.hpp>

int main( int argc, char** argv )
{
    // 沿Z轴转90度的旋转矩阵
    Eigen::Matrix3d R = Eigen::AngleAxisd(M_PI / 2, Eigen::Vector3d(0, 0, 1)).toRotationMatrix();
    // .toRotationMatrix()内部通过Eigen::Matrix3d的构造函数将旋转向量Eigen::AngleAxisd（存储旋转角度和轴）转换为旋转矩阵

    Sophus::SO3d SO3_R(R); // Sophus::SO(3)可以直接从旋转矩阵R构造一个 SO (3) 李群对象 `SO3_R`
    Sophus::SO3d SO3_v = Sophus::SO3d::exp(Eigen::Vector3d(0, 0, M_PI / 2));
    // 也可以通过旋转向量（轴角）来构造SO(3)对象，Sophus::SO3d::exp()函数将旋转向量转换为SO(3)李群对象 `SO3_R`
    Eigen::Quaterniond q(R); // 从旋转矩阵构造四元数
    Sophus::SO3d SO3_q(q); // 也可以通过四元数来构造SO(3)对象 `SO3_R`

    // 输出SO(3)时，以旋转矩阵形式输出
    cout << "SO(3) from matrix:\n" << SO3_R.matrix() << endl;
    cout << "SO(3) from vector:\n" << SO3_v.matrix() << endl;
    cout << "SO(3) from quaternion:\n" << SO3_q.matrix() << endl;

    // 使用对数映射获得它的李代数
    Eigen::Vector3d so3 = SO3_R.log();
    cout << "so3 = " << so3.transpose() << endl;
    // hat 为向量到反对称矩阵
    cout << "so3 hat=\n" << Sophus::SO3d::hat(so3) << endl; // hat()函数将李代数向量转换为反对称矩阵
    // 相对的，vee为反对称到向量
    cout << "so3 hat vee= " << Sophus::SO3d::vee(Sophus::SO3d::hat(so3)).transpose() << endl; // vee()函数将反对称矩阵转换为李代数向量

    // 增量扰动模型的更新
    Eigen::Vector3d update_so3(1e-4, 0, 0); // 沿X轴的微小旋转
    // update_so3是李代数，先用exp映射到李群，再左乘更新
    Sophus::SO3d SO3_updated = Sophus::SO3d::exp(update_so3) * SO3_R;
    cout << "SO3 updated = \n" << SO3_updated.matrix() << endl;

    /********************萌萌的分割线*****************************/
    cout << "************我是分割线*************" << endl;

    // 对SE(3)操作大同小异
    Eigen::Vector3d t(1, 0, 0); // 定义三维平移向量t=(1,0,0)T
    Sophus::SE3d SE3_Rt(R, t); // Sophus::SE3d可以直接从旋转矩阵R和平移向量t构造一个 SE(3) 李群对象 `SE3_Rt`
    Sophus::SE3d SE3_qt(q, t); // 也可以通过四元数q和平移向量t构造一个 SE(3) 李群对象 `SE3_qt`
    cout << "SE3 from R,t= \n" << SE3_Rt.matrix() << endl;
    cout << "SE3 from q,t= \n" << SE3_qt.matrix() << endl;

    // 李代数se(3) 是一个六维向量，方便起见先typedef一下
    typedef Eigen::Matrix<double, 6, 1> Vector6d;
    Vector6d se3 = SE3_Rt.log();
    cout << "se3 = " << se3.transpose() << endl;

    // 观察输出，会发现在Sophus中，se(3)的平移在前，旋转在后.
    // 同样的，有hat和vee两个算符
    cout << "se3 hat = \n" << Sophus::SE3d::hat(se3) << endl;
    cout << "se3 hat vee = " << Sophus::SE3d::vee(Sophus::SE3d::hat(se3)).transpose() << endl;

    // 最后，演示一下更新
    Vector6d update_se3;
    update_se3.setZero(); // setZero()函数将向量初始化为零
    update_se3(0, 0) = 1e-4; // 沿X轴的微小平移
    Sophus::SE3d SE3_updated = Sophus::SE3d::exp(update_se3) * SE3_Rt;
    cout << "SE3 updated = \n" << SE3_updated.matrix() << endl; // matrix()函数将SE(3)李群对象转换为4x4的变换矩阵

    return 0;
}