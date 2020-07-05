/* ===========================================================================
 * Programming Assignment 04 ( Wrote in C++11 )
 * Hyeonsoo Kim
 * The Major of CSE, Pusan National Univ.
 * 2020.06.23
 * Note :
 *
 * Compile Option : -std=c++14 -O2
 *   O2 : Optimization Level 2 , VERY CRITICAL option for SPEED!
 * ===========================================================================
 */


#include <iostream>
#include <fstream>
#include <functional>
#include <ctime>
#include <cstring>
#include <cerrno>
#include <queue>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;
typedef pair<int, int> pairofint;

// Macros for pair of int as 'segment tree node' : first=sum, second=number of accessing of this segments.
#define sum_in(x) x.first
#define cnt_of(x) x.second

/* 
 * Line Object
 * Line is vertical line. all of lines are parallel to y-axis.
 */
typedef tuple<int, pairofint, int> line;
#define x(_line) get<0>(_line)		  // Location on x-axis.
#define segment(_line) get<1>(_line)  // segment infomation : pair<int,int>(start, end)
#define property(_line) get<2>(_line) // property : if it is 1, line is leftside of rectangle, the other side is -1.   


class SegmentTree{
private:
	vector<pairofint> contents; // Binary Tree implemented as array, root node is in contents[0].
	vector<int> *y; // data of points on y_axis. point's y-cordinate used making rectangles. 
public:
	SegmentTree(size_t number_of_rectangle=0, vector<int> *_y=nullptr):y(_y){
		if (_y == nullptr) return;
		contents.resize((number_of_rectangle*2));
		}
	void update(const size_t idx,  
				const pair<size_t, size_t>& target_range, //
				const pair<size_t, size_t>& range,
				const int var) {
		int tg_right = target_range.second;
		int tg_left = target_range.first;
		int r_right = range.second;
		int r_left = range.first;
		int mid = (tg_left + tg_right) >> 1;

		// node refresh routine.
		function<void(void)> node_refresh = [&](void)-> void {
			if ( cnt_of(contents.at(idx)) == 0 ) {
				if ( tg_left == tg_right ) 
					sum_in(contents.at(idx)) = 0;
				else sum_in(contents.at(idx)) = sum_in(contents.at((idx*2)+1)) + sum_in((contents.at(idx*2+2))); 
				}
			else sum_in(contents.at(idx)) = abs(y->at(tg_right) - y->at(tg_left-1));
			};


		if (tg_right < r_left || tg_left > r_right) return;
		if (r_left <= tg_left && tg_right <= r_right) {
			cnt_of(contents.at(idx)) += var;
			node_refresh();
			return;
			}
		// subnodes refresh
		this->update(idx*2+1, {tg_left, mid}, range, var);
		this->update(idx*2+2,{mid+1, tg_right},range,var);
		node_refresh();
		}
	int query(void) const {
		return sum_in((contents.at(0)));
		}
	};

class driver_routine {
private:
	int argc;
	const char *procname;
	const char *filename;
	const char *output[2] = {"Intersection Area : ","Elapsed Time : "};
	int answer;
	clock_t duration;
public:
	driver_routine(const int _argc, const char **argv){
		if ( _argc != 3 ) {
			cout << "[X] ERROR! Argument is mismatched!\n\tUsage : "<< argv[0]  <<  " -i [filename]"<<endl;
			exit(-1);
			}
		argc = _argc;
		procname = argv[0];
		filename = argv[2]; 
		cout << "[O] PA04 : Caculate The SUM of AREAs of Intersections made by overlapped rectangles..."<<endl;
		}
	void solve(void) {
		clock_t start=0, end=0;
		vector<line> vlines;
		vector<int> y_axis;
		// 1. input phsae 
		int sum_area = [&]() -> const int {
			start = clock();
			int res = 0;
			try {
				cout << "[*] Input Rectangles ... ";
				fstream input(filename, ios::in);
				if (!input.is_open()) throw errno;
				int x1=0, x2=0, y1=0, y2=0, n=0;
				input >> n;
				for (int i = 0; i < n  ; i++) {
					input >> x1>> y1>> x2>> y2;
					vlines.push_back({x1,{y1,y2},1});
					vlines.push_back({x2,{y1,y2},-1});
					y_axis.push_back(y1);
					y_axis.push_back(y2);
					res += abs(x2-x1) * abs(y2-y1);
					}
				input.close();
				cout << "OK!" <<endl;
				sort(y_axis.begin(), y_axis.end());
				y_axis.erase(unique(y_axis.begin(), y_axis.end()),y_axis.end());
				sort(vlines.begin(), vlines.end(), 
					[](line& first, line& second) {
						// Sorting Criteria : x cordinaes.
						return x(first) < x(second); 
						});
				return res;
				}
			catch( int e ) {
				cout << "[X] ERROR! " << strerror(e) << endl;
				exit(e); 
				}
			}();
		//2-(1) solution phase - calculate area of union
		function<const size_t(const int)> getbound = [&](const int x) ->const size_t {
			return distance(y_axis.begin(),lower_bound(y_axis.begin(), y_axis.end(),x));
			};
		int area_union = [&]() -> const int {
			cout << "[*] Caculate Intersection's area..."; 
			int res = 0;
			SegmentTree ST(y_axis.size()-1, &y_axis);
			int prev_xpos = x(vlines.at(0));
			for ( line& vl : vlines) {
				res += (x(vl)-prev_xpos) * ST.query();
				prev_xpos = x(vl);
				ST.update(0,{1,y_axis.size()-1},{getbound(segment(vl).first)+1,getbound(segment(vl).second)},property(vl));
				}
			return res;	
			}();
		//2-(2) solution phase - calculate ares of intersections
		// area of intersections = sum of rectangles - union
		this->answer = sum_area - area_union;
		cout << "OK!" <<endl;
		end = clock();
		this->duration = end-start;
		}
	friend ostream& operator<<(ostream &os, driver_routine &d){
		os << d.output[0] << d.answer << '\n'
		   << d.output[1] << d.duration ;
		return os;
		}
	};

int main(int argc, const char **argv) {
	driver_routine D(argc,argv);
	D.solve();
	cout << "[O] Caculation is completed!\n" << D << "\n This results are written on output.txt."<< endl;
	fstream output("output.txt",ios::out);
	output << D;
	output.close();
	return 0;
	}
