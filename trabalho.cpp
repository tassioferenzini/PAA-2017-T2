#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
#include <set>

using namespace std;

extern "C" {
	struct subproblem {
		int size_d;
		int size_m;
		int *mandatory;
		int *disallowed;
		int lower_bound;
		int value_solution;
		int n_partition_idx;
		int *pred;
		int partial_cost;
	};

	// Prototypes
    void parse_input(char *);
    void create_adjacency_list(int );
    int root_connect(int );
    int prim_leftist_heap(int s, subproblem *subp);
    bool viable_solution(int cap, subproblem *subp);
    void sort_edges(int size, int *edges);
    bool create_cycle(subproblem *subp);
    bool allowed(int edge, subproblem *subp);

	// Global Variables
    int **MAdj;
    int **Dist;
    int **LisAdjP;
    int **LisAdjN;
    int *CardP;
    int *CardN;
    int *I_edge;
    int *J_edge;
    int *PosStack;
    int *PosTop;
    int *Q;
    int *mark;
    int **AGM_LisAdjP;
    int *AGM_CardP;
    int M_edges;
    int Hq, Tq;
    int Dim;
    int IndTop;
    int InfiniteDistance;
    int Root;
    int *partition_keys;
    int pk_size;
    int *root_edges;
    int Cap;
    
	// AVL tree resources
	#include "avl.h"

    struct vertex_entry {
		int vertex_number;
		int key;
		struct avltree_node node;
    };

}; // extern "C"

#include "heap.cpp"
#include "CPUTimer.h"

void parse_input(char *file_input) {
    FILE *fp;
    char line_parse[256];
    int i, j, w;

    InfiniteDistance = 1;

    fp = fopen(file_input, "r");

    if (fp == NULL) {
		printf("Input file %s not found.\n", file_input);
    }
  
    fscanf(fp, "%[^\n]\n", line_parse);
    
    Dim = atoi(line_parse);
    Dim++; // we count the root
    Root = Dim;

    M_edges = Dim*Dim;

    // allocate memory for the graph
    MAdj = (int **) calloc(Dim + 1, sizeof(int *));
    for (i = 0; i < Dim + 1; i++)
	MAdj[i] = (int *) calloc(Dim + 1,  sizeof(int));

    Dist = (int **) calloc(Dim + 1, sizeof(int *));
    for (i = 0; i < Dim + 1; i++)
	Dist[i] = (int *) calloc(Dim + 1,  sizeof(int));

    LisAdjP = (int **) calloc(Dim + 1, sizeof(int *));
    for (i = 0; i < Dim + 1; i++)
	LisAdjP[i] = (int *) calloc(Dim + 1,  sizeof(int));

    LisAdjN = (int **) calloc(Dim + 1, sizeof(int *));
    for (i = 0; i < Dim + 1; i++)
	LisAdjN[i] = (int *) calloc(Dim + 1,  sizeof(int));

    CardP = (int *) calloc(Dim + 1, sizeof(int));

    CardN = (int *) calloc(Dim + 1, sizeof(int));
  
    I_edge = (int *) calloc ((Dim + 1) * (Dim + 1), sizeof(int));
    J_edge = (int *) calloc ((Dim + 1) * (Dim + 1), sizeof(int));
 
    PosStack = (int *) calloc (Dim + 1, sizeof(int));
    PosTop = (int *) calloc (Dim + 1, sizeof(int));

    Q = (int *) calloc (Dim + 1, sizeof(int));
    mark = (int *) calloc (Dim + 1, sizeof(int));
  
    AGM_LisAdjP = (int **) calloc(Dim + 1, sizeof(int *));
    for (i = 0; i < Dim + 1; i++)
	AGM_LisAdjP[i] = (int *) calloc(Dim + 1,  sizeof(int));;
  
    AGM_CardP = (int *) calloc(Dim + 1, sizeof(int));

    for (i = 1; i <= Dim; i++) {
		for (j = 1; j <= Dim; j++) {
	    	MAdj[i][j] = 0;
		}
    }
  
    // Partition_keys saves only the half of the graph because the graph is undirected
    pk_size = (int) ((Dim) * (Dim) / 2) + (Dim / 2) + 1;
    int pk_index = 1;
    partition_keys = (int *) calloc(pk_size+1, sizeof(int));
    j = 1; // count the number of edges
   
    for (int k = 1; k <= Dim; k++) {
		for (i = 1; i <= Dim; i++) {
			w = 0;
			fscanf(fp, "%d", &w);
			if (w == 0)
			printf("PARSE ERROR!\n");

			MAdj[k][i] = 1;
			Dist[k][i] = w;

			I_edge[j] = k;
			J_edge[j] = i;
			j++;
			InfiniteDistance += w;
		}

		for (i = k; i <= Dim; i++) {
			partition_keys[pk_index++] = i;
		}
    }

    sort_edges(pk_size, partition_keys);
    root_edges = (int *) calloc(Dim+1, sizeof(int));
    for (i = 1; i <= Dim; i++) {
		root_edges[i] = i;
	}
    sort_edges(Dim, root_edges);

    fclose(fp);
}

