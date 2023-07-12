#ifndef __STNODE_H
#define __STNODE_H

#include <map>
#include "stCell.h"
using namespace std;

struct myCoparisson{
   bool operator()(stCellId *id1, stCellId *id2) const {
      return (*id1==*id2)<0;
   }
};

class stNode {
   public:
      stNode() {	     
      }
      ~stNode() {
		 map<stCellId*, stCell*, myCoparisson>::iterator iter;
	     for(iter=root.begin(); iter!=root.end(); iter++) {
		    delete iter->first; //stCellId *
			delete iter->second; //stCell *
		 }
      }
	  stCell *insertPoint(stCellId *cellId, int dimensionality) {
         stCell *cell = findInNode(cellId); // looks for the cell
		 if (!cell) { // the cell doesn't exist, so...
		    cell = new stCell(dimensionality); // creates it
			stCellId *id = new stCellId(dimensionality); // creates its id
		    *id = *cellId; // defines its id containt
			cell->setId(id); // copy the id's pointer in the cell
			root[id] = cell; // insert the cell in the map structure
		 }	     
		 cell->insertPoint(); // new point
		 return cell;
	  }
	  stCell *findInNode(stCellId *cellId) {
		 map<stCellId*, stCell*, myCoparisson>::iterator iter = root.find(cellId);
		 if (iter!=root.end()) {
		    return iter->second;
		 }
		 return 0;
	  }
	  map<stCellId*, stCell*, myCoparisson> *getRoot() {
	     return &root;
	  }
   private:
	  map<stCellId*, stCell*, myCoparisson> root;	  
};

#endif //__STNODE_H
