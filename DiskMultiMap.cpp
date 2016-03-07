#include "DiskMultiMap.h"
#include <functional>

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
        m_header.hashmap_end = numBuckets * sizeof(Bucket) + m_header.hashmap_head;
        m_header.m_size = 0;
        // LET 0 mean NULL
        m_header.m_deleted_node_head = 0;
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
    // if its closed already do nothing
    if (!m_bf.isOpen()) return;
    // idk
    m_bf.close();
}

bool DiskMultiMap::insert(const std::string &key, const std::string &value, const std::string &context) {
    if (key.size() > 120 || value.size() > 120 || context.size() > 120) return false;
    int hash = std::hash<std::string>()(key);
    int index = (hash % m_header.num_buckets) + m_header.hashmap_head;
    Bucket bucket;
    // get the bucket at that hash index
    m_bf.read(bucket, index);
    // if the list was empty, just make head equal to this value
    // if the list is not empty then put head in another place, then put this value in head
    // create new node,pointing to head
    BucketNode p(key,value,context, bucket.head);

    // CHECK TO SEE IF WE HAVE ANY DELETED NODES
    if (m_header.m_deleted_node_head != 0){
        // if we do then put the value on one of thsoe!
        // store the deleted head in temp
        BucketNode temp;
        m_bf.read(temp, m_header.m_deleted_node_head);
        // write p to deleted head's offset
        p.m_offset = m_header.m_deleted_node_head;
        m_bf.write(p,m_header.m_deleted_node_head);

        // set head to p
        bucket.head = p.m_offset;
        // save bucket
        m_bf.write(bucket, index);
        // poitn deleted head to its next value
        m_header.m_deleted_node_head = temp.m_next;
        // save the header
        m_bf.write(m_header,0);
        return true;
    }
    // OTHERWISE JUST EXPAND THE FILE SIZE

    // write the new node to the file at the current latest offset
    // which is the size of the list (or the size of 1 item was added) multiplied by the size of a disk node
    BinaryFile::Offset offset = (sizeof(BucketNode) * (m_header.m_size+1)) + m_header.hashmap_end;
    // set the nodes offset to the place where it was palced
    p.m_offset = offset;
    // add to the latest offset
    // currently m_size is only incremented
    m_bf.write(p, offset);
    m_header.m_size++;
    // ave info in header
    m_bf.write(m_header, 0);
    // set head to p
    bucket.head = p.m_offset;
    // save the bucket
    m_bf.write(bucket, index);
    return true;
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string &key) {
    return DiskMultiMap::Iterator();
}

int DiskMultiMap::erase(const std::string &key, const std::string &value, const std::string &context) {
    return 0;
}

DiskMultiMap::Iterator::Iterator() {
    m_offset = 0;
}

bool DiskMultiMap::Iterator::isValid() const {
    return m_offset != 0;
}

DiskMultiMap::Iterator &DiskMultiMap::Iterator::operator++() {
    BucketNode bucketNode;
    
}

MultiMapTuple DiskMultiMap::Iterator::operator*() {
    return MultiMapTuple();
}
