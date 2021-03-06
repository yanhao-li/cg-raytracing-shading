// C++ include
#include <iostream>
#include <string>
#include <vector>

// Utilities for the Assignment
#include "utils.h"

// Image writing library
#define STB_IMAGE_WRITE_IMPLEMENTATION // Do not include this line twice in your project!
#include "stb_image_write.h"


// Shortcut to avoid Eigen:: everywhere, DO NOT USE IN .h
using namespace Eigen;

int img_size = 800;

void raytrace_sphere() {
	std::cout << "Simple ray tracer, one sphere with orthographic projection" << std::endl;

	const std::string filename("sphere_orthographic.png");
	MatrixXd C = MatrixXd::Zero(img_size,img_size); // Store the color
	MatrixXd A = MatrixXd::Zero(img_size,img_size); // Store the alpha mask

	// parallel projection
	// The camera is orthographic, pointing in the direction -z and covering the unit square (-1,1) in x and y
	Vector3d origin(-1,1,1);
	//two axis
	//image size / c.col() = length per pixel.
	Vector3d x_displacement(2.0/C.cols(),0,0);
	Vector3d y_displacement(0,-2.0/C.rows(),0);

	// Single light source
	const Vector3d light_position(-1,1,1);

	for (unsigned i=0; i < C.cols(); ++i) {
		for (unsigned j=0; j < C.rows(); ++j) {
			// Prepare the ray for that pixel(i, j)
			Vector3d ray_origin = origin + double(i)*x_displacement + double(j)*y_displacement;
			Vector3d ray_direction = RowVector3d(0,0,-1);

			// Intersect with the sphere
			// NOTE: this is a special case of a sphere centered in the origin and for orthographic rays aligned with the z axis
			Vector2d ray_on_xy(ray_origin(0),ray_origin(1));
			const double sphere_radius = 1;

			if (ray_on_xy.norm() < sphere_radius) {
				// The ray hit the sphere, compute the exact intersection point
				Vector3d ray_intersection(ray_on_xy(0),ray_on_xy(1),sqrt(sphere_radius*sphere_radius - ray_on_xy.squaredNorm()));

				// Compute normal at the intersection point
				Vector3d ray_normal = ray_intersection.normalized();

				// Simple diffuse model
				C(i,j) = (light_position-ray_intersection).normalized().transpose() * ray_normal;

				// Clamp to zero
				C(i,j) = std::max(C(i,j),0.);

				// Disable the alpha mask for this pixel
				A(i,j) = 1;
			}
		}
	}

	// Save to png
	write_matrix_to_png(C,C,C,A,filename);

}

void raytrace_parallelogram() {
	std::cout << "Simple ray tracer, one parallelogram with orthographic projection" << std::endl;

	const std::string filename("plane_orthographic.png");
	MatrixXd C = MatrixXd::Zero(img_size,img_size); // Store the color
	MatrixXd A = MatrixXd::Zero(img_size,img_size); // Store the alpha mask

	// The camera is orthographic, pointing in the direction -z and covering the unit square (-1,1) in x and y
	Vector3d origin(-3,3,1);
	Vector3d x_displacement(6.0/C.cols(),0,0);
	Vector3d y_displacement(0,-6.0/C.rows(),0);

	// TODO: Parameters of the parallelogram (position of the lower-left corner + two sides)
	Vector3d pgram_origin(-2,-1,0);
	// vector ab
	Vector3d pgram_u = Vector3d(1,-1,0) - pgram_origin;
	// vector ad
	Vector3d pgram_v = Vector3d(-1,1,0) - pgram_origin;

	// Single light source
	const Vector3d light_position(-3, 3, 1);

	for (unsigned i=0; i < C.cols(); ++i) {
		for (unsigned j=0; j < C.rows(); ++j) {
			// Prepare the ray
			Vector3d ray_origin = origin + double(i) * x_displacement + double(j) * y_displacement;
			Vector3d ray_direction = RowVector3d(0,0,-1);

			// TODO: Check if the ray intersects with the parallelogram
			Matrix3d Am;
			Am << pgram_u(0), pgram_v(0), -ray_direction(0),  
				   pgram_u(1), pgram_v(1), -ray_direction(1),  
				   pgram_u(2), pgram_v(2), -ray_direction(2);
					 
			Vector3d bm;
			Vector3d ae = ray_origin - pgram_origin;
			bm << ae(0), ae(1), ae(2);

			// Solve Ax = B
			Vector3d x = Am.colPivHouseholderQr().solve(bm);
			double u = x(0), v = x(1), t = x(2);

			if (u >= 0 && u <= 1 && v >= 0 && v <= 1 && t > 0) {
				// TODO: The ray hit the parallelogram, compute the exact intersection point
				Vector3d ray_intersection = ray_origin + t * ray_direction;

				// TODO: Compute normal at the intersection point
				Vector3d ray_normal = pgram_u.cross(pgram_v).normalized();

				// Simple diffuse model
				C(i,j) = (light_position - ray_intersection).normalized().transpose() * ray_normal;

				// Clamp to zero
				C(i,j) = std::max(C(i,j),0.);

				// Disable the alpha mask for this pixel
				A(i,j) = 1;
			}
		}
	}

	// Save to png
	write_matrix_to_png(C,C,C,A,filename);
}

