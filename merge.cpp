#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAMLINE 100000

int nextComma(FILE *file) {
	int c = 0;
	while (!feof(file) && c != ',')
		c = getc(file);
	return (c == ',');
}

int mergeBetaClusters(int numberOfBetaClusters, int numberOfDimensions, double **minBetaClusters, double **maxBetaClusters, int *correlationClustersBelongings) {
	int merge, i=0, j, k, aux;
	int numCorrelationClusters=0;
	// merges beta-clusters
	while (i<numberOfBetaClusters) {
		j=i+1;		
		while (j<numberOfBetaClusters) {
			// discovers if beta-cluster i shares database space with beta-cluster j
			merge=1;
			for(k=0; merge && k<numberOfDimensions; k++) {
				if (!(maxBetaClusters[i][k] > minBetaClusters[j][k] && minBetaClusters[i][k] < maxBetaClusters[j][k])) {
					merge=0; // beta-clusters i and j do not share database space
				}//end if
			}//end for
			if (merge) { // merges both beta-clusters
				if (correlationClustersBelongings[i]==-1 && correlationClustersBelongings[j]==-1) { // both clusters belong to no merged cluster
					correlationClustersBelongings[i]=correlationClustersBelongings[j]=numCorrelationClusters++; // new merged cluster
				} else {
					if (correlationClustersBelongings[i]!=-1 && correlationClustersBelongings[j]!=-1) { // both clusters belong to some merged cluster(s)
						if (correlationClustersBelongings[i]!=correlationClustersBelongings[j]) { // both clusters belong to different merged clusters
							numCorrelationClusters--;
							for (k=0; k<numberOfBetaClusters; k++) {
								if (k!=i && k!=j && (correlationClustersBelongings[k]==correlationClustersBelongings[i] || correlationClustersBelongings[k]==correlationClustersBelongings[j])) {
									correlationClustersBelongings[k]=(correlationClustersBelongings[i]>correlationClustersBelongings[j]) ? correlationClustersBelongings[j] : correlationClustersBelongings[i];
								}//end if
							}//end for
							if (correlationClustersBelongings[i]>correlationClustersBelongings[j]) {
								aux = correlationClustersBelongings[i]; // deleted cluster 
								correlationClustersBelongings[i]=correlationClustersBelongings[j];								 
							} else { 
								aux = correlationClustersBelongings[j]; // deleted cluster 
								correlationClustersBelongings[j]=correlationClustersBelongings[i];
							}//end if
							for (k=0; k<numberOfBetaClusters; k++) {
								if (correlationClustersBelongings[k] > aux) {
									correlationClustersBelongings[k]--;
								}//end if
							}//end for
						}//end if
					} else { // only one of the beta-clusters belongs to some merged cluster
						(correlationClustersBelongings[i]==-1) ? correlationClustersBelongings[i]=correlationClustersBelongings[j] : correlationClustersBelongings[j]=correlationClustersBelongings[i];
					}//end if
				}//end if
			}//end if
			j++; // next beta-cluster
		}//end while
		if (correlationClustersBelongings[i] == -1) {
			correlationClustersBelongings[i] = numCorrelationClusters++; // new merged cluster
		}//end if
		i++; // next cluster
	}//end while
    return numCorrelationClusters;
}//end

int main(int argc, char **argv) {
	double **minBounds = new double*[TAMLINE];
	double **maxBounds = new double*[TAMLINE];
	int *clustersBelongings = new int[TAMLINE];	
	char basePath[500],fileName[500],aux[500];	
	
	// get the input parameters
	strcpy(basePath,argv[1]);
	int numberOfPartitions = atoi(argv[2]);
	int sample = atoi(argv[3]);
	int numberOfDimensions = atoi(argv[4]);
	
	// creates the output file
	strcpy(fileName,basePath);
	strcat(fileName,"/merged.dat");
	FILE *merged = fopen(fileName, "w");	
	if (!merged) {
		printf("Error on creating the output file.");
		return 1; //error
	}//end if
	
	// reads the beta-clusters bounds
	FILE *unmerged;
	int numberOfBetaClusters = 0;
	char sampleRead = 0;
	for (int p=0; p<numberOfPartitions; p++) {
		// opens the input file wrt to partition p
		strcpy(fileName, basePath);
		strcat(fileName,"/part-");        
		sprintf(aux,"%d",p);
		for (int x=0; x<5.0-strlen(aux); x++)
			strcat(fileName,"0");		
		strcat(fileName,aux);
		
		if (sample && !sampleRead) {
			strcat(fileName,"-sample");
			p--;
			sampleRead = 1;
		}
		
		unmerged = fopen(fileName, "r");
		if (!unmerged) {
			printf("Error on opening the input file.");
			return 1; //error
		}//end if
		
		int numberOfBetaClustersPartition = 0, newCluster;
		while (1) {
			newCluster = 0;
			for (int j=0; j<numberOfDimensions; j++) {
				if (nextComma(unmerged)) {
					if (!newCluster) {
						newCluster = 1;
						minBounds[numberOfBetaClustersPartition+numberOfBetaClusters] = new double[50];
						maxBounds[numberOfBetaClustersPartition+numberOfBetaClusters] = new double[50];
					}
					fscanf(unmerged,"%lf",&minBounds[numberOfBetaClustersPartition+numberOfBetaClusters][j]);
				}
				if (nextComma(unmerged))
					fscanf(unmerged,"%lf",&maxBounds[numberOfBetaClustersPartition+numberOfBetaClusters][j]);
			}//end for	
			if (newCluster)
				numberOfBetaClustersPartition++;
			else
				break; // all beta-clusters of this partiton were read
		}//end while
		numberOfBetaClusters += numberOfBetaClustersPartition;
		
		fclose(unmerged); // this file will not be used anymore
	}
	
	for (int i=0; i<numberOfBetaClusters; i++) { // initiation
		clustersBelongings[i]=-1;
	}//end for
	
	int numberOfClusters = mergeBetaClusters(numberOfBetaClusters,numberOfDimensions,minBounds,maxBounds,clustersBelongings);
	
	fprintf(merged,"Number of clusters: %d\n\n", numberOfClusters);
	fprintf(merged,"Clusters belongings:\n");
	for (int i=0; i<numberOfBetaClusters; i++) {
		fprintf(merged,"%d ",clustersBelongings[i]);
		delete [] minBounds[i];
		delete [] maxBounds[i];
	}//end for
	fclose(merged);
	
	delete [] minBounds;
	delete [] maxBounds;
	delete [] clustersBelongings;
	
	return 0; // success
}
