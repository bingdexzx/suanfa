
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// ===============================
// 任务：识别图像中的红色圆环
// 输入：一帧视频图像
// 输出：检测目标的矩形框
// 如果没有找到目标，返回空矩形
// ===============================

Rect detectRedRing(const Mat& frame)
{
    Rect targetBox;

    Mat hsv, mask0,mask1,mask2;

    Scalar red_ranges_low[2] = {Scalar(0,90,70),Scalar(10,255,255)};
    Scalar red_ranges_high[2] = {Scalar(160,150,100),Scalar(179,255,255)};

    // BGR to HSV
    cvtColor(frame, hsv, COLOR_BGR2HSV);

    // TODO 1：提取红色区域
    // 提示：inRange()
    // 注意：红色在 HSV 的 H 通道两端

   inRange(
    hsv,
    red_ranges_low[0],
    red_ranges_low[1],
    mask1
   );

   inRange(
    hsv,
    red_ranges_high[0],
    red_ranges_high[1],
    mask2
    );

    bitwise_or(mask1,mask2,mask0);


    // TODO 2：去除噪声（选做）
    // 提示：morphologyEx()
    Mat kernel = getStructuringElement(MORPH_ELLIPSE)
    morphologyEx(mask,mask,MORPH_CLOSE)
    

    // TODO 3：寻找轮廓
    // 提示：findContours()


    // TODO 4：选择合适的目标
    // 提示：contourArea()、boundingRect()


    return targetBox;
}


int main(int argc, char** argv)
{
    string videoPath = argc > 1 ? argv[1] : "video.mp4";

    VideoCapture cap(videoPath);

    if (!cap.isOpened())
    {
        cout << "视频打开失败！" << endl;
        return -1;
    }

    Mat frame;

    while (cap.read(frame))
    {
        // 调用自己的检测算法
        Rect box = detectRedRing(frame);

        // 检测到目标就画框
        if (box.area() > 0)
        {
            rectangle(frame, box,
                      Scalar(0, 255, 0), 2);
        }

        imshow("Result", frame);

        // 按 ESC 退出
        int key = waitKey(33);
        if (key == 27)
            break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}