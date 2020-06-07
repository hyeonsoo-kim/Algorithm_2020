/* ===========================================================================
 * Programming Assignment 03 ( Wrote in C++11 )
 * Hyeonsoo Kim
 * The Major of CSE, Pusan National Univ.
 * 2020.06.08
 * Note :
 *  This Program is implement of compress and decompress file. 
 *  using huffman code 
 *
 * Compile Option : -std=c++11 -O2
 *   O2 : Optimization Level 2 , VERY CRITICAL option for SPEED!
 * ===========================================================================
 */

#include <iostream>
#include <fstream>
#include <queue>
#include <functional>
#include <utility>
#include <map>
#include <cerrno>
#include <cstring>
#include <cstdint>
#include <string>
using namespace std;

/*
 * Huffman code Data structre 
 */

template <typename T>
class treenode {
	private:
		T key;
		treenode<T> *leftchild ;
		treenode<T> *rightchild;
	public:
		explicit treenode(T *_key = nullptr, 
						  treenode<T>* _left = nullptr, 
						  treenode<T>* _right = nullptr
						 	):leftchild(_left), rightchild(_right) { 
							if(_key!=nullptr) this->key = T(*_key); 
							else this->key = T();
								}
		treenode(const treenode<T> *_tnode) { 
				if (_tnode != nullptr) {
					this->key = _tnode->key;
					this->leftchild = _tnode->leftchild;
					this->rightchild = _tnode->rightchild;
					}
			} 
		// Getters
		T* Key(void) { return &(this->key); }
		treenode<T> *Leftchild(void) { return this->leftchild; }
		treenode<T> *Rightchild(void) { return this->rightchild;}
	
		const T* Key(void) const { return &(this->key); }
		const treenode<T>* Leftchild(void) const { return this->leftchild; }
		const treenode<T>* Rightchild(void) const { return this->rightchild;}
	
		// Setters
		void change_key(const T* new_key) { 
			if (new_key != nullptr) 
				key = T(*new_key);
			}
		void change_leftchild(treenode<T> *new_left) { this->leftchild = new_left;}
		void change_rightchild(treenode<T> *new_right) { this->rightchild = new_right;}
	};

typedef pair<char, uint64_t> _data;
class comp {
	public:
	bool operator()(const treenode<_data>* lhs, const treenode<_data>* rhs ){
		const _data* lhsdata = lhs->Key();
		const _data* rhsdata = rhs->Key();
		bool result = lhsdata->second == rhsdata->second;
		if ( result ) { 
			result &= lhs->Leftchild() == nullptr && rhs->Rightchild() == nullptr;
			if ( result ) result = lhsdata->first < rhsdata->first;
			}
		else 
			result = lhsdata->second > rhsdata->second;
		return result; 
		} 
	};
typedef priority_queue < treenode<_data>* , vector<treenode<_data>*>, comp>  nodepq; 
typedef map<char, string> huffman_code_table;

class huffman_tree {
	private:
		treenode<_data> *root = nullptr;
		void traverse(huffman_code_table *tbl, treenode<_data> *start, string code){
			if ( start != nullptr) {
				this->traverse(tbl, start->Leftchild(), string(code +"0"));
				this->traverse(tbl, start->Rightchild(), string(code +"1"));
				if( start->Key()->first != '\0') {
					(*tbl)[start->Key()->first] = code;
					} 
				}
			}
	public:
		huffman_tree(const string* text) {
			nodepq PQ;
			map < char, uint64_t > *freq_table = [text](void) -> map<char, uint64_t>* { 
				map<char, uint64_t> *table = new map<char, uint64_t>; 
				map < char, uint64_t >::iterator it;
				for (auto c : (*text)) {
					it = table->find(c);
					if ( it != table->end())
						(*table)[c]++;
					else (*table)[c] = 1;
					}
				 return table;
				}();	
			for ( auto& x: (*freq_table) ) {
				treenode<_data> *_node = new treenode<_data>(new _data(x.first,x.second));
				PQ.push(_node);
				}
			this->root = [&](void) -> treenode<_data>* {
				size_t nofnodes = PQ.size();
				treenode<_data> *new_root = nullptr;
				for ( size_t i = 0; i < nofnodes-1 ; i++ ) {
					new_root = new treenode<_data>(new _data);
					
					new_root->change_leftchild(PQ.top());
					PQ.pop();

					new_root->change_rightchild(PQ.top());
					PQ.pop();
					
					new_root->change_key(new _data('\0',
						new_root->Leftchild()->Key()->second + new_root->Rightchild()->Key()->second
						));
					
					PQ.push(new_root);					
					}
					return new_root;
				}();
			}
		huffman_code_table *generate(void) {
			huffman_code_table *res = new huffman_code_table;
			this->traverse(res,this->root,string());	
			return res;
			} 
	};

#define DECOMPRESS	1
#define COMPRESS	0
#define RESULT		-1

function<int8_t(const char)> get_opcode = [&](const char x) -> int8_t { x == 'r' ? -1 : (x-'c'); return x; };


struct _metadata {
	string filename;
	huffman_code_table *htable = nullptr;
	size_t filesize = 0;
	size_t orig_filesize = 0;
	double compress_rate = 0.0;
	_metadata(const char *name):filename(string(name)){}
	~_metadata(void){ if( htable != nullptr ) delete htable; } 	
	};

string str_pack(const char x) {
	switch(x) {
		case '\t': return string("TAB");
		case '\n': return string("NEWLINE");
		case '\r': return string("CR");
		case ' ' : return string("SPACE");
		case '_' : return string("HALF-SPACE");
		default  : return string(string("[")+x+string("]"));
		}
	}

ostream& operator<< ( ostream& os, _metadata& m) {
	os << "Filename    : " << m.filename << '\n'
	   << "Size of file: " << m.filesize << '\n';
	if ( m.htable != nullptr ) {
		os << "[Huffman Code Table]" << '\n';
		for ( auto& x : (*m.htable) ) {
			os << '\t' << str_pack(x.first) << " ==> " << x.second << '\n';
			} 
		}
	return os;
	}


class File {
	private:
		int8_t opcode;
		fstream link;
		_metadata *metadata;
		string contents;
		string metatata_pack(void) {
			} 
	public:
		explicit File( const char _opcode, const char* name):opcode(get_opcode(_opcode)), metadata(new _metadata(name)){
			try {
				this->link.open(this->metadata->filename, ( this->opcode == RESULT ? ios::out: ios::in) | ios::binary);
				if (!this->link.is_open()) throw errno;
				getline(this->link, this->contents, '\0');
				this->metadata->filesize = this->contents.size();
				this->metadata->orig_filesize = this->metadata->filesize;

				// validation check
				// read huffman code table
				if ( _opcode == DECOMPRESS )
					;
				else {
					huffman_tree hf(&(this->contents));
					this->metadata->htable = hf.generate();
					} 
				} 
			catch(int e){ perror(strerror(e)); }
			}
		~File(void) { this->link.close(); if ( metadata != nullptr ) delete metadata;}
		friend ostream& operator<< ( ostream& os, File& F ) {
			os << (*F.metadata);
			return os;
			}
	};

int main(int argc, const char** argv){
	File f(COMPRESS, argv[2]);
	cout << f << endl;
	return 0;
	}
