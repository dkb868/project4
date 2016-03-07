#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "DiskMultiMap.h"
#include "MultiMapTuple.h"
#include "BinaryFile.h"

// because pointing to '0' would be pointing to the beginning of our header.
// this may be unnecessary but i feel like it
#define NULLPTR -1;


// this struct will deal with the header information
struct Header {
    int num_buckets;
    BinaryFile::Offset m_deleted_node_head;
    BinaryFile::Offset hashmap_head;
    BinaryFile::Offset hashmap_end;
};
// the struct for our bucket, since this is an open hash table, the bucket just has a  linked list
struct Bucket {
    Bucket() : head(NULL) {};
    // head of linked list;
    BinaryFile::Offset head;
};


class DiskMultiMap
{
public:

    class Iterator
    {
    public:
        Iterator();
        // You may add additional constructors
        bool isValid() const;
        Iterator& operator++();
        MultiMapTuple operator*();

    private:
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

private:
    BinaryFile m_bf;
    Header m_header;
    // Your private member declarations will go here
};

#endif // DISKMULTIMAP_H_
