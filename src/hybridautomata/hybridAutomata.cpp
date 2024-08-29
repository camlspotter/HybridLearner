/*
 * hybridAutomata.cpp
 *
 *  Created on: 22-Sep-2021
 *      Author: amit
 */

#include "hybridAutomata.h"
#include <bits/stdc++.h>	//used for assert with count in: assert(list_locations.count(Loc_Id)!=0);
#include <assert.h>

using namespace std;

hybridAutomata::hybridAutomata() {
	initial_id = 0;
	dimension = 0;
}

/*
hybridAutomata::hybridAutomata(std::map<int, location::ptr>& list_locs, location::ptr init_loc,
		int dim) {
	list_locations = list_locs;	//assigning a map to another map
	initial_loc = init_loc;
	dimension = dim;
}
*/

location::ptr hybridAutomata::getLocation(int Loc_Id){
	//iterate through the transition and find for given input Name that match with the source_location
	assert(list_locations.count(Loc_Id) != 0);
	return list_locations[Loc_Id];
}

void hybridAutomata::addLocation(location::ptr& loc){
	int key = loc->getLocId();
	list_locations[key] = loc;	//storing the loc with the proper loc_id as the key
}

void hybridAutomata::setLocations(std::map<int, location::ptr>& mapped_location_list){
	this->list_locations = mapped_location_list;
}

unsigned int hybridAutomata::getInitialId() const {
	return initial_id;
}

void hybridAutomata::setInitialId(unsigned int initialId) {
	initial_id = initialId;
}

std::map<int, location::ptr> hybridAutomata::getLocations(){
	return list_locations;
}

location::ptr& hybridAutomata::getInitialLocation() {
	return initial_loc;
}

void hybridAutomata::setInitialLocation(location::ptr& initLoc) {
	initial_loc = initLoc;
}


int hybridAutomata::getDimension() const {
	return dimension;
}

void hybridAutomata::setDimension(int dim) {
	this->dimension = dim;
}




