/*
Author:莊晏碩
Date:2022/12/10
Purpose:用Huffman演算法進行檔案的壓縮與解壓縮
*/
#include<algorithm>
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<utility>
#include<vector>
#include<string>
using namespace std;

class node{
	public:
		unsigned char data;
		int freq;
		node* left;
		node* right;
		node(int d='\0',int f=0,node* l=NULL,node* r=NULL) //constructor
		{
			data=d;
			freq=f;
			left=l;
			right=r;
		}
};
class minheap{
	public:
		int root; //root在vector中的位置
		vector<node*> arr; //存所有node 
		minheap(int r=0) //constructor
		{
			root=r;
		}
		void create_node(int d,int f,node* l,node* r); //創造新的node放進vector
		void heapify(int n); //調整成min heap格式
		node* get_min(void); //取出min heap最上方的node，並重新整理min heap
		node* huffman(void); //調整成huffman tree格式
};
vector< pair<unsigned char,string> > create_table(node* root,string s); //存編碼表
void encode(string in_file,string out_file); //壓縮 
void decode(string in_file,string out_file); //解壓縮

int main(int argc,char* argv[])
{
	if(argv[1][1]=='c') encode(argv[3],argv[5]);
	else if(argv[1][1]=='u') decode(argv[3],argv[5]);
	return 0;
}

void minheap::create_node(int d,int f,node* l,node* r)
{
	node* temp;
	temp=new node(d,f,l,r);
	arr.push_back(temp);
}

void minheap::heapify(int n)
{
    int smallest=n;
    int left=2*n+1;
    int right=2*n+2;
    if(left<arr.size() && arr[left]->freq < arr[smallest]->freq) smallest = left;
	if(right<arr.size() && arr[right]->freq < arr[smallest]->freq) smallest = right;
	if(left<arr.size() && arr[left]->freq == arr[smallest]->freq && arr[left]->data < arr[smallest]->data) smallest = left;
	if(right<arr.size() && arr[right]->freq == arr[smallest]->freq && arr[right]->data < arr[smallest]->data) smallest = right;
    if(smallest != n)
	{
        swap(arr[smallest],arr[n]);
        heapify(smallest);
    }
}

node* minheap::get_min(void)
{
	node* temp=arr[0];
	arr[0]=arr[arr.size()-1];
	arr.pop_back();
	heapify(0);
	return temp;
}

node* minheap::huffman(void)
{
//	node* left,right;
	while(arr.size()>1)
	{
		node* left=get_min();
		node* right=get_min();
		if(left->data>right->data) swap(left,right);
		create_node(min(left->data,right->data),left->freq+right->freq,left,right);
	//	cout<<left->data<<" "<<left->freq<<endl;
	//	cout<<right->data<<" "<<right->freq<<endl;
	//	cout<<min(left->data,right->data)<<" "<<left->freq+right->freq<<endl<<endl;
	}
	return get_min();
}

vector< pair<unsigned char,string> > create_table(node* root,string s)
{
	string t="";
	vector< pair<unsigned char,string> > v,w;
	if(root->left)
	{
		t=s+"0";
		w=create_table(root->left,t);
		for(int i=0;i<w.size();i++) v.push_back(make_pair(w[i].first,w[i].second));
	}
	if(root->right)
	{
		t=s+"1";
		w=create_table(root->right,t);
		for(int i=0;i<w.size();i++) v.push_back(make_pair(w[i].first,w[i].second));
	}
	if(!root->left && !root->right)
	{
		v.push_back(make_pair(root->data,s));
		//cout<<root->data<<" "<<root->freq<<" "<<s<<endl;
	} 
	return v;
}

void encode(string in_file,string out_file)
{
	bool new_data;
	unsigned char file_input;
	minheap m;
	string data="",code="";
	vector< pair<unsigned char,int> > input;
	vector< pair<unsigned char,string> > table;
	ifstream in;
	in.open(in_file.c_str(),ios::binary);
	
	in>>file_input;
	while(!in.eof())
	{
		new_data=1;
		data+=file_input;
		for(int i=0;i<input.size();i++)
		{
			if(file_input==input[i].first)
			{
				input[i].second++;
				new_data=0;
				break;
			}
		}
		if(new_data) input.push_back(make_pair(file_input,1));
		in>>file_input;
	}
	in.close();
	
	for(int i=0;i<input.size();i++)
	{
		m.create_node(input[i].first,input[i].second,0,0);
	//	cout<<input[i].first<<" "<<input[i].second<<endl;
	}
	for(int i=(m.arr.size())/2;i>=0;i--)
	{
		m.heapify(i);
	}
	
	node* huffman_top=m.huffman();
	//cout<<huffman_top->data<<" "<<huffman_top->freq<<endl;
	table=create_table(huffman_top,"");
	ofstream out;
	out.open(out_file.c_str(),ios::binary);
	for(int i=0;i<table.size();i++)
	{
		//cout<<table[i].first<<": "<<table[i].second<<endl;
		out<<table[i].first<<": "<<table[i].second<<endl;
	}
	
	//cout<<endl;
	out<<endl;
	for(int i=0;i<data.size();i++)
	{
		for(int j=0;j<table.size();j++)
		{
			if(data[i]==table[j].first)
			{
				code+=table[j].second;
				//cout<<table[j].second;
				out<<table[j].second;
				break;
			}
		}
	}
	
	//cout<<endl<<endl<<data.size()*8<<" "<<code.size()<<endl;
	out<<endl<<endl<<"uncompressed size: "<<data.size()*8<<endl
	<<"compressed size: "<<code.size()<<endl
	<<"compressed rate: "<<(double)data.size()*8/code.size()<<endl;
	out.close();
}

void decode(string in_file,string out_file)
{
	string file_input;
	ifstream in;
	in.open(in_file.c_str(),ios::binary);
	vector<string> input,data,table;
	
	in>>file_input;
	while(!in.eof())
	{
		input.push_back(file_input);
		in>>file_input;
	}
	in.close();
	/*for(int i=0;i<input.size();i++)
	{
		cout<<input[i]<<endl;
	}*/
	
	string key="";
	ofstream out;
	out.open(out_file.c_str(),ios::binary);
	int code_place=input.size()-10;
	for(int i=0;i<input[code_place].size();i++)
	{
		key+=input[code_place][i];
		//cout<<key<<endl;
		for(int j=1;j<code_place;j+=2)
		{
			if(key==input[j])
			{
				out<<input[j-1][0];
				key="";
				break;
			}
		}
	}
	out.close();
}
