#include "DiskMultiMap.h"
#include <functional>
#include <cassert>
#include <cmath>

DiskMultiMap::DiskMultiMap() {
    m_header.num_buckets = 0;
    m_header.hashmap_head = sizeof(Header);
    m_header.hashmap_end = m_header.hashmap_head;
    m_header.m_size = 0;
    // LET 0 mean NULL
    m_header.m_deleted_node_head = 0;
}

DiskMultiMap::~DiskMultiMap() {
    close();
}

bool DiskMultiMap::createNew(const std::string &filename, unsigned int numBuckets) {
    if (!m_bf.isOpen()){
        m_bf.close();
    }
    if (m_bf.createNew(filename)) {
        m_header.num_buckets = numBuckets;
        m_header.hashmap_end = numBuckets * sizeof(Bucket) + m_header.hashmap_head;
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
    int index = getBucketIndex(key);
    Bucket bucket;
    // get the bucket at that hash index
    assert(m_bf.read(bucket, index));
    // if the list was empty, just make head equal to this value
    // if the list is not empty then put head in another place, then put this value in head
    // create new node,pointing to head
    BucketNode bucketNode(key, value, context, bucket.head);

    // CHECK TO SEE IF WE HAVE ANY DELETED NODES
    if (m_header.m_deleted_node_head != 0){
        // if we do then put the value on one of thsoe!
        // store the deleted head in temp
        BucketNode temp;
        m_bf.read(temp, m_header.m_deleted_node_head);
        // write p to deleted head's offset
        bucketNode.m_offset = m_header.m_deleted_node_head;
        m_bf.write(bucketNode, m_header.m_deleted_node_head);

        // set head to p
        bucket.head = bucketNode.m_offset;
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
    // TODO filelength -1 or +1 or ?
    BinaryFile::Offset offset = m_bf.fileLength(); // now using filelength instead of size
    // set the nodes offset to the place where it was palced
    bucketNode.m_offset = offset;
    // add to the latest offset
    // currently m_size is only incremented
    assert(m_bf.write(bucketNode, offset));
    m_header.m_size++;
    // save info in header
    assert(m_bf.write(m_header, 0));
    // set head to p
    bucket.head = bucketNode.m_offset;

    // save the bucket
    assert(m_bf.write(bucket, index));

    // FINALLY VERIFY THAT THE DATA WAS ACTUALLY INSERTED 100% CONFIRMED
    BucketNode temp;
    assert(m_bf.read(temp,bucketNode.m_offset));
    cout << "FINAL Bucket Node Key: " << temp.m_key << endl;
    cout << " FINAL Bucket Node Value: " << temp.m_value <<endl;
    cout << "FINAL Bucket Offset: " <<temp.m_offset << endl;
    return true;
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string &key) {
    if (key.size() > 120) return DiskMultiMap::Iterator();
    int index = getBucketIndex(key);
    Bucket bucket;
    // get the bucket at that hash index
    m_bf.read(bucket, index);
    // if the list is empty then return
    if (bucket.head == 0) return DiskMultiMap::Iterator();
    BucketNode searchNode;
    m_bf.read(searchNode, bucket.head);
    while(true) {
        // if we find it return it
        if (searchNode.m_key == key)
            return DiskMultiMap::Iterator(searchNode.m_offset,this);
        // if we are at the  end, return invalid
        if (searchNode.m_next == 0)
            return DiskMultiMap::Iterator();
        // else increment
        m_bf.read(searchNode, searchNode.m_next);
    }
}



int DiskMultiMap::erase(const std::string &key, const std::string &value, const std::string &context) {
    if (key.size() > 120 || value.size() > 120 || context.size() > 120) return 0;
    int erase_count = 0; // count the erases lol
    int index = getBucketIndex(key);
    Bucket bucket;
    // get the bucket at that hash index
    m_bf.read(bucket, index);
    // if the list is empty then return
    if (bucket.head == 0) return 0;

    // if we want to delete first
    // point head to second node
    BucketNode tempNode;
    BucketNode searchNode;
    m_bf.read(tempNode, bucket.head);
    // if value is found
    // TODO use strcmp
    if (tempNode.m_key == key && tempNode.m_value == value && tempNode.m_context == context){
        // head = p->next
        bucket.head = tempNode.m_next;
        // increment count
        erase_count++;
        push_front_deleted(tempNode.m_offset);
        // save the bucket
        m_bf.write(bucket, index);
    }
    while(tempNode.m_next != 0) {
        // if we find it return it
        m_bf.read(searchNode, tempNode.m_next);
        if (searchNode.m_key == key && searchNode.m_value == value && searchNode.m_context == context){
            // tempnode points to node above
            tempNode.m_next = searchNode.m_next;
            // something was deleted so icnrement delete count
            erase_count++;
            push_front_deleted(searchNode.m_offset);
            // save the node above, aka tempnode
            m_bf.write(tempNode,tempNode.m_offset);
            // continue since we already changed tempnodes next value,
            // tempnode.m_next will already give us something new
            continue;
        }
        // else increment
        m_bf.read(tempNode, tempNode.m_next);
    }


    return erase_count;

}

DiskMultiMap::Iterator::Iterator() {
    m_offset = 0;
}


DiskMultiMap::Iterator::Iterator(BinaryFile::Offset offset, DiskMultiMap *map) {
    m_offset = offset;
    m_bf = &map->m_bf;
}

DiskMultiMap::Iterator::Iterator(DiskMultiMap *map) {
    m_bf = &map->m_bf;
}

bool DiskMultiMap::Iterator::isValid() const {
    return m_offset != 0;
}

DiskMultiMap::Iterator &DiskMultiMap::Iterator::operator++() {
    if (!isValid()){
        return *this; // TODO get pointer to binary file instaed of entire map
    }

    BucketNode bucketNode; // TODO while soemthign
    (*m_bf).read(bucketNode,m_offset);
    std::string original_key = bucketNode.m_key;
    while (true) {
        // if we reach the end break
        if (bucketNode.m_next == 0) {
            // set state to invalid then return
            m_offset = 0;
            return *this;
        }
        // increment bucketNode
        (*m_bf).read(bucketNode,bucketNode.m_next);
        // if the key is found
        if (bucketNode.m_key == original_key){
            // point to it
            m_offset = bucketNode.m_offset;
            return *this;
        }
    }


}

MultiMapTuple DiskMultiMap::Iterator::operator*() {
    if (!isValid()){
        // hopefully empty strings
        return MultiMapTuple();
    }
    MultiMapTuple tuple;
    BucketNode bucketNode;
    // read the bucket node at the current offset
    (*m_bf).read(bucketNode,m_offset);

    // put all that data into tuple
    tuple.context = bucketNode.m_context;
    tuple.key = bucketNode.m_key;
    tuple.value = bucketNode.m_value;
    return tuple;
}

// hashes a string and gets the location of the bucket,
// since this fucked my code earlier, better have its own function
int DiskMultiMap::getBucketIndex(const std::string &key) {
    long hash = std::hash<std::string>()(key);
    hash = abs(hash);
    BinaryFile::Offset index = ((hash % m_header.num_buckets) * sizeof(Bucket))+ m_header.hashmap_head;
    return index;
}

// put a node at the front of the dleeted node linked list
bool DiskMultiMap::push_front_deleted(BinaryFile::Offset offset) {
    // if the list was empty, just make head equal to this value
    // if the list is not empty then put head in another place, then put this value in head

    BucketNode p;
    // get the node from the file
    m_bf.read(p, offset);
    // cleaning up
    p.m_value[0] = p.m_key[0] = p.m_context[0] = 0;
    // point that thing to head
    p.m_next = m_header.m_deleted_node_head;
    // point head to p;
    m_header.m_deleted_node_head = p.m_offset;
    // save header
    m_bf.write(m_header,0);
    // save our data
    m_bf.write(p, p.m_offset);
    return true;
}
