#include "DiskMultiMap.h"
#include "DiskList.h"

// this struct will deal with the header information
struct Header {

};
// the struct for our bucket, since this is an open hash table, the bucket just has a  linked list
struct Bucket {
    Bucket(const std::string &filename) :  m_list(filename){};
    DiskList m_list;
};


DiskMultiMap::DiskMultiMap() {

}

DiskMultiMap::~DiskMultiMap() {

}

bool DiskMultiMap::createNew(const std::string &filename, unsigned int numBuckets) {
    return false;
}

bool DiskMultiMap::openExisting(const std::string &filename) {
    return false;
}

void DiskMultiMap::close() {

}

bool DiskMultiMap::insert(const std::string &key, const std::string &value, const std::string &context) {
    return false;
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string &key) {
    return DiskMultiMap::Iterator();
}

int DiskMultiMap::erase(const std::string &key, const std::string &value, const std::string &context) {
    return 0;
}

DiskMultiMap::Iterator::Iterator() {

}

bool DiskMultiMap::Iterator::isValid() const {
    return false;
}

DiskMultiMap::Iterator &DiskMultiMap::Iterator::operator++() {
    return <#initializer#>;
}

MultiMapTuple DiskMultiMap::Iterator::operator*() {
    return MultiMapTuple();
}
