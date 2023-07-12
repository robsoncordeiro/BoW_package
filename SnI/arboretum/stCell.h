#ifndef __STCELL_H
#define __STCELL_H

#include "stCellId.h"
#include "stNode.h"

class stNode; //informs that the class exists

class stCell {
   public:
      stCell(int dimensionality) {
	     usedCell = 0;
		 sumOfPoints = 0;
		 nextLevel = 0;
		 id = 0;
		 P = new int[dimensionality];
		 for (int i=0; i<dimensionality; i++) {
		    P[i] = 0;
		 }
      }
      ~stCell() { 
	     delete []P;
		 if (nextLevel) {
			//delete nextLevel;
		 }
      }
	  void insertPoint() {
	     sumOfPoints++;		 
	  }
	  int getSumOfPoints() {
	     return sumOfPoints;
	  }
	  char getUsedCell() {
	     return usedCell;
	  }	  
	  void useCell() {
	     usedCell = 1;
	  }
	  int getP(int i) {
	     return P[i];
	  }
	  stCellId *getId() {
	     return id;
	  }
	  void setId(stCellId *id) {
	     this->id = id;
	  }

	  void insertPointPartial(stCellId *sonsCellId, int dimensionality) {
 	     for (int i=0; i<dimensionality; i++) {
	        if (!sonsCellId->getBitValue(i,dimensionality)) {
		       P[i]++; // counts the point, since it is in 
			           // this cell's lower half regarding i
	        }
	     }
	  }

      stNode *nextLevel;
   private:
      char usedCell;
      int *P;      
      int sumOfPoints;
	  stCellId *id;
};

#endif //__STCELL_H
