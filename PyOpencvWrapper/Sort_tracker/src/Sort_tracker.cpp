/*
 * Sort_tracker.cpp
 *
 *  Created on: 02-04-2020
 *      Author: DSD
 */
#include "Sort_tracker.h"
#include <iostream>
// #include <opencv_utility.h>
namespace vtpl
{

Sort_tracker::Sort_tracker(int max_age, int min_hits, double iou_threshold,
                           bool showMsg)
    : _isFirstFrame(true), _frame_count(0)
{
  std::cout << "Sort_tracker Constructor()..." << std::endl;
  _max_age = max_age;
  _min_hits = min_hits;
  _iouThreshold = iou_threshold;
  _showMsg = showMsg;
}

Sort_tracker::~Sort_tracker()
{
  std::cout << "Sort_tracker Destructor()..." << std::endl;
  trackers.clear();

  // variables used in the for-loop
  predictedBoxes.clear();
  iouMatrix.clear();
  assignment.clear();
  unmatchedDetections.clear();
  unmatchedTrajectories.clear();
  allItems.clear();
  matchedItems.clear();
  matchedPairs.clear();
  frameTrackingResult.clear();
}

int Sort_tracker::load()
{
  std::cout << "Sort_tracker load()..." << std::endl;
  int ret = 0;
  return ret;
}
// Computes IOU between two bounding boxes
double Sort_tracker::getIOU(cv::Rect_<float> bb_test, cv::Rect_<float> bb_gt)
{

  float in = (bb_test & bb_gt).area();
  float un = bb_test.area() + bb_gt.area() - in;

  if (un < DBL_EPSILON)
    return 0;

  return (double)(in / un);
}

// Computes modified IOU between two bounding boxes
double Sort_tracker::getModIOU(cv::Rect_<float> bb_test, cv::Rect_<float> bb_gt,
                               float rc_ext, int height, int width)
{

  int col_ext = int(bb_gt.width * (rc_ext / 2.0f));
  int row_ext = int(bb_gt.height * (2 * rc_ext));

  cv::Rect temp_rect;
  temp_rect.x = MAX(0, bb_gt.x - col_ext);
  temp_rect.y = MAX(0, bb_gt.y - row_ext);
  int t_right = MIN(width - 1, bb_gt.x + bb_gt.width);
  int t_bottom = MIN(height - 1, bb_gt.y + bb_gt.height);
  t_right = MIN(width, t_right + col_ext);
  t_bottom = MIN(height, t_bottom + row_ext);
  temp_rect.width = MAX(0, t_right - temp_rect.x - 1);
  temp_rect.height = MAX(0, t_bottom - temp_rect.y - 1);

  int col_ext1 = int(bb_test.width * (rc_ext / 2.0f));
  int row_ext1 = int(bb_test.height * (2 * rc_ext));

  cv::Rect temp_rect1;
  temp_rect1.x = MAX(0, bb_test.x - col_ext1);
  temp_rect1.y = MAX(0, bb_test.y - row_ext1);
  int t_right1 = MIN(width - 1, bb_test.x + bb_test.width);
  int t_bottom1 = MIN(height - 1, bb_test.y + bb_test.height);
  t_right1 = MIN(width, t_right1 + col_ext1);
  t_bottom1 = MIN(height, t_bottom1 + row_ext1);
  temp_rect1.width = MAX(0, t_right1 - temp_rect1.x - 1);
  temp_rect1.height = MAX(0, t_bottom1 - temp_rect1.y - 1);

  float in = (temp_rect1 & temp_rect).area();
  float un = temp_rect1.area() + temp_rect.area() - in;

  if (un < DBL_EPSILON)
    return 0;

  return (double)(in / un);
}
void Sort_tracker::unload()
{
  std::cout << "Sort_tracker unload()..." << std::endl;
}
std::vector<vtpl::TrackingBox>
Sort_tracker::getResult(std::vector<vtpl::TrackingBox> tracking_box_vec,
                        float rc_ext, int height, int width, bool iou_mod)
{
  if (_showMsg) {
    std::cout << "_max_age :: " << _max_age << "; _min_hits :: " << _min_hits
              << "; _iouThreshold :: " << _iouThreshold << std::endl;
    std::cout << "Sort_tracker getResult()..." << std::endl;
  }
  _frame_count++;

  if (trackers.size() == 0) // the first frame met
  {
    // initialize kalman trackers using first detections.
    int loop_cnt = 0;
    for (std::vector<vtpl::TrackingBox>::iterator it = tracking_box_vec.begin();
         it != tracking_box_vec.end(); it++) {
      KalmanTracker trk = KalmanTracker((it)->box);
      trackers.push_back(trk);
      if (_showMsg) {
        std::cout << (it)->frame_id << "," << loop_cnt++ << "," << (it)->box.x
                  << "," << (it)->box.y << "," << (it)->box.width << ","
                  << (it)->box.height << endl;
      }
    }
    return frameTrackingResult;
  }

  if (_showMsg) {
    for (std::vector<vtpl::TrackingBox>::iterator it = tracking_box_vec.begin();
         it != tracking_box_vec.end(); it++) {
      std::cout << (it)->frame_id << "," << (it)->box.x << "," << (it)->box.y
                << "," << (it)->box.width << "," << (it)->box.height << endl;
    }
  }

  // get predicted locations from existing trackers.
  predictedBoxes.clear();

  for (auto it = trackers.begin(); it != trackers.end();) {
    Rect_<float> pBox = (*it).predict();
    if (pBox.x >= 0 && pBox.y >= 0) {
      predictedBoxes.push_back(pBox);
      it++;
    } else {
      it = trackers.erase(it);
      if (_showMsg) {
        std::cerr << "Box invalid at frame: " << _frame_count << endl;
      }
    }
  }

  // associate detections to tracked object (both represented as bounding boxes)
  // dets : detFrameData[fi]
  trkNum = (uint)predictedBoxes.size();
  detNum = (uint)tracking_box_vec.size();

  iouMatrix.clear();
  iouMatrix.resize(trkNum, vector<double>(detNum, 0));

  for (unsigned int i = 0; i < trkNum;
       i++) // compute iou matrix as a distance matrix
  {
    for (unsigned int j = 0; j < detNum; j++) {
      // use 1-iou because the hungarian algorithm computes a minimum-cost
      // assignment.
      if (iou_mod) {
        iouMatrix[i][j] =
            1 - getModIOU(predictedBoxes[i], tracking_box_vec.at(j).box, rc_ext,
                          height, width);
      } else {
        iouMatrix[i][j] =
            1 - getIOU(predictedBoxes[i], tracking_box_vec.at(j).box);
      }
    }
  }

  // solve the assignment problem using hungarian algorithm.
  // the resulting assignment is [track(prediction) : detection], with
  // len=preNum
  HungarianAlgorithm HungAlgo;
  assignment.clear();
  if (iouMatrix.size() > 0)
    HungAlgo.Solve(iouMatrix, assignment);

  // find matches, unmatched_detections and unmatched_predictions
  if (unmatchedTrajectories.size() > 0)
    unmatchedTrajectories.clear();
  if (unmatchedDetections.size() > 0)
    unmatchedDetections.clear();
  if (allItems.size() > 0)
    allItems.clear();
  if (matchedItems.size() > 0)
    matchedItems.clear();

  if (detNum > trkNum) //	there are unmatched detections
  {
    if (_showMsg)
      std::cout << "New Track Object ID ::" << trkNum + 1 << std::endl;
    for (unsigned int n = 0; n < detNum; n++)
      allItems.insert(n);

    for (unsigned int i = 0; i < trkNum; ++i)
      matchedItems.insert(assignment[i]);

    set_difference(allItems.begin(), allItems.end(), matchedItems.begin(),
                   matchedItems.end(),
                   insert_iterator<set<int>>(unmatchedDetections,
                                             unmatchedDetections.begin()));
  } else if (detNum < trkNum) // there are unmatched trajectory/predictions
  {
    if (_showMsg)
      std::cout << "HERE 2" << std::endl;
    for (unsigned int i = 0; i < trkNum; ++i)
      if (assignment[i] ==
          -1) // unassigned label will be set as -1 in the assignment algorithm
        unmatchedTrajectories.insert(i);
  } else
    ;

  // filter out matched with low IOU
  matchedPairs.clear();
  for (unsigned int i = 0; i < trkNum; ++i) {
    if (assignment[i] == -1) // pass over invalid values
      continue;
    if (1 - iouMatrix[i][assignment[i]] < _iouThreshold) {
      if (_showMsg)
        std::cout << "***************LOW IOU :: " << _iouThreshold
                  << " :: " << (1 - iouMatrix[i][assignment[i]]) << std::endl;
      unmatchedTrajectories.insert(i);
      unmatchedDetections.insert(assignment[i]);
    } else
      matchedPairs.push_back(cv::Point(i, assignment[i]));
  }

  // updating trackers
  // update matched trackers with assigned detections.
  // each prediction is corresponding to a tracker
  int detIdx, trkIdx;
  for (unsigned int i = 0; i < matchedPairs.size(); i++) {
    trkIdx = matchedPairs[i].x;
    detIdx = matchedPairs[i].y;
    trackers[trkIdx].update(tracking_box_vec.at(detIdx).box);
  }

  // create and initialise new trackers for unmatched detections
  for (auto umd : unmatchedDetections) {
    KalmanTracker tracker = KalmanTracker(tracking_box_vec.at(umd).box);
    trackers.push_back(tracker);
  }

  frameTrackingResult.clear();

  // get trackers' output
  for (auto it = trackers.begin(); it != trackers.end();) {
    if (_showMsg) {
      std::cout << "(*it).m_time_since_update :: " << (*it).m_time_since_update
                << "\n (*it).m_hit_streak :: " << (*it).m_hit_streak
                << "\n _min_hits :: " << _min_hits
                << "\n _frame_count :: " << _frame_count << std::endl;
    }

    if (((*it).m_time_since_update < 1) &&
        ((*it).m_hit_streak >= _min_hits || _frame_count <= _min_hits)) {
      TrackingBox res;
      res.is_ready_for_publish = false;
      res.box = (*it).get_state();
      res.id = (*it).m_id + 1;
      res.frame_id = _frame_count;
      if (!(*it).m_is_published) {
        (*it).m_is_published = true;
        res.is_ready_for_publish = true;
      }
      res.hist_length = (*it).m_hit_streak;
      // res.miss_counter = (*it).m_time_since_update;
      frameTrackingResult.push_back(res);
      it++;

    } else
      it++;

    // remove dead tracklet
    if (it != trackers.end() && (*it).m_time_since_update > _max_age) {
      if (_showMsg)
        std::cout << "Deleting the trackid::::" << (*it).m_id << std::endl;
      it = trackers.erase(it);
    }
  }

  return frameTrackingResult;
}

} /* namespace vtpl */
