#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <stdexcept>
//#include <utility>

#include <Tiger/Matrix.h>

#define IMAX UINTMAX_MAX
#define PI 3.1415
typedef uintmax_t I;
typedef long double F;

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
		return std::pow(p.x-x,2)+std::pow(p.y-y,2);
	}
};

class triplet_app{
	I n_points;
	uint64_t report_flag;
	point* points;
	
	enum solution_code{
		SOLVED,
		TRIVIAL,
		INVALID,
		NOT_SEPARATE
	};


	public:
	
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
		for(I i=0; i<n_points; i++){
			points[i].x = 1e-9 * (rand() % 1000000000);
			points[i].y = 1e-9 * (rand() % 1000000000);
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


	void solve_greedy_walk(F step_factor, F step_length, I iters){

	}

	void solve_naive_walk(F step_factor, F step_length, I iters){
		const F pt = PI/3;
		F* new_values = (F*)malloc(sizeof(F)*n_points*2);
		F max_dev;
		for(I i=0; i<iters; i++){
			max_dev=0;
			for(I n=0; n<n_points; n++){
				point p = points[n];
				point p1 = points[p.p1];
				point p2 = points[p.p2];
				max_dev = std::max(max_dev, std::max(std::abs(p.x), std::abs(p.y)));

				F distance = p1.distance(p2);
				F angle = std::atan2(p2.y-p1.y,p2.x-p1.x);

				F c1 = std::cos(angle-pt);
				F c2 = std::cos(angle+pt);
				F s1 = std::sin(angle-pt);
				F s2 = std::sin(angle+pt);

				point o11(p1.x + distance * c1, p1.y + distance * s1);
				point o22(p1.x + distance * c2, p1.y + distance * s2);

				F o1[] = {p1.x + distance * c1, p1.y + distance * s1};
				F o2[] = {p1.x + distance * c2, p1.y + distance * s2};

				F d1 = std::pow(p.x-o1[0],2)+std::pow(p.y-o1[1],2.0);
				F d2 = std::pow(p.x-o2[0],2)+std::pow(p.y-o2[1],2.0);
				d1 = p.distance(o11);
				d2 = p.distance(o22);
				//std::cout << p.x << " "<< p.y << " " << o1[0] << " "<< o1[1] << " " << o2[0] << " " << o2[1]<< "\n";
				if( d1 < d2 ){
					F fac = std::min(step_factor,step_length/d1);
					new_values[n*2] = p.x + (o11.x-p.x) * fac;
					new_values[n*2+1] = p.y + (o11.y-p.y) * fac;
				}else{
					F fac = std::min(step_factor,step_length/d2);
					new_values[n*2] = p.x + (o22.x-p.x) * fac;
					new_values[n*2+1] = p.y + (o22.y-p.y) * fac;
				}

			}
			for(I n=0; n<n_points; n++){
				points[n].x = new_values[n*2]/max_dev;
				points[n].y = new_values[n*2+1]/max_dev;
			}

			F error = 0;

			for(I x = 0; x<n_points; x++){
				// Could cache p1 and p2 or distances, but it would probably not affect performance much at all 
				point p = points[x];
				error  += std::abs(p.distance(points[p.p1]) - p.distance(points[p.p2])) 
					+ std::abs(p.distance(points[p.p1]) - points[p.p1].distance(points[p.p2]));
					+ std::abs(p.distance(points[p.p2]) - points[p.p1].distance(points[p.p2]));
			}
			
			for(I n1=0; n1<n_points; n1++){
				for(I n2=0; n2<n_points; n2++){
					if(n1!=n2 && points[n1].distance(points[n2]) < 0.001){
						points[n1].x = 1e-9 * (rand() % 1000000000);
						points[n1].y = 1e-9 * (rand() % 1000000000);
					}
				}
			}

			std::cout << "Iteration " << i+1 << " Error: " << error << "\n";
		}	
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

	solution_code query_solved(F epsilon){
		point p;
		bool all_zero = true;
		bool separate = true;
		for(I i=0; i<n_points; i++){
		 	p = points[i];
			if (std::abs( p.distance(points[p.p1]) - p.distance(points[p.p2])) 
			 +  std::abs( p.distance(points[p.p1]) - points[p.p1].distance(points[p.p2])) 
			 +  std::abs( p.distance(points[p.p2]) - points[p.p1].distance(points[p.p2])) > epsilon) 
			return INVALID;
			
			if(std::abs(p.x)  + std::abs(p.y) > epsilon) all_zero = false;
			for(I n=0; n<n_points; n++){
				if(points[n].distance(points[i])<epsilon && n!=i){
					separate = false;
					break;
				}
			}
		}
		if(all_zero){
			return TRIVIAL;
		}
		if(!separate){
			return NOT_SEPARATE;
		}
		return SOLVED;
	}

	std::string query_solved_string(){
		switch(query_solved(0.01)){
		case INVALID:	return "Invalid solution";
		case TRIVIAL:	return "Trivial solution";
		case SOLVED:	return "Successfully solved!";
		case NOT_SEPARATE:  return "Points not separate!";
		}
		return "Bullshit";
	};
};

int main(int arg_count, char *arg_vector[]){
	srand(time(NULL));
	if(arg_count < 4){
		std::cout << "Unknown syntax, use syntax: triplets iterations n_points report_flag \n";
		return 0;
	}
	I iters = std::stoi(arg_vector[1]);
	I n = std::stoi(arg_vector[2]);
	uint64_t report_flag = std::stoi(arg_vector[3]);

	triplet_app app (n, report_flag);
	app.generate_points();
	app.randomize_points();
	app.solve_naive_walk(0.20, 0.20, iters);
	std::cout << app.query_solved_string() << "\n";
	if(report_flag & 0b1){
		app.output_points();
	}
	if(report_flag & 0b10){
		app.output_configuration();
	}
	return 0;
}
