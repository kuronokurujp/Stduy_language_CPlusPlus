#ifndef __RENDER_PATH_TRACING_RAYTRACE_SPACE_H__
#define __RENDER_PATH_TRACING_RAYTRACE_SPACE_H__

#include <memory>

#include "Math/ray.h"

#include "Render/PathTracing/Light/light.h"
#include "Render/PathTracing/Camera/camera.h"
#include "Render/PathTracing/Collision/hit_table_list.h"

#include "Render/PathTracing/Collision/bvh_node.h"

// レイトレースを行う空間
class RayTraceSpace
{
public:
	RayTraceSpace(const int in_screen_size, const double in_w_size, const double in_h_size);
	// レイ照射して取得した色を取得
	void OutputRayColor(Color* in_p_out, const Ray& in_r_ray, const int in_depth, const int in_max_depth);

public:

	// ウィンドウを作る
	int _width;
	int _height;

	Color _screen_upper_corrner_color;
	Color _screen_under_corrner_color;

	HitTableList world;
	shared_ptr<BvhNode> bvh_node;
	Camera _camera;
	LightSpace _light_space;
};

#endif // __RENDER_PATH_TRACING_RAYTRACE_SPACE_H__
