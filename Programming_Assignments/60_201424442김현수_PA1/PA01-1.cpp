/* ===============================================================
 * Progrmming Assignment 01 - 1
 * Hyeonsoo Kim
 * Major of Computer Science and Engineering, PNU
 * 2020.04.20
 * ===============================================================
 */

#include <functional>
#include <iostream>
#include <fstream>
#include <vector>
#include <cerrno>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>
#include <limits>
#define linevector vector<line>
#define linevecidx linevector::iterator
#define DBL_MAX std::numeric_limits<double>::max()

using namespace std;
typedef std::ios_base::openmode fileiomode;

class V2 : public std::pair<int,int>{
	public:
		V2():std::pair<int,int>(0,0){}
		explicit V2(const int x, const int y):std::pair<int,int>(x,y){}
		V2(const V2& X):std::pair<int,int>(X){}
		const bool operator< ( const V2 &rhs ) {
			bool result = this->first < rhs.first;
			if ( !result && this->first == rhs.first )
				result = this->second < rhs.second;
			return result; 
			}
		const bool operator== (const V2 &rhs) {
			return (this->first == rhs.first) && (this->second == rhs.second);
			}
		V2 operator-(const V2 &rhs) {
			return V2(this->first  - rhs.first,
					  this->second - rhs.second );
			}
		const int cross(const V2 &rhs) {
			return ( this->first * rhs.second )-( this->second * rhs.first ); 
			}
		const double distance(const V2 &rhs){
			int disx = (this->first - rhs.first);
			int disy = (this->second - rhs.second);
			return (pow( disx*disx  + disy*disy ,0.5 ));
			}
#ifdef DEBUG
		friend ostream& operator<< ( ostream& os, V2& vec ) {
			os << "(" << vec.first << " , " << vec.second << ")";
			return os;
			}
#endif
	};

class line{
	//private:
	public:
		V2 start;
		V2 end;
		bool crossed_with_other = false;
		int tx,ty,c;
	//public:
		line(){}
		line(const line& X):start(X.start), end(X.end){
			tx = ( this->end.second - this->start.second);
			ty = -( this->end.first - this->start.first );
			c = (this->start.second * (-ty)) - tx*(this->start.first);
		}
		explicit line(const V2 &_a, const V2 &_b):start(_a),end(_b){
			tx = ( this->end.second - this->start.second);
			ty = -( this->end.first - this->start.first );
			c = (this->start.second * (-ty)) - tx*(this->start.first);
		}
		const bool is_crossed(const line& L){
			V2 s1(this->start), e1(this->end), s2(L.start), e2(L.end);
			int thisline_cross_s2 = (e1-s1).cross(s2-s1);
			int thisline_cross_e2 = (e1-s1).cross(e2-s1);
			int targetline_cross_s1 = (e2-s2).cross(s1-s2);
			int targetline_cross_e1 = (e2-s2).cross(e1-s2);
			
			int tl = (thisline_cross_s2*thisline_cross_e2), lt = (targetline_cross_s1*targetline_cross_e1);

			if (tl ==0 && lt == 0) {
				if ( e1 < s1 ) swap(e1,s1);
				if ( e2 < s2 ) swap(e2,s2);
				return !( e1 < s2 || e2 < s1 );
				}
			return tl <=0 && lt <= 0; 
		}
		void setmark(const bool _m) { 
			this->crossed_with_other = _m; 
			}
		const bool getmark() { \
			return this->crossed_with_other;
		    }
		const bool operator< ( const line &L ) {
			bool result = this->start == L.start;
			if ( !result && this->start < L.start)
				result = this->start < L.start;
			else
				result = this->end < L.end;
			return result;
			}
		const double distance(const line &L){
			double res = 0;
#ifdef DEBUG
			cout << "equation of line ax+by+c=0 -> (a,b,c) = (" << tx << "," << ty << ","<< c << ")" << endl;
#endif
			if (tx*L.start.first + ty *L.start.second +c == 0 && tx*L.end.first + ty *L.end.second +c == 0 ) 
				res = min ( min( this->start.distance(L.start), this->start.distance(L.end) ),
				            min( this->end.distance(L.start)  , this->end.distance(L.end)   ) 
				);	
			else {
				res = min (  static_cast<double>(abs(tx*L.start.first + ty * L.start.second + c)),
					     static_cast<double>(abs(tx*L.end.first + ty * L.end.second + c))
				      ) / pow((tx*tx + ty*ty),0.5);
				res = min ( res, min (  static_cast<double>(abs(L.tx*this->start.first + L.ty * this->start.second + c)),
					     static_cast<double>(abs(L.tx*this->end.first + L.ty * this->end.second + c))
				      ) / pow((L.tx*L.tx + L.ty*L.ty),0.5));
			}
			return res;
			}
#ifdef DEBUG
		friend ostream& operator<< (ostream& os, line& L){
			os << L.start<< " - " << L.end;
			return os;
			} 
#endif
};
	

