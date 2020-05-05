/* ===========================================================================
 * Programming Assignment 02 ( Wrote in C++11 )
 * Hyeonsoo Kim
 * The Major of CSE, Pusan National Univ.
 * 2020.05.05
 * Note : If input or output is too large ( over 50000 char ), this program
 *        will use VERY LARGE amount of disk for memoization.
 * Compile Option : -std=c++11 -O2
 *   O2 : Optimization Level 2 , VERY CRITICAL option for SPEED!
 * ===========================================================================
 */

#include <iostream>
#include <fstream>
#include <functional>
#include <cerrno>
#include <algorithm>
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#define matched 3
#define mismatched -2
#define gap -5
#define ctgap -2

#define COL 0
#define ROW 1

using namespace std;

int errstate;

/*
 * _dp_data : Dynamic Programming Data.
 *  # memoization data cell by cell of memoization table.
 *    char score : score of cell
 *    char flag  : direction of backtracing.
         =1 : insertion gap : column move left
         =2 : Deletion gap : row move up
         =3 : Match/Mismatch : Diagonal move.
 */
class _dp_data {
    private:
        char score;
        char flag;
    public:
        explicit _dp_data(char _s=0, char _f=-1):score(_s), flag(_f) {}
        const char* pack() const {
            char *result = new char[3];
            result[0] = score;
            result[1] = flag;
            result[2] = 0; // NULL Chracter
            return result;
            }
        void unpack(const char *_data) {
            if (_data[2] != 0 ) {
                cout << "[X] ERROR! Type Mismatch!" <<endl;
                exit(5);
                }
            else {
                this->score = _data[0];
                this->flag = _data[1];
                }
            }
        void unpack(const string& _data) {
            unpack(_data.c_str());
            }
        const char getscore() {
            return this->score;
            }
        const char getflag () {
            return this->flag ;
            }
    };



class dynamic_programming {
    private:
        fstream file;		// A file-stream for memoization file
        size_t col;			// munber of column for memoization array.
        size_t row;			// number of row for memoization array.
        string col_data;
        string row_data;
        string local_align[2];
        string result_file_name;// Output filename.

        size_t maxr;
        size_t maxc;
        char maxscore;

