
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

// ===============================
// 任务：识别图像中的红色圆环
// 输入：一帧视频图像
// 输出：检测目标的矩形框
// 如果没有找到目标，返回空矩形
// ===============================



Mat kernel = getStructuringElement(MORPH_ELLIPSE,Size(3,3));

Scalar red_ranges_low[2] = {Scalar(0,130,137),Scalar(11,255,255)};
Scalar red_ranges_high[2] = {Scalar(168,130,137),Scalar(179,255,255)};

Rect detectRedRing(const Mat& frame)
{
    Rect targetBox;

    Mat hsv, mask0,mask1,mask2;

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;


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
    //cv::imshow("mask0",mask0);

    // TODO 2：去除噪声（选做）
    // 提示：morphologyEx()
    morphologyEx(mask0,mask0,MORPH_CLOSE,kernel);
    morphologyEx(mask0,mask0,MORPH_OPEN,kernel);
    cv::imshow("mask0",mask0);

    // TODO 3：寻找轮廓
    // 提示：findContours()
    findContours(mask0.clone(),contours,hierarchy,RETR_CCOMP,CHAIN_APPROX_SIMPLE);
    // TODO 4：选择合适的目标
    // 提示：contourArea()、boundingRect()
    for(int cnt =0 ; cnt < contours.size(); cnt++ ){
        //filt out small ex_contours 
        if (contourArea(contours[cnt]) < 80){
           
             continue; 
        }
        //find the ex_contour
        if(hierarchy[cnt][3] != -1 ){
            continue;
        }
        //find the in_contours in ex_contour
        if(hierarchy[cnt][2] == -1){
            continue;
        }
        int next = hierarchy[cnt][2];
        double inAera = 0;
        int in_cnt=-1;
        // get the biggest in_contour
        while(next != -1){
            if(contourArea(contours[next]) < 90){
                next = hierarchy[next][0];
                continue;
            }
            if(contourArea(contours[next]) > inAera){
                inAera = contourArea(contours[next]);
                in_cnt = next;
        
            }
            next = hierarchy[next][0];
        }
        // max in_contour area too small -> GET OUT
        if(inAera < 80){
            continue;
        }

        // cala ex_contour circularity
        double experi = arcLength(contours[cnt], true);
        double exArea = contourArea(contours[cnt]);
        if (experi <= 1e-6) continue;
        double circularity = 4.0 * CV_PI * exArea / (experi * experi);
        if (circularity < 0.85 || circularity > 1.18) continue;

        // Cala in_contour circularity
        double inPeri = arcLength(contours[in_cnt], true);
        if (inPeri <= 1e-6) continue;
        double inCircularity = 4.0 * CV_PI * inAera / (inPeri * inPeri);
        if (inCircularity < 0.85 || inCircularity > 1.18) continue;


        //inCounter not a circle
        if (cv::matchShapes(contours[cnt],contours[in_cnt],CONTOURS_MATCH_I1,0)>0.2)
        {
            continue;
        }

        // cala the offset of the two circles
        Point2f oc, ic;
        float orad = 0, irad = 0;
        minEnclosingCircle(contours[cnt],    oc, orad);
        minEnclosingCircle(contours[in_cnt], ic, irad);
        if (orad <= 1e-6) {
            continue;
        }
        double dx = oc.x - ic.x, dy = oc.y - ic.y;
        double offset = std::sqrt(dx * dx + dy * dy) / orad;   //if offset > 0.15 -> getout
        if (offset > 0.2) {
            continue;
        }

        // file too thin or too thick ring
        //double rRatio = (double)irad / orad;
        //if (rRatio < 0.2 || rRatio > 0.9) {
        //    continue;
        //}

        Rect Box = boundingRect(contours[cnt]);
        
        //filt rectangle && ellipse 
        double rate = (double) Box.width/Box.height;
        if(rate>1.2 || rate <0.8) {
            continue;
        }
        targetBox = Box;
    }

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

        cv::imshow("Result", frame);

        // 按 ESC 退出
        int key = waitKey(33);
        if (key == 27)
            break;
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}