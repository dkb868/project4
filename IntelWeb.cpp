//
// Created by mitrikyle on 3/7/16.
//

#include "IntelWeb.h"


IntelWeb::IntelWeb() {
    // TODO?
}

IntelWeb::~IntelWeb() {
    // TODO
}

bool IntelWeb::createNew(const std::string &filePrefix, unsigned int maxDataItems) {
    // make sure the maps are already closed
    close();
    std::string initiator_string = filePrefix;
    initiator_string.append("-initiator-map");
    std::string target_string = filePrefix;
    target_string.append("-target-map");
    // TODO don't just use maxDataItems, actually use the load factor
    if (m_initiator_map.createNew(initiator_string, maxDataItems) &&
            m_target_map.createNew(target_string,maxDataItems)){
        // if succesful
        return true;
    } else {
        // unsuccesful
        m_initiator_map.close();
        m_target_map.close();
        return false;
    }
}

bool IntelWeb::openExisting(const std::string &filePrefix) {
    // make sure the maps are already closed
    close();
    std::string initiator_string = filePrefix;
    initiator_string.append("-initiator-map");
    std::string target_string = filePrefix;
    target_string.append("-target-map");
    // try to open the files with the datas
   if ( m_initiator_map.openExisting(initiator_string) && m_target_map.openExisting(target_string)){
       return true;
   } else {
       // unsuccessful
       m_initiator_map.close();
       m_target_map.close();
       return false;
   }

}

void IntelWeb::close() {
    // close all our maps
    m_target_map.close();
    m_target_map.close();
}

bool IntelWeb::ingest(const std::string &telemetryFile) {
    return false;
}

unsigned int IntelWeb::crawl(const std::vector <std::string> &indicators, unsigned int minPrevalenceToBeGood,
                             std::vector <std::string> &badEntitiesFound,
                             std::vector<InteractionTuple> &interactions) {
    return 0;
}

bool IntelWeb::purge(const std::string &entity) {
    return false;
}