        function<_dp_data* (size_t,size_t)> at = [&](size_t row_idx, size_t col_idx) -> _dp_data* {
            _dp_data *res = nullptr;
            if (this->file.is_open()) {
                this->file.seekg(((row_idx)*(col+1)+(col_idx))*3, ios::beg);
                char _data[3];
                this->file.read(_data,3);
                res = new _dp_data();
                res->unpack( _data );
                }
            return res;
            };
        void make_local_alignment(const size_t _r, const size_t _c) {
            function<void(void)> _function[3] = {
                [&](void) -> void { local_align[ROW]+="-"; local_align[COL]+=col_data[_c-1];}, // insertout (1)
                [&](void) -> void { local_align[COL]+="-"; local_align[ROW]+=row_data[_r-1];}, // deleteout (2)
                [&](void) -> void {
                    if ( row_data[_r-1] == col_data[_c-1]) {
                        for (char i = 0 ; i<2 ; i++ )
                            local_align[i]+=row_data[_r-1];
                        }
                    } // matchout  (3)
                };
            if (this->file.is_open()) {
#ifdef DEBUG
                cout << "[*] row "<< _r << ", col " << _c << " ...." <<endl;
#endif
                _dp_data *data = at(_r,_c);
                char flag = data->getflag();
                switch(flag) {
                    case -1:
                        return;
                    case  1:
                        make_local_alignment(_r,_c-1);
                        break;//insertout
                    case  2:
                        make_local_alignment(_r-1,_c  );
                        break; //deleteout
                    case  3:
                        make_local_alignment(_r-1,_c-1);
                        break;//matchout
                    }
                _function[flag-1]();
                }
            }
    public:
        dynamic_programming(const char *filename1, const char *filename2):col(0U), row(0U), maxr(0U), maxc(0U), maxscore(0U) {

            cout << "[O] Programming Assignment 02 start calulation of Local Alignment" <<endl;
            const char *files[2] = {filename1, filename2}; // array of filenames.

            fstream inp[2]; // array of filestream for 2 input files.

            for (size_t i = 0; i< 2 ; i++) {	// for each input filestreams
                inp[i].open(files[i], ios::in);	// filename 1 and filename 2 open each other.
                }

            string buf; // sequence data buffer.
            bool is_ok = inp[0].is_open() && inp[1].is_open() ; // if 2 files are open together, is OK. otherwise not.

            if (is_ok) {
                string *data[2] = { &(this->col_data), &(this->row_data) }; // 2 sequence from files. using indexing.
                /*
                 * Input file process
                 *   Read Serials from file as string, line by line.
                 *   First, Read string for Column of memoization array.
                 * 	 Second, Read string for Row of it.
                 */
                for (size_t i = 0 ; i< 2 ; i++ ) {
                    getline(inp[i], buf);
#ifdef DEBUG
                    cout << "[*] " <<  buf << "\n\t" << "PRESS [ENTER(RETURN)] to continue." << endl;
                    getchar();
#endif
                    while(!inp[i].eof()) {
                        getline(inp[i],buf);	// Read string from file line by line.
#ifdef DEBUG
                        cout << buf << endl;
#endif
                        (*(data[i]))+=buf;	// Append string read from file, to Colunm ( or Row ) string.
                        buf.clear();	// clear buffer. for protecting memory corruption.
                        }
                    inp[i].close();
                    }

                col = this->col_data.size(); // The length of column is The length of colunm data.
                row = this->row_data.size(); // Assign length to low in the same manner of column.

                //output filename generate.
                this->result_file_name = string("output_")+string(filename1)+string("_")+string(filename2)+string(".align");

                /*
                 * Memoization Array File Ceration.
                 * The Array for Memoization in this program saved for file <dp.dat>.
                 */
                this->file.open("dp.dat",ios::out|ios::binary);

                cout << "[*] Memoization Table: " << row << " by " << col << ".. initialize.." <<endl;

                const char* nulldata = _dp_data().pack();
                for (size_t i = 0; i<=row ; i++) {
                    for (size_t j = 0; j<=col ; j++ ) {
                        this->file.write(nulldata,3);
                        }
                    }
                cout << "\t[!] Memoization Table: " << row << " by " << col << " table is established." <<endl;

                delete nulldata;
                this->file.close();
                }
            else {
                if (file.is_open())
                    this->file.close();
                errstate = errno;
                }
            }
        ~dynamic_programming() {
            this->file.close();
            cout << "Memoization Table is removed."<<endl;
            remove("dp.dat");
            }
        void scoring() {
            this->file.open("dp.dat", ios::out|ios::in|ios::binary);

            /*
             * Making Data object for dynamic Programming
             *  - Calculate Value of cell in memoization table according to Smith-Waterman Algorithm.
             */
            function<_dp_data*(char *)> max_and_flagging = [&](char* _data) -> _dp_data* {
                char _score = max( max( _data[0], _data[3] ), max(_data[1], _data[2]));
                char _flag = -1;
                if (_score == _data[1])
                    _flag = 1;
                else if (_score == _data[2])
                    _flag = 2;
                else if (_score == _data[3])
                    _flag = 3;
                _dp_data *res = new _dp_data(_score,_flag);
                return res;
                };

            if (file.is_open()) {
                /*
                 * Core of Dynamic Programming
                 */

                for (size_t i = 1; i<=row ; i++) {
                    for (size_t j = 1; j<=col ; j++ ) {
                        char _cases[4] = {
                            0, // not tagged, terminate
                            at(i,j-1)->getscore() + static_cast<char>(gap),   // insertion gap
                            at(i-1,j  )->getscore() + static_cast<char>(gap), // deletion  gap
                            at(i-1,j-1)->getscore() + \
                                ( row_data[i-1] == col_data[j-1] ? static_cast<char>(matched) : static_cast<char>(mismatched) ) // match or mismatch
                            };

                        _dp_data *_data = max_and_flagging(_cases);

                        // Pin to start of back-tracing for make local alignment
                        if ( _data->getscore() > this->maxscore) {
                            this->maxscore = _data->getscore();
                            this->maxc = j;
                            this->maxr = i;
                            }

                        const char *_data_p = _data->pack();

                        //memoization
                        file.seekp(((i*(col+1))+(j))*3, ios::beg);
                        file.write(_data_p,3);

                        //clean temp-objects for protect from memory corruption.
                        delete _data;
                        delete _data_p;

                        }
                    }
                cout << "\t[!] Memoization Table: scoring complete!" <<endl;
                }
            this->file.close();
            }

