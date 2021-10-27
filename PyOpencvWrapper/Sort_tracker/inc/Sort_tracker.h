/*
 * Sort_tracker.h
 *
 *  Created on: 14-02-2019
 *      Author: vadmin
 */

#ifndef SORT_TRACKER_H_
#define SORT_TRACKER_H_

#include "Hungarian.h"
#include "KalmanTracker.h"
#include <opencv2/opencv.hpp>

namespace vtpl
{

typedef struct TrackingBox {
  TrackingBox() : is_ready_for_publish(false) {}
  int frame_id;
  int id;
  cv::Rect_<float> box;
  bool is_ready_for_publish;
  int hist_length;

  // For supporting LPR9
  int obj_type = -1;
  int associated_obj_type = -1;
  cv::Rect associated_obj_rect = cv::Rect(0, 0, 0, 0);
  // int miss_counter;
} TrackingBox;

class Sort_tracker
{
private:
  bool _isFirstFrame;

  // update across frames
  int _frame_count;
  int _max_age;
  int _min_hits;
  double _iouThreshold;
  std::vector<vtpl::KalmanTracker> trackers;

  // variables used in the for-loop
  std::vector<cv::Rect_<float>> predictedBoxes;
  std::vector<std::vector<double>> iouMatrix;
  std::vector<int> assignment;
  set<int> unmatchedDetections;
  set<int> unmatchedTrajectories;
  set<int> allItems;
  set<int> matchedItems;
  std::vector<cv::Point> matchedPairs;
  std::vector<vtpl::TrackingBox> frameTrackingResult;
  unsigned int trkNum = 0;
  unsigned int detNum = 0;

  bool _showMsg; // show message

public:
  Sort_tracker(int max_age, int min_hits, double iou_threshold, bool showMsg);
  virtual ~Sort_tracker();

  int load();
  // Computes IOU between two bounding boxes
  double getIOU(cv::Rect_<float> bb_test, cv::Rect_<float> bb_gt);

  // Computes modified IOU between two bounding boxes
  double getModIOU(cv::Rect_<float> bb_test, cv::Rect_<float> bb_gt,
                   float rc_ext, int height, int width);
  std::vector<vtpl::TrackingBox> getResult(std::vector<vtpl::TrackingBox>,
                                           float rc_ext, int height, int width,
                                           bool iou_mod);
  void unload();
};

} /* namespace vtpl */

#endif /* SORT_TRACKER_H_ */
