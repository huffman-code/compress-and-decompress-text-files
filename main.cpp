#include <string>
#include <string.h>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <map>
#include <cctype>
#include <algorithm>
#include <bitset>
#include <regex>
#include <iomanip>

using namespace std;
typedef std::uint8_t byte;
int sizeLastElement;

struct Node
{
    char ch;
    int freq;
    Node *left=nullptr;
    Node *right=nullptr;
};

Node* NewNode(char ch, int freq, Node* left, Node* right)
{
    Node* node = new Node();
    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;
    return node;
}

struct comp
{
    bool operator()(Node* l, Node* r)
    {
        // the highest priority item has the lowest frequency
        return l->freq > r->freq;
    }
};

bool isLeaf(Node* root)
{
    return root->left == nullptr && root->right == nullptr;
}

void writeFreqToFile(map<char,int> freq,string file2)
{
    ofstream MyFile(file2);
    string d=to_string(freq.size());
    MyFile <<d<<'\n';
    for (auto pair: freq)
    {
        string m=to_string(pair.second);
        MyFile << pair.first << char(15) << m << char(16) <<"\n";
    }
    MyFile.close();
}

void writeToFile(byte arrq[],int Size,int k,string file2)
{
    fstream MyFile;
    MyFile.open(file2, fstream::app);
    //write to comp file
    string m=to_string(sizeLastElement);
    MyFile<<m<<'\n';
    MyFile.close();
    fstream MyFile2;
    MyFile2.open(file2, fstream::app|std::ios::binary);
    for(int i=0; i<Size; i++)
    {
        MyFile2 << arrq[i] ;
    }
    MyFile2.close();

}

