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
 * Caution : Decompress Function is INCOMPLETE. This function Will be Updated.
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
#include <cstdio>
#include <cstdlib>
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
		const bool is_leaf(void) { return (this->leftchild == nullptr && this->rightchild == nullptr); }
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
class huffman_tree {
	private:
		treenode<_data> *root = nullptr;
		void traverse(huffman_code_table *tbl, treenode<_data> *start, string code){
			if ( start != nullptr) {
				this->traverse(tbl, start->Leftchild(), string(code +"0"));
				this->traverse(tbl, start->Rightchild(), string(code +"1"));
				if( start->Key()->first != '\0') {
					cout << '\t' << str_pack(start->Key()->first) << "\t( Frequent = " << ( start -> Key() -> second) << " )\t==>\t" << code << '\n';
					(*tbl)[start->Key()->first] = code;
					} 
				}
			}
	public:
		huffman_tree(const string* text) {
			if (text != nullptr && text->size() > 0) {
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
						( (new_root->Leftchild()) -> Key() ) -> second + ( (new_root->Rightchild()) -> Key() ) -> second
						));
					
					PQ.push(new_root);					
					}
					return new_root;
				}();
				}
			}
		huffman_tree(huffman_code_table *table) {
			// Constructor for recovering huffman tree from huffman code table.
			if (table != nullptr) {
				this->root = new treenode<_data>(new _data);
				treenode<_data> *new_node = this->root;
				_data _dt;
				for ( auto& x : (*table) ) {
					for ( auto c: x.second ) {
						if ( c == '0' ){
							if ( new_node->Leftchild() == nullptr )	{
								new_node->change_leftchild(new treenode<_data>(new _data));
								}
							new_node = new_node->Leftchild();
							}
						else if ( c == '1'){
							if ( new_node->Rightchild() == nullptr )	{
								new_node->change_rightchild(new treenode<_data>(new _data));
								}
							new_node = new_node->Rightchild();
							}
						}	
						_dt.first = x.first;
						new_node->change_key(&_dt);
						new_node = this->root;
					}
				}
			}
		operator treenode<_data>*(void) { return this->root; }
		huffman_code_table *generate(void) {
			huffman_code_table *res = new huffman_code_table;
			cout << "[*] Huffman Table is generated ( or read )" <<endl;
			this->traverse(res,this->root,string());	
			return res;
			} 
	};

#define DECOMPRESS	1
#define COMPRESS	0
#define RESULT		-1

function<int8_t(const char)> get_opcode = [&](const char x) -> int8_t { x == 'r' ? -1 : (x-'c'); return x; };

#define PREFIX_ERRORMSG "[X] ERROR! "

struct _metadata {
	string filename;
	huffman_code_table *htable = nullptr;
	size_t filesize = 0;
	size_t orig_filesize = 0;
	double compress_rate = 0.0;
	_metadata(const char *name):filename(string(name)){}
	~_metadata(void){ if( htable != nullptr ) delete htable; } 	
	};



#define MAGIC "HF"

string Packmetadata(_metadata *m) {
	string packed_data(MAGIC);
	string packed;
	function<const char*(const size_t)> itoa = [&](const size_t x) -> const char *{
		char *res = new char[22];
		sprintf(res, "%21lu", x);
		return res; 
		};
	const char *numbuf = itoa(m->filesize);
	packed += numbuf;
	delete numbuf;
	numbuf = itoa(m->orig_filesize); 
	packed += numbuf;
	delete numbuf;
	packed += m->filename.substr(0,m->filename.size() - 4);

	if ( m->htable != nullptr ) {
		packed +=  "$";
		for( auto& x : *(m->htable)) {
			packed +=  string ( x.first + x.second );
			packed +=  "|";
			}
	}
	packed +=  "$";
	numbuf = itoa(packed.size());
	packed_data += numbuf;
	packed_data += packed;
	delete numbuf;
	return packed_data;
	}