class functionalities {
	private:
	int errstate=0;
	function<linevecidx(linevector* x, const size_t)> lineidxit = [&](linevector *x, const size_t dist) -> linevecidx{ 
		if ( x == nullptr ) { errstate=5; printerror(errstate); exit(errstate); }
		return (*x).begin() + dist; 
		};
	function<void(const int)> printerror  = [](const int err) -> void {
		cerr << "\t Message : " << strerror(err) << ". Then, this Program terminated."<<endl;
		};
#define for_each_lines_in_vector(lines,_index) for(linevecidx _index = lineidxit(lines,0) ; _index != lines->end(); _index++)

#ifdef DEBUG
	function<void(linevector*)> dump= [&](linevector *_data) -> void {
		for_each_lines_in_vector(_data,idx){
			cout <<(*idx) << endl;
			}				
		};
#endif
	void find_crossed_line_merge(linevector *_data, const size_t start_idx, const size_t end_idx){
		int left = start_idx, right=end_idx, middle=(left+right)/2;
		int idx1 = left, idx2 = middle+1; 
		while (idx1<=middle && idx2 <=right ) {
#ifdef DEBUG
			cout << "\t(*) Check lines " << idx1 << " and " << idx2 <<endl;
#endif
			if (_data->at(idx1).is_crossed(_data->at(idx2))) {
#ifdef DEBUG
				cout << "\t\t(!) Crossed line! at " << idx1 << " and " << idx2 <<endl;
				cout << "\t\t\t" << idx1 << " : " << _data->at(idx1) << "\n"
				     << "\t\t\t" << idx2 << " : " << _data->at(idx2) << endl;
#endif
				if ( _data->at(idx1).getmark() ) {
					_data->at(idx2).setmark(true);
					}
				else {
					_data->at(idx1).setmark(true);
					_data->at(idx2).setmark(true);
					}
				}
			idx2++;
			if (idx2 > right && idx1 <= middle) { 
				idx2 = middle+1;
				idx1++;
				continue;
				}  
			}
#ifdef DEBUG
		cout << "[*] Solution Calculate complete! Scope : [" <<start_idx << "->" <<end_idx << "]" << endl;
#endif
		}


	public:
	function<fstream*(const string&, const fileiomode&)> fileopen = [&](const string& filename, const fileiomode& mode) -> fstream* {
		fstream* input = new fstream(filename,mode);
		if (!input->is_open()) {
			errstate = errno;
			cerr << "[X] ERROR! The file '" << filename << "' CANNOT be OPENED!"<< endl;
			printerror(errstate);
			input->close();
			delete input;
			return nullptr;
			}
		return input;
		};  
	
	function<linevector* (fstream*)> read = [&](fstream *file) -> linevector* {
		linevector *res = new linevector;
		if (file != nullptr) {
			int x1=0, y1=0, x2=0, y2=0, n=0, cnt=0;
			(*file) >> n;
			while(true){
				(*file) >> x1 >> y1 >> x2 >> y2;
				if (x1 == -1) break;
				res->push_back(line(V2(x1,y1),V2(x2,y2)));			
				cnt++;
				}
			if ( cnt != n ) { 
				errstate=5;
				cerr << "[X] ERROR! The number of datas are mismatch to the number of actual datas!"
					 << " (The number of datas are " << n << ". But,The " << cnt << " actual datas exsit!)"
					 << endl;
				printerror(errstate);
				delete res;
				exit(errstate);
				}
			}
		else { delete res; exit(errstate); }
		return res;
		};	

