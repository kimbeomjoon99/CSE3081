#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <string>
#include <cmath>
#include <queue>
#include <deque>
using namespace std;

// for encoding
ifstream input;
string huff_encoded = "";

typedef struct contextnode{
    char c;
    string code;
}contextnode;

typedef struct node{
    contextnode* con;
    struct node* left;
    struct node* right;
    int f;
    int flag;
}node;

node* freq[128];
node* root;
vector<node*> newnodes;
vector<char> text;

struct compare{
    bool operator()(node* &a, node* &b){
        if (a->f < b->f) return a > b;
        return a < b;
    }
};

// for decoding
vector<int> hexbit;
string bintext;
string huff_decoded = "";
vector<string> decoder;

void setfreq(){
    for (int i = 0; i < 128; i++){
        freq[i] = (node*)malloc(sizeof(node));
        freq[i]->con = (contextnode*)malloc(sizeof(contextnode));
        freq[i]->con->c = i;
        freq[i]->f = 0;
        freq[i]->flag = 1;
    }
}

void readfile(){
    while(!input.eof()){
        char c;
        input.get(c);
        if (input.eof()) break;
        text.push_back(c);
        freq[c]->f++;
    }
}

void makehuffman(){
    priority_queue<node*, vector<node*>, compare> pq;
    for (int i = 0; i < 128; i++){
        if (freq[i]->f) pq.push(freq[i]);
        //else free(freq[i]);
    }
    while(!pq.empty()){
        node* right = pq.top();
        pq.pop();
        if (pq.empty()){
            root = right;
            break;
        }
        node* left = pq.top();
        pq.pop();
        node* fnode = (node*)malloc(sizeof(node));
        fnode->f = right->f + left->f;
        fnode->right = right;
        fnode->left = left;
        fnode->flag = 0;
        newnodes.push_back(fnode);
        pq.push(fnode);
    }
}

void makecode(node* N, string code){
    if (N->flag){
        N->con->code = code;
        return;
    }
    else{
        string lcode = code;
        string rcode = code;
        lcode += "0";
        rcode += "1";
        makecode(N->left, lcode);
        makecode(N->right, rcode);
    }
}

void compress(string resfilename){
    resfilename += ".zz";
    FILE* output = fopen(resfilename.c_str(), "wb");
    int i;
    // encoding
    for (i = 0; i < text.size(); i++){
        huff_encoded += freq[text[i]]->con->code;
    }
    /*
    for (i = 0; i < text.size(); i++){
        cout << text[i]; 
    }
    cout << "\n";
    cout << huff_encoded << "\n";
    */
    int size = huff_encoded.size() / 8;
    int unusedbit = 0;
    if (huff_encoded.size() % 8) {
        size++;
        unusedbit = 8 - (huff_encoded.size() % 8);
        for (i = 0; i < huff_encoded.size() % 8; i++){
            huff_encoded += "0";
        }
    }
    // making header(metadata)
    unsigned char unused = 0;
    for (i = 0; i < unusedbit; i++){
        unused = unused | (1 << i);
    }
    fputc(unused, output);
    //cout << unusedbit << " " << (int)unused << "\n";
    for (i = 0; i < 128; i++){
        if (!freq[i]->f) {
            unsigned char res = 0;
            fputc(res, output);
        }
        else{
            int j;
            unsigned char res = 0;
            res = res | (1 << 0);
            string tmp = freq[i]->con->code;
            int l = tmp.size();
            int k = 64;
            vector<int> temp;
            while(k){
                //cout << l << " ";
                if (l / k){
                    temp.push_back(1);
                    l -= k;
                }
                else temp.push_back(0);
                if (k == 1) break;
                k /= 2;
            }
            for (j = 0; j < 7; j++){
                //cout << temp[j] << " ";
                if (temp[j]){
                    res = res | (1 << (j + 1));
                }
            }
            fputc(res, output);
            int bytes = tmp.size() / 8;
            if (tmp.size() % 8 > 0){
                bytes++;
            }
            for (j = 0; j < bytes - 1; j++){
                unsigned char hcode = 0;
                for (int x = 0; x <= 7; x++){
                    if (tmp[(j * 8) + x] == '1'){
                        hcode = hcode | (1 << (x));
                    }
                }
                fputc(hcode, output);
            }
            unsigned char Hcode = 0;
            for (int x = 0; x < tmp.size() % 8; x++){
                if (tmp[((bytes - 1) * 8) + x] == '1'){
                    Hcode = Hcode | (1 << (x));
                }
            }
            if (!(tmp.size() % 8)){
                for (int x = 0; x < 8; x++){
                    if (tmp[((bytes - 1) * 8) + x] == '1'){
                        Hcode = Hcode | (1 << (x));
                    }
                }
            }
            //cout << (char)i << " " << tmp.size() << " " << (int)res << " " << tmp << " " << (int)Hcode << " ";
            fputc(Hcode, output);
        }
    }
    //cout << "\n";
    // write encoded text
    //cout << huff_encoded << "\n";
    //cout << size << "\n";
    for (i = 0; i < size; i++){
        unsigned char res = 0;
        for (int j = (8 * i) + 7; j >= (8 * i); j--){
            if (huff_encoded[j] == '1'){
                res = res | (1 << j - (8 * i));
            }
        }
        fputc(res, output);
    }
    fclose(output);
}

