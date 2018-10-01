#include <Rcpp.h>
#include <vector>

#define UNCOVERED	0
#define COVERED		1

#define UNMARKED	0
#define MARKED		1
#define UNASSIGNED	-1
#define NONE		-1

const std::pair<int,int> NOT_FOUND = std::make_pair(-1,-1);

class assignment_problem {
public:
	int ITER;									//Number of iterations

	//Constructor
	assignment_problem(std::vector<double>);

	void update_rows(std::vector<double>, std::vector<int>);
	void update_cols(std::vector<double>, std::vector<int>);
	
	std::vector<int> get_assignment() {
		return row_assign;
	}

	double get_cost() {
		return cost;
	}

	std::vector<double> get_row_duals() {
		return row_dual;
	}
	
	std::vector<double> get_col_duals() {
		return col_dual;
	}
	
	int get_n() {
		return n;
	}
	
	std::vector<double> get_C() {
		return C;
	}

private:
	int n;										//It's an nxn problem
	int step;									//Step of the Munkres algorithm
	int assignments;							//Number of assignments (<=n)
	double cost;								//Optimal cost
	std::vector<double> C;						//Cost matrix
	std::vector<double> c;						//Slack matrix
	std::vector<int> row_assign;				//The assignment (row i to row_assign[i])
	std::vector<int> col_assign;				//The assignment in reverse (column j to col_assign[j])
	std::vector<double> row_dual;				//Dual row values
	std::vector<double> col_dual;				//Dual column values
	std::vector<int> covered_rows;				//Which rows are covered
	std::vector<int> covered_cols;				//Which columns are covered
	std::vector< std::pair<int, int> > path;	//Augmenting path

	//The Munkres algorithm
	void hungarian();
	void step_one();
	void step_two();
	void step_three();
	void step_four();
	double find_smallest_uncovered_val();

	void update_hungarian();
	void compute_cost();
};

//Define the Rcpp class wrapper
RCPP_MODULE(APMod){
    Rcpp::class_<assignment_problem>( "assignment_problem" )
        .constructor< std::vector<double> >("documentation for default constructor")
        .field( "ITER", &assignment_problem::ITER, "documentation for x")
        .method( "get_assignment", &assignment_problem::get_assignment, "documentation for print1")
        .method( "get_cost", &assignment_problem::get_cost, "documentation for print2")
        .method( "get_row_duals", &assignment_problem::get_row_duals, "documentation for print3")
		.method( "get_col_duals", &assignment_problem::get_col_duals, "documentation for print3")
		.method( "get_n", &assignment_problem::get_n, "documentation for print3")
		.method( "get_C", &assignment_problem::get_C, "documentation for print3")
		.method( "update_rows", &assignment_problem::update_rows, "documentation for print3")
		.method( "update_cols", &assignment_problem::update_cols, "documentation for print3")
        ;
}