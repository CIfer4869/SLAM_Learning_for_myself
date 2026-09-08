#include <iostream> // 导入库的头文件，包含输入输出流的定义
#include <cmath>
using namespace std;

#include <Eigen/Core>
// Eigen 几何模块
#include <Eigen/Geometry>

/****************************
* 本程序演示了 Eigen 几何模块的使用方法
****************************/

int main ( int argc, char** argv )
{
    // Eigen/Geometry 模块提供了各种旋转和平移的表示
    // 3D 旋转矩阵是3×3的矩阵，所以直接使用 Matrix3d 或 Matrix3f进行定义
    Eigen::Matrix3d rotation_matrix = Eigen::Matrix3d::Identity();
    // `::`——类作用域解析符，说明后面的 `Identity()` 是属于 Matrix3d 这个类的静态成员函数，不是对象的成员方法。
    // Matrix3d::Identity() 表示返回一个3×3的单位矩阵
    // Eigen::Matrix3d::Identity()————命名空间::类名::静态函数


    // 旋转向量使用 AngleAxis, 它底层不直接是Matrix，但运算可以当作矩阵（因为重载了运算符）
    Eigen::AngleAxisd rotation_vector ( M_PI/4, Eigen::Vector3d ( 0,0,1 ) );     //沿 Z 轴旋转 45 度
    // Eigen::AngleAxisd——类型：double 精度的角轴类。存储两个成员：angle(double)、axis(Vector3d)。
    // Eigen::Vector3d(0,0,1)——类型：double 精度的三维向量类。存储三个成员：x(double)、y(double)、z(double)，这里表示旋转轴的方向向量
    // 这里旋转角度为 M_PI/4，旋转轴为 (0,0,1)，即绕 Z 轴旋转 45 度


    cout.precision(3);
    // cout.precision(n)：设置有效数字总位数，不是小数点后位数

    cout<<"rotation matrix =\n"<<rotation_vector.matrix() <<endl;                //用matrix()转换成矩阵
    /* 
    `<<` 运算符详解
        `cout`：标准输出对象（控制台打印）
        `<<`：输出运算符，重载了多种类型的输出方式——把右边的数据 “送入” 输出流，打印到屏幕
        可以链式拼接：`a << b << c << d`，从左往右依次输出 a，b，c，d
    */

    // 也可以直接赋值
    rotation_matrix = rotation_vector.toRotationMatrix();
    /*
    此前已定义Eigen::Matrix3d rotation_matrix = Eigen::Matrix3d::Identity();
    rotation_vector 是 Eigen::AngleAxisd 角轴对象
    相当于Eigen::Matrix3d rotation_matrix = rotation_vector.toRotationMatrix();
    
    .toRotationMatrix()解析
        `AngleAxisd` 的成员函数
        功能：把角轴（角度‑轴表示）通过罗德里格斯公式，计算得到 3×3 旋转矩阵
        返回值类型：`Eigen::Matrix3d`
    */
    
    // 用 AngleAxis 可以进行坐标变换
    Eigen::Vector3d v ( 1,0,0 );
    /* 
    构造语法：Eigen::Vector3d 变量名(x, y, z);
    括号中传入的三个参数：
    v[0] <==> v.x()  第一个参数
    v[1] <==> v.y()  第二个参数
    v[2] <==> v.z()  第三个参数
    .x()/.y()/.z()既可以读取分量，也可以用来修改分量的值。
    */

    Eigen::Vector3d v_rotated = rotation_vector * v; // 使用重载的乘法运算符，进行坐标变换
    cout<<"(1,0,0) after rotation = "<<v_rotated.transpose()<<endl; // .transpose()用于将列向量转置为行向量
    // 或者用旋转矩阵
    v_rotated = rotation_matrix * v;
    cout<<"(1,0,0) after rotation = "<<v_rotated.transpose()<<endl;

    // 欧拉角: 可以将旋转矩阵直接转换成欧拉角
    Eigen::Vector3d euler_angles = rotation_matrix.eulerAngles ( 2,1,0 ); // ZYX顺序，即roll pitch yaw顺序
    /*
    函数原型
        Vector3d Matrix3d::eulerAngles(int a0, int a1, int a2) const;
        作用：从旋转矩阵提取欧拉角。
        参数：a0, a1, a2：欧拉角的旋转轴顺序，取值为 0、1、2，分别对应 X、Y、Z 轴。
        euler_angles[0]  → 绕Z轴转角(yaw，偏航)；euler_angles[1]  → 绕Y轴转角(pitch，俯仰)；euler_angles[2]  → 绕X轴转角(roll，滚转)
        返回值：Eigen::Vector3d，用向量存储三个欧拉角，单位为弧度。
    */
    cout<<"yaw pitch roll = "<<euler_angles.transpose()<<endl;

    // 欧氏变换矩阵/刚体变换矩阵/旋转+平移矩阵
    // 欧氏变换矩阵使用 Eigen::Isometry——`Isometry3d`：三维欧氏变换（刚体变换），4×4 齐次变换矩阵，包含旋转 + 平移。
    // 内部存储：旋转矩阵`Matrix3d` + 平移向量`Vector3d`；
    // `d`代表`double`双精度；`Isometry3f`为 float 版本。
    Eigen::Isometry3d T=Eigen::Isometry3d::Identity();                // 虽然称为3d，实质上是4＊4的旋转+平移矩阵
    T.rotate ( rotation_vector );                                     // 按照rotation_vector进行旋转
    /*
        T.rotate(R)：操作对象——整个位姿 T
        传入旋转对象，执行右乘叠加 T = T * R
        在当前位姿之上追加旋转，原有旋转、平移都会保留，只做乘法叠加
        可接收 AngleAxisd角轴 / Matrix3d旋转矩阵

        T.rotation()：操作对象——T 内部的旋转矩阵
        返回位姿内部旋转矩阵的引用，用于读写
        赋值时直接覆盖内部旋转，旧旋转直接丢弃，平移分量完全不受影响
        是覆盖，不是矩阵叠加
    */

    T.pretranslate ( Eigen::Vector3d ( 1,3,4 ) );                     // 把平移向量设成(1,3,4)，右乘叠加平移
    /*
        T.pretranslate(T_t)：操作对象——整个位姿 T
        叠加：T = T * T_t，右乘平移变换，局部坐标系追加平移，旋转保持不变
    
        T.translation()：操作对象——T 内部的平移向量
        返回位姿内部平移向量的引用，用于读写
    */
    cout << "Transform matrix = \n" << T.matrix() <<endl;

    // 用变换矩阵进行坐标变换
    Eigen::Vector3d v_transformed = T*v;                              // 相当于R*v+t
    cout<<"v tranformed = "<<v_transformed.transpose()<<endl;

    // 对于仿射和射影变换，使用 Eigen::Affine3d 和 Eigen::Projective3d 即可，略

    // 四元数
    // 可以直接把AngleAxis赋值给四元数，反之亦然
    Eigen::Quaterniond q = Eigen::Quaterniond ( rotation_vector );
    // Eigen::Quaterniond——类型：double 精度的四元数类，输入参数为 Eigen::AngleAxisd 或 Eigen::Matrix3d（一个四维向量）
    // 存储四个成员：x(double)、y(double)、z(double)、w(double)，其中 w 为实部，(x,y,z) 为虚部。

    cout<<"quaternion = \n"<<q.coeffs() <<endl;   // 请注意coeffs的顺序是(x,y,z,w),w为实部，前三者为虚部
    // .coeffs()——返回四元数的系数向量，类型为 Eigen::Vector4d，存储顺序为 (x, y, z, w)，其中 w 为实部，(x, y, z) 为虚部。
    // 也可以把旋转矩阵赋给它
    q = Eigen::Quaterniond ( rotation_matrix );
    cout<<"quaternion = \n"<<q.coeffs() <<endl;
    // 使用四元数旋转一个向量，使用重载的乘法即可
    v_rotated = q*v; // 注意数学上是qvq^{-1}
    cout<<"(1,0,0) after rotation = "<<v_rotated.transpose()<<endl;

    return 0;
}