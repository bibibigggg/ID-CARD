#pragma once
#include "CDB.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include "MainDlg.h"

class CCamDB : public CLoginDB {
public:
    CCamDB();
    ~CCamDB();

    bool Send_Img2DB(int imageID, const std::string& InputPhoneNum, cv::Mat& img_frame);

    //bool Send_Img2DB(const std::string& imagePath, int imageID, const std::string& InputPhoneNum);
    bool ConvertBlob(cv::Mat& img, std::vector<BYTE>& imageBlob);
    bool CheckImageExists(int imageID);

private:
    std::vector<BYTE> img_Blob; // 이미지 BLOB 데이터를 저장
};
