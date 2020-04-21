/* ===============================================================
 * Progrmming Assignment 01 - 2
 * Hyeonsoo Kim
 * Major of Computer Science and Engineering, PNU
 * 2020.04.20
 * ===============================================================
 */

#include <iostream>
#include <fstream>
#include <utility>
#include <tuple>
#include <cerrno>
#include <cstring>

using namespace std;

class building {
	private:
		tuple<int,int,int> contents;
	public:
		explicit building(int x1=0, int ht=0, int x2=0):contents(tuple<int,int,int>(x1,ht,x2)){}
		const int start(void) const { return get<0>(this->contents); }
		const int end(void) const { return get<2>(this->contents); }
		const int height(void) const { return get<1>(this->contents); }
		building& operator=(const building &B) {
			get<0>(this->contents) = get<0>(B.contents);
			get<1>(this->contents) = get<1>(B.contents);
			get<2>(this->contents) = get<2>(B.contents);
			return (*this);
			}
	};

class point {
	private:
		pair<int,int> contents;
	public:
		explicit point(int x = 0, int h = 0):contents(pair<int,int>(x,h)){}
		int& left(void) { return this->contents.first; }
		const int left(void) const { return this->contents.first;}
		int& ht(void) { return this->contents.second; }
		const int ht(void) const { return this->contents.second;}
		};

ostream& operator<< (ostream &os, const point &P){
	os << P.left() << ", " << P.ht();
	return os;
	}

class skyline {
	private:
		point* arr;
		size_t cap = 0;
		size_t _size = 0;
	public:
		skyline(const size_t _cap):cap(_cap),arr(new point[_cap]){}
		const size_t size(void) const { return this->_size;}
		operator point*() { return this->arr;}
		void append(const point &P) {
			if (_size > 0 && arr[_size-1].ht() == P.ht()) return;
			if (_size > 0 && arr[_size-1].left() == P.left()) {
				arr[_size-1].ht() = max(arr[_size-1].ht(), P.ht());
				return;
				}
			arr[_size] = P;
			_size++;
			}

		skyline *merge(skyline *other){
			size_t part1_sz = this->_size, part2_sz = other->size();
			skyline *res = new skyline(part1_sz+part2_sz);
			int idx1 = 0, idx2 = 0, h1 = 0, h2 = 0;
			while (idx1 < part1_sz && idx2 < part2_sz) {
				if(this->arr[idx1].left() < other->arr[idx2].left()){
					int x1 = this->arr[idx1].left();
					h1 = this->arr[idx1].ht();
					
					int max_height = max(h1,h2);
					res->append(point(x1,max_height));
					idx1++;
					}
				else {
					int x2 = other->arr[idx2].left();
					h2 = other->arr[idx2].ht();
					
					int max_height = max(h1,h2);
					res->append(point(x2,max_height));
					idx2++;
					}
				}
			while(idx1 < part1_sz) {
				res->append(this->arr[idx1]);
				idx1++;
				}
			while(idx2 < part2_sz) {
				res->append(other->arr[idx2]);
				idx2++;
				}
			
			return res;
			}
			friend ostream& operator<< (ostream &os, skyline &S) {
				for (size_t i = 0; i< S.size() ; i++) os << S.arr[i] << endl;
				return os;
				}
		};


class functionality {
	private:
		skyline *draw_partial_skyline(building *_data, const size_t start, const size_t end){
			skyline *res = nullptr;
			if ( start == end ) {
				res = new skyline(2);
				res->append(point(_data[start].start(), _data[start].height()));
				res->append(point(_data[start].end(), 0));
				return res;
				}
			size_t mid = (start+end) /2;
			skyline *left  = draw_partial_skyline(_data, start, mid);
			skyline *right = draw_partial_skyline(_data, mid+1, end);
			res = left->merge(right);
			delete left;
			delete right;
			return res;
			}		
	public:
		pair<size_t, building*> input(const char *filename) {
			fstream file(filename, ios::in);
			building *res = nullptr;
			size_t n = 0;
			if (!file.is_open()) { file.close(); cerr << "[X] ERROR! The file <" << filename << "> cannot be opened!\n\t(Message) "<< strerror(errno) << " Please check file." <<endl;}
			else {
				file >> n;
				res = new building[n];
				int x1, h, x2;
				for (size_t i = 0; i< n ; i++){
					file >> x1 >> h >> x2;
					if ( x1 == -1 ) break;
					res[i] = building(x1,h,x2);
					};
			}
			return pair<size_t, building*>(n,res);
			}
		void output(skyline *_result){
			fstream file("PA01-2.out",ios::out);
			if(!file.is_open()) { file.close(); return; }
			cout << *_result;
			file << *_result;
			file.close();
			}
		skyline *solve(pair<size_t,building*>& src){
				return draw_partial_skyline(src.second, 0, src.first-1);
			}
		};


int main(int argc, const char **argv) {
	if (argc !=2 ){
		cerr << "[X] ERROR! Arguments are insufficient!\n\t(Usage) " << argv[0] << " [filename]. Please check usage."<<endl;
		return -1;
		}
	functionality driver;

	pair<size_t, building*> source = driver.input(argv[1]);
	if (source.second == nullptr) return -1;

	skyline *answer = driver.solve(source);

	driver.output(answer);

	return 0;
	}