int root_connect(int s) {
    int *pred = (int *) calloc (Dim + 1, sizeof(int));
    
    for (int i = 1; i <= Dim; i++) {
		pred[i] = s;
    }

    int total_dist = 0;
    for (int v = 1; v <= Dim; v++) {
		if (v != pred[v]) total_dist += Dist[v][pred[v]];
    }
    
    return total_dist;
}

int prim_leftist_heap(int s, subproblem *subp) {
    int v,k,l, vmin, dmin;
    int *pred = (int *) calloc (Dim + 1, sizeof(int));
    int *key = (int *) calloc (Dim + 1, sizeof(int));
    
    leftist h;
    h.init(Dim);

    for (v=1; v<=Dim; v++) {
    	pred[v] = -1;
        key[v] = InfiniteDistance;
    }

    int *disallowed_i = (int *) calloc (subp->size_d + 1, sizeof(int));
    int *disallowed_j = (int *) calloc (subp->size_d + 1, sizeof(int));
    for (v=1; v<=subp->size_d; v++) {
    	disallowed_i[v] = I_edge[subp->disallowed[v]];
    	disallowed_j[v] = J_edge[subp->disallowed[v]];
    }

    key[s] = 0;
    pred[s] = s;
    k = 1;

    // O(n*log(n))
    for (v = 1; v <= Dim; v++) {
		if (s != v) { 		// Excluding the root vertex
			h.insert(v, key[v]);
		}
    }
    
    // O(n*log(n))
    for(l=1; l<=CardP[s]; l++) {
		v = LisAdjP[s][l];

		if (key[v] > Dist[s][v]) {
			// Here we make a lookup for vertex "v", and if found, remove it for key modification
			// O(log(n))
			int found = h.remove(&v, &key[v]);

			if (found == 0) continue;

			key[v] = Dist[s][v]; 

			// O(log(n))
			h.insert(v, key[v]);
			pred[v] = s;
		}
    }

    // O(n*log(n) + m*log(n))
    for (k = 2; k <= Dim; k++) {	
		// EXTRACT-MIN = log(n)
		h.deletemin(&vmin, &dmin);

		/* We discover all the vertex connected to the solution-added vertex */
		// Count all the calls and we have O(m*log(n)) here
		for(l=1; l<=CardP[vmin]; l++) {
			v = LisAdjP[vmin][l];
			if ( key[v] > Dist[vmin][v]) {
				// log(n)
				int found = h.remove(&v, &key[v]);
				if (found == 0) continue;
			
				bool is_disallowed = false;
				for (int d=1; d<=subp->size_d; d++) {
					if ((vmin == disallowed_i[d] || vmin == disallowed_j[d])
						 && (v == disallowed_i[d] || v == disallowed_j[d])) {
						is_disallowed = true;
						break;
					}
				}
				if (is_disallowed)
					continue;

				key[v] = Dist[vmin][v];
			
				// log(n)
				h.insert(v, key[v]);
				pred[v] = vmin;
			}
		}
    }

    int qtd_trees = 0;
   
    // Put the mandatory vertex in the MST
	for (v=1; v<=subp->size_m; v++) {
		int i_a = I_edge[subp->mandatory[v]];
		int j_a = J_edge[subp->mandatory[v]];
		if (i_a == Root)
			qtd_trees++;
		if (pred[j_a] != i_a && pred[i_a] != j_a) {
			pred[j_a] = i_a;
		}
	}

	int total_agm = 0;
	for (int v=1; v<=Dim; v++) {
		if (v != pred[v]) {
			total_agm += Dist[v][pred[v]];
		}
	}

	int min_trees = (int)ceil(Dim / (float)Cap);
	v=1;
	while (v <= Dim && qtd_trees < min_trees) {
		int edge = root_edges[v];
		if (allowed(edge, subp)) {
			int j_a = J_edge[edge];
			pred[j_a] = Root;
			qtd_trees++;
		}
		v++;
	}

	printf("Solution edges= ");
	int total_solution = 0;
	for (int v=1; v<=Dim; v++) {
		if (v != pred[v]) {
			total_solution += Dist[v][pred[v]];
			printf("(%d, %d), ", v, pred[v]);
		}
	}

	printf("\nSolution Cost=%d LB=%d ", total_solution, total_agm);

    subp->pred = pred;
    subp->lower_bound = total_agm;
    subp->value_solution = total_solution;

    free(key);
    free(disallowed_i);
    free(disallowed_j);

    h.delete_all();

    return total_solution;
}

