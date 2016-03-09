#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "DiskMultiMap.h"
#include "MultiMapTuple.h"
#include "BinaryFile.h"
#include <cstring>



// this struct will deal with the header information
struct Header {
    int num_buckets;
    BinaryFile::Offset m_deleted_node_head;
    BinaryFile::Offset hashmap_head;
    BinaryFile::Offset hashmap_end;
    // stores the amount of bucket nodes
    int m_size;
};
// the struct for our bucket, since this is an open hash table, the bucket just has a  linked list
struct Bucket {
    Bucket() : head(0) {};
    // head of linked list;
    BinaryFile::Offset head;
};

struct BucketNode {
    BucketNode() {
        m_key[0]= m_value[0] = m_context[0] = 0;
        m_offset = 0;
        m_next = 0;
    };
    BucketNode(const string &key, const string &value, const string &context, BinaryFile::Offset next){
        strcpy(m_key, key.c_str());
        strcpy(m_value, value.c_str());
        strcpy(m_context, context.c_str());
        m_offset = 0;
        m_next = next;
    };
    char m_key[121];
    char m_value[121];
    char m_context[121];
    BinaryFile::Offset m_offset;
    BinaryFile::Offset m_next;

};


class DiskMultiMap

{
public:
    class Iterator
    {
    public:
        Iterator();
        Iterator(BinaryFile::Offset offset, DiskMultiMap* map);
        Iterator(DiskMultiMap* map);
        // You may add additional constructors
        bool isValid() const;
        Iterator& operator++();
        MultiMapTuple operator*();

    private:
        BinaryFile::Offset m_offset;
        DiskMultiMap* m_map;
        // Your private member declarations will go here
    };

    DiskMultiMap();
    ~DiskMultiMap();
    bool createNew(const std::string& filename, unsigned int numBuckets);
    bool openExisting(const std::string& filename);
    void close();
    bool insert(const std::string& key, const std::string& value, const std::string& context);
    Iterator search(const std::string& key);
    int erase(const std::string& key, const std::string& value, const std::string& context);
    int getBucketIndex(const std::string& key);

private:
    BinaryFile m_bf;
    Header m_header;
    // Your private member declarations will go here
};

#endif // DISKMULTIMAP_H_