        void getlocalalignment() {
            int mmcount = 0; // mismatch (gap) count

            vector<pair<string,int>> subseq_c, subseq_r; // ident-subsequsnce vector.

            this->file.open("dp.dat", ios::in|ios::binary);
            make_local_alignment(maxr,maxc); // make local alignment.

            cout << "[*] Generating Sub-ident-sqeucnces...\n\tResult will be able to check reading console msg and file "
                 << this->result_file_name << endl;

            auto col_sqit1 = local_align[COL].begin();// Iterator for local align obtained from colunm string
            auto col_sqit2 = local_align[COL].begin();// Iterator for local align obtained from colunm string
            auto row_sqit1 = local_align[ROW].begin();// Iterator for local align obtained from row string
            auto row_sqit2 = local_align[ROW].begin();// Iterator for local align obtained from row string
            int score_sq = 0;
            while ( col_sqit2 != local_align[COL].end() && row_sqit2 != local_align[ROW].end() ) {
                if ( *col_sqit2 == *row_sqit2 ) {
                    if ( *col_sqit2 == '-' )
                        mmcount++;
                    score_sq += matched;
                    }
                else {
                    mmcount++;
                    if ( *col_sqit2 == '-' || *row_sqit2 == '-' )
                        score_sq += ( (mmcount>=1)? ctgap : gap );
                    else
                        score_sq += mismatched;
                    }
                if (mmcount== 5) {
                    subseq_c.push_back( pair<string,int>( string(col_sqit1, col_sqit2), score_sq ) );
                    subseq_r.push_back( pair<string,int>( string(row_sqit1, row_sqit2), score_sq ) );
                    col_sqit1 = col_sqit2;
                    row_sqit1 = row_sqit2;
                    mmcount  = 0;
                    score_sq = 0;
                    }
                col_sqit2++;
                row_sqit2++;
                }
            if (subseq_c.size() == 0 && subseq_r.size() == 0) {
                subseq_c.push_back(pair<string,int>(this->local_align[COL],score_sq));
                subseq_r.push_back(pair<string,int>(this->local_align[ROW],score_sq));
                }
            this->file.close();



            this->file.open(this->result_file_name, ios::out);
            size_t i = 0;
            auto it1 = subseq_c.begin();
            auto it2 = subseq_r.begin();
            stringstream stream;
            while(it1 != subseq_c.end() && it2 != subseq_c.end()) {
                //packing result.
                stream << "Subsequence " << i+1 << "(" << (*it1).second << ")" << '\n'
                       << "seq1 : " << (*it1).first <<'\n'
                       << "seq2 : " << (*it2).first <<endl;
                it1++;
                it2++;
                i++;
                }
            string resultmsg = stream.str();
            //print result to file and console.
            cout << resultmsg;
            if (this->file.is_open()) {
                (this->file) << resultmsg;
                }
            this->file.close();

#ifdef DEBUG
            this->file.open("dp.dat", ios::in|ios::binary);
            if (this->file.is_open()) {
                char buf[(col+1)*3];
                int j = 0;

                cout.width(9);
                cout <<"- ";

                for(int i = 0; i< col ; i++ ) {
                    cout.width(9);
                    cout << col_data[i] << ' ';
                    }
                cout << endl;

                while (!file.eof()) {
                    file.read(buf,(col+1)*3);
                    for (int i =0 ; i< col+1 ; i++) {
                        if ( i == 0 && j>=1 && j <= row ) {
                            cout << row_data[j-1] << ' ';
                            }
                        else if ( j > row && i == 0)
                            cout << "* ";
                        else if ( i ==0 && j == 0 )
                            cout << "- ";
                        char buffer[3];
                        memcpy(buffer, buf+(3*i), 3);
                        _dp_data x;
                        x.unpack(buffer);
                        char s[12];
                        sprintf ( s, "(%3d,%3d)",static_cast<int>(x.getscore()),static_cast<int>(x.getflag()));
                        cout << s << ' ';
                        }
                    cout << endl;
                    j++;
                    }
                }
            this->file.close();
#endif
            }
    };
/*
 * Driver Routine Object
 */

class functionality {

        /*
         * Private Data for Driver Routine
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
        functionality(const char* _name):progname(string(_name)), p(nullptr) {
            errstate = 0;
            }
        ~functionality() {
            if (p != nullptr)
                delete p;
            }
        function<const int(const int, const string&)> printerror = [&](const int _err, const string& msg) ->const int{
            errstate = _err;
            cout << "[X] ERROR! " << msg << endl;
            if ( errstate == -1 )
                cout << "\t Please check usage above.";
            else if (errstate == -2)
                cout << "\t Runtime Exception occured!";
            else
                perror("\t(Message) ");
            cout << " this program terminated!"<<endl;
            return errstate;
            };
        int solve(const char *filename1, const char *filename2) {
            if (filename1 != nullptr && filename2 != nullptr) {
                this->p = new dynamic_programming(filename1,filename2);
                this->p->scoring();
                this->p->getlocalalignment();
                return 0;
                }
            return -1;
            }
    };

int main(int argc, const char **argv) {
    functionality Driver(argv[0]);
    if(argc!=3) {
        Driver.printerror(-1, (string("Usage : ")+string(argv[0])+string(" [filename1] [filename2]")));
        }
    else {
        if ( Driver.solve(argv[1],argv[2]) == 0 ) {
            cout << "[O] Solve Problem! Program is successful." <<endl;
            }
        else {
            Driver.printerror(-1, string("Solution failed!"));
            }
        }
    return 0;
    }
