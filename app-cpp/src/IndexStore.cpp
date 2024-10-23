#include "IndexStore.hpp"

#include<algorithm>
#include<iostream>
#include<string>
#include <mutex>

IndexStore::IndexStore() {
    // TO-DO initialize the DocumentMap and TermInvertedIndex members
    documentMap = {};
    reverseDocumentMap = {};
    termInvertedIndex = {};
}

// long IndexStore::putDocument(std::string documentPath) {
//     // TO-DO assign a unique number to the document path and return the number ✅
//     // IMPORTANT! you need to make sure that only one thread at a time can access this method ✅

//     std::lock_guard<std::mutex> lock(documentMapMutex);

//     if (documentMap.contains(documentPath)) {
//         return documentMap[documentPath];
//     }

//     long documentNumber = documentMap.size() + 1;
//     documentMap[documentPath] = documentNumber;

//     // reverse map for getDocument method for constant complexity while retrieving docs
//     reverseDocumentMap[documentNumber] = documentPath;

//     return documentNumber;
// }
long IndexStore::putDocument(std::string documentPath, std::string clientName) {

    std::lock_guard<std::mutex> lock(documentMapMutex);
    long documentNumber = documentMap.size() + 1;
    DocumentInfo docInfo = { documentPath, clientName };
    documentMap[documentNumber] = docInfo;

    return documentNumber;
}

DocumentInfo  IndexStore::getDocument(long documentNumber) {
    // std::lock_guard<std::mutex> lock(documentMapMutex);
    std::string documentPath = "";
    // TO-DO retrieve the document path that has the given document number

    return documentMap[documentNumber];
}


// std::string IndexStore::getDocument(long documentNumber) {
//     std::string documentPath = "";
//     // TO-DO retrieve the document path that has the given document number ✅

//     if (reverseDocumentMap.contains(documentNumber)) {
//         documentPath = reverseDocumentMap[documentNumber];
//     }

//     return documentPath;
// }

void IndexStore::updateIndex(long documentNumber, const std::unordered_map<std::string, long> &wordFrequencies) {
    // TO-DO update the TermInvertedIndex with the word frequencies of the specified document ✅
    // IMPORTANT! you need to make sure that only one thread at a time can access this method ✅

    std::lock_guard<std::mutex> lock(termInvertedIndexMutex);

    for (const auto &wordFrequency : wordFrequencies) {
        std::string word = wordFrequency.first;
        long frequency = wordFrequency.second;

        auto itr = termInvertedIndex.find(word);
        if (itr != termInvertedIndex.end()) {
            itr->second.push_back({documentNumber, frequency});
        }
        else{
            termInvertedIndex[word] = {{documentNumber, frequency}};
        }
    }
}

std::vector<DocFreqPair> IndexStore::lookupIndex(std::string term) {
    // TO-DO return the document and frequency pairs for the specified term ✅

    std::vector<DocFreqPair> results = {};
    if (termInvertedIndex.contains(term)) {
        results = termInvertedIndex[term];
    }

    return std::move(results);
}
