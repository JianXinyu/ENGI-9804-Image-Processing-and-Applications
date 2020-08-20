## Dependencies:

- OpenCV version 4.4.0

- armadillo: a scientific calculation library used for the fast computation of side window filter

## Usage

- build the project.

- put the images that you want to process in the folder ./original_image in the directory where .exe file is.

- change the input image name in the main().

- run the code, check the step by step process by clicking off the image panel.

- check the results in the folder ./output_image

  

## Some OpenCV functions

### mixChannles()

函数功能：mixChannels主要就是把输入的矩阵（或矩阵数组）的某些通道拆分复制给对应的输出矩阵（或矩阵数组）的某些通道中，其中的对应关系就由fromTo参数制定.
接口：void  mixChannels (const Mat*  src , int  nsrc , Mat*  dst , int  ndst , const int*  fromTo , size_t  npairs );
src: 输入矩阵，可以为一个也可以为多个，但是矩阵必须有相同的大小和深度.
nsrc: 输入矩阵的个数.
dst: 输出矩阵，可以为一个也可以为多个，但是所有的矩阵必须事先分配空间（如用create），大小和深度须与输入矩阵等同.
ndst: 输出矩阵的个数
fromTo:设置输入矩阵的通道对应输出矩阵的通道，规则如下：首先用数字标记输入矩阵的各个通道。输入矩阵个数可能多于一个并且每个矩阵的通道可能不一样，
第一个输入矩阵的通道标记范围为：0 ~src[0].channels() - 1，第二个输入矩阵的通道标记范围为：src[0].channels() ~src[0].channels() + src[1].channels() - 1,
以此类推；其次输出矩阵也用同样的规则标记，第一个输出矩阵的通道标记范围为：0 ~dst[0].channels() - 1，第二个输入矩阵的通道标记范围为：dst[0].channels()
~dst[0].channels() + dst[1].channels() - 1, 以此类推；最后，数组fromTo的第一个元素即fromTo[0]应该填入输入矩阵的某个通道标记，而fromTo的第二个元素即
fromTo[1]应该填入输出矩阵的某个通道标记，这样函数就会把输入矩阵的fromTo[0]通道里面的数据复制给输出矩阵的fromTo[1]通道。fromTo后面的元素也是这个
道理，总之就是一个输入矩阵的通道标记后面必须跟着个输出矩阵的通道标记.
npairs: 即参数fromTo中的有几组输入输出通道关系，其实就是参数fromTo的数组元素个数除以2.



### Canny()

void Canny( InputArray image, OutputArray edges, double threshold1, double threshold2, int apertureSize = 3, bool L2gradient = false);
第一个参数：输入图像（八位的图像）
第二个参数：输出的边缘图像
第三个参数：下限阈值，如果像素梯度低于下限阈值，则将像素不被认为边缘
第四个参数：上限阈值，如果像素梯度高于上限阈值，则将像素被认为是边缘（建议上限是下限的2倍或者3倍）
第五个参数：为Sobel()运算提供内核大小，默认值为3
第六个参数：计算图像梯度幅值的标志，默认值为false



### approxPolyDP()

近似轮廓，精度与轮廓周长成正比,主要功能是把一个连续光滑曲线折线化，对图像轮廓点进行多边形拟合。
函数声明：void approxPolyDP(InputArray curve, OutputArray approxCurve, double epsilon, bool closed)
InputArray curve:一般是由图像的轮廓点组成的点集
OutputArray approxCurve：表示输出的多边形点集
double epsilon：主要表示输出的精度，就是两个轮廓点之间最大距离数，5,6,7，，8，，,,，
bool closed：表示输出的多边形是否封闭





