#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <stdexcept>
#include <chrono>
//#include <utility>

#include "atan2.hpp"
#include <Tiger/Matrix.h>

#define IMAX UINTMAX_MAX
typedef uintmax_t I;
typedef double F;
constexpr F PI = 3.14159265359;

typedef hmma::Matrix<hmma::DenseMatrixBase, F> Matrix;

struct point{
	F x;
	F y;
	I p1;
	I p2;

	point(){
		
	}

	point(F x, F y)
		: x(x)
		, y(y)
	{}

	point(I p1, I p2)
		: p1(p1)
		, p2(p2)
	{}

	F distance(point p){
		return std::sqrt(std::pow(p.x-x,2)+std::pow(p.y-y,2));
	}
};

class triplet_app{
	I n_points;
	uint64_t report_flag;
	point* points;
	
	enum solution_code{
		SOLVED,
		TRIVIAL,
		INVALID
	};


	public:

	void report(uint64_t report_flag){
		if(report_flag & 0b1){
			std::cout << "Error:" << get_error() << "\n";
		}
		if(report_flag & 0b10){
			output_points();
		}
		if(report_flag & 0b100){
			output_configuration();
		}
	}
	
	triplet_app(I n, uint64_t flag){
		n_points = n;
		report_flag = flag;
		points = (point*)malloc(sizeof(point)*n_points);
		if(points==nullptr){
			throw std::invalid_argument("Too many points, could not allocate!");
		}
	}

	~triplet_app(){
		free(points);
	}

	void generate_points(){
		for(I i=0; i<n_points; i++){
			I j,k;
			do{
				j = rand() % n_points;
				k = rand() % n_points;
			}while(j==k || i==j || i==k);
			points[i]=point(j,k);
		}	
	}


	Matrix generate_system_matrix(){
		return Matrix(n_points, n_points*(n_points+1)/2);
	}

	I get_ci(I i1, I i2){
		// Combinatorial Index
		// Deterministically determines the index of an element in a dense array of all unique pairings of all points
		//   0 1 2 3 i1
		// 0 0 - - -
		// 1 1 4 - -
		// 2 2 5 7 -
		// 3 3 6 8 9
		// i2
		if(i1 < i2) std::swap(i1, i2);
		if(i1==i2 || i1>n_points) return IMAX;
		return i1*(2*n_points-1-i1)/2 - 1 + i2; // Don't ask...
	}

	auto reverse_combinatorial_index(I n){
		//if(n > (n_points+1)*n_points/2) return {IMAX, IMAX};
		

	}

	void randomize_points(){
		F scalar = (F)1 / RAND_MAX;
		for(I i=0; i<n_points; i++){
			points[i].x = scalar * (F) rand();
			points[i].y = scalar * (F) rand();
		}
	}


	void solve_newton(F k, I iterations){
		I matrix_width = (n_points+1)*n_points/2*2; // Both x and y
		I matrix_height = n_points*2; // p -> p1 == p -> p2 and p -> p1 == p1 -> p2
		F* new_values = (F*)malloc(sizeof(F)*n_points*2);
	
		for(I y=0; y<matrix_height; y++){

		}

		for(I i=0; i<iterations; i++){

			for(I x = 0; x<n_points; x++){
				point p = points[x];
				point p1 = points[p.p1];
				point p2 = points[p.p2];
				//F e1 = std::abs(p.distance(p.p1) - p.distance(p.p2));
				//F e2 = std::abs(p.distance(p.p1) - p.p1.distance(p.p2));
				//F dx1 = 2*(p2.x - p1.x); // D((x-x1)^2 - (x-x2)^2) = 2x - 2x1 - ( 2x - 2x2) = 2x2 - 2x1
				//F dx2 = 2*(p.x - p1.x); // D((x-x1)^2 - (x1-x2)^2) = 2x - 2x1 - 0 = 2x - 2x1
				//F dy1 = 2*(p2.y - p1.y);
				//F dy1 = 2*(p.x - p1.x);
				F d1 = p.distance(p1);
				F d2 = p.distance(p2);
				F d3 = p1.distance(p2);

				F e = std::abs(d1-d2) + std::abs(d2-d3) + std::abs(d1-d3); 
				F dx = 2*(p.x + p2.x - 2*p1.x);
				F dy = 2*(p.y + p2.y - 2*p1.y);
				new_values[2*x]= p.x - k*e/dx;
				new_values[2*x+1]= p.y - k*e/dy;
			}

			for(I x = 0; x<n_points; x++){
				points[x].x=new_values[2*x];
				points[x].y=new_values[2*x+1];
			}
			
			F error = 0;

			for(I x = 0; x<n_points; x++){
				// Could cache p1 and p2 or distances, but it would probably not affect performance much at all 
				point p = points[x];
				error  += std::abs(p.distance(points[p.p1]) - p.distance(points[p.p2])) 
					+ std::abs(p.distance(points[p.p1]) - points[p.p1].distance(points[p.p2]));
					+ std::abs(p.distance(points[p.p2]) - points[p.p1].distance(points[p.p2]));
			}

			std::cout << "Iteration " << i+1 << " Error: " << error << "\n";
		}



		free(new_values);
	}

