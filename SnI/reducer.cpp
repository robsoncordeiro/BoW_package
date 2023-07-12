#include <string.h>
#include <time.h>
#include <iostream>

using namespace std;

#ifndef __GNUG__
#include "stCorrelationClustering.h"
#endif //__GNUG__

#ifdef __GNUG__
#include "stCorrelationClustering.cpp"
#endif //__GNUG__

// default values
#define NORMALIZE_FACTOR 0 // Independent

// global variables
clock_t startTime;

/**
 * Initiates the measurement of run time.
 */
void initClock() {
	startTime=clock();
}//end initClock

/**
 * Prints the elapsed time.
 */
void printElapsed() { 
	printf("Elapsed time: %0.3lf sec.\n",(double)(clock()-startTime)/CLOCKS_PER_SEC);
}//end printElapsed

/**
 * Initiates the clustering process.
 *
 * @param argc Number of arguments received.
 * @param argv Array with the arguments received.
 * @return success (0), error (1).
 */
int main(int argc, char **argv) {
	int H = 0, numberOfDimensions = 0;
	double alpha = 0;
	char point[10000], key[1000], databaseName[1000], rm_cmd[1000];
	
	initClock(); // initiates the meassurement of run time
	
	// reads the input parameters
	FILE *parameters;
	parameters=fopen("parameters", "r");
	fscanf(parameters,"%lf",&alpha);
	fscanf(parameters,"%d",&H);
	fclose(parameters);
	
	// reads the data dimensionality
	FILE *dimensionality;
	dimensionality=fopen("dimensionality", "r");
	fscanf(dimensionality,"%d",&numberOfDimensions);
	fclose(dimensionality);
	
	// first validations
	if (H < 2) {
		cout << "Error: MrCC needs at least two resolution levels (H >= 2) to perform the clustering process.";
	}//end if
	
	// reads objects from the source database
	int numberOfObjects = 0;
	cin >> key;
	sprintf(databaseName,"database_%s.dat",key);
	FILE *database = fopen(databaseName, "w");
	
	while(cin) { // reads a point per iteration
        if (numberOfObjects) {
			cin >> key; // reads the key
        }
       	cin >> point; // reads the point
       	if (cin) {
			int posPoint = 0; char num[1000];
			for (int j=0; j<numberOfDimensions; j++) {
				int posNum = 0;
				while (point[posPoint] != '_' && point[posPoint] != 0) {
					num[posNum] = point[posPoint];
					posNum++; posPoint++;
				}
				num[posNum] = 0;
				posPoint++;
				fputs(num,database);
				fputs((j==numberOfDimensions-1) ? "\n" : " ",database);
			}//end for
			numberOfObjects++;
			//cout << key << " " << point << "\n";
       	}
	}
	// closes the database in "w" mode
	fclose(database);
	
	if (numberOfObjects) {
		database = fopen(databaseName,"r"); // opens the database in "r" mode
		
		// creates an object of the class stCorrelationClustering
		stCorrelationClustering *sCluster = new stCorrelationClustering(0, database, numberOfDimensions, NORMALIZE_FACTOR, numberOfObjects, 
																		(2*numberOfDimensions), -1, alpha, H, 0, 1, 1);
		
		// the database file will not be used anymore, thus close it
		fclose(database);
		
		// looks for correlation clusters
		sCluster->findCorrelationClusters();
		
		// mounts the result file
		int numCorrelationClusters = sCluster->getNumCorrelationClusters(), *correlationClustersBelongings = sCluster->getCorrelationClustersBelongings();
		char **dimCorrelationClusters = sCluster->getDimCorrelationClusters();
		double **minBetaClusters = sCluster->getMinBetaClusters(), **maxBetaClusters = sCluster->getMaxBetaClusters(),
		*normalizeSlope = sCluster->getCalcTree()->getNormalizeSlope(),
		*normalizeYInc = sCluster->getCalcTree()->getNormalizeYInc();
		
		char aux[1000];
		for (int i=0; i<numCorrelationClusters; i++) {
			sprintf(aux,"%d",i);
			cout << "bounds for beta-cluster " << aux << "\n";
			for (int j=0; j<numberOfDimensions; j++) {
				sprintf(aux,"%d",j);
				cout << "\tdimension " << aux << ": ";
				sprintf(aux,"%lf",((minBetaClusters[i][j]*normalizeSlope[j])+normalizeYInc[j]));
				cout << "min, " << aux;       
				sprintf(aux,"%lf",((maxBetaClusters[i][j]*normalizeSlope[j])+normalizeYInc[j]));
				cout << " max, " << aux << "\n";
			}
			cout << "end of description\n\n";
		}   
		
		// disposes the used structures
		delete sCluster;
	} else {
		cout << "no point was assigned to this reducer\n";
	}
	
	//delete the temporary data file
	sprintf(rm_cmd,"rm -f %s",databaseName);
	system(rm_cmd);
	
	printElapsed(); // prints the elapsed time
	
	return 0; // success
}