void raytrace_perspective() {
	std::cout << "Simple ray tracer, one parallelogram with perspective projection" << std::endl;

	const std::string filename("plane_perspective.png");
	MatrixXd C = MatrixXd::Zero(img_size,img_size); // Store the color
	MatrixXd A = MatrixXd::Zero(img_size,img_size); // Store the alpha mask

	// The camera is perspective, pointing in the direction -z and covering the unit square (-1,1) in x and y
	Vector3d origin(0,0,10);
	Vector3d x_displacement(6.0/C.cols(),0,0);
	Vector3d y_displacement(0,-6.0/C.rows(),0);

	// TODO: Parameters of the parallelogram (position of the lower-left corner + two sides)
	Vector3d pgram_origin(2,-1,-4);
	// vector ab
	Vector3d pgram_u = Vector3d(5,-3,-2) - pgram_origin;
	// vector ad
	Vector3d pgram_v = Vector3d(0,-3,-2) - pgram_origin;

	// Single light source
	const Vector3d light_position(3,-5,3);

	for (unsigned i=0; i < C.cols(); ++i) {
		for (unsigned j=0; j < C.rows(); ++j) {
			// TODO: Prepare the ray (origin point and direction)
			Vector3d ray_origin = origin;
			Vector3d ray_direction = Vector3d(0,0,-1) * double(10) + double(i) * x_displacement + double(j) * y_displacement;
			
			// TODO: Check if the ray intersects with the parallelogram
			Matrix3d Am;
			Am << pgram_u(0), pgram_v(0), -ray_direction(0),  
				   pgram_u(1), pgram_v(1), -ray_direction(1),  
				   pgram_u(2), pgram_v(2), -ray_direction(2);
					 
			Vector3d bm;
			Vector3d ae = ray_origin - pgram_origin;
			bm << ae(0), ae(1), ae(2);

			// Solve Ax = B
			Vector3d x = Am.colPivHouseholderQr().solve(bm);
			double u = x(0), v = x(1), t = x(2);

			if (u >= 0 && u <= 1 && v >= 0 && v <= 1 && t > 0) {
				// TODO: The ray hit the parallelogram, compute the exact intersection point
				Vector3d ray_intersection = ray_origin + t * ray_direction;;

				// TODO: Compute normal at the intersection point
				Vector3d ray_normal = pgram_v.cross(pgram_u).normalized();

				// Simple diffuse model
				C(i,j) = (light_position-ray_intersection).normalized().transpose() * ray_normal;

				// Clamp to zero
				C(i,j) = std::max(C(i,j),0.);

				// Disable the alpha mask for this pixel
				A(i,j) = 1;
			}
		}
	}

	// Save to png
	write_matrix_to_png(C,C,C,A,filename);
}

void raytrace_shading(){
	std::cout << "Simple ray tracer, one sphere with different shading" << std::endl;

	const std::string filename("shading.png");
	MatrixXd CR = MatrixXd::Zero(img_size,img_size); // Store the color
	MatrixXd CB = MatrixXd::Zero(img_size,img_size); // Store the color
	MatrixXd CG = MatrixXd::Zero(img_size,img_size); // Store the color
	MatrixXd A = MatrixXd::Zero(img_size,img_size); // Store the alpha mask

	// The camera is perspective, pointing in the direction -z and covering the unit square (-1,1) in x and y
	Vector3d origin(-1,1,1);
	Vector3d x_displacement(2.0/CR.cols(),0,0);
	Vector3d y_displacement(0,-2.0/CR.rows(),0);

	// Single light source
	const Vector3d light_position(-1,1,1);
	double ambient = 0.1;
	MatrixXd diffuse = MatrixXd::Zero(img_size, img_size);
	MatrixXd specular = MatrixXd::Zero(img_size, img_size);

	for (unsigned i=0; i < CR.cols(); ++i) {
		for (unsigned j=0; j < CR.rows(); ++j) {
			// Prepare the ray
			Vector3d ray_origin = origin + double(i)*x_displacement + double(j)*y_displacement;
			Vector3d ray_direction = RowVector3d(0,0,-1);

			// Intersect with the sphere
			// NOTE: this is a special case of a sphere centered in the origin and for orthographic rays aligned with the z axis
			Vector2d ray_on_xy(ray_origin(0),ray_origin(1));
			const double sphere_radius = 0.9;

			if (ray_on_xy.norm() < sphere_radius) {
				// The ray hit the sphere, compute the exact intersection point
				Vector3d ray_intersection(ray_on_xy(0),ray_on_xy(1),sqrt(sphere_radius*sphere_radius - ray_on_xy.squaredNorm()));

				// Compute normal at the intersection point
				Vector3d ray_normal = ray_intersection.normalized();

				// TODO: Add shading parameter here
				diffuse(i,j) = (light_position-ray_intersection).normalized().transpose() * ray_normal;
				specular(i,j) = pow(ray_normal.transpose() * (ray_origin + light_position - ray_intersection).normalized(), 100000);

				// Simple diffuse model
				CR(i,j) = (235.0 / 255) * (ambient + diffuse(i,j) + specular(i,j));
				CG(i,j) = (183.0 / 255) * (ambient + diffuse(i,j) + specular(i,j));
				CB(i,j) = (52.0 / 255) * (ambient + diffuse(i,j) + specular(i,j));

				// Clamp to zero
				CR(i,j) = std::max(CR(i,j),0.);
				CG(i,j) = std::max(CG(i,j),0.);
				CB(i,j) = std::max(CB(i,j),0.);

				// Disable the alpha mask for this pixel
				A(i,j) = 1;
			}
		}
	}

	// Save to png
	write_matrix_to_png(CR,CG,CB,A,filename);
}

int main() {
	raytrace_sphere();
	raytrace_parallelogram();
	raytrace_perspective();
	raytrace_shading();

	return 0;
}