void create_adjacency_list(int n) {
    int i,j,p;

    for (i=1; i<=n; i++) {
		CardP[i] = 0;
		CardN[i] = 0;
    }

    for(i=1; i<=n; i++) {
		for(j=1; j<=n; j++) {
	    	if (MAdj[i][j] == 1) {
				p = CardP[i] + 1;
				LisAdjP[i][p] = j;
				CardP[i] = p;

				p = CardN[j] + 1;
				LisAdjN[j][p] = i;
				CardN[j] = p;
			}
		}
    }
}

void sort_edges(int size, int *edges) {
	int v1 = -1;
	int v2 = -1;
	double value = -1;
	int index = -1;
	int j = -1;
	for (int i=1; i<=size; i++) {
		v1 = I_edge[edges[i]];
		v2 = J_edge[edges[i]];
		value = Dist[v1][v2];
		index = edges[i];
		j = i;
		v1 = I_edge[edges[j-1]];
		v2 = J_edge[edges[j-1]];

		while ((j > 1) && (Dist[v1][v2] > value)) {
			edges[j] = edges[j - 1];
			j = j - 1;
			if (j-1 > 0) {
				v1 = I_edge[edges[j-1]];
				v2 = J_edge[edges[j-1]];
			}
	    }
		edges[j] = index;
	}
}

bool viable_solution(int cap, subproblem *subp) {
	stack<int> nos;
	int pred = -1;
	int demanda = 0;
	int v = -1;
	for(int i=1; i<=Dim; i++) {
		pred = subp->pred[i];
		if (pred == Root) {
			nos.push(i);
			demanda = 0;
			while(!nos.empty()) {
				v = nos.top();
				nos.pop();
				demanda++;
				if (demanda > cap)
					return false;
				for(int j=1; j<=Dim; j++) {
					if (subp->pred[j] == v)
						nos.push(j);
				}
			}
		}
	}
	return true;
}

bool search_cycle(int v, int *visited, int *pred) {
	int p = pred[v];
	for(int i=1; i<=Dim;i++) {
		visited[i] = 0;
	}
	visited[v] = 1;
	while(p > 0 && p != Root) {
		visited[p] = 1;
		p = pred[p];
		
		// If it gets a visited predecessor, it's a cycle
		if (p > 0 && visited[p])
			return true;
	}
	return false;
}

bool create_cycle(subproblem *subp) {
	int *pred = (int *) calloc (Dim + 1, sizeof(int));
	int v = -1;
	
	for(v=1; v<=Dim;v++) {
		pred[v] = -1;
	}

	// Puts in predd all the vertex of the mandatory edges
	for(v=1; v<=subp->size_m;v++) {
		int i = I_edge[subp->mandatory[v]];
		int j = J_edge[subp->mandatory[v]];
		if (pred[j] > -1)
			return 1;
		pred[j] = i;
	}

	int *visited = (int *) calloc (Dim + 1, sizeof(int));
	bool form = false;
	for(v=1; v<=Dim;v++) {
		// Searching cycles from each vertex
		form = search_cycle(v, visited, pred);
		if (form) break;
	}

	if (form) {
		cout << "\nMandatory vertices forming cycle:" << endl;
		for(v=1; v<=subp->size_m;v++) {
			int i = I_edge[subp->mandatory[v]];
			int j = J_edge[subp->mandatory[v]];
			printf("(%d, %d), ", i, j);
		}
	}

	free(pred);
	free(visited);

	return form;
}

