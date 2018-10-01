#include "dynhung.h"
#include <iostream> //cout
#include <algorithm> //transform, fill_n

assignment_problem::assignment_problem(std::vector<double> cost_matrix) {
	C = cost_matrix;
	n = (int) sqrt(C.size());
	ITER = 0;

	if (n != sqrt(C.size())) {
		throw std::length_error("cost matrix not square");
	}

	hungarian();
}


void assignment_problem::hungarian() {
	//Fill with zeros
	covered_rows.resize(n);
	covered_cols.resize(n);

	//Initialise assignments
	std::vector<int> unassigned(n, UNASSIGNED);
	row_assign = unassigned;
	col_assign = unassigned;

	//Copy the cost matrix into the slack matrix
	c = C;
	
	//Start at the beginning
	step = 1;
	assignments = 0;
	
	while (assignments < n) {
		if (step == 1) {
			step_one();
		} else if (step == 2) {
			step_two();
		} else if (step == 3) {
			step_three();
		} else if (step == 4) {
			step_four();
		}
				
		ITER++;
	}

	compute_cost();
}

void assignment_problem::update_hungarian() {
	//Recompute the slack matrix
	c.clear();
	c.resize(n*n);

	//Repair the slack matrix (fix tiny errors due to rounding errors)
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			double slack = C[i*n + j] - row_dual[i] - col_dual[j];
			if (row_assign[i] == j) {
				slack = 0;
			}
			if (slack < 0) {
				slack = 0;
			}
			if (slack < 1e-15) {
				slack = 0;
			}
			
			c[i*n + j] = slack;
		}
		std::cout << std::endl;
	}	

	//Continue with the Hungarian
	step = 3;
	ITER = 0;

	while (assignments < n) {
		if (step == 3) {
			step_three();
		} else if (step == 4) {
			step_four();
		}
		ITER++;
	}

	compute_cost();
}

void assignment_problem::compute_cost() {
	cost = 0;
	for (int i = 0; i < n; i++) {
		int j = row_assign[i];
		cost += C[i*n + j];
	}
}

//Note: the user must indicate which rows changed!
void assignment_problem::update_rows(std::vector<double> cost_matrix, std::vector<int> rows_changed) {
	for (unsigned i = 0; i < rows_changed.size(); i++) {
		if (rows_changed[i] < 0 || rows_changed[i] >= n) {
			throw std::domain_error("Row index is out of bounds.");
		}		
	}
	C = cost_matrix;
	
	for (unsigned i = 0; i < rows_changed.size(); i++) {
		int row_change = rows_changed[i];
				
		//Remove the ith edge
		int the_col = row_assign[row_change];
		if (the_col != UNASSIGNED) {
			assignments--;
			col_assign[the_col] = UNASSIGNED;
			row_assign[row_change] = UNASSIGNED;
		}
				

		//Update the ith dual value
		double smallest = DBL_MAX;
		for (int j = 0; j < n; j++) {
			double slack = C[row_change*n + j] - col_dual[j];
			if (slack < smallest) {
				smallest = slack;
			}
		}
		row_dual[row_change] = smallest;
	}
	
	update_hungarian();
}

//Note: the user must indicate which columns changed!
void assignment_problem::update_cols(std::vector<double> cost_matrix, std::vector<int> cols_changed) {
	for (unsigned i = 0; i < cols_changed.size(); i++) {
		if (cols_changed[i] < 0 || cols_changed[i] >= n) {
			throw std::domain_error("Column index is out of bounds.");
		}		
	}
	C = cost_matrix;
	
	for (unsigned i = 0; i < cols_changed.size(); i++) {
		int col_change = cols_changed[i];
				
		//Remove the ith edge
		int the_row = col_assign[col_change];
		if (the_row != UNASSIGNED) {
			assignments--;
			row_assign[the_row] = UNASSIGNED;
			col_assign[col_change] = UNASSIGNED;
		}

		//Update the ith dual value
		double smallest = DBL_MAX;
		for (int k = 0; k < n; k++) {
			double slack = C[k*n + col_change] - row_dual[k];
			if (slack < smallest) {
				smallest = slack;
			}
		}
		col_dual[col_change] = smallest;
	}
	
	update_hungarian();
}

void assignment_problem::step_one() {
	col_dual.clear();
	col_dual.resize(0);
	row_dual.clear();
	row_dual.resize(0);

	//Allocate memory
	col_dual.reserve(n);
	row_dual.reserve(n);

	//Subtract smallest element from each row
	for (int i = 0; i < n; i++) {
		double smallest = DBL_MAX;
		for (int j = 0; j < n; j++) {
			if (c[i*n + j] < smallest) {
				smallest = c[i*n + j];
			}
		}
		
		//Record the dual value
		row_dual.push_back(smallest);

		for (int j = 0; j < n; j++) {
			c[i*n + j] -= row_dual[i];
		}
	}
	
	//Subtract smallest element from each column
	for (int j = 0; j < n; j++) {
		double smallest = DBL_MAX;
		for (int i = 0; i < n; i++) {
			if (c[i*n + j] < smallest) {
				smallest = c[i*n + j];
			}
		}
	

		//Record the dual value
		col_dual.push_back(smallest);

		for (int i = 0; i < n; i++) {
			c[i*n + j] -= col_dual[j];
		}
	}

	step = 2;
}


