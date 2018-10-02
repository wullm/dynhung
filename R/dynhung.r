#' @useDynLib dynhung
#' @import Rcpp
NULL
loadModule("APMod", TRUE);

#' Optimal Matching
#'
#' Solve the linear sum assignment problem using the Hungarian method. 
#' @param cost_matrix a square matrix with non-negative cost values
#' @keywords assignment problem
#' @export
#' @examples #Example:
#' A <- matrix(c(1,2,3,2,4,6,3,6,9), nrow=3)
#' B <- matrix(c(1,2,3,2,4,6,3,6,0), nrow=3)
#' AP_A <- solve_assignment(A)
#' AP_B <- solve_assignment(B)
#' matching_A <- optimal_matching(AP_A)
#' matching_B <- optimal_matching(AP_B)
#' print("Optimal matching with A:")
#' print(matching_A)
#' print("Optimal matching with B:")
#' print(matching_B)
#' update_rows(AP_A, B, 3)
#' update_cols(AP_B, A, 3)
#' matching_A <- optimal_matching(AP_A)
#' matching_B <- optimal_matching(AP_B)
#' print("Optimal matching with A:")
#' print(matching_A)
#' print("Optimal matching with B:")
#' print(matching_B)
#'
#' #Compare with clue package function solve_LSAP
#' library(clue)
#' solve_LSAP(A)
#' solve_LSAP(B)
solve_assignment <- function(cost_matrix) {
	if (!is.numeric(cost_matrix)) {
		stop("Cost matrix is not numeric.")
	}
	
	#Convert matrix to 1-dimensional array
	cost_matrix <- as.vector(t(cost_matrix))
	
	n <- sqrt(length(cost_matrix))
	if (abs(n - round(n)) > .Machine$double.eps) {
		stop("Cost matrix is not square.")
	}
		
	AP <- new(assignment_problem, cost_matrix)
	# matching <- AP$get_assignment()
	
	# #Convert 0-based indexing to 1-based indexing
	# matching <- matching + 1
	
	return(AP)
}

#' Optimal Matching
#'
#' Solve the linear sum assignment problem using the Hungarian method. 
#' @param AP an object of the assignment_problem class produced with solve_assignment()
#' @keywords assignment problem
#' @export
#' @examples
#' optimal_matching()
optimal_matching <- function(AP) {
	matching <- AP$get_assignment()
	
	#Convert 0-based indexing to 1-based indexing
	matching <- matching + 1
	
	return(matching)
}

#' Optimal Cost
#'
#' Solve the linear sum assignment problem using the Hungarian method. 
#' @param AP an object of the assignment_problem class produced with solve_assignment()
#' @keywords assignment problem
#' @export
#' @examples
#' optimal_cost()
optimal_cost <- function(AP) {
	cost <- AP$get_cost()
	
	return(cost)
}

#' Update rows of the cost matrix for the assignment problem
#'
#' Solve the linear sum assignment problem using the Hungarian method. 
#' @param AP an object of the assignment_problem class produced with solve_assignment()
#' @param cost_matrix a new cost matrix
#' @param rows_changed a vector of row indices
#' @keywords assignment problem
#' @export
#' @examples
#' update_rows()
update_rows <- function(AP, cost_matrix, rows_changed) {
	if (!is.numeric(cost_matrix)) {
		stop("Cost matrix is not numeric.")
	}
	
	#Convert matrix to 1-dimensional array
	cost_matrix <- as.vector(t(cost_matrix))
	
	n <- sqrt(length(cost_matrix))
	if (abs(n - round(n)) > .Machine$double.eps) {
		stop("Cost matrix is not square.")
	}
	
	if (n != AP$get_n()) {
		stop("Cost matrix has changed in size.");
	}
	
	#Convert 1-based indexing to 0-based indexing
	rows_changed = rows_changed - 1
	
	AP$update_rows(cost_matrix, rows_changed)
}

#' Update columns of the cost matrix for the assignment problem
#'
#' Solve the linear sum assignment problem using the Hungarian method. 
#' @param AP an object of the assignment_problem class produced with solve_assignment()
#' @param cost_matrix a new cost matrix
#' @param cols_changed a vector of column indices
#' @keywords assignment problem
#' @export
#' @examples
#' update_cols()
update_cols <- function(AP, cost_matrix, cols_changed) {
	if (!is.numeric(cost_matrix)) {
		stop("Cost matrix is not numeric.")
	}
	
	#Convert matrix to 1-dimensional array
	cost_matrix <- as.vector(t(cost_matrix))
	
	n <- sqrt(length(cost_matrix))
	if (abs(n - round(n)) > .Machine$double.eps) {
		stop("Cost matrix is not square.")
	}
	
	if (n != AP$get_n()) {
		stop("Cost matrix has changed in size.");
	}
	
	#Convert 1-based indexing to 0-based indexing
	cols_changed = cols_changed - 1
	
	AP$update_cols(cost_matrix, cols_changed)
}