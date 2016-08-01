#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <string>
using namespace std;

void print(vector<bool> v)
{
	for(int i=0;i<v.size();i++)
		cout<<(char)('0'+v[i]);
	cout<<endl;
}

vector<bool> xor_bitwise(const vector<bool>& a, const vector<bool>& b)
{
	int n=a.size();
	vector<bool> ans(n);
	for(int i=0;i<n;i++)
		ans[i]=a[i]^b[i];
	return ans;
} 

vector<int> readFile(string fileName)
{
	ifstream fin;
	fin.open(fileName.c_str());
	vector<int> arr;
	int x;
	while(1)
	{
		fin>>x;
		if(fin.eof())
			break;
		arr.push_back(x);
	}
	fin.close();
	return arr;
}

vector<bool> permute(vector<bool> v, vector<int> p)
{
	int n=p.size();
	vector<bool> u(n);
	for(int i=0;i<n;i++)
		u[i]=v[p[i]-1];
	return u;
}

vector<vector<bool> > keyscheduler(vector<bool> key)
{
	vector<vector<bool> > keys;
	vector<int> rotate={1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};
	vector<int> PC1=readFile("PC1.txt");
	vector<int> PC2=readFile("PC2.txt");
	vector<bool> x=permute(key,PC1);
	for(int i=0;i<=15;i++)
	{
		vector<bool> y(56);
		for(int j=0;j<28;j++)
		{
			y[j]=x[(rotate[i]+j)%28];
			y[28+j]=x[28+(rotate[i]+j)%28];
		}
		x=y;
		keys.push_back(permute(y,PC2));
	}
	return keys;

}

vector<bool> e(vector<bool> v)
{
	vector<int> E=readFile("Enlarge.txt");
	return permute(v,E);
}

vector<bool> p(vector<bool> v)
{
	vector<int> P=readFile("Permute.txt");
	return permute(v,P);
}

vector<bool> s(vector<bool> v)
{
	vector<bool> ans;
	int S[4][16];
	for(int i=0;i<48;i+=6)
	{
		ifstream fin;
		fin.open((string("Sbox")+to_string(i/6)+string(".txt")).c_str());
		for(int j=0;j<4;j++)
			for(int k=0;k<16;k++)
				fin>>S[j][k];
		fin.close();
		int x=S[2*v[i]+v[i+5]][8*v[i+1]+4*v[i+2]+2*v[i+3]+v[i+4]];
		ans.push_back(x>>3);
		ans.push_back((x>>2)&1);
		ans.push_back((x>>1)&1);
		ans.push_back(x&1);
	}

	return ans;
}

vector<bool> f(vector<bool> v, vector<bool> key)
{
	cout<<"**********F begins\n\n";
	print(v);
	vector<bool> enlarged=e(v);			print(enlarged);
	vector<bool> xored=xor_bitwise(enlarged,key);		print(xored);
	vector<bool> substituted=s(xored);		print(substituted);
	vector<bool> permuted=p(substituted);		print(permuted);
	return permuted;
}


string encrypt(vector<bool> plain,vector<bool> key)
{
	vector<vector<bool> > roundkeys=keyscheduler(key);
	vector<int> ip=readFile("IP.txt");
	vector<int> invip=readFile("invIP.txt");

	vector<bool> m=permute(plain,ip);
	int n=m.size();
	cout<<endl;
	for(int i=0;i<16;i++)
	{
		vector<bool> temp(m.begin()+32,m.end());
		vector<bool> q(m.begin(),m.begin()+32);
		vector<bool> temp2=xor_bitwise(f(temp,roundkeys[i]),q);
		temp.insert(temp.end(),temp2.begin(),temp2.end());
		m=temp;
	}
	vector<bool> finl(m.begin()+32,m.end());
	finl.insert(finl.end(),m.begin(),m.begin()+32);
	vector<bool> encrypted=permute(finl,invip);
	string ans="";
	for(int i=0;i<n;i++)
		ans+='0'+encrypted[i];
	return ans;
}

int main()
{
	string plaintext;
	cout<<"Enter text to encrypt: ";
	cin>>plaintext;
	int n=plaintext.size();
	vector<bool> plain(n);
	for(int i=0;i<n;i++)
	{
		if(plaintext[i]=='0')
			plain[i]=0;
		else
			if(plaintext[i]=='1')
				plain[i]=1;
			else
			{
				cout<<"Please enter a valid boolean string to encrypt...\n";
				cout<<"The program will now exit!\n";
				exit(1);
			}
	}

	while(n%64!=0)
	{
		plain.push_back(0);
		n++;
	}

	vector<bool> key={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	//vector<bool> key={0,1,1,1,0,1,0,1,0,0,1,0,1,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,0,0,1,0,1,1,1,0,1,0,0,1,0,0,1,0,0,1,1,1,1,0,0,1,0,1,1,0,1,1,1,0,0,0,0};
	string ciphertext="";
	int ptr=0;
	do
	{
		vector<bool> t(plain.begin()+ptr,plain.begin()+ptr+64);
		ciphertext+=encrypt(t,key);
		ptr+=64;
	}while(ptr<n);

	cout<<"The encrypted text is: "<<ciphertext<<"\n";
}