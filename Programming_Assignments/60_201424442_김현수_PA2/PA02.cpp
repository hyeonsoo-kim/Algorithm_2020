/* ===========================================================================
 * Programming Assignment 02
 * Hyeonsoo Kim
 * The Major of CSE, Pusan National Univ.
 * 2020.05.05
 * ===========================================================================
 */
 
#include <iostream>
#include <fstream>
#include <functional>
#include <cerrno>
#include <stdexcept>
#include <cstdio>
#include <string>
using namespace std;

/* 
 * Driver Routine Object
 */

class functionality {

		/* 
		 * Private Data for Driver Routine
		 *  int errstate : error state number
		 *  str progname : program name
		 */
	private:
		/* 
		 * errstate : Error number of system.
		 *   Normaly, errstate is error number in posix system. but there are 2 special cases
		 *     -1 : Argument Error 
		 *     -2 : An Exception is caught
		 */ 
		int errstate;
		string progname;
	public:
		functionality(const char* _name):errstate(0), progname(string(_name)){}
		function<const int(const int, const string&)> printerror = [&](const int _err, const string& msg) ->const int{
				errstate = _err;
				cout << "[X] ERROR! " << msg << endl;
				if ( errstate == -1 ) cout << "\t Please check usage above."; 
				else if (errstate == -2) cout << "\t Runtime Exception occured!";
				else perror("\t(Message) ");
				cout << " this program terminated!"<<endl;
				return errstate;
			};	
};

int main(int argc, const char **argv) {
		functionality Driver(argv[0]);
		if(argc !=3 ){
			return Driver.printerror(-1, (string("Usage : ")+string(argv[0])+string(" [only 2 filename]")));
			}
		else {
			}
		return 0;
}
