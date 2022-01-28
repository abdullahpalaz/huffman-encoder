#include <iostream>
#include <queue>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <map>

struct heapNode{
    char letter;
    unsigned frequency;
    heapNode* leftChild;
    heapNode* rightChild;

    heapNode(char l, unsigned freq){
        this->letter = l;
        this->frequency = freq;
        this->rightChild = this->leftChild = nullptr;
    }

};

struct comparator{
    bool operator()(heapNode* a, heapNode* b)
    {
        return a->frequency > b->frequency;
    }
}; //compare heapNodes by their frequency, from low to high


std::vector<heapNode*> heapVector; //this is needed because I can't traverse a priority queue, which means I can't find
// the the heapNode with same letter and add 1 to it's frequency.
std::priority_queue<heapNode*, std::vector<heapNode*>, comparator> heap; //for auto sorting

std::map<char, std::string>* charHuffmanMap = new std::map<char, std::string>; //this will hold chars and their huffman codes


void huffmanCodeFinder(struct heapNode* nodePtr, const std::string huffmanCode){
    if (nodePtr == nullptr){
        return;
    }
    if (nodePtr->letter != '|'){
        charHuffmanMap->insert({nodePtr->letter, huffmanCode});
    }
    huffmanCodeFinder(nodePtr->leftChild, huffmanCode+"0");
    huffmanCodeFinder(nodePtr->rightChild, huffmanCode+"1");
}


void printTreeToFile(struct heapNode* node, std::string prefix, std::ofstream &huffmanTreeFile){
    if (node == nullptr){
        return;
    }
    if (node->letter == '|'){
        huffmanTreeFile<<std::endl;
        huffmanTreeFile<<prefix<<"-"<<node->frequency;
    } else{
        huffmanTreeFile<<std::endl;
        huffmanTreeFile<<prefix<<'"'<<node->letter<<'"';
    }
    printTreeToFile(node->leftChild, std::string(prefix.length(), ' ')+" left child is  ", huffmanTreeFile);
    printTreeToFile(node->rightChild, std::string(prefix.length(), ' ')+" right child is ", huffmanTreeFile);
} //this function is used for -l command.

