//
// Created by mitrikyle on 3/7/16.
//

#include <sstream>
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
    ifstream file;
    std::string line;
    // open the file
    file.open("/home/mitrikyle/ClionProjects/project4/log.txt");
    // if succesful
    if(file.is_open()){
        // while we aren't at the end of the file
        while(getline(file,line)){
            // Things are parsed and stored in maps
            std::cout << line << endl;
            std::string buffer,machine,initiator,target;
            std::stringstream ss(line);
            // we know the first thign in the file is the machine
            // the second is the intiator
            // and 3rd is the target
            // THE DATA MUST ALWAYS BE IN THIS ORDER IN A VALID LOGFILE
            ss >> machine;
            ss >> initiator;
            ss >> target;
            cout << "Machine: " << machine << endl;
            cout << "Initiator: " << initiator << endl;
            cout << "Target: " << target << endl;
            // store this data in our maps.
            // unstable code below TODO WARNING
          //  m_initiator_map.insert(initiator, target, machine);
          //  m_target_map.insert(target,initiator,machine);

        }
    }
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
