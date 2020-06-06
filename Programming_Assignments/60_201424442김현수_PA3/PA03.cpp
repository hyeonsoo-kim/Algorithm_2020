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
		void change_leftchild(const treenode<T> *new_left) { this->leftchild = new_left;}
		void change_rightchild(const treenode<T> *new_right) { this->rightchild = new_right;}
		// operators
		bool operator<(const treenode<T>& rhs) {
			return this->key < rhs.key;
			}
	};
class huffman_tree;

#define DECOMPRESS	'd'
#define COMPRESS	'c'
#define RESULT		'r'

typedef map<char, unsigned> huffman_code_table;

class File {
	private:
		char opcode;
		string filename;
		fstream contents;
		huffman_code_table *htable;
		size_t filesize;
	public:
		explicit File( const char _opcode, const char* name ):opcode(_opcode), filename(string(name)){
			try {
				this->contents.open(this->filename, ( this->opcode == DECOMPRESS ? (ios::in) : (ios::out) ) | ios::binary);
				if (!this->contents.is_open()) throw errno;

				if (opcode == DECOMPRESS) { 
					this->htable = [&](void) -> huffman_code_table*{

						}(); 
					}
				else this->htable = [&](void) -> huffman_code_table*{

						}();
				}
				this->filesize = this->contents.tellg();
			catch(int e){ perror(strerror(e)); }
			}
	};

typedef pair<char, uint64_t> _data; 
class huffman_tree {
	private:
		priority_queue< treenode<_data>> PQ;
	};


int main(int argc, const char** argv){
	return 0;
	}