void freenode(node* N){
    int i;
    for (i = 0; i < 128; i++){
        if (freq[i]->con) free(freq[i]->con);
        free(freq[i]);
    }
    for (i = 0; i < newnodes.size(); i++){
        free(newnodes[i]);
    }
}

void readbfile(){
    input.seekg(0, input.end);
    int size = (int)input.tellg();
    input.seekg(0, input.beg);
    unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char) * size);
    input.read((char*)buffer, size);
    input.close();
    for (int i = 0; i < size; i++){
        hexbit.push_back((int)buffer[i]);
    }
    for (int i = 0; i < hexbit.size(); i++){
        vector<int> tmp;
        int k = 128;
        int data = hexbit[i];
        while(k){
            if (data / k){
                tmp.push_back(1);
                data -= k;
            }
            else tmp.push_back(0);
            k /= 2;
        }
        for (int j = 7; j >= 0; j--){
            if (tmp[j]) bintext += "1";
            else bintext += "0";
        }
    }
    free(buffer);
    //cout << bintext << "\n";
}

int decode(){
    int i, j;
    for (i = 0; i < 128; i++){
        decoder.push_back("");
    }
    int ind = 0;
    int unused = 0;
    for (; ind < 8; ind++){
        if (bintext[ind] == '1') unused++;
        else break;
    }
    if (ind != 8) ind = 8;
    for (i = 0; i < unused; i++){
        int s = bintext.size();
        bintext = bintext.substr(0, s - 1);
    }
    /*
    for (i = 0; i < bintext.size(); i++){
        cout << bintext[i];
        if ((i + 1) % 8 == 0) cout << " ";
    }
    */
    //cout << "\n";
    //cout << bintext << "\n";
    for (i = 0; i < 128; i++){
        //cout << ind << " ";
        if (bintext[ind] == '0'){
            ind += 8;
            decoder[i] = "X";
            continue;
        }
        else if (bintext[ind] == '1'){
            int nextind = ind + 8;
            ind++;
            int s = 0;
            for (j = 6; j >= 0; j--){
                if (bintext[ind] == '0') {
                    ind++;
                    continue;
                }
                int tmp = 1;
                for (int k = 0; k < j; k++){
                    tmp *= 2;
                }
                s += tmp;
                ind++;
            }
            ind = nextind;
            nextind += 8;
            if (s > 8){
                int tmp = s / 8;
                if (s % 8) tmp++;
                tmp--;
                nextind += (8 * tmp);
            }
            for (j = 0; j < s; j++){
                if (bintext[ind] == '1'){
                    decoder[i] += "1";
                }
                else decoder[i] += "0";
                ind++;
            }
            ind = nextind;
        }
    }
    //cout << "\n";
    /*
    for (i = 0; i < 128; i++){
        cout << (char)i << " " << decoder[i] << " ";
    }
    cout << "\n";
    */
    return ind;
}

void makehuff(int ind){
    string tmp = "";
    int i, j;
    while(ind < bintext.size()){
        tmp += bintext[ind];
        for (i = 0; i < 128; i++){
            if (decoder[i] == tmp){
                char c = (char)i;
                huff_decoded += c;
                tmp = "";
            }
        }
        ind++;
    }
    //cout << huff_decoded << "\n";
}

void decompress(string resfilename){
    resfilename += ".yy";
    FILE *output = fopen(resfilename.c_str(), "w");
    for (int i = 0; i < huff_decoded.size(); i++){
        fprintf(output, "%c", huff_decoded[i]);
    }
    fclose(output);
}

int main(int argc, char *argv[]){
    ios_base::sync_with_stdio(0);
    cin.tie(0), cout.tie(0);
    if (argc != 3){
        printf("usage: ./mp3_20180492 -c(-d) filename\n");
        return 1;
    }
    else if (!(!strcmp(argv[1], "-c") || !strcmp(argv[1], "-d"))){
        printf("you should give the option '-c' or '-d'\n");
        return 1;
    }

    string resfilename = "";
    resfilename += argv[2];
    
    
    if (!strcmp(argv[1], "-c")){
        input.open(argv[2]);
        if (!input){
            cout << "file open error!\n";
            return 1;
        }
        setfreq();
        readfile();
        makehuffman();
        string code = ""; // vector<int> code;
        makecode(root, code);
        compress(resfilename);
        freenode(root);
    }
    else if (!strcmp(argv[1], "-d")){
        input.open(argv[2], ios::binary);
        if (!input){
            cout << "file open error!\n";
            return 1;
        }
        readbfile();
        int ind = decode();
        makehuff(ind);
        decompress(resfilename);
    }

    return 0;
}