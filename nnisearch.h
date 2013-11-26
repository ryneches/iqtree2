#ifndef NNISEARCH_H
#define NNISEARCH_H

//#ifdef __cplusplus
//extern "C" {
//#endif

#include <stdio.h>
#include <stdlib.h>
#include "pll/pll.h"
#include <string>
#include <sstream>
#include <set>
#include <algorithm>
#include <map>
#include <vector>
using namespace std;

const int TOPO_ONLY = 0;
const int NO_BRAN_OPT = 1;
const int ONE_BRAN_OPT = 2;
const int FIVE_BRAN_OPT = 4;

#define IQTREE_NEWZPERCYCLE 10

/* This is the info you need to copy the vector*/
typedef struct
{
  int node_number;
  int num_partitions;
  size_t *partition_sizes;
  double **lh_values;
  int **expVector;
} LH_VECTOR;


typedef struct {
	nodeptr p;
	int nniType;
	char* idString;
    double z0[PLL_NUM_BRANCHES]; // p
    double z1[PLL_NUM_BRANCHES]; // p->next
    double z2[PLL_NUM_BRANCHES]; // p->next->next
    double z3[PLL_NUM_BRANCHES]; // q->next
    double z4[PLL_NUM_BRANCHES]; // q->next->next
	double likelihood;
} pllNNIMove;

inline bool comparePLLNNIMove(const pllNNIMove &a, const pllNNIMove &b)
{
    return a.likelihood < b.likelihood;
}

LH_VECTOR backup_likelihood_pointers(pllInstance *tr, partitionList *pr, nodeptr p);

static int cmp_nni(const void* nni1, const void* nni2);

int compareDouble(const void * a, const void * b);

pllNNIMove *getNonConflictNNIList(pllInstance* tr);

void _update(pllInstance *tr, partitionList *pr, nodeptr p);

#define MAX_NUM_DELTA 10000

typedef struct {
	double delta[MAX_NUM_DELTA];
	int num_delta;
	double delta_min;
	int doNNICut;
} NNICUT;

typedef struct {
	vector<pllNNIMove> nniList;
	double curLogl;
	int evalType;
	int numAppliedNNIs; // total number of applied NNIs sofar
	int curNumPosNNIs; // number of positive NNIs found in the current NNI step
	int curNumAppliedNNIs; // number of applied NNIs at the current step
	int curNumNNISteps;
} SearchInfo;


/*
 *  Evaluate NNI moves for the current internal branch
 *  @param tr the current tree data structure
 *  @param pr partition data structure
 *  @param p the node representing the current branch
 *  @return number of positive NNIs found
 */
int evalNNIForBran(pllInstance* tr, partitionList *pr, nodeptr p, SearchInfo &searchinfo);

/**
 * Perturb the best tree
 *
 * Given the best tree, apply some NNIs to escape local optimum
 * @param tr
 * @param pr
 * @param nnis list of all NNI to apply
 * @param numNNI size of the array nnis
 * @return
 */
double perturbTree(pllInstance *tr, partitionList *pr, pllNNIMove *nnis, int numNNI);

/**
 * 	do 1 round of fastNNI
 *  return new tree log-likelihood if found improving NNI otherwise -1.0
 *
 *  @param[in] tr the tree data structure
 *  @param[in] pr partition data structure
 *  @param[out] nniList list containing information about the 2(n-3) evaluated NNIs
 *  @param[in/out] tabuNNIs tabu list
 *  @param[out] nni_count number of NNI that have been applied
 *  @param[out] deltaNNI average improvement made by one NNI
 */
double doNNISearch(pllInstance* tr, partitionList *pr, SearchInfo &searchinfo);

/**
 *  perturb the current tree by randomly carrying some negative NNI moves
 *  @param[in] tr the tree
 *  @param[in] nniList list of all possible NNIs
 */
double pertub(pllInstance* tr, pllNNIMove* nniList);

/**
 *  @brief Do 1 NNI move.
 *  @param[in] tr: the tree data structure
 *  @param[in] pr partition data structure
 *  @param[in] swap: represents one of the 2 NNI moves. Could be either 0 or 1
 *  @param[in] evalType: NO_NR, WITH_ONE_NR, WITH_FIVE_NR
 */
double doOneNNI(pllInstance * tr, partitionList *pr, nodeptr p, int swap, int evalType);

/**
 *  Go through all 2(n-3) internal branches of the tree and
 *  evaluate all possible NNI moves
 */
void evalAllNNI(pllInstance* tr);

/*
 * 	@brief evaluate all NNIs within the subtree specified by node p
 * 	populates the list containing all possible NNI moves
 *
 * 	@param[in] tr: the tree data structure
 * 	@param[in] pr partition data structure
 * 	@param[in] p node pointer that specify the subtree
 */
void evalNNIForSubtree(pllInstance* tr, partitionList *pr, nodeptr p, SearchInfo &searchinfo);

/*
 *  @brief return the array which can be used to store evaluated NNI moves
 *
 *  @param[in] tr: the tree data structure
 */
pllNNIMove *getNNIList(pllInstance* tr);


//#ifdef __cplusplus
//}
//#endif

#endif