ostream& operator<< ( ostream& os, _metadata& m) {
	os << "Filename\t\t: " << m.filename << '\n'
	   << "Size of file\t\t: " << m.filesize << '\n'
	   << "Original size of file\t: " << m.orig_filesize << '\n'
	   << "Compression rate\t: " << m.compress_rate <<" \% \n"; 

/*	if ( m.htable != nullptr ) {
		os << "[Huffman Code Table]" << '\n';
		for ( auto& x : (*m.htable) ) {
			os << '\t' << str_pack(x.first) << " ==> " << x.second << '\n';
			} 
		}*/
	return os;
	}

void calculate_rate(_metadata *m) {
	if ( m->orig_filesize > m->filesize ) 
		m->compress_rate = ( static_cast<double>(m->filesize) *100.0f ) / static_cast<double>(m->orig_filesize) ; 
	}

class File {
	private:
		int8_t opcode;
		fstream link;
		_metadata *metadata;
		string contents;
		string* encode_contents(void) {
			string *_compressed_contents = nullptr;
			try {
				// create huffman table and code table
				huffman_tree hf(&(this->contents));
				this->metadata->htable = hf.generate();
				
				// create string stands for compressed contents 
				_compressed_contents = new string();
				// encode
				for ( auto x : this-> contents ) {
					_compressed_contents->append( ( ( *(this->metadata->htable) )[x] ) );
					}
				} 
			catch(int e) {
				if ( e == 0 ) 
					perror(PREFIX_ERRORMSG);
				exit(-1);
				}
			return _compressed_contents;	
			}
		string *decoding_contents(void) {
			//decode contents		
			}
	public:
		explicit File( char _opcode = RESULT, 
					   const char* _name = nullptr, 
					   huffman_code_table *_tbl = nullptr,
					   string *_contents = nullptr
					 ):opcode(get_opcode(_opcode)), metadata(new _metadata(_name)){
			try {
				this->link.open(this->metadata->filename, ( this->opcode == RESULT ? ios::out: ios::in) | ios::binary);
				if (!this->link.is_open()) { this->link.close(); throw errno; }

				if (this->opcode != RESULT) getline(this->link, this->contents, '\0');
				else if ( _contents != nullptr ) this->contents = string(*_contents); 

				this->metadata->filesize = this->contents.size();
				this->metadata->orig_filesize = this->metadata->filesize;

				if ( this->opcode != COMPRESS && _tbl != nullptr ) 
					this->metadata->htable = _tbl;
				} 

			catch(int e){ 
				this->link.close();
				perror(PREFIX_ERRORMSG);
				remove(this->metadata->filename.c_str());
				exit(errno); 
				}
			}
		~File(void) { if(this->link.is_open()) this->link.close(); }
		void write_result(const char _mode) {
			if (this->link.is_open()) {
				if (_mode == COMPRESS) {
					//pack metadata
					string _pack = Packmetadata(this->metadata);
					link << _pack;
					} 
				link << (this->contents);
				}
			else { cout << PREFIX_ERRORMSG << endl; }
			}
		File* compress(void) {
			string *encoded = this->encode_contents();
			string *compressed_contents = [&](void) -> string *{
				string *comp = new string;
				uint8_t buf = 0;
				int8_t bufcount = 7;
				for ( char x : (*encoded) ) {
					buf |= ( (x-0x30) << bufcount );
					bufcount--;
					if ( bufcount == -1 ){
						(*comp)+=static_cast<char>(buf);	
						bufcount += 8;
						buf = 0;
						} 
					}
				return comp;
				}();
			delete encoded;	
			encoded = nullptr;

			File *compressed_file = new File(RESULT, 
											 string(this->metadata->filename + ".zip").c_str(),
											 this->metadata->htable,
											 compressed_contents
											);
			compressed_file->metadata->orig_filesize = this->metadata->filesize;
			calculate_rate(compressed_file->metadata);
 			return compressed_file;
			}	
		File *decompress(void) {
			cout << " Decompress : " << this->contents.size() << endl;
			//seperate information data
			string *buf = new string( this->contents.substr(0,23) );	
			//validation check
			if ( buf->substr(0,2) != "HF" ) {
				cout << PREFIX_ERRORMSG << ": This is BAD File! File can be tained or this cannot be DECOMPRESSABLE!"<< endl;  
				exit(-1);
				}
			size_t length_metadata = atoi(buf->substr(2,21).c_str());
#ifdef DEBUG
			cout << length_metadata << endl;
#endif
			//metadata and huffman_table unpack
			delete buf;
			buf= new string(this->contents.substr(23,length_metadata));

			size_t   orig_filesize = atoi(buf->substr(0,21).c_str());
			size_t decomp_filesize = atoi(buf->substr(22,21).c_str());	 			
#ifdef DEBUG
			cout << orig_filesize << endl;
			cout << decomp_filesize <<endl;
#endif

			string new_filename;
			for (auto x : buf->substr(42,length_metadata)) {
				if ( x == '$') break;
				new_filename += x;
				}
#ifdef DEBUG
 			cout << new_filename << endl;
			cout << (buf->substr(42+new_filename.size(),length_metadata)) << endl;;	
#endif
			uint8_t table_offset = 0;
			huffman_code_table* table = [&](const string& pt) -> huffman_code_table* {
				huffman_code_table *res = new huffman_code_table;
				string _buf;
				for ( auto x : pt ) {
					table_offset++;
					if ( x == '$') break;
					else if ( x == '|') {
						(*res)[ (_buf[0]) ] = _buf.substr(1,(_buf.size()-1));
						_buf.clear();
						continue;
						}
					_buf += x;
					}
				return res;			
				}(buf->substr(43+new_filename.size(),length_metadata));	
			//recover huffman_tree
			huffman_tree rhf(table);
#ifdef DEBUG
			rhf.generate();
#endif
			delete buf;
			size_t start_of_data = (23+length_metadata+1);
			buf = new string( this->contents.begin()+start_of_data,this->contents.end());
			File *decompressed_file = new File(RESULT,new_filename.c_str(),nullptr,buf);
			decompressed_file->metadata->filesize = decomp_filesize;
			decompressed_file->metadata->orig_filesize = decomp_filesize;
			string *decompressed_contents = [&](void) -> string* {
					string *res= new string;
					treenode<_data> *ptr = rhf;
					if ( !buf->empty() ) {
						for (string::iterator it = (this->contents.begin()+start_of_data) ; it != this->contents.end() ; it++ ) {
						//for (auto &x : (*buf)) {
							uint8_t _bit = (*it);
							for ( int8_t i = 7 ; i>=0 ; i-- ){
								uint8_t _digit = (( _bit & ( 0x01 << i ) ) >> i) & 0x01 ;
								if ( _digit == 1 ) ptr = ptr->Rightchild();
								else ptr = ptr->Leftchild();
								if (ptr->is_leaf()) {
										(*res)+=(ptr->Key()->first);
										ptr = rhf;
									}
								}
							if ( res->size() == 1024 ) {
								(decompressed_file->link) << (*res);
								res->clear();
								}
							}
							(decompressed_file->link) << (*res);
							res->clear();
						}
					else {
						delete res;
						res = nullptr;
						}	
					return res;
					}();
			delete buf;
			buf = nullptr;
			return decompressed_file;
			}
		friend ostream& operator<< ( ostream& os, File& F ) {
			os << (*F.metadata);
			return os;
			}
	};


int main(int argc, const char** argv){
	File *result = nullptr ;
	if ( argc != 3) {
		cout << PREFIX_ERRORMSG << " Usage : " << argv[0] 
			 << " -[c|d] filename " << endl;
		return -1;
		}
	char _opcode = argv[1][1] - 'c';

	File source(_opcode, argv[2]);

	if (_opcode == COMPRESS) {	
		result = source.compress();
		result->write_result(COMPRESS);
		}
	else if (_opcode == DECOMPRESS) {
		result = source.decompress();
		}
	else {
		cout << PREFIX_ERRORMSG << "'" 
			 << argv[1][1] << "' is Invalid Operation! Please check supported operation.\n\t( -d : decompress file, -c : compress file )"<<endl;
		return -1;
		}

	if ( result != nullptr) {
		cout << (*result) << endl;
		delete result;
		}

	return 0;
	}
