#ifndef LEEKSCRIPT_H__
#define LEEKSCRIPT_H__

/*
value
	 ---> null
	 ---> boolean
	 ---> number
	 			---> real
						 ---> long
						 		  ---> integer
	 ---> map<K, V>
	 		 ---> object = map<string, ?>

	 	     ---> array<V> = map<int, V>
			   		   ---> string = array<int> (= map<int, int>)
					   ---> interval = array<int> (= map<int, int>)

		     ---> set<V> = map<V, null>

	 ---> function
	 ---> class
*/

#include "vm/VM.hpp"
#include "vm/Module.hpp"
#include "vm/value/LSNumber.hpp"
#include "vm/value/LSArray.hpp"
#include "vm/value/LSObject.hpp"

#endif
