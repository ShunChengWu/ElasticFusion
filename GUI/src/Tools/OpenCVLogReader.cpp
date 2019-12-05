/*
 * This file is part of ElasticFusion.
 *
 * Copyright (C) 2015 Imperial College London
 *
 * The use of the code within this file and all code within files that
 * make up the software that is ElasticFusion is permitted for
 * non-commercial purposes only.  The full terms and conditions that
 * apply to the code within this file are detailed within the LICENSE.txt
 * file and at <http://www.imperial.ac.uk/dyson-robotics-lab/downloads/elastic-fusion/elastic-fusion-license/>
 * unless explicitly stated.  By downloading this file you agree to
 * comply with these terms.
 *
 * If you wish to use any of this code for commercial purposes then
 * please email researchcontracts.engineering@imperial.ac.uk.
 *
 */

#include <dirent.h>
#include "OpenCVLogReader.h"
#include <CxxTools/PathTool.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <Utils/Intrinsics.h>
//namespace {
//    std::string CheckEnd(std::string path){
//        if (strcmp(&path[path.length() - 1], "/") != 0) {
//            return path + "/";
//        } else {
//            return path;
//        }
//        return path;
//    };
//    std::vector<std::string> get_files_in_folder(std::string path, std::string type, bool return_full, bool sort) {
//        std::vector<std::string> file_vec;
//        DIR *dir;
//        struct dirent *ent;
//        if ((dir = opendir(path.c_str())) != NULL) {
//            path = CheckEnd(path);
//            while ((ent = readdir(dir)) != NULL) {
//                if (ent->d_name[0] != '.') {
//                    /* print all the files and directories within directory */
//                    //printf ("%s\n", ent->d_name);
//                    file_vec.push_back(return_full ? path + ent->d_name : ent->d_name);
//                }
//            }
//            closedir(dir);
//        } else {
//            /* could not open directory */
//            perror("");
//            exit(EXIT_FAILURE);
//        }
//        if (sort) std::sort(file_vec.begin(), file_vec.end());
//
//        if (type == "") return file_vec;
//
//        std::vector<std::string> filtered;
//        for (auto name: file_vec) {
//            if (name.size() > type.size()) {
//                std::string tmp = name.substr(name.size() - type.size(), type.size());
//                if (tmp == type) filtered.push_back(name);
//            }
//        }
//        return filtered;
//    }
//    std::vector<std::string> splitLine(std::string s , char delimiter) {
//        std::vector<std::string>tokens;
//        std::string token;
//        std::istringstream tokenStream(s);
//        while (std::getline(tokenStream, token, delimiter))
//        {
//            if(token != "")
//                tokens.push_back(token);
//        }
//        return tokens;
//    }
//    std::string removeType(const std::string &name) {
//        auto t = name.find_last_of('.');
//        if(t != std::string::npos)
//            return name.substr(0, t);
//        else
//            return name;
//    }
//}

OpenCVLogReader::OpenCVLogReader(std::string file, bool flipColors)
 : LogReader(file, flipColors)
{
//    assert(pangolin::FileExists(file.c_str()));
//    fp = fopen(file.c_str(), "rb");
    auto path_depth_imgs_ = file + "/depth";
    auto path_color_imgs_ = file + "/photo";
    depth_images =
            tools::PathTool().get_files_in_folder(path_depth_imgs_, "", true, true);
    color_images =
            tools::PathTool().get_files_in_folder(path_color_imgs_, "", true, true);
    if(depth_images.size() != color_images.size())
        throw std::runtime_error("The size of depth images and color images mismatch!.\n");

    currentFrame = 0;
    numFrames = depth_images.size();


    cvColor = cv::imread(color_images.front(), -1);
    if (cvColor.empty()) {
        std::cout << "Error: color image file not read!" << std::endl;
    }
    cvDepthFloat = cv::imread(depth_images.front(), -1);
    if (cvDepthFloat.empty()) {
        std::cout << "Error: depth image file not read!" << std::endl;
    }
    cvDepthFloat.convertTo(cvDepth, CV_16U);
    cv::flip(cvDepth, cvDepth, 1);
//    auto tmp = fread(&numFrames,sizeof(int32_t),1,fp);
//    assert(tmp);

//    depthReadBuffer = new unsigned char[numPixels * 2];
//    imageReadBuffer = new unsigned char[numPixels * 3];
//    decompressionBufferDepth = new Bytef[Resolution::getInstance().numPixels() * 2];
//    decompressionBufferImage = new Bytef[Resolution::getInstance().numPixels() * 3];
}

OpenCVLogReader::~OpenCVLogReader()
{
//    delete [] depthReadBuffer;
//    delete [] imageReadBuffer;
//    delete [] decompressionBufferDepth;
//    delete [] decompressionBufferImage;
//    fclose(fp);
}

void OpenCVLogReader::getBack()
{
//    assert(filePointers.size() > 0);
//
//    fseek(fp, filePointers.top(), SEEK_SET);
//
//    filePointers.pop();

    getCore();
}

void OpenCVLogReader::getNext()
{
//    filePointers.push(ftell(fp));

    getCore();
}