void assignment_problem::step_two() {
	//Count the number of zeros per row and column.
	std::vector<int> row_zeros(n);
	std::vector<int> col_zeros(n);
	
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++) {
			if (c[i*n + j] == 0) {
				row_zeros[i]++;
				col_zeros[j]++;
			}
		}
	}
	
	//Find the unassigned row with zeros that has the least number of zeros
	bool done = false;
	while (!done) {
		int smallest = INT_MAX;
		int row = NONE;
		for (int i = 0; i < n; i++) {
			if (row_assign[i] == UNASSIGNED && row_zeros[i] > 0 && row_zeros[i] < smallest) {
				row = i;
				smallest = row_zeros[i];
			}
		}

		if (row == NONE) {
			done = true;
		} else {
			//Find the unassigned column that has a zero in row r with the least number of zeros
			int col = NONE;
			smallest = INT_MAX;
			for (int j = 0; j < n; j++) {
				if (c[row*n + j] == 0 && col_zeros[j] < smallest && col_assign[j] == UNASSIGNED) {
					col = j;
					smallest = col_zeros[j];
				}
			}

			if (col != NONE) {
				//Assign this row to this column
				assignments++;
				row_assign[row] = col;
				col_assign[col] = row;

				//Remove this zero from the counters
				col_zeros[col] = 0;
				for (int i = 0; i < n; i++) {
					if (c[i*n + col] == 0) {
						row_zeros[i]--;
					}
				}
			}

		}

	}
	
	step = 3;
}


void assignment_problem::step_three() {
	//Clear all column covers
	covered_cols.clear();
	covered_cols.resize(n);
	covered_rows.clear();
	covered_rows.resize(n);
	
	//Initialise row marks
	std::vector<int> marked_rows;
	marked_rows.resize(n);
	
	//Reset all row covers
	for (int i = 0; i < n; i++) {
		if (row_assign[i] == UNASSIGNED) {
			covered_rows[i] = UNCOVERED;
			marked_rows[i] = MARKED;
		} else {
			covered_rows[i] = COVERED;
		}
	}

	bool done = false;
	while (!done) {
		//Find marked rows
		bool found = false;
		int i = 0;
		while (!found && i < n) {
			if (marked_rows[i] == MARKED) {
				found = true;
			} else {
				i++;
			}
		}

		if (!found) {
			done = true;
			step = 4;
		} else {			
			//Find uncovered zeros in this row
			for (int j = 0; j < n; j++) {
				if (c[i*n + j] == 0 && covered_cols[j] == UNCOVERED) {
					if (col_assign[j] != UNASSIGNED) {
						//This column is currently assigned to
						int assigned_row = col_assign[j];
						//Uncover and mark that row and cover this column
						covered_rows[assigned_row] = UNCOVERED;
						marked_rows[assigned_row] = MARKED;
						covered_cols[j] = COVERED;
					} else {
						//We are going to assign this row to this column
						if (row_assign[i] == UNASSIGNED) {
							assignments++;
						} else {
							//This row is currently assigned to 
							int assigned_col = row_assign[i];
							//But no longer, so clear the reverse assignment
							col_assign[assigned_col] = UNASSIGNED;
						}

						//Assign this row to this column
						row_assign[i] = j;
						col_assign[j] = i;

						done = true;
						step = 3;
					}
				}
			}
			
			marked_rows[i] = UNMARKED;

		}
	}
}


void assignment_problem::step_four() {
	//Find the smallest uncovered value (TODO: in O(n) rather than O(n^2))
	//Subtract from each element of each uncovered column.
	//Add to each element of each covered row. Go to step 4.
	double smallest = find_smallest_uncovered_val();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (covered_rows[i] == COVERED) {
				c[i*n + j] += smallest;
			}
			if (covered_cols[j] == UNCOVERED) {
				c[i*n + j] -= smallest;
			}
		}
	}

	//Update dual values
	for (int i = 0; i < n; i++) {
		if (covered_rows[i] == UNCOVERED) {
			row_dual[i] += smallest;
		}
		if (covered_cols[i] == COVERED) {
			col_dual[i] -= smallest;
		}
	}
	
	step = 3;
}

double assignment_problem::find_smallest_uncovered_val() {
	double smallest = DBL_MAX;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (c[i*n + j] < smallest && covered_rows[i] == UNCOVERED && covered_cols[j] == UNCOVERED) {
				smallest = c[i*n+j];
			}
		}
	}

	
	return smallest;
}