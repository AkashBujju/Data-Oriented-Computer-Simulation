#ifndef SIM_H
#define SIM_H

#include "math.h"

typedef struct Car {
	int id;
	Vector3 position;
	Vector3 start_position;
	Vector3 to_follow_position;
	float should_stop;
	float current_relax_secs;
	float vel;
} Car;

typedef struct Road {
	int id;
	int left_lane_id;
	int right_lane_id;
	int to_forward_id;
	int to_backward_id;
	Vector3 position;
} Road;

typedef struct Junction {
	int id;
	int to_left_id;
	int to_right_id;
	int to_up_id;
	int to_down_id;
	int to_top_left_signal_id;
	int to_down_left_signal_id;
	int to_top_right_signal_id;
	int to_down_right_signal_id;
	Vector3 position;
} Junction;

typedef struct Signal {
	int id;
	int main_junction_id;
	Vector3 position;
} Signal;

#endif