void OpenCVLogReader::getCore()
{
    cvColor = cv::imread(color_images[currentFrame], -1);
    if (cvColor.empty()) {
        std::cout << "Error: depth image file not read!" << std::endl;
    }
    assert(cvColor.isContinuous());
    if(flipColors) cv::cvtColor(cvColor, cvColor, cv::COLOR_RGB2BGR);
    imageSize = cvColor.rows * cvColor.cols;
//    std::cout << cvColor.type() << std::endl;
    rgb = cvColor.data;


    cvDepthFloat = cv::imread(depth_images[currentFrame], -1);
    cvDepthFloat.convertTo(cvDepthFloat, CV_32FC1);
    if (cvDepthFloat.empty()) std::cout << "Error: depth image file not read!" << std::endl;
    double norm;
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            double fx = ( c -  Intrinsics::getInstance().cx()) / Intrinsics::getInstance().fx();
            double fy = ( r - Intrinsics::getInstance().cy()) / Intrinsics::getInstance().fy();
            norm = 1/std::sqrt(fx*fx+fy*fy+1);
            cvDepthFloat.at<float>(r, c) = cvDepthFloat.at<float>(r, c) * norm;
//            printf("%f ",cvDepthFloat.at<float>(r, c));
        }
        printf("\n");
    }

    cvDepthFloat.convertTo(cvDepth, CV_16UC1);
//    for (int r = 0; r < height; ++r) {
//        for (int c = 0; c < width; ++c) {
//            printf("%d ",cvDepth.at<unsigned short>(r, c));
//        }
//        printf("\n");
//    }

    cv::flip(cvDepth, cvDepth, 1);
    depthSize = cvDepth.rows * cvDepth.cols;
    assert(cvDepth.isContinuous());
    depth = (unsigned short*)cvDepth.data;

//
//    auto tmp = fread(&timestamp,sizeof(int64_t),1,fp);
//    assert(tmp);
//
//    tmp = fread(&depthSize,sizeof(int32_t),1,fp);
//    assert(tmp);
//    tmp = fread(&imageSize,sizeof(int32_t),1,fp);
//    assert(tmp);
//
//    tmp = fread(depthReadBuffer,depthSize,1,fp);
//    assert(tmp);
//
//    if(imageSize > 0)
//    {
//        tmp = fread(imageReadBuffer,imageSize,1,fp);
//        assert(tmp);
//    }
//
//    if(depthSize == numPixels * 2)
//    {
//        memcpy(&decompressionBufferDepth[0], depthReadBuffer, numPixels * 2);
//    }
//    else
//    {
//        unsigned long decompLength = numPixels * 2;
//        uncompress(&decompressionBufferDepth[0], (unsigned long *)&decompLength, (const Bytef *)depthReadBuffer, depthSize);
//    }
//
//    if(imageSize == numPixels * 3)
//    {
//        memcpy(&decompressionBufferImage[0], imageReadBuffer, numPixels * 3);
//    }
//    else if(imageSize > 0)
//    {
//        jpeg.readData(imageReadBuffer, imageSize, (unsigned char *)&decompressionBufferImage[0]);
//    }
//    else
//    {
//        memset(&decompressionBufferImage[0], 0, numPixels * 3);
//    }
//
//    depth = (unsigned short *)decompressionBufferDepth;
//    rgb = (unsigned char *)&decompressionBufferImage[0];
//
//    if(flipColors)
//    {
//        for(int i = 0; i < Resolution::getInstance().numPixels() * 3; i += 3)
//        {
//            std::swap(rgb[i + 0], rgb[i + 2]);
//        }
//    }

    currentFrame++;
}

void OpenCVLogReader::fastForward(int frame)
{
//    while(currentFrame < frame && hasMore())
//    {
//        filePointers.push(ftell(fp));
//
//        auto tmp = fread(&timestamp,sizeof(int64_t),1,fp);
//        assert(tmp);
//
//        tmp = fread(&depthSize,sizeof(int32_t),1,fp);
//        assert(tmp);
//        tmp = fread(&imageSize,sizeof(int32_t),1,fp);
//        assert(tmp);
//
//        tmp = fread(depthReadBuffer,depthSize,1,fp);
//        assert(tmp);
//
//        if(imageSize > 0)
//        {
//            tmp = fread(imageReadBuffer,imageSize,1,fp);
//            assert(tmp);
//        }
//
//        currentFrame++;
//    }
}

int OpenCVLogReader::getNumFrames()
{
    return numFrames;
}

bool OpenCVLogReader::hasMore()
{
    return currentFrame + 1 < numFrames;
}


void OpenCVLogReader::rewind()
{
//    if (filePointers.size() != 0)
//    {
//        std::stack<int> empty;
//        std::swap(empty, filePointers);
//    }
//
//    fclose(fp);
//    fp = fopen(file.c_str(), "rb");
//
//    auto tmp = fread(&numFrames,sizeof(int32_t),1,fp);
//    assert(tmp);

    currentFrame = 0;
}

bool OpenCVLogReader::rewound()
{
    return filePointers.size() == 0;
}

const std::string OpenCVLogReader::getFile()
{
    return file;
}

void OpenCVLogReader::setAuto(bool value)
{

}