std::ifstream::pos_type filesize(string filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

Node* huffmanBuild(string data,string file2)
{
    if (data.length() == 0)
    {
        return nullptr ;
    }
    map<char, int> freq;
    //count freq of each char
    for (char c : data)
    {
        freq[c]++;
    }
    //write freq in file
    writeFreqToFile(freq,file2);
    //priority queue to save and arrange freq of nodes
    priority_queue<Node*, vector<Node*>, comp> pq;
    //add node for each char to priority queue
    for (auto pair: freq)
    {
        pq.push(NewNode(pair.first, pair.second, nullptr, nullptr));
    }
    // while there is more than one node in the queue
    while (pq.size() != 1)
    {
        //remove the two lowest frequency from the queue
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();
        //create new node with the sum of the previous two node
        int sum = left->freq + right->freq;
        pq.push(NewNode('\0', sum, left, right));
    }
    //last node is the root
    Node* root = pq.top();
    return root;
}

void encode(Node* root, string str,map<char, string> &huffmanCode)
{
    //creating huffman code for each char
    if (root == nullptr)
    {
        return;
    }
    // found a leaf node
    if (isLeaf(root))
    {
        huffmanCode[root->ch] = (str != " ") ? str : "1";
    }
    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}


string Compression (Node*root,string file)
{
    std::string str;
    map<char, string> huffmanCode;
    //call encode function map each char with its code
    encode(root, " ", huffmanCode);
    //printing hashmap
    cout <<'\n'<<"Huffman Codes are:\n" << '\n';
    for (auto pair: huffmanCode)
    {
        cout << pair.first << " " << pair.second << '\n';
    }
    //concatenate the encoded text
    for (char ch: file)
    {
        str += huffmanCode[ch];
    }
    //remove spaces between encoded text
    str.erase(std::remove_if(str.begin(), str.end(),[](char &c)
    {
        return std::isspace<char>(c, std::locale::classic());
    }),str.end());
    return str;
}

std::vector<std::string> Split(const std::string str)
{
    int NumSubstrings = str.length() / 8;
    std::vector<std::string> ret;
//split string with 8 bits
    for (auto i = 0; i < NumSubstrings; i++)
    {
        ret.push_back(str.substr(i * 8, 8));
    }
    // If there are leftover characters, create a shorter item at the end.
    if (str.length() % 8 != 0)
    {
        ret.push_back(str.substr(8 * NumSubstrings));
    }
    return ret;
}



void convertToDecimal(std::vector<std::string> data, int k,string file2)
{
    int h=0;
    int arr[data.size()];
    byte arrq[data.size()];
    //convert string to int
    for (auto i = data.begin(); i != data.end(); ++i)
    {
        arr[h]=stoi(*i, 0, 2);
        h++;
    }
    //fill the byte array
    for(int j=0; j<data.size(); j++)
    {
        if(arr[j]>255)
        {
            for (int i = 3; i >= 0; ++i)
            {
                byte b = (arr[j] >> 8 * i) & 0xFF;
                arrq[j]=b;
            }
        }
        else
        {
            arrq[j]=(byte)arr[j];
        }
    }
    //write to comp file
    writeToFile(arrq,data.size(),k,file2);

}

string* convertCompFile(std::vector<char> bytes)
{
    int j=0;
    string *text =new string[bytes.size()];
    for (auto i = bytes.begin(); i != bytes.end(); ++i)
    {
        //convert char to int
        int ia = (int)*i;
        //convert int to binary
        bitset<8> bin_x(ia);
        //convert binary to string
        text[j] = bin_x.to_string();
        j++;
    }
    return text;
}
std::vector<std::string> padding(std::vector<std::string> path)
{
    //padding last element with 0 if less than 8 digits
    int n = path.size();
    std::ostringstream ss;
    ss << std::setw(8) << std::setfill('0') << path[n - 1] ;
    std::string str = ss.str();
    path[n-1]=str;
    return path;
}

Node* rebuildHuffmanTree(map<char,int> params)
{
    priority_queue<Node*, vector<Node*>, comp> pq;
    //add node for each char to priority queue
    for (auto pair: params)
    {
        pq.push(NewNode(pair.first, pair.second, nullptr, nullptr));
    }
    // while there is more than one node in the queue
    while (pq.size() != 1)
    {
        //remove the two lowest frequency from the queue
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();
        //create new node with the sum of the previous two node
        int sum = left->freq + right->freq;
        pq.push(NewNode('\0', sum, left, right));
    }
    //last node is the root
    Node* root = pq.top();
    return root;
}


string decode_file(struct Node* root, string s)
{
    string ans = "";
    struct Node* curr = root;
    for (int i=0; i<s.size(); i++)
    {
        if (s[i]=='0')
        {
            curr = curr->left;
        }
        else
        {
            curr = curr->right;
        }
        if (curr->left==nullptr and curr->right==nullptr)
        {
            ans.push_back(curr->ch);
            curr = root;
        }
    }

    return ans;
}






string readCompFile2(string file1)
{
    string key,value,line;
    std::string d;
    std::string p;
    char* data;
    std::string compText;
    map<char,int> params;
    char c[2];
    int k,m;
    //open comp file to read freq of each char
    std::ifstream paramFile;
    paramFile.open(file1);
    // get number of char
    getline(paramFile,d);
    std::istringstream(d) >> k;
    bool flag = false;
    //get the freq and char and assign it to (params) map
    for(int i=0; i<k; i++)
    {
        getline (paramFile,key,char(15));
        if(i==0)
        {
            c[0] = key[0];
        }
        else
        {
            c[0] = key[1];
        }

        if(c[0] == '\n')
        	flag = true;

        getline(paramFile,value,char(16));
        int value2;
        std::istringstream(value) >> value2;
        params[c[0]] = value2;
    }
    getline(paramFile,p);
    getline(paramFile,p);
    //get the number of digits of the last element
    std::istringstream(p) >> m;
    paramFile.close();
    //open file to read binary data
    std::ifstream par;
    par.open(file1,std::ios::binary);
    string mm;
    int z = 2;
    if(flag)
    	z=3;
    for(int i=0; i<k+z; i++)
    {
        getline(par,mm);
    }
    //get the binary data into buffer vector
    std::vector<char> bytes(std::istreambuf_iterator<char>(par), {});
    par.close();
    //convert bytes(comp data) vector to original file
    string* text=convertCompFile(bytes);
    for(int i=0; i<bytes.size(); i++)
    {
        //erase 0 padding from last element
        if(i==bytes.size()-1)
        {
            text[i].erase(0, 8-m);
        }
        compText.append(text[i]);
    }
    string decompText = decode_file(rebuildHuffmanTree(params),compText);
    return decompText;
}


int main()
{
    string data,dataappend;
    std::string temp;
    string file1;
    string file2;
    string tempfile;
    string decompText;
    int x;
    cout<<"Enter 0 for Compression 1 for Decompression :";
    cin>>x;
    if(x==0)
    {

        cout << "Enter name of File (without .txt): ";
        cin >> file1;
        tempfile=file1+".txt";
          std::ifstream input( tempfile, std::ios::binary );
    	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    // law 3ayzeen ne print el decimal value of each byte
    for(int i=0; i < buffer.size(); i++){
        dataappend += buffer[i];
    }

        cout<<'\n'<<"Original file size:"<< dataappend.size()<<'\n';
        cout<<'\n'<<"Original file :"<<'\n';
        cout << dataappend<<'\n';
        input.close();
        file2=file1+"comp.txt";
        clock_t tStart = clock();
        string p=Compression(huffmanBuild(dataappend,file2),dataappend);


        //spliting text to 8 digits
        std::vector<std::string> path=Split(p);

        //padding last element with 0 if less than 8 digits
        temp=path[path.size()-1];
        sizeLastElement=temp.length();
        path=padding(path);

        //convert string to decimal and create byte array
        //write to comp file
        convertToDecimal(path,sizeLastElement,file2);
        cout <<"Time taken for Compression : "<<(1.0*(clock() - tStart)/CLOCKS_PER_SEC)<<"sec"<<endl;
        //ifstream in_file(file1, ios::binary);
        //in_file.seekg(0, ios::end);
        //int file_size = in_file.tellg();
        cout << "Input File Size : "<<filesize(tempfile)<<" bytes."<<endl;

        cout<< "Compressed File Size : "<<filesize(file2)<<" bytes."<<endl;
        cout<< "Compression Ratio : "<<(100-(1.0*filesize(file2)/filesize(tempfile))*100)<<"%"<<endl;
    }
    else if (x==1)
    {
        cout << "Enter name of File (without .txt): ";
        cin >> file1;
        clock_t tStart = clock();
        // read comp file
        tempfile=file1+".txt";
        decompText
        =readCompFile2(tempfile);
        cout<<'\n'<<"Decompressed data : "<<'\n'<<decompText<<'\n';
        file2=file1+"Decomp.txt";
        ofstream decompFile(file2);
        decompFile<<decompText;
        decompFile.close();
        cout<<'\n'<<"Decompression Done !!"<<'\n';
        cout <<"Time taken for Decompression : "<<(1.0*(clock()- tStart)/CLOCKS_PER_SEC)<<"sec"<<endl;
        cout << "Compressed File Size : "<<filesize(tempfile)<<" bytes."<<endl;
        cout<< "Original File Size : "<<filesize(file2)<<" bytes."<<endl;
        cout<< "decompression Ratio : "<<((1.0*filesize(file2)/filesize(tempfile))*100)-100<<"%"<<endl;

}
return 0;
}
