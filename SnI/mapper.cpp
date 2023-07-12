#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

char nextComma(FILE *file) {
   char c = 0;
   while (!feof(file) && c != ',')
      c = getc(file);
   return (c == ',');
}

int readClusters(double **min, double **max, int dim) {
   FILE *file = fopen("part-00000", "r");
   int numClusters = 0, newCluster;
   while (1) {
      newCluster = 0;
      for (int j=0; j<dim; j++) {
         if (nextComma(file)) {
            if (!newCluster) {
               newCluster = 1;
	       min[numClusters] = new double[dim];
	       max[numClusters] = new double[dim];
	    }
	    fscanf(file,"%lf",&min[numClusters][j]);
	 }
         if (nextComma(file))
	    fscanf(file,"%lf",&max[numClusters][j]);
      }//end for	
      if (newCluster)
         numClusters++;
      else 
         break; // all clusters were read
   }//end while
   fclose(file); // this file will not be used anymore
   return numClusters;
}

char inCluster(double *pointD, double **min, double **max, int numClusters, int dim) {
   char belongsTo=0;
   for (int c=0; (!belongsTo) && c<numClusters; c++) {
      belongsTo=1;
      // verify if pointD belongs to cluster c
      for (int d=0; belongsTo && d<dim; d++) {				
         if (!(pointD[d] >= min[c][d] && pointD[d] <= max[c][d])) {
	    belongsTo=0; // pointD does not belong to cluster c
	 }//end if
      }//end for
   }//end for
   return belongsTo;
}

int main(int argc, char **argv) {
   char point [10000]; // one point
   char num [100]; // one value
   int dim = 0; // data dimensionality
   long s = 0; // data size
   int div = 0; // desired number of data divisions
   long pointId = 0; // point identifier
   long ss = 0; // sample size
   int numClusters = 0; //number of clusters found in the sampling phase
   double **min, **max, *pointD; //bounds for the clusters found in the sampling phase and
                                 //one point, as an array of double

   srand(time(NULL)); // initialize random seed
   
   // reads the data dimensionality
   FILE *dimensionality;
   dimensionality=fopen("dimensionality", "r");
   fscanf(dimensionality,"%d",&dim);
   fclose(dimensionality);

   // reads the data size
   FILE *size;
   size=fopen("size", "r");
   fscanf(size,"%ld",&s);
   fclose(size);

   // reads the desired number of data divisions
   FILE *divisions;
   divisions=fopen("divisions", "r");
   fscanf(divisions,"%d",&div);
   fclose(divisions);

   // reads the sample size
   FILE *sample;
   sample=fopen("sample", "r");
   fscanf(sample,"%ld",&ss);
   fclose(sample);

   pointD = new double[dim]; // one point, as an array of double
   if (s == ss) { //if there is no sampling, reads the
                  //bounds of the clusters found in the sampling phase
      min = new double*[1000]; //minimum bounds
      max = new double*[1000]; //maximum bounds
      numClusters = readClusters(min,max,dim);
   }

   while(cin) { // reads one line per iteration
      cin >> num; // point identifier
	  pointId = atol(num);

      cin >> point; // first value
      pointD[0] = atof(point); //save as double
      for (int i=1; i<dim; i++) {    	
         cin >> num; // other values
         pointD[i] = atof(num); //save as double
         strcat(point,"_");
         strcat(point,num);   	
      }
      cin >> num; // discarts the ground truth
      if (cin) { //  outputs the point with the appropriate key
        if ( (((double)rand()/RAND_MAX)*100) <= ((double)(ss*100)/s) ) { //sample the points
           if ((s != ss) ||  (!inCluster(pointD,min,max,numClusters,dim))) { 
			  sprintf(num,"%d",((int)floor(pointId/ceil((double)(s+1)/div)))); // file position based data division        
			  cout << num << "\t" << point << "\n";
           }
        }
      }
   }
   
   //cleans the used memory
   delete [] pointD;
   if (s == ss) {
      for (int i=0; i<numClusters; i++) {
         delete [] min[i];
         delete [] max[i];
      }
      delete [] min;
      delete [] max;
   }

   return 0; // success
}
