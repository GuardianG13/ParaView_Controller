

#ifndef PVC_JSON_H
#define PVC_JSON_H

//// JSON Functions ////

// Description:
// Parses Json object for keyword and places values in the array.
// CAUTION: Assumes that array passed in is large enough to hold all values.
void json_get_array_values(json_object *jobj, char *key, float a[]);

// Description:
// Parses Json object and prints out every array element's type.
// NOTE: Only used for debugging.
void json_parse(json_object * jobj);

// Description:
// 
void json_parse_array( json_object *jobj, char *key);
void print_json_value(json_object *jobj);

#include "PVC_json.cxx"
	
#endif //PVC_JSON_H