bool allowed(int edge, subproblem *subp) {
	for (int v = 1; v <= subp->size_d; ++v) {
		if (edge == subp->disallowed[v])
			return false;
	}
	return true;
}

int main(int argc, char *argv[]) {

    CPUTimer totaltime;
    totaltime.reset();
    totaltime.start();

    Cap = -1;

    if (argc != 3) {
		cout << "Usage: ./trabalho Capacity Instance.txt" << endl;
		cout << "Capacity - Maximum Root Capability (3, 5 or 10)" << endl;
		cout << "instance.txt - graph of input" << endl;
		return -1;
    }
    
    Cap = atoi(argv[1]);

    if (Cap != 3 && Cap != 5 && Cap != 10) {
		cout << "Permissible capacity values: 3, 5 or 10" << endl;
		return -1;
    }

	printf("Instance=%s Capacity=%d \n", argv[2], Cap);

    parse_input(argv[2]);
    create_adjacency_list (Dim);

    int upper_bound = 445; //root_connect(Root);

    subproblem s0;
    s0.size_d = 0;
    s0.size_m = 0;
    s0.disallowed = NULL;
    s0.mandatory = NULL;
    s0.n_partition_idx = 1;
    s0.partial_cost = 0;
    stack<subproblem> subproblems;
    subproblems.push(s0);

    totaltime.stop();
    int i = 0;
    while (subproblems.size() > 0 && totaltime.getCPUTotalSecs() < 3600) {
		totaltime.start();
		subproblem s = subproblems.top();
		subproblems.pop();

		// Calculate lower bound para for s
		prim_leftist_heap(Root, &s);
		if (s.lower_bound < upper_bound) {

			// If it is a better viable solution than the upper bound, it will be the new upper bound
			if (s.value_solution < upper_bound && viable_solution(Cap, &s)) {
				upper_bound = s.lower_bound;
			}

			// In this case, it has used all the possible edges
			if (s.n_partition_idx > pk_size) continue;

			// Creating subproblems. It gets an edge for partitioning
			int partition_edge = partition_keys[s.n_partition_idx];
			
			// Subproblem with the mandatory partition_edge
			subproblem s1;
			s1.size_m = s.size_m + 1;
			s1.size_d = s.size_d;
			s1.mandatory = new int[s1.size_m+1];
			for(i = 1; i<=s.size_m; i++) {
				s1.mandatory[i] = s.mandatory[i];
			}
                        
			s1.disallowed = new int[s1.size_d+1];
			for(i = 1; i<=s.size_d; i++) {
				s1.disallowed[i] = s.disallowed[i];
			}

			s1.mandatory[s1.size_m] = partition_edge;
			s1.n_partition_idx = s.n_partition_idx+1;
			
			// Gets the partial cost with the new fix edge
			s1.partial_cost = s0.partial_cost + Dist[I_edge[partition_edge]][J_edge[partition_edge]];

			// Only adds the subproblem it it's not wrost than the upper bound and if it has the amount of mandatory edges less than the amount of vertex
			if (s1.partial_cost < upper_bound && s.size_m < Dim && !create_cycle(&s1)) {
				subproblems.push(s1);
			} else {
				delete s1.mandatory;
				delete s1.disallowed;
			}

			// Subproblem with the disallowed partition edged 
			subproblem s2;
			s2.size_m = s.size_m;
			s2.size_d = s.size_d + 1;
			s2.mandatory = new int[s2.size_m+1];
			for(i = 1; i<=s.size_m; i++) {
				s2.mandatory[i] = s.mandatory[i];
			}
			s2.disallowed = new int[s2.size_d+1];

			for(i = 1; i<=s.size_d; i++) {
				s2.disallowed[i] = s.disallowed[i];
			}
                        
			s2.disallowed[s2.size_d] = partition_edge;
			s2.n_partition_idx = s.n_partition_idx+1;

			// The partial cost is the same because we do not add edges
			s2.partial_cost = s0.partial_cost;

			subproblems.push(s2);
		}

        // Making some house cleaning
        delete s.mandatory;
        delete s.disallowed;
        free(s.pred);

		totaltime.stop();
    }

    printf("UB= %d Time=%f\n", upper_bound, totaltime.getCPUTotalSecs());
    
    return(0);
}