int main(int argc, char **argv) {
    if (std::strcmp(argv[1], "-i") == 0){
        std::fstream fileToEncodeOrDecode(argv[2]); //open the txt file || UNTIL THIS PART, CODE IS STILL NOT IN THE
        // ENCODING OR DECODING PART, THIS PART IS COMMON FOR BOTH -ENCODE AND -DECODE
        if (std::strcmp(argv[3], "-encode") == 0){

            std::string stringToEncode;
            std::getline(fileToEncodeOrDecode, stringToEncode);
            fileToEncodeOrDecode.close();
            //this code block creates "stringToEncode" as the whole document in one string.


            for (char c: stringToEncode){
                bool heapNodeWithCorrectLetterFound = false;
                for (auto heapNode: heapVector){
                    if (heapNode->letter == c){
                        heapNode->frequency += 1;
                        heapNodeWithCorrectLetterFound = true;
                        break;
                    }
                }
                if (heapNodeWithCorrectLetterFound == false){
                    auto* heapN = new heapNode(c, 1);
                    heapVector.push_back(heapN);
                }
            } //this code block creates a new heapNode with a letter if heapNode with that letter does not exist in heapVector
            // if the heapNode with that letter exist, then code adds 1 to its frequency.


            for (heapNode* h: heapVector){
                heap.push(h);
            } // here we move all the heapNodes to priority queue, this is needed for sorting

            while (heap.size() != 1){
                auto firstNode = heap.top();
                heap.pop();
                auto secondNode = heap.top();
                heap.pop();
                heapNode* heapN = new heapNode{'|', firstNode->frequency+secondNode->frequency};
                heapN->leftChild = firstNode;
                heapN->rightChild = secondNode;
                heap.push(heapN);
            } // in this code block we construct the huffman tree, since first two elements will be the ones with least frequencies
            // we just get two of them and created a new node with letter '|' which is not a char node but a middle node. I will use
            // '|' for those middle nodes. at last, there will be one node remaining, which is the root node of the tree.

            std::string huffmanEncode;
            huffmanCodeFinder(heap.top(), huffmanEncode); //this function finds huffman codes of each letter and puts them to
            // the charHuffman map, which holds <letter, huffmanCode> pairs. but, if a node's letter is '|', then that means
            // this node is a middle node. middle nodes are included in that charHuffmanMap because they are needed for creating
            // the tree in -l command. in these middle nodes, instead of their huffmanCodes (which are unnecessary and would
            // create bugs) these nodes frequencies are added to the map.
            // Function declaration is above


            for (char c: stringToEncode){
                for (auto iterator = charHuffmanMap->begin(); iterator != charHuffmanMap->end(); iterator++){
                    if (iterator->first == c){
                        std::cout<<iterator->second;
                        break;
                    }
                }
            } //this code block searchs the charHuffmanMap for each char in the string that needed to be encoded and prints
            // its huffmanCode so that in total, there will be a long code, which is the huffmanCode that has been asked.

            std::ofstream mapFile("charHuffmanMap.txt");
            for (auto iterator = charHuffmanMap->begin(); iterator != charHuffmanMap->end(); iterator++){
                mapFile<<iterator->first<<" "<<iterator->second<<std::endl;
            }
            mapFile.close();
            // this code block writes that charHuffmanMap to a txt file so that we can call it for char searching in command -s.


            std::ofstream huffmanTreeFile("huffmanTree.txt");
            printTreeToFile(heap.top(), "", huffmanTreeFile);


        } else if(std::strcmp(argv[3], "-decode") == 0){ //DECODING HERE
            //decode code insert here
            std::string codeToDecode;
            std::getline(fileToEncodeOrDecode, codeToDecode);

            std::fstream charHuffmanMapFile("charHuffmanMap.txt");
            std::string line;
            while (std::getline(charHuffmanMapFile, line)){
                std::stringstream ss(line);
                char letter;
                std::string code;
                ss>>letter;
                ss>>code;
                charHuffmanMap->insert({letter, code});
            }
            charHuffmanMapFile.close(); //in this code block, we read the txt file and created the map, again. but this time
            // only with the chars, middle nodes are not included.


            std::string codeForOneChar;
            for (char c: codeToDecode){
                codeForOneChar += c;
                for (auto iterator = charHuffmanMap->begin(); iterator != charHuffmanMap->end(); iterator++){
                    if (iterator->second == codeForOneChar){
                        std::cout<<iterator->first;
                        codeForOneChar = "";
                    }
                }
            } //for each number in huffmanCode, we add that number to a string named codeForOneChar. after that, if that
            // string (codeForOneChar) is equal to the code of a char in charHuffmanMap (which we recreated above), then we print
            // the char to the screen and then make codeForOneChar string equal to empty string.

        }

    }else if (std::strcmp(argv[1], "-s") == 0){
        char* cPtr = argv[2];
        char c = *cPtr;

        std::fstream charHuffmanMapFile("charHuffmanMap.txt");
        std::string line;
        bool charFound = false;
        while (std::getline(charHuffmanMapFile, line)){
            if (line[0] == c){
                charFound = true;
                std::cout<<"Huffman code of "<<c<<" is: ";
                for (int i =2; i<line.size();i++){
                    std::cout<<line[i];
                }
            }
        }
        charHuffmanMapFile.close();
        if (charFound == false){
            std::cout<<"This char does not exist."<<std::endl;
        }



    }else if (std::strcmp(argv[1], "-l") == 0){
        std::fstream huffmanTreeFile("huffmanTree.txt");
        std::string line;
        while (std::getline(huffmanTreeFile, line)){
            if (line.empty()){
                continue;
            }
            std::cout<<line<<std::endl;
        }

    }
    return 0;
}
