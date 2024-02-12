#include "RayTracer.h";
#include <glm/gtx/string_cast.hpp>
void RayTracer::Raytrace(Camera cam, RTScene& scene, Image& image) {
	int w = image.width; int h = image.height;
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			Ray ray = RayThruPixel(cam, i, j, w, h);
			Intersection hit = Intersect(ray, scene);
			if (hit.dist >= 0) {
				image.pixels[abs(j - h) * w + i] = FindColor(hit, 1, scene);
			}
		}
	}
}; //page 9
Ray RayTracer::RayThruPixel(Camera cam, int i, int j, int width, int height) {
	float a = width / height;
	float alpha = 2 * (i + 0.5) / width - 1;
	float beta = 1 - 2 * (j + 0.5) / height;
	auto w = (cam.eye - cam.target) / length(cam.eye - cam.target);
	auto u = cross(cam.up,w) / length(cross(cam.up,w));
	auto v = cross(u, w);
	auto p0 = cam.eye;
	float angleFovy = cam.fovy * M_PI / 180.0f;
	auto d = normalize(a * alpha * tan(angleFovy / 2.0f) * u + beta * tan(angleFovy / 2.0f) * v - w);
	return Ray(p0, d);
};//page 10, 18
Intersection RayTracer::Intersect(Ray ray, Triangle& triangle) {
	vec3 p0 = ray.p0;
	vec3 p1 = triangle.P[0];
	vec3 p2 = triangle.P[1];
	vec3 p3 = triangle.P[2];
	vec3 d = ray.dir;
	vec3 n1 = triangle.N[0];
	vec3 n2 = triangle.N[1];
	vec3 n3 = triangle.N[2];
	mat4 A;
	A[0] = glm::vec4(p1, 1.0f);
	A[1] = glm::vec4(p2, 1.0f);
	A[2] = glm::vec4(p3, 1.0f),
	A[3] = glm::vec4(-d, 0.0f);
	vec4 C = vec4(p0, 1.0f);
	std::cout << glm::to_string(A) << std::endl;
	std::cout << glm::to_string(C) << std::endl;
	vec4 ans = inverse(A) * C;
	vec3 q = ans.x * p1 + ans.y * p2 + ans.z * p3;
	vec3 n = glm::normalize(ans.x*n1+ans.y*n2+ans.z*n3);
	float distance = ans.w;
	//std::cout <<glm::to_string(ans)<< std::endl;
	if (ans.x < 0||ans.y<0||ans.z<0||ans.w<0) {
		//std::cout << 22222 << std::endl;
		distance = -1.0f;//There is a miss;
	}
	Intersection inter = Intersection(q, n, d, &triangle, distance);
	return inter;
	
	}; //page 30, 33
Intersection RayTracer::Intersect(Ray ray, RTScene& scene) {
	float mindist = INFINITY;
	Intersection hit;
	hit.dist = -1.0f;
	for(auto& object : scene.triangle_soup) { // Find closest intersection; test all objects
		Intersection hit_temp = Intersect(ray, object);
		if (hit_temp.dist < mindist && hit_temp.dist >= 0) { // closer than previous hit
			std::cout << "111" << std::endl;
			mindist = hit_temp.dist;
			hit = hit_temp;
		}
	}
	//std::cout << glm::to_string(hit.triangle->material->ambient) << std::endl;
	return hit;
}; //page 11, 28, 31

glm::vec3 RayTracer::FindColor(Intersection hit, int recursion_depth, RTScene& scene) {
	if (hit.dist != INFINITY) {
		vec3 ambient = vec3(hit.triangle->material->ambient.x, hit.triangle->material->ambient.y, hit.triangle->material->ambient.z);
		vec3 diffuse = vec3(hit.triangle->material->diffuse.x, hit.triangle->material->diffuse.y, hit.triangle->material->diffuse.z);
		vec3 specular = vec3(hit.triangle->material->specular.x, hit.triangle->material->specular.y, hit.triangle->material->specular.z);
		vec3 emision = vec3(hit.triangle->material->emision.x, hit.triangle->material->emision.y, hit.triangle->material->emision.z);
		float shininess = hit.triangle->material->shininess;
		vec3 sum = vec3(0.0f,0.0f,0.0f);
		for (auto li : scene.light) {
			vec3 Lightcolor = li.second->color;
			vec3 direction_of_light = li.second->position;//NOT SURE;
			vec3 n = hit.N;
			vec3 hj = normalize(scene.camera->eye_default + direction_of_light),
			sum = sum + (ambient
				+ diffuse * max(dot(n,direction_of_light), 0.0f)
				+ specular * pow(max(dot(n, hj), 0.0f), shininess))
				* Lightcolor;
		}
		return sum;
	}
	/*
	* vec3 color;
	if (times > recursion_depth) { return color; }
	Ray ray2 = xxxx;
	Intersection hit2 = Intersect(ray2, scene);
	times++;
	auto specular = scene.triangle_soup[times].material->specular;
	color += glm::vec3(specular.x, specular.y, specular.z) * FindColor(hit2, recursion_depth, scene);
	return color;
	*/
}; //page 15