	void find_crossed_line(linevector*_data, const size_t start_idx, const size_t end_idx){ 
		if (_data != nullptr) {
			if (start_idx < end_idx ) {
#ifdef DEBUG
				cout << "[*] Find Crossed pair of lines" <<  " Scope : [" <<start_idx << "->" <<end_idx << "]" << endl;
#endif
				int mid = ( start_idx+end_idx ) / 2;
				this->find_crossed_line(_data, start_idx, mid);
				this->find_crossed_line(_data, mid+1, end_idx);
				this->find_crossed_line_merge(_data, start_idx, end_idx);
				}
			}
		}

	void eliminate(linevector** _data) {
		linevector *tmp = new linevector;
		for_each_lines_in_vector((*_data),idx){
			if ( !(*idx).getmark() ) tmp->push_back(*idx);  
		}
		delete (*_data);
		(*_data) = tmp;
	}

	const double merge(linevector *_data, const size_t start_idx, const size_t end_idx) {
		int left = start_idx, right=end_idx, middle=(left+right)/2;
		int idx1 = left, idx2 = middle+1;
		double res = DBL_MAX; 
		while (idx1<=middle && idx2 <=right ) {
#ifdef DEBUG
			cout << "\t(*) Check lines " << idx1 << " and " << idx2 <<endl;
			cout << "\t\t\t" << idx1 << " : " << _data->at(idx1) << "\n"
			     << "\t\t\t" << idx2 << " : " << _data->at(idx2) << endl;
#endif
			double dis= _data->at(idx1).distance(_data->at(idx2));
#ifdef DEBUG
			cout << "\t\t(!) Distance line " << idx1 << " to " << idx2 << " = " << dis << endl;
#endif
			res = min(res,dis);
			idx2++;
			if (idx2 > right && idx1 <= middle) { 
				idx2 = middle+1;
				idx1++;
				continue;
				}  
			}
#ifdef DEBUG
		cout << "[*] Solution Calculate complete! Scope : [" <<start_idx << "->" <<end_idx << "]" << endl;
#endif
		return res;
		}
	
	const double solve(linevector *_data, const size_t start_idx, const size_t end_idx) {
		double res = DBL_MAX;
		if (_data != nullptr) {
			if (start_idx < end_idx ) {
#ifdef DEBUG
				cout << "[*] Caluate Minimal distances between lines..." <<  " Scope : [" <<start_idx << "->" <<end_idx << "]" << endl;
#endif
				int mid = ( start_idx+end_idx ) / 2;
				double left = this->solve(_data, start_idx, mid);
				double right = this->solve(_data, mid+1, end_idx);
				res = this->merge(_data, start_idx, end_idx);
#ifdef DEBUG
				cout << "\t[!] left = " << left << " right = " << right << " merged = " << res << endl;
#endif
				res = min ( min(res, left), right );
			}
		}
		return res;	
	}

	function<void(fstream *,const int)> output = [&](fstream *_file, const int answer) -> void {
		if (_file!= nullptr){
			(*_file) << answer << endl;
			_file->close();
			}
		else { errstate = 5; exit(errstate); }
		};

};

	
int main(int argc, const char **argv) {

	int response = 0, answer = -1;

	if (argc != 2) { 
		if (argc <2) cerr << "[X] ERROR! You should type name of file for input data.\n" ;
		else cerr << "[X] ERROR! Arguments are mismatch. You should type 2 arguments! \n" ;
		cerr << "\tUsage : " << argv[0] << " [filename] . Then, this Program terminated."<<endl;
		response = -1;
		}

	else {
#ifdef DEBUG
		cout << "[O] Programming Assignment 01-1 Start!" << endl;
#endif
		functionalities F;
		fstream *file = F.fileopen(string(argv[1]), ios::in);
		linevector *data = F.read(file);
		sort(data->begin(),data->end());
		F.find_crossed_line(data,0,data->size()-1);
		F.eliminate(&data);
		answer =  static_cast<int>( F.solve(data,0,data->size()-1) ); 

#ifdef DEBUG
		cout << "[O] Soution complete! The answer is " << answer;
#else
		cout << answer << endl;
#endif

		file = F.fileopen(string("PA01-1.out"),ios::out);
		F.output(file, answer);	
#ifdef DEBUG
		cout << " => saved in the file <PA01-1.out>." <<endl;
#endif

		}

	return response;
	}