	void solve_buchberger(){}

	F get_error(){
		F error = 0;

		for(I x = 0; x<n_points; x++){
			// Could cache p1 and p2 or distances, but it would probably not affect performance much at all 
			point p = points[x];
				error  += std::abs(p.distance(points[p.p1]) - p.distance(points[p.p2])) 
						+ std::abs(p.distance(points[p.p1]) - points[p.p1].distance(points[p.p2]))
					    + std::abs(p.distance(points[p.p2]) - points[p.p1].distance(points[p.p2]));
		}
		return error;
	}

	void solve_greedy_walk(F step_factor, F step_length, I iters){

	}

	I solve_naive_walk(F step_factor, F step_length, I iters, F epsilon){
		static F* new_values;
		const F pt = PI/3;
		if (new_values == nullptr){
			new_values= (F*)malloc(sizeof(F)*n_points*2);
		}
		F max_dev;
		for(I i=0; i<iters; i++){
			max_dev=0;
			for(I n=0; n<n_points; n++){
				point p = points[n];
				point p1 = points[p.p1];
				point p2 = points[p.p2];
				max_dev = std::max(max_dev, std::max(std::abs(p.x), std::abs(p.y)));

				F distance = p1.distance(p2);
				F angle = fast_atan2(p2.y-p1.y,p2.x-p1.x);
				#if true
				bool c = fast_atan2(p.y-p1.y , p.x-p1.x) < angle;
				F offset = c ? -pt : pt;
				F x = p1.x + distance *std::cos(angle+offset);
				F y = p1.y + distance *std::sin(angle+offset);
				F d = std::sqrt(std::pow(p.x-x,2)+std::pow(p.y-y,2));
				F fac = std::min(step_factor,step_length/(d));
				new_values[n*2] = p.x + (x-p.x) * fac;
				new_values[n*2+1] = p.y + (y-p.y) * fac;
				
				#else
				F x1 = p1.x + distance *std::cos(angle+pt);
				F x2 = p1.x + distance *std::cos(angle-pt);
				F y1 = p1.y + distance *std::sin(angle+pt);
				F y2 = p1.y + distance *std::sin(angle-pt);
				F d1 = std::pow(p.x-x1,2)+std::pow(p.y-y1,2);
				F d2 = std::pow(p.x-x2,2)+std::pow(p.y-y2,2);
				if( d1 < d2 ){ // Branchless with 3 ternaries did not seem to be faster
					F fac = std::min(step_factor,step_length/std::sqrt(d1));
					new_values[n*2] = p.x + (x1-p.x) * fac;
					new_values[n*2+1] = p.y + (y1-p.y) * fac;
					//if(bool c = std::sin(std::atan2(p.y-p1.y, p.x-p1.x)-angle) < 0) std::cout << "Disagrees\n";
				}else{
					F fac = std::min(step_factor,step_length/std::sqrt(d2));
					new_values[n*2] = p.x + (x2-p.x) * fac;
					new_values[n*2+1] = p.y + (y2-p.y) * fac;
				}
				#endif

			}
			for(I n=0; n<n_points; n++){
				points[n].x = new_values[n*2]/max_dev;
				points[n].y = new_values[n*2+1]/max_dev;
			}
			
			for(I n1=0; n1<n_points; n1++){
				for(I n2=0; n2<n_points; n2++){
					if(n1!=n2 && points[n1].distance(points[n2]) < 0.001){
						const F scalar = (F)1 / RAND_MAX;
						points[n1].x = scalar * (F) rand();
						points[n1].y = scalar * (F) rand();
					}
				}
			}
			if( get_error() < epsilon) return i;

			//std::cout << "Iteration " << i+1 << " Error: " << error << "\n";
		}	
		//free(new_values);
		return iters;
	} 


	void output_points(){

		for(I n=0; n<n_points; n++){
			std::cout << points[n].x<< " " << points[n].y <<"\n";
		}
	}

	void output_configuration(){
		for(I n=0; n<n_points; n++){
			std::cout << points[n].p1 << " " << points[n].p2 << "\n";
		}
	}

