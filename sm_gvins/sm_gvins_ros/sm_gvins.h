#pragma once

#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <mutex>
#include <thread>
#include <opencv2/opencv.hpp>
#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>
#include <cv_bridge/cv_bridge.h>

#include "drawer_rviz.h"

class SM_GVINS{
   public:
    struct Options{
        std::string image0_topic_;
        std::string image1_topic_;
        std::string output_path_;
        int image_width_;
        int image_height_;
        cv::Mat K_;  // Camera intrinsic matrix
        cv::Mat D_;  // Distortion coeffs
        cv::Mat Tbc0_;
        cv::Mat Tbc1_;
    };
    
    SM_GVINS(ros::NodeHandle& nh, const Options& options = Options());

    ~SM_GVINS();

    void img0_callback(const sensor_msgs::ImageConstPtr &img_msg)
    {
        std::lock_guard<std::mutex> lock(m_buf_);
        img0_buf_.push(img_msg);
    }

    void img1_callback(const sensor_msgs::ImageConstPtr &img_msg)
    {
        std::lock_guard<std::mutex> lock(m_buf_);
        img1_buf_.push(img_msg);
    }

   private:
    void sync_process();

    cv::Mat getImageFromMsg(const sensor_msgs::ImageConstPtr &img_msg);

    Options options_;

    ros::Subscriber image0_sub_;
    ros::Subscriber image1_sub_;

    std::mutex m_buf_;
    std::queue<sensor_msgs::ImageConstPtr> img0_buf_;
    std::queue<sensor_msgs::ImageConstPtr> img1_buf_;

    std::atomic<bool> running_{true};
    std::thread sync_thread_;

    DrawerRviz drawer_;
};