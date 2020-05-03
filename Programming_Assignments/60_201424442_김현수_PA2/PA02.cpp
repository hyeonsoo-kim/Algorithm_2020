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
#include <vector>
#define matched 3
#define mismatched 2
#define gap -5
#define ctgap -2

using namespace std;

int errstate;

class dynamic_programming {
	private:
		fstream file;
		size_t col;
		size_t row;
		string col_data;
		string row_data;
		string _local_align[2];
	public:
		dynamic_programming(const char *filename1, const char *filename2):col(0U), row(0U){

			const char *files[2] = {filename1, filename2}; // array of filenames.

			fstream inp[2]; // array of filestream for 2 input files.

			for (size_t i = 0; i< 2 ; i++) {	// for each input filestreams
				inp[i].open(files[i], ios::in);	// filename 1 and filename 2 open each other.
			}

			string buf; // sequence data buffer.
			bool is_ok = inp[0].is_open() && inp[1].is_open() ; // if 2 files are open together, is OK. otherwise not.
				
			if (is_ok){
				string *data[2] = { &(this->col_data), &(this->row_data) }; // 2 sequence from files. using indexing.
				for (size_t i = 0 ; i< 2 ; i++ ){
					getline(inp[i], buf);
#ifdef DEBUG
					cout << "[*] " <<  buf << endl;
					getchar();
#endif
					while(!inp[i].eof()) {
						getline(inp[i],buf);
#ifdef DEBUG
						cout << buf << endl;
#endif
						(*(data[i]))+=buf;
						buf.clear();
						}
					inp[i].close();
					}

					col = this->col_data.size();
					row = this->row_data.size();

					this->file.open("dp.dat",ios::out|ios::binary);

					cout << "[*] DP : " << row << " by " << col << " table is established." <<endl; 
					for (size_t i = 0; i<=row ; i++){
						for (size_t j = 0; j<=col ; j++ ) {
							file << 0;
							}
						}
					this->file.close();
				}
			else {
				if (file.is_open())
					this->file.close();
				errstate = errno;
				}
			}
			~dynamic_programming() { this->file.close(); /*remove("dp.dat");*/ }
			void scoring() {
				this->file.open("dp.dat", ios::out|ios::in|ios::binary);

				function<const char(size_t,size_t)> at = [&](size_t row_idx, size_t col_idx) -> const char {
					file.seekg((row_idx)*(col+1)+(col_idx), ios::beg);
					char res;
					file.read(&res,1);
					return res;
					};

#define SCORE(X) ( (X) - 6 )

				if (file.is_open()){
					for (size_t i = 1; i<=row ; i++){
						for (size_t j = 1; j<=col ; j++ ) {
							char score = max( max( 0,
									  at(i,j-1) + gap),
									  max(at(i-1,j-1) + ( row_data[i-1] == col_data[j-1] ? matched : mismatched ),
									  at(i-1,j) + gap)
									) + 6 ;
							file.seekp((i*(col+1))+(j), ios::beg);
							file.write(&score,1);
							}
						}
					}
				this->file.close();
				}
			void getlocalalignment() {
				this->file.open("dp.dat", ios::in|ios::binary);
				char buf[col+1];
				int j = 0;
				
				cout << "    ";
				for(int i = 0; i< col ; i++ ) {
					cout.width(5);
					cout << col_data[i] << ' ';
				}
				cout << endl;
				
				while (!file.eof()) {
				file.read(buf,col+1);
				for (int i =0 ;i< col+1 ; i++){
					if ( i == 0 && j>=1 && j <= row ) {cout << row_data[j-1] << ' ';}
					else if ( j > row && i == 0) cout << "* ";
					cout.width(5);
					cout << ((int)(buf[i])-0x30) << ' ';
					}
				cout << endl;
				j++;
				}
				file.seekg(0,ios::end);
				size_t length = file.tellg();
				cout << length << endl;
				this->file.close();
				}
		};
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
		string progname;
		dynamic_programming *p;
	public:
		functionality(const char* _name):progname(string(_name)){errstate = 0;}
		~functionality() { if (p != nullptr) delete p; }
		function<const int(const int, const string&)> printerror = [&](const int _err, const string& msg) ->const int{
				errstate = _err;
				cout << "[X] ERROR! " << msg << endl;
				if ( errstate == -1 ) cout << "\t Please check usage above."; 
				else if (errstate == -2) cout << "\t Runtime Exception occured!";
				else perror("\t(Message) ");
				cout << " this program terminated!"<<endl;
				return errstate;
			};
		void solve(const char *filename1, const char *filename2) {
			this->p = new dynamic_programming(filename1,filename2);
			this->p->scoring();
			this->p->getlocalalignment();				
		}
};

int main(int argc, const char **argv) {
		functionality Driver(argv[0]);
		if(argc!=3){
			return Driver.printerror(-1, (string("Usage : ")+string(argv[0])+string(" [filename1] [filename2]")));
			}
		else {
			Driver.solve(argv[1],argv[2]);
			}
		return 0;
}