	solution_code check_error(F epsilon){
		point p;
		for(I i=0; i<n_points; i++){
		 	p = points[i];
			if (std::abs( p.distance(points[p.p1]) - p.distance(points[p.p2])) 
			 +  std::abs( p.distance(points[p.p1]) - points[p.p1].distance(points[p.p2])) 
			 +  std::abs( p.distance(points[p.p2]) - points[p.p1].distance(points[p.p2])) > epsilon) 
			return INVALID;
			
			for(I n=0; n<n_points; n++){
				if(points[n].distance(points[i])<epsilon && n!=i){
					return TRIVIAL;
				}
			}
		}
		return SOLVED;
	}

	bool is_solved(){
		return check_error(0.001) == SOLVED;
	}

	std::string query_solved_string(){

		switch(check_error(0.01)){
		case INVALID:	return "Invalid solution";
		case TRIVIAL:	return "Trivial solution";
		case SOLVED:	return "Successfully solved!";
		}
		return "Bullshit";
	};

	void run_perf_test(I n_tests, I depth, I steps){
		std::chrono::system_clock::time_point start, stop;
		I successes = 0;
		I solve = 0;
		start = std::chrono::high_resolution_clock::now();
		for(I x=0; x<n_tests; x++){
			generate_points();
			for(I y=0; y<depth; y++){
				randomize_points();
				solve_naive_walk(0.33, 0.50, steps, 0.001); // For n=3,  step factor of 1/3 is optimal according to testing, step length seems to be whatever
															// For n=4,  step factor of 0.40-0.60 and step length 0.5-1.0
															// For n=5,  step factor of 0.40-0.60 and step length 0.4-1.0
															// For n=6,  step factor of 0.30-0.60 and step length 0.2-1.0
															// For n=7,  step factor of 0.30-0.60 and step length 0.4-1.0
															// For n=8,  step factor of 0.40-0.60 and step length 0.3-1.0
															// For n=9,  step factor of 0.35-0.65 and step length 0.1-0.9
															// For n=10, step factor of 0.35-0.55 and step length 0.2-0.8
															// For n=11, step factor of 0.20-0.60 and step length 0.2-0.65
															// For n=12, step factor of 0.35-0.50 and step length 0.35-0.85
				successes += is_solved();
			}
		}
		stop = std::chrono::high_resolution_clock::now();
		solve += std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();

		std::cout << "Solved " << successes << "/" << n_tests*depth << " : " << (float) successes / (float) n_tests / (float) depth << "\n";
		std::cout << "Took " << (F)solve / 1E9 << " seconds in total" << "\n";
		std::cout << "Each step took an average of "<< (F)solve / (F)(n_tests*depth*steps) << " ns\n";
	}

	void run_parameter_test(I depth, F fac_scale, F max_scale, F epsilon){
		const I threshold = 100;
		I iters;
		
		F min_fac;
		F min_scale;
		I min_iters=threshold*depth;
		I max_succeses = 0;
		for(F x=fac_scale; x<1; x+=fac_scale){
			for(F y=max_scale; y<1; y+=max_scale){
				I successes = 0;
				I total=0;
				for(I i=0; i<depth; i++){
					generate_points();
					randomize_points();
					iters = solve_naive_walk(x, y, threshold, 0.001);
					successes += is_solved();
					total += is_solved() ? iters : 0;
				}
				std::cout << successes << "\n";
				if(successes > max_succeses || (successes==depth && total < min_iters)){
					min_iters=total;
					min_fac = x;
					min_scale = y;
					max_succeses = successes;
				}
				//std::cout << "Factor "<< x << " & length " << y << " yielded an average of " << (F) total / (F) successes 
				//	<< " iterations over " << successes << " successes out of " << depth << " total tests\n"; 
			}
		}
		std::cout << "Optimal factor and length found was " << min_fac << ", " << min_scale << "\n";
	}
};



int main(int arg_count, char *arg_vector[]){
	srand(time(NULL));
	if(arg_count < 6){
		std::cout << "Unknown syntax, use syntax: triplets n_points max_steps n_tests test_tries report_flag \n";
		return 0;
	}
	I n_points = std::stoi(arg_vector[1]);
	I steps = std::stoi(arg_vector[2]);
	I n_tests = std::stoi(arg_vector[3]);
	I test_depth = std::stoi(arg_vector[4]);
	uint64_t report_flag = std::stoi(arg_vector[5]);

	triplet_app app (n_points, report_flag);
	app.generate_points();
	I successes = 0;
	app.randomize_points();
	//app.run_perf_test(n_tests, test_depth, steps);

	app.run_parameter_test(test_depth, 0.05, 0.05, 0.01);
	

	
	return 0;
}
