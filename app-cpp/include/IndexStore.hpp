#ifndef INDEX_STORE_H
#define INDEX_STORE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>


struct DocFreqPair {
    long documentNumber;
    long wordFrequency;
};

struct DocumentInfo {
    std::string docPath;  
    std::string origin;   // Client name (origin)
};

class IndexStore {
    // TO-DO declare data structure that keeps track of the DocumentMap ✅
    // TO-DO declare data structures that keeps track of the TermInvertedIndex ✅
    // TO-DO declare two locks, one for the DocumentMap and one for the TermInvertedIndex ✅
    std::unordered_map<long, DocumentInfo> documentMap;
    std::unordered_map<long, std::string> reverseDocumentMap;
    std::unordered_map<std::string, std::vector<DocFreqPair>> termInvertedIndex;

    std::mutex documentMapMutex;
    std::mutex termInvertedIndexMutex;
    

    public:
        // constructor
        IndexStore();

        // default virtual destructor
        virtual ~IndexStore() = default;
        
        long putDocument(std::string documentPath, std::string clientName);
        DocumentInfo getDocument(long documentNumber);
        void updateIndex(long documentNumber, const std::unordered_map<std::string, long> &wordFrequencies);
        std::vector<DocFreqPair> lookupIndex(std::string term);
};

#endif