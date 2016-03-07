#include "DiskMultiMap.h"


DiskMultiMap::DiskMultiMap() {

}

DiskMultiMap::~DiskMultiMap() {

}

bool DiskMultiMap::createNew(const std::string &filename, unsigned int numBuckets) {
    if (!m_bf.isOpen()){
        m_bf.close();
    }
    if (m_bf.createNew(filename)) {
        m_header.num_buckets = numBuckets;
        m_header.hashmap_head = sizeof(Header);
        m_header.hashmap_end = numBuckets * sizeof(Bucket);
        // LET -1 mean NULL
        m_header.m_deleted_node_head = NULL;
        // save the header
        m_bf.write(m_header, 0);
        // store the location that we are creating a bucket
        int location = m_header.hashmap_head;
        // make new buckets in the hashmap
        for (int i = 0; i < numBuckets; i++) {
            Bucket bucket_to_insert;
            m_bf.write(bucket_to_insert, location);
            location += sizeof(Bucket);
        }
        return true;
    }
    // things were not created successfuly
    return false;

}

bool DiskMultiMap::openExisting(const std::string &filename) {
    if (!m_bf.isOpen()){
        m_bf.close();
    }
    
    return m_bf.openExisting(filename) && m_bf.read(m_header, 0);
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
