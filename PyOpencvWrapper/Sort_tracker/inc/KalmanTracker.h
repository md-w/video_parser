///////////////////////////////////////////////////////////////////////////////
// KalmanTracker.h: KalmanTracker Class Declaration

#ifndef KALMAN_H
#define KALMAN_H 2

#include "opencv2/video/tracking.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace vtpl{
	using namespace std;
	using namespace cv;

#define StateType Rect_<float>


	// This class represents the internel state of individual tracked objects observed as bounding box.
	class KalmanTracker
	{
	public:
		KalmanTracker()
		{
			init_kf(StateType());
			m_time_since_update = 0;
			m_hits = 0;
			m_hit_streak = 0;
			m_age = 0;
			m_id = kf_count;
			m_is_published = false;
			// m_obj_type = -1;
			// m_associated_obj_type = -1;
			// m_associated_obj_rect = cv::Rect(0, 0, 0, 0);
			// m_zone_id = -1;

			// // m_trk_start_point = cv::Point(0, 0);
			// // m_trk_end_point = cv::Point(0, 0);

			// m_is_no_helmet = false;
			// m_is_triple_riding = false;
			// m_is_pillion_riding = false;
			// m_nh_tr_pl_rect = cv::Rect(0, 0, 0, 0);

			kf_count++;
		}
		KalmanTracker(StateType initRect)
		{
			init_kf(initRect);
			m_time_since_update = 0;
			m_hits = 0;
			m_hit_streak = 0;
			m_age = 0;
			m_id = kf_count;
			m_is_published = false;
			// m_obj_type = -1;
			// m_associated_obj_type = -1;
			// m_associated_obj_rect = cv::Rect(0, 0, 0, 0);
			// m_zone_id = -1;

			// // m_trk_start_point = cv::Point(0, 0);
			// // m_trk_end_point = cv::Point(0, 0);

			// m_is_no_helmet = false;
			// m_is_triple_riding = false;
			// m_is_pillion_riding = false;
			// m_nh_tr_pl_rect = cv::Rect(0, 0, 0, 0);

			kf_count++;
		}

		~KalmanTracker()
		{
			m_history.clear();
		}

		StateType predict();
		void update(StateType stateMat);

		StateType get_state();
		StateType get_rect_xysr(float cx, float cy, float s, float r);

		static int kf_count;

		int m_time_since_update;
		int m_hits;
		int m_hit_streak;
		int m_age;
		int m_id;
		bool m_is_published;
		// int m_obj_type;
		// int m_associated_obj_type;
		// int m_zone_id;

		// // cv::Point m_trk_start_point;
		// // cv::Point m_trk_end_point;

		// bool m_is_no_helmet;
		// bool m_is_triple_riding;
		// bool m_is_pillion_riding;
		// cv::Rect m_nh_tr_pl_rect;
		// cv::Rect m_associated_obj_rect;

	private:
		void init_kf(StateType stateMat);

		cv::KalmanFilter kf;
		cv::Mat measurement;

		std::vector<StateType> m_history;
	};
}
#endif