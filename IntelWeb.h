#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include "DiskMultiMap.h"
#include <string>
#include <vector>

class IntelWeb
{
public:
    IntelWeb();
    ~IntelWeb();
    bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
    bool openExisting(const std::string& filePrefix);
    void close();
    bool ingest(const std::string& telemetryFile);
    unsigned int crawl(const std::vector<std::string>& indicators,
                       unsigned int minPrevalenceToBeGood,
                       std::vector<std::string>& badEntitiesFound,
                       std::vector<InteractionTuple>& interactions
    );
    bool purge(const std::string& entity);

private:
    // Your private member declarations will go here
    // For now, two multi maps will be used
    // one from intiation to associated values
    DiskMultiMap m_initiator_map;
    // and oen from target to associated values
    DiskMultiMap m_target_map;
};

#endif // INTELWEB_H_


